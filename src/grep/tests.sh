#!/bin/bash
SUCCESS=0
FAIL=0
COUNTER=0
DIFF=""
RED=`tput setaf 1`
GREEN=`tput setaf 2`
BASE=`tput setaf 7`
PURPLE=`tput setaf 5`
${BASE}
# SET_FLAGS
FLAG_DELETE_LOGS=0
FLAG_SHOW_DIFF_TERMINAL=0
FLAG_WRITE_DIFF_FILE=1
FLAG_EXIT_FIRST_FAIL=1


 rm -f fails/*.log

s21_command=(
    "./s21_grep"
    )
sys_command=(
    "grep"
    )

flags=(
    "i"
    "v"
    "c"
    "l"
    "n"
    "h"
    "s"
    "o"
    # "e ^="

)

tests=(
   
    "^print s21_grep.c FLAGS -f test_files/test_ptrn_grep.txt"
    "s21_grep.c FLAGS -f test_files/test_ptrn_grep.txt"
"= test_files/test_1_grep.txt test_files/test_2_grep.txt FLAGS"
"s test_files/test_0_grep.txt FLAGS"
"for s21_grep.c s21_grep.h Makefile FLAGS"
"for s21_grep.c FLAGS"
"-e ^int s21_grep.c s21_grep.h Makefile FLAGS"
"-e for -e ^int s21_grep.c FLAGS"
"-e regex -e ^print s21_grep.c FLAGS -f test_files/test_ptrn_grep.txt"
"-e while -e void s21_grep.c Makefile FLAGS -f test_files/test_ptrn_grep.txt"
"-e intel -e int FLAGS test_files/test_7_grep.txt"
"-e int -e intel FLAGS test_files/test_7_grep.txt"
"-e while -e void s21_grep.c Makefile FLAGS -f test_files/test_ptrn_grep.txt -f test_files/test_ptrn_grep2.txt"
 "-c -e . test_files/test_1_grep.txt"
)

manual=(
    "-c int test_files/test_6_grep.txt"
    "-f test_files/test_3_grep.txt test_files/test_5_grep.txt"
    "^print s21_grep.c -v -o -f test_files/test_ptrn_grep.txt"
"^print test_files/test_ptrn_grep.txt -v -c -o -f test_files/test_ptrn_grep.txt"
"^print s21_grep.c -i -f test_files/test_ptrn_grep.txt"
"int test_files/test_1_grep.txt"
"-n -e ^\} test_files/test_1_grep.txt"
"-c -e '/\' test_files/test_1_grep.txt"
"-ce ^int test_files/test_1_grep.txt test_files/test_2_grep.txt"
"-e int test_files/test_1_grep.txt"
"-nivh = test_files/test_1_grep.txt test_files/test_2_grep.txt"
"-e"
"s test_files/test_0_grep.txt"
"-intel test_files/test_7_grep.txt"
"-ie INT test_files/test_5_grep.txt"
"-echar test_files/test_1_grep.txt test_files/test_2_grep.txt"
"-ne = -e out test_files/test_5_grep.txt"
 "int test_files/test_5_grep.txt"
 "-i int test_files/test_5_grep.txt"

"-v test_files/test_1_grep.txt -e ank"
"-noe ) test_files/test_5_grep.txt"

"-o -e int test_files/test_4_grep.txt"
"-e = -e out test_files/test_5_grep.txt"
"-noe ing -e as -e the -e not -e is test_files/test_6_grep.txt"
"-e ing -e as -e the -e not -e is test_files/test_6_grep.txt"
"-e . test_files/test_1_grep.txt -e '.'"
"-l for no_file.txt test_files/test_2_grep.txt"
"-c for no_file.txt test_files/test_2_grep.txt"
"-e int -si no_file.txt s21_grep.c no_file2.txt s21_grep.h"
 "-si s21_grep.c -f no_pattern.txt"

 "-n for test_files/test_1_grep.txt test_files/test_2_grep.txt"
 "-v int test_files/test_7_grep.txt"
 "-c int test_files/test_6_grep.txt"
 "-vn int test_files/test_7_grep.txt"
 "-vin int test_files/test_7_grep.txt"
 "-si no_file.txt s21_grep.c no_file2.txt s21_grep.h"
 "-lin for test_files/test_1_grep.txt test_files/test_2_grep.txt"
 "-c -l aboba test_files/test_1_grep.txt test_files/test_5_grep.txt"
)

run_test() {
    param=$(echo "$@" | sed "s/FLAGS/$var/")
    "${s21_command[@]}" $param > "${s21_command[@]}".log
    "${sys_command[@]}" $param > "${sys_command[@]}".log
    DIFF="$(diff -q "${s21_command[@]}".log "${sys_command[@]}".log)"
    let "COUNTER++"
    # if [ "$DIFF" == "Files "${s21_command[@]}".log and "${sys_command[@]}".log are identical" ]
    if [ -z "$DIFF" ]
    then
        let "SUCCESS++"
        RESULT="SUCCESS"
        echo "<${PURPLE}${COUNTER}${BASE}>[${GREEN}${SUCCESS}${BASE}/${RED}${FAIL}${BASE}] ${GREEN}${RESULT}${BASE} grep $param"
    else
        let "FAIL++"
        RESULT="FAIL"
        echo "<${PURPLE}${COUNTER}${BASE}>[${GREEN}${SUCCESS}${BASE}/${RED}${FAIL}${BASE}] ${RED}${RESULT}${BASE} grep $param"
        
        if [ ${FLAG_SHOW_DIFF_TERMINAL} == 1 ]
        then
            echo "`diff -y "${s21_command[@]}".log "${sys_command[@]}".log`"
            echo
        fi

        if [ ${FLAG_WRITE_DIFF_FILE} == 1 ]
        then
            # write to file
            echo "#########################################################" > "fails/FAIL-#${COUNTER}".log
            echo >> "fails/FAIL-#${COUNTER}".log
            echo "${sys_command[@]}" $param >> "fails/FAIL-#${COUNTER}".log
            echo >> "fails/FAIL-#${COUNTER}".log
            echo "#########################################################" >> "fails/FAIL-#${COUNTER}".log
            echo >> "fails/FAIL-#${COUNTER}".log
            echo "`diff -y "${s21_command[@]}".log "${sys_command[@]}".log`" >> "fails/FAIL-#${COUNTER}".log
        fi



        if [ ${FLAG_EXIT_FIRST_FAIL} == 1 ]
        then
            exit
        fi
    fi

    if [ ${FLAG_DELETE_LOGS} == 1 ]
    then
            rm -f "${s21_command[@]}".log "${sys_command[@]}".log
            rm -f fails/*.log
    fi
     
    }



echo "^^^^^^^^^^^^^^^^^^^^^^^"
echo "TESTS WITH NORMAL FLAGS"
echo "^^^^^^^^^^^^^^^^^^^^^^^"
printf "\n"
echo "#######################"
echo "MANUAL TESTS"
echo "#######################"
printf "\n"

for i in "${manual[@]}"
do
    var="-"
    run_test "$i"
done

# printf "\n"
# echo "#######################"
# echo "AUTOTESTS"
# echo "#######################"
# printf "\n"
# echo "======================="
# echo "1 PARAMETER"
# echo "======================="
# printf "\n"

for var1 in "${flags[@]}"
do
    for i in "${tests[@]}"
    do
        var="-$var1"
        run_test "$i"
    done
done
# printf "\n"
# echo "======================="
# echo "2 PARAMETERS"
# echo "======================="
# printf "\n"

for var1 in "${flags[@]}"
do
    for var2 in "${flags[@]}"
    do
        if [ $var1 != $var2 ]
        then
            for i in "${tests[@]}"
            do
                var="-$var1 -$var2"
                run_test "$i"
            done
        fi
    done
done
# printf "\n"
# echo "======================="
# echo "3 PARAMETERS"
# echo "======================="
# printf "\n"
for var1 in "${flags[@]}"
do
    for var2 in "${flags[@]}"
    do
        for var3 in "${flags[@]}"
        do
            if [ $var1 != $var2 ] && [ $var2 != $var3 ] && [ $var1 != $var3 ]
            then
                for i in "${tests[@]}"
                do
                    var="-$var1 -$var2 -$var3"
                    run_test "$i"
                done
            fi
        done
    done
done
# printf "\n"
# echo "======================="
# echo "4 PARAMETERS"
# echo "======================="
# printf "\n"
for var1 in "${flags[@]}"
do
    for var2 in "${flags[@]}"
    do
        for var3 in "${flags[@]}"
        do
            for var4 in "${flags[@]}"
            do
                if [ $var1 != $var2 ] && [ $var2 != $var3 ] \
                && [ $var1 != $var3 ] && [ $var1 != $var4 ] \
                && [ $var2 != $var4 ] && [ $var3 != $var4 ]
                then
                    for i in "${tests[@]}"
                    do
                        var="-$var1 -$var2 -$var3 -$var4"
                        run_test "$i"
                    done
                fi
            done
        done
    done
done
# printf "\n"
# echo "#######################"
# echo "AUTOTESTS"
# echo "#######################"
# printf "\n"
# echo "======================="
# echo "DOUBLE PARAMETER"
# echo "======================="
# printf "\n"

for var1 in "${flags[@]}"
do
    for var2 in "${flags[@]}"
    do
        if [ $var1 != $var2 ]
        then
            for i in "${tests[@]}"
            do
                var="-$var1$var2"
                run_test "$i"
            done
        fi
    done
done

# printf "\n"
# echo "#######################"
# echo "AUTOTESTS"
# echo "#######################"
# printf "\n"
# echo "======================="
# echo "TRIPLE PARAMETER"
# echo "======================="
# printf "\n"

for var1 in "${flags[@]}"
do
    for var2 in "${flags[@]}"
    do
        for var3 in "${flags[@]}"
        do
            if [ $var1 != $var2 ] && [ $var2 != $var3 ] && [ $var1 != $var3 ]
            then
                for i in "${tests[@]}"
                do
                    var="-$var1$var2$var3"
                    run_test "$i"
                done
            fi
        done
    done
done


echo
echo "${RED}FAIL: ${FAIL}${BASE}"
echo "${GREEN}SUCCESS: ${SUCCESS}${BASE}"
echo "${PURPLE}TOTAL: $COUNTER"${BASE}
echo

