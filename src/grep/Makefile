all: s21_grep

s21_grep:
	gcc -Wall -Wextra -Werror ./s21_grep.c -o s21_grep

valgrind:
	bash test_leaks.sh

tests:
	bash tests.sh

clang_check:
	clang-format -n *.c

clang_fix:
	clang-format -i *.c
