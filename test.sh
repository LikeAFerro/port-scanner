#!/usr/bin/env bash
# Simple test script for the port-scanner program.

set -e # Exit on any command failure

SCANNER=./port-scanner
PASS=0
FAIL=0

check_pass() {  # Check that the command succeeds (exit code 0)
    local desc="$1"; shift  # Description of the test
    if "$@" > /dev/null 2>&1; then  # Run the command and check if it succeeds
        echo "  PASS: $desc"
        ((++PASS))
    else
        echo "  FAIL: $desc"
        ((++FAIL))
    fi
}

check_fail() {  # Check that the command fails (non-zero exit code)
    local desc="$1"; shift  # Description of the test
    if ! "$@" > /dev/null 2>&1; then # Run the command and check if it fails
        echo "  PASS: $desc"
        ((++PASS))
    else
        echo "  FAIL: $desc (expected non-zero exit)"
        ((++FAIL))
    fi
}

check_output() {
    local desc="$1"; local pattern="$2"; shift 2    # Description and regex pattern to check in output
    local out
    out=$("$@" 2>/dev/null || true) # Run the command and capture its output, ignoring stderr
    if echo "$out" | grep -qE "$pattern"; then  # Check if the output matches the expected pattern
        echo "  PASS: $desc (output: $out)"
        ((++PASS))
    else
        echo "  FAIL: $desc (output: $out)"
        ((++FAIL))
    fi
}

echo "=== Valid usage ==="
check_pass  "help flag"             $SCANNER --help
check_pass  "single port"           $SCANNER 127.0.0.1 -p 80
check_pass  "port range"            $SCANNER 127.0.0.1 -f 80 -t 90
check_pass  "all ports (well known)" $SCANNER 127.0.0.1 -f 1 -t 1024
check_pass  "ipv6 integration"      $SCANNER ::1 -p 80

echo "=== Output validation ==="
check_pass "port 22 has a result"   $SCANNER 127.0.0.1 -p 22
check_output "port 80 has a result, verbose mode" "Port 80 is (OPEN|CLOSED|UNKNOWN|FILTERED)" \
    $SCANNER 127.0.0.1 -p 80 -v

echo "=== Invalid usage ==="
check_fail "no arguments"           $SCANNER
check_fail "missing IP"             $SCANNER -p 80
check_fail "-p and -f together"     $SCANNER 127.0.0.1 -p 80 -f 1
check_fail "invalid port"           $SCANNER 127.0.0.1 -p 99999
check_fail "min > max"              $SCANNER 127.0.0.1 -f 90 -t 80

echo ""
echo "Results: $PASS passed, $FAIL failed"
[ $FAIL -eq 0 ]