#!/bin/sh

## widths for report output columns:
W1=35
W2=35
# test_summary "test name" "test result" "expected result"
test_string_eq(){
# test
	printf "%${W1}s  " "$1"
# result
	printf "%${W2}s  " "$2"
# expected result
	printf "%${W2}s  " "$3"
# success or failure
	[ "$2" = "$3" ] && echo ' \e[32msuccess\e[0m' || echo ' \e[31mfailure\e[0m'
}

# test_summary "test name" "test result" "expected result"
test_num_eq(){
# test
	printf "%${W1}s  " "$1"
# result
	printf "%${W2}s  " "$2"
#expected result
	printf "%${W2}s  " "$3"
# success or failure
	[ "$2" -eq "$3" ] && echo ' \e[32msuccess\e[0m' || echo ' \e[31mfailure\e[0m'
}

# test_summary "test name" "d1" "d2"
test_float_eq(){
# test
	printf "%${W1}s  " "$1"
# result
	printf "%${W2}s  " "$2"
#expected result
	printf "%${W2}s  " "$3"
# success or failure
	RDIFF=`echo "6 k ${2} ${3} - d * v 100 * ${2} ${3} + 2 / d * v / p" | sed -E -e 's/-([0-9.]+)/_\1/g' | dc`
	RDIFFp=`printf "%3.0f" "${RDIFF}"`
	[ "$RDIFFp" -le 1 ] && echo " \e[32m${RDIFFp}% error\e[0m" || echo " \e[31m${RDIFFp}% error\e[0m"

}

## HEAD
printf "%${W1}s  %${W2}s  %${W2}s\n" "TEST" "RESULT" "EXPECTED RESULT"
printf "%${W1}s  %${W2}s  %${W2}s\n" "----" "------" "---------------"
## some reverse tests
echo "concise notation to normal"
v="1.2(2)E3"
w=`echo "$v" | ./concise | tr '	' ' '`
test_string_eq "$v" "$w" "1200 200"

v="1.2(9)"
w=`echo "$v" | ./concise | tr '	' ' '`
test_string_eq "$v" "$w" "1.2 0.9"

v="1.2(9)"
w=`echo "$v" | ./concise --latex`
test_string_eq "$v" "$w" "1.2\pm 0.9"

v="1.2(9)E3"
w=`echo "$v" | ./concise --latex`
test_string_eq "$v" "$w" "(1.2\pm 0.9)\times 10^{3}"

v="-1.2(9)E3"
w=`echo "$v" | ./concise --latex`
test_string_eq "$v" "$w" "(-1.2\pm 0.9)\times 10^{3}"

v="-1.2(9)E-7"
w=`echo "$v" | ./concise --latex`
test_string_eq "$v" "$w" "(-1.2\pm 0.9)\times 10^{-7}"

v="-1.200(189)E-7"
w=`echo "$v" | ./concise --cross`
test_string_eq "$v" "$w" "(-1.20±0.19)×10^{-7}"

v="-1.200(189)E-7"
w=`echo "$v" | ./concise --dot --parentheses`
test_string_eq "$v" "$w" "(-1.20±0.19)·10^(-7)"


v="-1.200(189)E-7"
w=`echo "$v" | ./concise --latex`
test_string_eq "$v" "$w" "(-1.20\pm 0.19)\times 10^{-7}"

echo "normal notation to concise"
## some fwd tests
v="-1.2 0.99"
w=`echo "$v" | ./concise`
test_string_eq "$v" "$w" "-1.20(99)"

v="1.9 0.49"
w=`echo "$v" | ./concise`
test_string_eq "$v" "$w" "1.90(49)"

v="-1.2±0.99"
w=`echo "$v" | ./concise`
test_string_eq "$v" "$w" "-1.20(99)"

v="-1.2E3±0.99E3"
w=`echo "$v" | ./concise`
test_string_eq "$v" "$w" "-1.20(99)E3"

v="-1.2E3±0.99E3"
w=`echo "$v" | ./concise --latex`
test_string_eq "$v" "$w" "-1.20(99)\times 10^{3}"

v="-1.2E3±0.99E3"
w=`echo "$v" | ./concise --asterisk`
test_string_eq "$v" "$w" "-1.20(99)*10^(3)"

v="-1.2E3±0.99E3"
w=`echo "$v" | ./concise --dot`
test_string_eq "$v" "$w" "-1.20(99)·10⁺⁰³"

echo "unreasonable edge-cases"
v="-1.2±0.99E2"
w=`echo "$v" | ./concise`
test_string_eq "$v" "$w" "-1(99)"

v="1.2±9"
w=`echo "$v" | ./concise`
test_string_eq "$v" "$w" "1(9)"

v="1.2±90"
w=`echo "$v" | ./concise`
test_string_eq "$v" "$w" "1(90)"

v="1.2±91"
w=`echo "$v" | ./concise`
test_string_eq "$v" "$w" "1(91)"

v="1.2±912"
w=`echo "$v" | ./concise`
test_string_eq "$v" "$w" "0(9)E2"

v="1.2±900"
w=`echo "$v" | ./concise`
test_string_eq "$v" "$w" "0(9)E2"

v="1.2±100"
w=`echo "$v" | ./concise`
test_string_eq "$v" "$w" "0(1)E2"

