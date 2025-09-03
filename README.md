# Concurrent Programming Lab 1: Linked List Implementation

## Overview
This project implements a linked list data structure using three different approaches:
1. **Serial Implementation** - Single-threaded execution
2. **Mutex Implementation** - Multi-threaded with one mutex protecting the entire list
3. **Read-Write Lock Implementation** - Multi-threaded with read-write locks for better performance

## Features
- Supports Member(), Insert(), and Delete() operations
- Handles values from 0 to 2^16 - 1 (65535)
- Maintains a sorted linked list
- Prevents duplicate insertions
- Measures and compares performance between implementations

## Compilation

### Using Makefile (Recommended)
```bash
make
```

### Manual Compilation
```bash
gcc -Wall -Wextra -std=c99 -pthread -O2 -o lab1 lab1.c
```

## Usage
```bash
./lab1 <n> <m> <thread_count> <m_member> <m_insert>
```

### Parameters:
- **n**: Initial number of elements to populate the list
- **m**: Total number of operations to perform
- **thread_count**: Number of threads to use for parallel implementations
- **m_member**: Fraction of operations that should be Member() calls (0.0 to 1.0)
- **m_insert**: Fraction of operations that should be Insert() calls (0.0 to 1.0)
- **m_delete**: Automatically calculated as (1.0 - m_member - m_insert)

### Example:
```bash
./lab1 1000 10000 4 0.8 0.1
```
This runs with:
- 1000 initial elements
- 10000 total operations
- 4 threads
- 80% Member operations
- 10% Insert operations  
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
