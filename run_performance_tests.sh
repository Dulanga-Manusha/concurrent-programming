#!/bin/bash

# Performance Testing Script for Concurrent Linked List Implementation
# This script runs performance tests for all three cases and generates data for the tables

echo "=== Concurrent Linked List Performance Testing ==="
echo "Compiling performance test program..."

# Compile the performance test program
gcc -Wall -Wextra -std=c99 -pthread -O2 -o performance_test performance_test.c -lm

if [ $? -ne 0 ]; then
    echo "Compilation failed!"
    exit 1
fi

echo "Compilation successful!"
echo ""

# Check CPU information
echo "=== System Information ==="
echo "CPU Information:"
lscpu | grep -E "Model name|CPU\(s\)|Core\(s\) per socket|Socket\(s\)|Thread\(s\) per core"
echo ""
echo "Memory Information:"
free -h | head -2
echo ""

# Calculate number of samples needed for 95% confidence level and ±5% accuracy
# Using t-distribution (assuming we need at least 30 samples for good statistics)
SAMPLES=50

echo "Running performance tests with $SAMPLES samples each..."
echo "This may take several minutes..."
echo ""

# Test parameters
N=1000
M=10000

echo "=== Case 1: n=$N, m=$M, Member=99%, Insert=0.5%, Delete=0.5% ==="
echo "Running performance test..."
./performance_test 1 $N $M $SAMPLES > case1_results.txt
cat case1_results.txt
echo ""

echo "=== Case 2: n=$N, m=$M, Member=90%, Insert=5%, Delete=5% ==="
echo "Running performance test..."
./performance_test 2 $N $M $SAMPLES > case2_results.txt
cat case2_results.txt
echo ""

echo "=== Case 3: n=$N, m=$M, Member=50%, Insert=25%, Delete=25% ==="
echo "Running performance test..."
./performance_test 3 $N $M $SAMPLES > case3_results.txt
cat case3_results.txt
echo ""

echo "=== Performance Testing Complete ==="
echo "Results have been saved to:"
echo "- case1_results.txt"
echo "- case2_results.txt" 
echo "- case3_results.txt"
echo ""

# Generate a summary table
echo "=== Summary Tables ==="
echo ""

# Function to extract data from result files
extract_data() {
    local file=$1
    local impl=$2
    local threads=$3
    
    if [ "$impl" = "Serial" ] && [ "$threads" != "1" ]; then
        echo -e "-\t-"
        return
    fi
    
    grep "$impl" "$file" | grep -E "\s$threads\s" | awk '{printf "%.6f\t%.6f", $3, $4}'
}

# Case 1 Table
echo "Case 1: n = 1,000 and m = 10,000, mMember = 0.99, mInsert = 0.005, mDelete = 0.005"
echo -e "Implementation\t\tNo of threads"
echo -e "\t\t\t1\t\t2\t\t4\t\t8"
echo -e "\t\t\tAverage\tStd\tAverage\tStd\tAverage\tStd\tAverage\tStd"

for impl in "Serial" "Mutex" "Read-Write Lock"; do
    echo -n -e "$impl\t"
    if [ "$impl" = "Read-Write Lock" ]; then
        echo -n -e "\t"
    fi
    for threads in 1 2 4 8; do
        echo -n -e "$(extract_data case1_results.txt "$impl" $threads)\t"
    done
    echo ""
done
echo ""

# Case 2 Table
echo "Case 2: n = 1,000 and m = 10,000, mMember = 0.90, mInsert = 0.05, mDelete = 0.05"
echo -e "Implementation\t\tNo of threads"
echo -e "\t\t\t1\t\t2\t\t4\t\t8"
echo -e "\t\t\tAverage\tStd\tAverage\tStd\tAverage\tStd\tAverage\tStd"

for impl in "Serial" "Mutex" "Read-Write Lock"; do
    echo -n -e "$impl\t"
    if [ "$impl" = "Read-Write Lock" ]; then
        echo -n -e "\t"
    fi
    for threads in 1 2 4 8; do
        echo -n -e "$(extract_data case2_results.txt "$impl" $threads)\t"
    done
    echo ""
done
echo ""

# Case 3 Table
echo "Case 3: n = 1,000 and m = 10,000, mMember = 0.50, mInsert = 0.25, mDelete = 0.25"
echo -e "Implementation\t\tNo of threads"
echo -e "\t\t\t1\t\t2\t\t4\t\t8"
echo -e "\t\t\tAverage\tStd\tAverage\tStd\tAverage\tStd\tAverage\tStd"

for impl in "Serial" "Mutex" "Read-Write Lock"; do
    echo -n -e "$impl\t"
    if [ "$impl" = "Read-Write Lock" ]; then
        echo -n -e "\t"
    fi
    for threads in 1 2 4 8; do
        echo -n -e "$(extract_data case3_results.txt "$impl" $threads)\t"
    done
    echo ""
done

echo ""
echo "Performance testing completed. Tables are ready for your report!"
