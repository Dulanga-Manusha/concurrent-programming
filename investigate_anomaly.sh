#!/bin/bash

# Critical Issue Investigation Script
# Focuses on the anomalous behavior in Case 3

echo "=== Concurrent Linked List - Critical Issue Investigation ==="
echo "Target: Case 3 Mutex Performance Reversal (4→8 threads)"
echo ""

# Function to run specific test with enhanced logging
run_debug_test() {
    local case_num=$1
    local threads=$2
    local samples=$3
    
    echo "Testing Case $case_num with $threads threads ($samples samples)..."
    
    # Create a temporary debug version
    gcc -DDEBUG_MODE -g -pthread -O2 -o debug_test performance_test.c -lm 2>/dev/null
    
    if [ $? -eq 0 ]; then
        ./debug_test $case_num 1000 10000 $samples
    else
        echo "Debug compilation failed, using regular version..."
        ./performance_test $case_num 1000 10000 $samples
    fi
    
    echo ""
}

# Test 1: Focus on Case 3 anomaly with more samples
echo "=== INVESTIGATION 1: Case 3 Extended Sampling ==="
echo "Running Case 3 with 100 samples to verify performance reversal..."
run_debug_test 3 4 100

echo "=== INVESTIGATION 2: Single Core Testing ==="
echo "Testing if the anomaly persists on single CPU core..."

# Check if taskset is available
if command -v taskset >/dev/null 2>&1; then
    echo "Running Case 3 on single core (CPU 0)..."
    taskset -c 0 ./performance_test 3 1000 10000 20
else
    echo "taskset not available - skipping single core test"
fi

echo ""

echo "=== INVESTIGATION 3: Different Optimization Levels ==="
echo "Testing with different compiler optimizations..."

# Test with no optimization
echo "Compiling with -O0 (no optimization)..."
gcc -Wall -Wextra -std=c99 -pthread -O0 -o test_O0 performance_test.c -lm
if [ $? -eq 0 ]; then
    echo "Running Case 3 with -O0:"
    ./test_O0 3 1000 10000 10
    rm -f test_O0
else
    echo "O0 compilation failed"
fi

# Test with maximum optimization
echo "Compiling with -O3 (maximum optimization)..."
gcc -Wall -Wextra -std=c99 -pthread -O3 -o test_O3 performance_test.c -lm
if [ $? -eq 0 ]; then
    echo "Running Case 3 with -O3:"
    ./test_O3 3 1000 10000 10
    rm -f test_O3
else
    echo "O3 compilation failed"
fi

echo ""

echo "=== INVESTIGATION 4: Manual Operation Count Verification ==="
echo "Creating enhanced debug version with operation counting..."

# Create a version with operation counting
cat > debug_count.c << 'EOF'
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>

// Global operation counters (for debugging)
int total_member_ops = 0;
int total_insert_ops = 0;
int total_delete_ops = 0;
pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

// Include the same data structures and functions as performance_test.c
struct list_node_s {
    int data;
    struct list_node_s* next;
};

struct list_node_s* head_p = NULL;
pthread_mutex_t list_mutex;
int n, m, thread_count;
double m_member, m_insert, m_delete;

// Simplified debug version of mutex thread function
void* debug_thread_function_mutex(void* rank) {
    long my_rank = (long) rank;
    int ops_per_thread = m / thread_count;
    int remainder = m % thread_count;
    int my_ops = ops_per_thread + (my_rank < remainder ? 1 : 0);
    
    unsigned int seed = time(NULL) + my_rank + getpid();
    
    int local_member = 0, local_insert = 0, local_delete = 0;
    
    for (int i = 0; i < my_ops; i++) {
        double operation = (double) rand_r(&seed) / RAND_MAX;
        
        if (operation < m_member) {
            local_member++;
        } else if (operation < m_member + m_insert) {
            local_insert++;
        } else {
            local_delete++;
        }
    }
    
    // Update global counters
    pthread_mutex_lock(&counter_mutex);
    total_member_ops += local_member;
    total_insert_ops += local_insert;
    total_delete_ops += local_delete;
    pthread_mutex_unlock(&counter_mutex);
    
    printf("Thread %ld: %d ops (%d Member, %d Insert, %d Delete)\n", 
           my_rank, my_ops, local_member, local_insert, local_delete);
    
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: %s <threads> <total_ops> <samples>\n", argv[0]);
        return 1;
    }
    
    thread_count = atoi(argv[1]);
    m = atoi(argv[2]);
    int samples = atoi(argv[3]);
    
    // Case 3 parameters
    m_member = 0.50;
    m_insert = 0.25;
    m_delete = 0.25;
    
    printf("=== Debug Operation Count Test ===\n");
    printf("Threads: %d, Operations: %d, Samples: %d\n", thread_count, m, samples);
    printf("Expected: %.0f Member, %.0f Insert, %.0f Delete\n", 
           m * m_member, m * m_insert, m * m_delete);
    printf("\n");
    
    for (int sample = 0; sample < samples; sample++) {
        total_member_ops = 0;
        total_insert_ops = 0;
        total_delete_ops = 0;
        
        pthread_t* threads = malloc(thread_count * sizeof(pthread_t));
        
        printf("Sample %d:\n", sample + 1);
        
        for (long t = 0; t < thread_count; t++) {
            pthread_create(&threads[t], NULL, debug_thread_function_mutex, (void*)t);
        }
        
        for (int t = 0; t < thread_count; t++) {
            pthread_join(threads[t], NULL);
        }
        
        printf("Total: %d Member, %d Insert, %d Delete (Sum: %d)\n", 
               total_member_ops, total_insert_ops, total_delete_ops,
               total_member_ops + total_insert_ops + total_delete_ops);
        
        if (total_member_ops + total_insert_ops + total_delete_ops != m) {
            printf("ERROR: Operation count mismatch!\n");
        }
        
        printf("\n");
        free(threads);
    }
    
    return 0;
}
EOF

echo "Compiling debug count version..."
gcc -pthread -o debug_count debug_count.c
if [ $? -eq 0 ]; then
    echo "Testing operation distribution for 4 and 8 threads:"
    echo "--- 4 Threads ---"
    ./debug_count 4 10000 3
    echo "--- 8 Threads ---"
    ./debug_count 8 10000 3
    rm -f debug_count debug_count.c
else
    echo "Debug count compilation failed"
    rm -f debug_count.c
fi

echo ""

echo "=== INVESTIGATION 5: Thread Timing Individual Measurement ==="
echo "Measuring per-thread execution times..."

# Create individual thread timing test
cat > thread_timing.c << 'EOF'
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

int thread_count, total_ops;
double thread_times[16]; // Support up to 16 threads

void* timing_thread(void* rank) {
    long my_rank = (long) rank;
    struct timeval start, end;
    
    int ops_per_thread = total_ops / thread_count;
    int remainder = total_ops % thread_count;
    int my_ops = ops_per_thread + (my_rank < remainder ? 1 : 0);
    
    unsigned int seed = time(NULL) + my_rank + getpid();
    
    gettimeofday(&start, NULL);
    
    // Simulate work (without actual list operations)
    for (int i = 0; i < my_ops; i++) {
        double dummy = (double) rand_r(&seed) / RAND_MAX;
        dummy = dummy * dummy; // Some computation
    }
    
    gettimeofday(&end, NULL);
    
    double thread_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    thread_times[my_rank] = thread_time;
    
    printf("Thread %ld: %.6f seconds (%d operations)\n", my_rank, thread_time, my_ops);
    
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <threads> <total_ops>\n", argv[0]);
        return 1;
    }
    
    thread_count = atoi(argv[1]);
    total_ops = atoi(argv[2]);
    
    printf("=== Thread Timing Test ===\n");
    printf("Threads: %d, Total Operations: %d\n\n", thread_count, total_ops);
    
    pthread_t* threads = malloc(thread_count * sizeof(pthread_t));
    
    for (long t = 0; t < thread_count; t++) {
        pthread_create(&threads[t], NULL, timing_thread, (void*)t);
    }
    
    for (int t = 0; t < thread_count; t++) {
        pthread_join(threads[t], NULL);
    }
    
    double total_time = 0;
    for (int t = 0; t < thread_count; t++) {
        total_time += thread_times[t];
    }
    
    printf("\nAverage thread time: %.6f seconds\n", total_time / thread_count);
    printf("Total thread-seconds: %.6f\n", total_time);
    
    free(threads);
    return 0;
}
EOF

gcc -pthread -o thread_timing thread_timing.c
if [ $? -eq 0 ]; then
    echo "--- 4 Threads Timing ---"
    ./thread_timing 4 10000
    echo ""
    echo "--- 8 Threads Timing ---"
    ./thread_timing 8 10000
    rm -f thread_timing thread_timing.c
else
    echo "Thread timing compilation failed"
    rm -f thread_timing.c
fi

echo ""
echo "=== INVESTIGATION COMPLETE ==="
echo ""
echo "SUMMARY OF FINDINGS:"
echo "1. Extended sampling results above"
echo "2. Single core testing (if available)"
echo "3. Optimization level comparison"
echo "4. Operation count verification"
echo "5. Thread timing analysis"
echo ""
echo "RECOMMENDATION:"
echo "Based on these results, determine if the performance reversal is:"
echo "- A genuine implementation bug (most likely)"
echo "- Statistical artifact (less likely with 100+ samples)"
echo "- System-specific behavior (WSL2/virtualization effect)"
echo ""
echo "Next steps: Review the findings above and fix any identified issues."
