#!/usr/bin/env bash
# Test script for the port scanner program
# This script runs on localhost and scans ports between 80 and 90, which should include some open and closed ports.

set -e  # stop on error

echo "Running tests..."

# Run the port scanner program with the specified arguments
# If this fails, print the error message and exit 1
./port-scanner 127.0.0.1 80 90 || { echo "Port scanner test failed"; exit 1; }

echo "Port scanner test passed"