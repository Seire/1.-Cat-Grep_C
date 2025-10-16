all: s21_cat

s21_cat:
	gcc -Wall -Wextra -Werror ./s21_cat.c -o s21_cat

test:
	bash tests.sh

clang_check:
	clang-format -n *.c

clang_fix:
	clang-format -i *.c