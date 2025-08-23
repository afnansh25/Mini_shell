# --- inside test.sh ---

ref_run() {
  printf "%s\necho \$?\n" "$1" \
    | bash 1>ref.out 2>ref.err
  tail -n1 ref.out > ref.code
  sed -n '1,$p' ref.out | sed '$d' > ref.stdout   # drop last line (the $? number)
}

sut_run() {
  # send command, then print $? from YOUR shell, then exit
  printf "%s\necho \$?\nexit\n" "$1" \
    | ./minishell 1>sut.raw 2>sut.err

  # strip prompts from stdout (adjust if your prompt changes)
  sed '/^pink_bow/d' sut.raw > sut.out_with_code
  tail -n1 sut.out_with_code > sut.code
  sed -n '1,$p' sut.out_with_code | sed '$d' > sut.stdout
}

case_run() {
  cmd="$1"
  ref_run "$cmd"; sut_run "$cmd"
  echo "CMD: $cmd"
  diff -u ref.stdout sut.stdout || echo "[Δ] STDOUT differs"
  diff -u ref.err    sut.err    || echo "[Δ] STDERR differs"
  if ! diff -u ref.code sut.code >/dev/null; then
    echo -n "[Δ] EXIT differs: "; printf "bash=%s minishell=%s\n" "$(cat ref.code)" "$(cat sut.code)"
  fi
  echo "---"
}


# variables & empty/spacey commands
case_run '$pwd'
case_run '$PWD'
case_run 'cat $PWD'
case_run '$USER'
case_run 'cat $USER'
case_run '""'
case_run '" "'

# # redirections (with & without spaces)
# case_run 'printf "A\n" >out.txt; cat out.txt'
# case_run 'rm -f out.txt; printf "A\n" > out.txt; cat out.txt'
# case_run 'rm -f out.txt; printf "A\n" >>out.txt; printf "B\n" >> out.txt; cat out.txt'
# case_run 'cat Makefile | wc -l >out.txt; cat out.txt'
# case_run 'cat Makefile | wc -l > out.txt; cat out.txt'

# # directories / permissions / ENOENT
# case_run '/'
# case_run './non'
# case_run '/bin/ls /root/nope 2>/dev/null; echo $?'

# # PATH presence
# case_run 'unset PATH; ls'
# case_run 'unset PATH; /bin/ls'

# # quoting combos
# case_run "echo 'a b' \"c d\" e"
# case_run "echo '\"' \"'\""
# case_run "echo \$HOME '\$HOME' \"\$HOME\""

# # pipelines
# case_run 'echo hi | wc -c'
# case_run 'cat Makefile | grep -n main | wc -l'
# case_run '| ls'            # syntax error expected
# case_run 'ls |'            # syntax error expected
# case_run '|='

# # heredoc (non-interactive)
# case_run 'cat <<EOF
# one
# $PWD
# EOF'
# case_run 'cat <<"EOF"
# one
# $PWD
# EOF'
# case_run "cat <<'$USER'
# x
# $PWD
# $USER"