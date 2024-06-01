#!/bin/bash

SUCCESS=0
FAIL=0
COUNTER=0

arr=(
    "10.0.1.11  1.1.1.1  any any 6"
    "10.0.1.11  1.1.1.1  any any 17"

    "10.0.2.12  1.1.1.1  any any 6"
    "10.0.2.12  1.1.1.1  any any 17"

    "10.0.2.12  8.8.8.8  any any 6"
    "10.0.2.12  8.8.8.8  any any 17"

    "10.0.3.13  any      any any any"

    "any        1.2.3.4  any any 17"

    "any        1.2.3.4  any any 6"

    "10.9.6.0   10.0.9.1 any any 6"
    "10.9.7.255 10.0.9.1 any any 17"

    "10.9.5.255 10.0.9.1 any any 6"
    "10.9.8.0   10.0.9.1 any any 6"
)

results=(
    "accept"
    "drop"

    "drop"
    "drop"

    "accept"
    "drop"

    "accept"

    "drop"

    "accept"

    "accept"
    "accept"

    "drop"
    "drop"
)

run_test() {
    RESULT=$(echo "$@" | ./firewall "--db_test")
    if [ "$RESULT" == "${results[$COUNTER]}" ]
    then
        let SUCCESS++
        echo "$((COUNTER + 1)) - Success $RESULT"
    else
        let FAIL++
        echo "$((COUNTER + 1)) - Fail $RESULT"
    fi
    let COUNTER++
}

echo "======================="
echo "MANUAL TESTS"
echo "======================="
printf "\n"

for test in "${arr[@]}"
do
    run_test $test
done
printf "\n"

echo "FAILED: $FAIL"
echo "SUCCESSFUL: $SUCCESS"
echo "ALL: $COUNTER"
printf "\n"