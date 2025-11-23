# Ultimate Minishell Evaluation Guide (Deep Dive Version)

This document is a **function-by-function, structure-by-structure** explanation of the whole Minishell. It equips you to answer ANY evaluation question: architecture rationale, parsing rules, expansion semantics, execution pipeline, builtins behavior, error & signal handling, memory strategy, and edge cases.

---
## 0. High-Level Lifecycle
1. Read line (prompt, signals active).
2. Lex to tokens (build linked list of `t_token`).
3. Validate syntax (early rejection: pipes, redirs, quotes).
4. Expand tokens (variables, `$?`, tilde, quote rules, removal of empties, ambiguous redirect detection).
5. Classify / build commands (`t_cmd` list; collect args, redirs, heredocs).
6. Pre-exec: process heredocs (all before any fork; may set `exit_code`).
7. Optimize single parent builtins or assignments (run without `fork`).
8. Allocate execution context (`t_exec`: pipes, pids).
9. Fork children, set up `dup2` I/O, run builtin or external via `execve`.
10. Parent waits; last command status picked; signals restored.
11. Loop resumes until EOF or explicit `exit`.

---
## 1. Core Data Structures (`minishell.h`)

### `t_shell`
```c
typedef struct s_shell { int exit_code; char **envp; char **exp; } t_shell;
```
- `exit_code`: Global last command status ($?).
- `envp`: Owned copy of environment (modifiable).
- `exp`: Auxiliary list tracking exported names that have no value yet.

### `t_token`
Represents one syntactic unit produced by the lexer.
```c
typedef struct s_token { char *value; t_token_type type; t_quote_type quote; int ambiguous; struct s_token *next; } t_token;
```
- `value`: Raw (after quote processing) string.
- `type`: WORD / CMD / ARG / PIPE / REDIR_* / HEREDOC.
- `quote`: Context for expansion rules (NO_QUOTE / SINGLE_QUOTE / DOUBLE_QUOTE).
- `ambiguous`: Set when a redirect target expands to empty (=> redirect error like `cat > $EMPTY`).

### `t_cmd`
One pipeline element.
```c
typedef struct s_cmd { char **argv; int infile; int outfile; int append; t_heredoc *heredocs; int n_heredocs; struct s_cmd *next; int redir_error; } t_cmd;
```
- `argv`: NULL-terminated array; `argv[0]` command name.
- `infile` / `outfile`: FD or -1 if none.
- `append`: 1 if `>>` selected; otherwise 0.
- `heredocs`: Array of heredoc descriptors; built before execution proper.
- `redir_error`: Flag to skip execution (e.g. ambiguous or open failure).

### `t_exec`
Execution context for a pipeline.
```c
typedef struct s_exec { int **pipes; pid_t *pids; int cmd_count; int status; t_cmd *cmd_head; t_shell *shell; } t_exec;
```
- Allocated once per pipeline; freed after waiting.
- `pipes`: Array (cmd_count - 1) of two-int arrays.
- `pids`: Track child processes for wait/exit status.

### Expansion Helper `t_exp`
Holds scanning state for `$`, quotes inside expansion (bit flags):
- `f` bit0: in_single; bit1: in_double.
Used to implement a mini state machine while constructing expanded result.

---
## 2. Lexing (Tokenizer)
Entry: `tokens(line, &head, shell)`.

### Responsibilities:
1. Scan input char-by-char.
2. Produce tokens while respecting complex quotes (embedded single in double, escapes if handled).
3. Early quote error via `has_unclosed_quote`.
4. Distinguish operators vs words via `get_token_type`.

### Important Functions:
- `get_token_type(char c, char next, int *len)`: Sets type and length (e.g. `<<`, `>>`).
- `get_word(...)`: Accumulates a word possibly containing quotes; returns unquoted version if necessary via `rmv_quotes`.
- `handle_complex_quotes`: Consolidates segments with embedded quotes (pre-expansion normalization).
- `new_token`, `token_add_back`: Build linked list.

### Quote Classification:
`determine_quote_type` helps later expansion to know if the whole token was enclosed; influences whether `$` is expanded.

---
## 3. Syntax Validation
Function: `validate_syntax(t_token *tokens)` plus granular checks.

Key Checks:
- `check_first_pipe`: Reject leading pipe.
- `check_double_pipe`: Reject `||` (not supported base project).
- `check_redir_rules`: Ensures redirection is followed by a WORD/HEREDOC limiter not another operator.
- `check_pipe_then_redir`: Edge ordering rules.

Error Helpers:
- `err_newline`, `err_pipe`, `err_tok`, `semicolon_error` set `shell->exit_code` and print consistent error messages.

Result: Either tokens proceed or are freed and loop continues.

---
## 4. Expansion Phase
Entry: `expand_token_list(token_head, shell)` after syntax passes.

### Variable & Special Sequence Processing:
- `$VAR`: Extract name via `extract_var_name`; value via `get_var_value`.
- `$?`: Replace with numeric `shell->exit_code`.
- `~` at start via `do_tilde` maps to `HOME` if not quoted or part of a larger word.
- Backslash rules (`do_backslash`) inside double quotes if implemented.

### Quote Rules:
- Single quotes: Literal (no expansion).
- Double quotes: Expand `$`, keep spaces intact.
- Unquoted: Split unaffected; expansions may produce empty string.

### Ambiguous Redirect Detection:
`after_redir(prev, curr, shell)` marks `curr->ambiguous` if expansion results in empty => later `report_ambiguous_redirect` triggers message like `minishell: : ambiguous redirect` and `redir_error` set.

### Removal of Empty Tokens:
`remove_empty_tokens` prunes tokens whose value becomes empty unless structural (e.g. a needed heredoc limiter). Protects argument arrays from stray empties.

### Internal State Machine:
`t_exp.f` bits track entering/leaving quotes — prevents `$` inside single quotes from expanding and ensures nested sequences processed correctly.

---
## 5. Command Construction
Entry: `parse_pipeline(token_list)` loops building `t_cmd` via `parse_cmd`.

### Steps per Command:
1. Initialize struct (`init_cmd`).
2. Collect heredocs first: `collect_heredocs` -> list -> array.
3. Collect redirections: `handle_redirection` (calls specialized `handle_redir_in`, `handle_redir_out`, `handle_redir_append`, `handle_heredoc`). Opens files immediately to surface errors early (sets `redir_error`).
4. Gather command + args: `handle_cmd_and_args` uses `collect_args`, `arg_list_to_array`.
5. Link into pipeline via `cmd->next`.

### Variable Assignment Shortcut:
Tokens like `NAME=value` (validated by `is_variable_assignment`) are treated as single command with no fork when alone.

---
## 6. Heredoc Processing (Pre-Execution)
All heredocs are processed before any pipeline forking: `handle_all_heredocs(cmd_list, shell)`.

### Flow per Heredoc:
1. Create pipe.
2. Fork child to read lines until delimiter match (`hd_should_stop`).
3. If delimiter unquoted: expand `$` occurrences in lines (`expand_line_heredoc`).
4. Child writes into pipe write-end; parent retains read-end as future `infile`.
5. Signals: In heredoc child, `hd_install_sig` sets a dedicated `SIGINT` handler: closes write FD, sets global `g_signo`, ensures parent detects interruption; status 130 mapped to interruption.
6. `hd_wait_and_check` waits; on SIGINT sets `shell->exit_code = 1` and aborts pipeline.

Edge Case: Multiple heredocs attach sequentially; each stored in `cmd->heredocs` array but only the *last resulting infile* matters if later infile redirs override.

---
## 7. Execution Context Allocation
Function: `init_exec_struct(cmd_list, shell)`.
- Counts commands via `count_cmds`.
- Allocates `pipes` only if more than one command.
- Each pipe created with `pipe()`; failures call `error_exit("pipe", ...)`.
- Allocates `pids` array.

Memory Safety: Every allocation error invokes `error_exit`, which prints and exits child or entire shell appropriately, releasing all allocated resources.

---
## 8. Pipeline Execution (`execute_pipeline`)
Sequence:
1. Heredocs handled.
2. Single-command optimizations (`handle_single_cmd`): parent builtin or simple assignment — no fork.
3. Ignore interactive Ctrl+C / Ctrl+\ temporarily (`signal(SIGINT, SIG_IGN)`).
4. Allocate exec context.
5. `fork_and_execute_all` forks each command.
6. Parent waits in `wait_all_children` and picks final status:
   - Normal exit: `shell->exit_code = WEXITSTATUS(st)`.
   - Signal: `128 + signal_number`; prints `Quit: 3` on SIGQUIT.
7. Free context; restore interactive signals.

### Child Path (`run_child`):
1. Setup redirections and pipes with `setup_io` using `dup2` or `xdup2` (error protected).
2. Close *all* pipe FDs and any other open redirection FDs (`close_pipe_files_child`) to prevent indefinite blocking.
3. If builtin: run and `exit(status)`.
4. Path resolution: `get_cmd_path` -> `handle_explicit_path` if contains `/`, else `resolve_from_path` using `$PATH` segments.
5. Failure: `error_exit(cmd, ...)` sets status 126/127 with specific messaging rules (directory vs not found).
6. Success: `execve(path, argv, shell->envp)`; on error prints via `perror`.

### FD Strategy:
- Parent closes unused ends after each fork: `close_pipe_parent`.
- Child closes all after duplications to achieve EOF propagation for readers.
- `close_if_open` centralizes safe closure and sentinel reset.

---
## 9. Builtins Deep Dive

### Classification:
- Parent-only: `cd`, `export`, `unset`, `exit` (stateful or terminating).
- Child-capable: `echo`, `pwd`, `env` (stateless output); plus parent ones when part of pipeline (still run in child for consistency).

### `cd` Logic:
`resolve_target` handles cases: no arg / `--` -> HOME; `-` -> OLDPWD (prints new PWD); `~` prefix expansion; absolute vs relative path (decides logical vs physical update). Updates `OLDPWD` first then `PWD` using `set_logical_pwd` -> complex rules to reflect user's typed form vs canonical directory.
Errors: Missing HOME / OLDPWD produce message and non-zero exit status.

### `export` Flow:
1. Parse token via `parse_export_arg`: splits name / value.
2. Validate identifier: `is_valid_identifier` (leading alpha or underscore then alnum/underscore).
3. Append semantics: `NAME+=value` sets `out.append` -> `env_append` concatenates.
4. If no `=`: adds name to `exp` (pending export) unless already in `envp`.
5. Printing: When called with no args uses sorted environment + pending names; quoting rules in `print_escaped_value_fd`.

### `unset`:
Ensures valid identifier, then remove by index (`env_remove_at`). Errors flagged but non-fatal.

### `exit`:
`exec_exit(cmd, shell, interactive)` prints `exit` in interactive mode. Rules:
- Non-numeric first arg -> immediate exit 255.
- Too many args -> do not exit; set exit_code=1.
- One valid numeric: normalize through modulo 256 after potential long long range check (`is_within_long_long`).
Memory cleanup performed before `exit()`.

### `echo`:
Parses repeated `-n` flags; stops scanning at first non-`-n` argument. Joins remaining with spaces; prints newline unless suppressed.

### `env`:
With no args: print `envp`. With arg beginning `-`: invalid option message. With a program name: attempt exec (similar to running external command with current environment only).

### Variable Assignment Shortcut:
In isolation (no pipes) `NAME=value` updates `envp` via `handle_variable_assignment` without forking; exit_code set to 0 on success.

---
## 10. Path Resolution & Error Semantics
Workflow:
1. If command contains `/`: use as-is; check `access(X_OK)`.
2. Else split `$PATH` (`find_path_variable`) and iterate: join segment + `/cmd` (via `build_cmd_path`), test `access`.
3. Distinguish exit codes:
   - 127: Command not found (ENOENT without slash OR missing path var OR path not resolved).
   - 126: Found but not executable (EACCES) or is directory (`EISDIR`). Directory case prints `is a directory`.
4. `error_exit` centralizes messaging then `exit_child` after cleaning.

---
## 11. Signals & Interactive Behavior
### Prompt Mode:
- `SIGINT` (Ctrl+C): Custom `sigint_handler` prints newline, resets input buffer (`rl_replace_line("",0)`), redisplays prompt, sets `exit_code = 130` (implicitly through later $? usage if you wish).
- `SIGQUIT` ignored.
### During Pipeline Execution:
- Shell temporarily ignores SIGINT/SIGQUIT while children run; children inherit default handlers so `Ctrl+C` kills them; parent later maps status.
### Heredoc Mode:
- Dedicated handler closes write FD, sets global `g_signo`; interruption sets exit_code=1 and aborts pipeline.

---
## 12. Memory Management Strategy
Principles:
- Own all duplicated environment strings; free on shell shutdown or exit.
- For each pipeline: allocate `t_exec`, pipes, pids; free after waiting.
- For each command list: `free_cmd_list` walks nodes; frees `argv`, heredoc limiters, closes FDs.
- Error conditions ALWAYS funnel through `error_exit` to ensure all dynamic resources are freed before exiting.
- Helper `free_arr` (not shown) releases `char **` arrays.

Leak Testing:
Use suppression file to ignore Readline’s known leaks.
```bash
valgrind --leak-check=full --suppressions=valgrind_script/ignore_readline_leaks.txt ./minishell
```

---
## 13. Exit Status Propagation
- Builtins executed in parent set `shell->exit_code` directly.
- Child exit: `wait_all_children` picks last command.
  - Normal: `WEXITSTATUS`.
  - Signal: `128 + signal_number`; prints `Quit: 3` for SIGQUIT.
- `exit` builtin may override with normalized long long.
- Heredoc interrupt sets `exit_code = 1`.
- `$?` expansion uses current `shell->exit_code`.

Common Codes:
- 0: Success.
- 1: General error (e.g. builtin usage issue, heredoc interrupt).
- 126: Found but not executable / is directory.
- 127: Command not found.
- 130: Interrupted by SIGINT (child side) -> 128 + 2.

---
## 14. Detailed FD & Pipe Discipline
Why crucial: A single unclosed write-end keeps readers blocking forever.

Pattern:
- Parent after each fork closes both ends of previous pipes (`close_pipe_parent`).
- Child after `dup2` closes ALL pipe FDs and any redirection FDs.
- Redirection open failures set `redir_error`; command effectively skipped: either not forked or child exits early with appropriate status.

Diagnosing Issues:
Use:
```bash
lsof -p $(pgrep minishell)
```
while pipeline runs; only relevant FDs should remain.

---
## 15. Edge Cases & Behavior Matrix
| Input | Expected | Notes |
|-------|----------|-------|
| `echo "$UNSET"` | (empty line) | Empty expansion preserved as empty arg -> echo prints newline. |
| `cat < nonexist` | Error, exit_code=1 | `open_read_fd` failure triggers redir_error + message via `errno_msg`. |
| `> a > b echo hi` | File `a` truncated, then `b`, output to `b` | Later redir overrides outfile. |
| `echo -n -n test` | `test` (no newline) | Consecutive `-n` flags respected. |
| `cd` with HOME unset | Error printed, exit_code possibly non-zero | Fails before chdir. |
| `VAR=1` | exit_code=0, env updated | Single assignment optimization. |
| `VAR=1 | cat` | Treated as command, forks (no optimization) | Pipeline forces normal execution path. |
| `echo $?'` | `$?` expanded before trailing single quote?` | Depends on tokenization; unmatched quotes rejected earlier. |
| `<<EOF cat` then Ctrl+C | Abort heredoc, exit_code=1 | Pipeline not executed. |

---
## 16. Common Evaluation Questions (Q&A)
1. Q: Why process ALL heredocs first? A: To avoid partial pipeline creation and to ensure interruptions (Ctrl+C) stop before side-effects (creating files, forking children).
2. Q: Difference between parent & child builtin? A: Parent builtins modify shell state (env, cwd, termination); running them in a child would lose side-effects. In pipelines, they must run in a child to preserve semantics of pipe flow.
3. Q: How do you detect ambiguous redirect? A: Expansion yields empty string for a token following redirection; token marked `ambiguous`, sets `redir_error` and prints error.
4. Q: Why ignore SIGINT before forking? A: Prevent main shell from exiting on Ctrl+C intended for child; children inherit default enabling expected behavior.
5. Q: How is `$?` implemented? A: Replaced during expansion by converting `shell->exit_code` to string when scanning `$?` pattern.
6. Q: Memory cleanup guarantee? A: Centralized in `error_exit` and `free_cmd_list` / `free_exec_data`; every allocation site funnels error path there.
7. Q: How do you handle `cd -`? A: Resolve `OLDPWD`; if set, print new directory after change (POSIX behavior).
8. Q: Why store `exp` separately from `envp`? A: To track names exported without assigned values so future assignments appear correctly when printing `export` list.
9. Q: What ensures no zombie processes? A: `wait_all_children` loops every pid synchronously; no background tasks left.
10. Q: How do you prevent blocking on heredoc SIGINT? A: Custom handler closes write FD, causing read loop to detect closure and exit; parent interprets status.

---
## 17. Practical Validation Checklist
- Parsing: Invalid `|`, `||`, `><`, unclosed quotes all yield proper errors + non-zero status.
- Expansion: `$HOME`, `$?`, `$VAR` (unset -> empty), tilde at start only.
- Redirection: Overwrite vs append semantics verified with diff of files.
- Heredoc: Quoted delimiter prevents expansion; unquoted allows `$USER` substitution.
- Builtins: `export VAR`, `export VAR=`, `export VAR+=suffix`, `unset VAR`, `cd ~`, `cd -`, `exit 42`, `exit "notnum"`.
- Pipes: `env | grep PATH | wc -l` returns expected integer.
- Errors: Executing directory -> exit_code=126; unknown command -> 127.
- Signals: `sleep 5` then Ctrl+C -> exit_code=130.

---
## 18. Recommended Test Script Snippets
```bash
# Basic
./minishell <<EOF
echo hello
echo $?
unset UNSETVAR
export VAR=VALUE
echo $VAR
exit
EOF

# Pipeline & redirs
echo 'test' | cat | wc -c
grep PATH <(env)  # (Process substitution not supported; ensure rejection or treat literally)

# Heredoc expansion
cat <<EOF
Line $USER
EOF

# Ambiguous redirect
export EMPTY=
cat > $EMPTY

# Builtin in pipeline (should run in child)
cd .. | echo hi

# Signals
sleep 10
# Press Ctrl+C quickly

# Exit numeric range
exit 999999999999999999999
```

---
## 19. Final Tips for Evaluation
- Always relate behavior to POSIX rules when answering.
- Emphasize separation of concerns: lex → validate → expand → build → heredoc → exec.
- Mention FD discipline and why every close matters.
- Stress signal design: interactive vs execution vs heredoc.
- Show awareness of subtle builtin differences (parent state changes).
- Reference standardized exit codes and mapping (126,127,130).
- Demonstrate test methodology (Valgrind, lsof, stress pipelines).

You now have exhaustive coverage. Review sections 8–16 for the most likely evaluator probing areas.

---
## 20. Quick Reference Summary (Condensed)
- Readline → tokens → validate → expand → cmds → heredocs → (single builtin?) → fork/exec → wait → status.
- Expansion: respects quotes; `$?` and tilde special; ambiguous redirect aborts.
- Execution: pipes allocated upfront; each child duplicates FDs then closes originals.
- Builtins: parent vs child contexts determined by persistence needs.
- Errors centralized in `error_exit`; memory fully reclaimed.
- Signals: custom handlers only in interactive & heredoc phases.

End of Guide.

