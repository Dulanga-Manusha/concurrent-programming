# Concurrent Programming Lab 1: Linked List Implementation

## Overview
This project implements a concurrent linked list data structure using three different synchronization approaches to compare their performance characteristics:

1. **Serial Implementation** - Single-threaded baseline execution
2. **Mutex Implementation** - Multi-threaded with one mutex protecting the entire list
3. **Read-Write Lock Implementation** - Multi-threaded with read-write locks for optimized read concurrency

## Features
- **Thread-safe operations**: Member(), Insert(), and Delete()
- **Value range**: 0 to 2^16 - 1 (65535)
- **Sorted linked list**: Maintains order for efficient operations
- **Duplicate prevention**: No duplicate values allowed
- **Comprehensive performance measurement**: Statistical analysis with proper sampling
- **Configurable workloads**: Adjustable operation mix ratios

## Project Structure
```
├── lab1.c                    # Main implementation (interactive demo)
├── performance_test.c        # Automated performance testing
├── run_performance_tests.sh  # Comprehensive test script
├── Makefile                  # Build configuration
├── DESIGN.md                 # Design explanation (Step 1)
├── TESTING_GUIDE.md          # Testing instructions
├── RESULTS_SUMMARY.md        # Performance results (Step 3)
└── README.md                 # This file
```

## Quick Start

### Compilation
```bash
# Compile all programs
make

# Or compile individually
make lab1              # Interactive demo
make performance_test  # Performance testing
```

### Run Interactive Demo
```bash
./lab1 <n> <m> <thread_count> <m_member> <m_insert>
```

**Example:**
```bash
./lab1 1000 10000 4 0.8 0.1
# 1000 initial elements, 10000 operations, 4 threads
# 80% Member, 10% Insert, 10% Delete operations
```

### Run Performance Tests
```bash
# Quick test (small sample size)
make quick-test

# Comprehensive test (for assignment results)
make performance
```

## Assignment Requirements Implementation

### Step 1: Design Solution ✅
See `DESIGN.md` for detailed explanation of:
- Thread-based operation distribution strategy
- Random number generation approach
- Three synchronization implementations
- Expected performance characteristics

### Step 2: Implementation ✅
Complete implementations in `lab1.c` and `performance_test.c`:
- **Serial**: Single-threaded baseline
- **Mutex**: Global mutex protection
- **Read-Write Locks**: Concurrent reads, exclusive writes

### Step 3: Performance Results ✅
See `RESULTS_SUMMARY.md` for:
- Comprehensive performance tables for all three cases
- Statistical analysis with 50 samples per configuration
- 95% confidence level with ±5% accuracy
- System specifications and analysis

## Test Cases

### Case 1: Read-Heavy Workload
- **Parameters**: n=1000, m=10000
- **Operation Mix**: 99% Member, 0.5% Insert, 0.5% Delete
- **Expected**: Read-write locks should excel

### Case 2: Moderate Mixed Workload  
- **Parameters**: n=1000, m=10000
- **Operation Mix**: 90% Member, 5% Insert, 5% Delete
- **Expected**: Balanced performance

### Case 3: Write-Heavy Workload
- **Parameters**: n=1000, m=10000
- **Operation Mix**: 50% Member, 25% Insert, 25% Delete
- **Expected**: High contention, limited parallelism

## Performance Results Summary

| Case | Read-Write Lock Best At | Mutex Best At | Key Insight |
|------|------------------------|---------------|-------------|
| 1    | All thread counts      | Single thread | RW-locks scale excellently |
| 2    | Low thread counts      | Higher thread counts | Moderate write contention |
| 3    | Single thread only     | Most configurations | High writes limit parallelism |

## System Requirements
- **CPU**: Minimum 4 physical cores (hyperthreading not counted)
- **OS**: Linux with pthread support
- **Compiler**: GCC with C99 support
- **Memory**: Sufficient for concurrent execution

## Development Commands
```bash
# Clean all compiled files
make clean

# Test with different thread counts
make test-threads

# Run specific performance case
./performance_test 1 1000 10000 50  # Case 1 with 50 samples

# Check system information
lscpu | grep -E "Model name|CPU\(s\)|Core\(s\)"
```

## Key Implementation Details

### Thread Safety
- Thread-local random number generators (`rand_r()`)
- Proper synchronization primitive initialization/cleanup
- Load-balanced work distribution among threads

### Performance Measurement
- High-resolution timing with `gettimeofday()`
- Statistical analysis (mean, standard deviation)
- Multiple samples for confidence intervals

### Memory Management
- Proper cleanup of linked list nodes
- No memory leaks in any implementation
- Efficient memory allocation patterns

## Files for Assignment Submission
1. **Source Code**: `lab1.c`, `performance_test.c`
2. **Design Document**: `DESIGN.md` (Step 1)
3. **Performance Results**: `RESULTS_SUMMARY.md` (Step 3)
4. **Build System**: `Makefile`
5. **Testing Guide**: `TESTING_GUIDE.md`

This implementation provides a complete solution for the concurrent programming assignment with proper statistical analysis and comprehensive performance evaluation.  
- 10% Delete operations

## Testing

### Run predefined tests:
```bash
make test
```

### Test with different thread counts:
```bash
make test-threads
```

### Clean compiled files:
```bash
make clean
```

## Implementation Details

### Design Strategy
1. **Operation Distribution**: Each thread performs approximately m/thread_count operations
2. **Random Seed**: Each thread uses a unique seed based on current time + thread rank
3. **Value Range**: All operations use values between 0 and 65535
4. **Synchronization**:
   - **Mutex**: Simple but potentially bottleneck with high contention
   - **Read-Write Lock**: Allows multiple concurrent readers, better for read-heavy workloads

### Expected Performance Characteristics
- **Read-heavy workloads** (high m_member): Read-write locks should outperform mutex
- **Write-heavy workloads** (high m_insert + m_delete): Performance difference may be minimal
- **Thread scaling**: Performance should improve with more threads up to a point, then may degrade due to contention

## Sample Output
```
Configuration:
Initial elements (n): 1000
Total operations (m): 10000
Number of threads: 4
Member fraction: 0.80
Insert fraction: 0.10
Delete fraction: 0.10

=== Serial Implementation ===
Initial list size: 1000
Serial execution time: 0.123456 seconds

=== Mutex Implementation ===
Initial list size: 1000
Mutex execution time: 0.045678 seconds
Speedup over serial: 2.70x

=== Read-Write Lock Implementation ===
Initial list size: 1000
Read-write lock execution time: 0.032109 seconds
Speedup over serial: 3.84x
Speedup over mutex: 1.42x

=== Performance Summary ===
Serial time:     0.123456 seconds
Mutex time:      0.045678 seconds (2.70x speedup)
RW-Lock time:    0.032109 seconds (3.84x speedup)
```

## Notes
- The program ensures thread-safe random number generation using `rand_r()`
- Each execution uses a different random seed for reproducible but varied results
- Memory is properly managed with no memory leaks
- The linked list maintains sorted order for all implementations
