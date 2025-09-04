# Concurrent Linked List - Performance Comparison

> A comprehensive implementation comparing serial, mutex-based, and read-write lock synchronization approaches for concurrent linked list operations.

## 🚀 Quick Start for New Users

### Prerequisites
- **Linux/Unix environment** (tested on Ubuntu 22.04)
- **GCC compiler** with pthread support
- **Make** build tool

### Get Running in 3 Steps

1. **Clone and build:**
   ```bash
   git clone https://github.com/Dulanga-Manusha/concurrent-programming.git
   cd concurrent-programming
   make
   ```

2. **Try the interactive demo:**
   ```bash
   ./lab1 1000 10000 4 0.8 0.1
   ```

3. **Run performance tests:**
   ```bash
   make performance
   ```

That's it! 🎉

## 📋 What This Project Does

This project implements and compares **three different approaches** to handle concurrent access to a sorted linked list:

| Approach | Description | Best For |
|----------|-------------|----------|
| **Serial** | Single-threaded baseline | Comparison baseline |
| **Mutex** | One lock for entire list | Simple thread safety |
| **Read-Write Lock** | Separate read/write locks | Read-heavy workloads |

### Supported Operations
- **Member(x)** - Check if value x exists in the list
- **Insert(x)** - Add value x to the list (if not exists)
- **Delete(x)** - Remove value x from the list (if exists)

## 🛠️ Available Programs

### 1. Interactive Demo (`lab1`)
Test different configurations manually:
```bash
./lab1 <initial_size> <operations> <threads> <member_ratio> <insert_ratio>
```

**Example:**
```bash
./lab1 1000 10000 4 0.8 0.1
# 1000 initial elements, 10000 operations, 4 threads
# 80% Member, 10% Insert, 10% Delete operations
```

### 2. Performance Testing (`performance_test`)
Automated benchmarking with statistical analysis:
```bash
./performance_test <case_number> <thread_count> [sample_size]
```

**Examples:**
```bash
./performance_test 1 4        # Case 1 with 4 threads (default 50 samples)
./performance_test 2 8 100    # Case 2 with 8 threads (100 samples)
```

### 3. Automated Test Suite
Run comprehensive performance tests:
```bash
# Full test suite (recommended)
make performance

# Quick test for development
make quick-test
```

## 📊 Test Cases Available

### Case 1: Read-Heavy Workload
- **Operations**: 99% Member, 0.5% Insert, 0.5% Delete
- **Best for**: Testing read-write lock advantages
- **Expected**: Read-write locks should outperform mutex

### Case 2: Balanced Workload  
- **Operations**: 90% Member, 5% Insert, 5% Delete
- **Best for**: General performance comparison
- **Expected**: Mixed results across synchronization methods

### Case 3: Write-Heavy Workload
- **Operations**: 50% Member, 25% Insert, 25% Delete
- **Best for**: Testing contention handling
- **Expected**: Higher contention, varied performance

## 🔧 Build Options

```bash
# Build everything
make all

# Individual targets
make lab1              # Interactive demo
make performance_test  # Performance testing

# Utility targets
make clean            # Remove built files
make performance      # Run full test suite
make quick-test       # Run quick tests
```

## 📁 Project Files

| File | Purpose |
|------|---------|
| `lab1.c` | Main implementation with interactive demo |
| `performance_test.c` | Automated performance testing framework |
| `run_performance_tests.sh` | Comprehensive test automation script |
| `Makefile` | Build system with all necessary targets |
| `README.md` | This guide |

## 🧪 Sample Usage Session

```bash
# 1. Build the project
make clean && make

# 2. Quick interactive test
./lab1 100 1000 2 0.8 0.1

# 3. Run automated performance tests
./performance_test 1 4 10

# 4. Full benchmark suite
make performance

# 5. Check results
ls -la *.txt
```

## ⚙️ Parameters Explained

### Interactive Demo Parameters
- `initial_size` (n): Number of elements to pre-populate (e.g., 1000)
- `operations` (m): Total operations to perform (e.g., 10000)  
- `threads`: Number of worker threads (e.g., 2, 4, 8)
- `member_ratio`: Proportion of Member operations (0.0-1.0)
- `insert_ratio`: Proportion of Insert operations (0.0-1.0)
- Delete ratio is calculated as: `1.0 - member_ratio - insert_ratio`

### Performance Test Parameters
- `case_number`: Which test case to run (1, 2, or 3)
- `thread_count`: Number of threads to use
- `sample_size`: Number of test runs for statistical accuracy (optional)

## 🎯 Expected Results

- **Serial**: Baseline performance, single-threaded
- **Mutex**: Simple thread safety, potential bottleneck
- **Read-Write Locks**: Optimized for read-heavy workloads

Performance will vary based on:
- **Workload mix** (read vs write ratio)
- **Thread count** (parallelization vs contention)
- **System hardware** (CPU cores, memory architecture)

## 🐛 Troubleshooting

### Compilation Issues
```bash
# Ensure pthread library is available
gcc --version
ldconfig -p | grep pthread
```

### Permission Issues
```bash
# Make test script executable
chmod +x run_performance_tests.sh
```

### Performance Issues
- Test on different thread counts (1, 2, 4, 8)
- Verify system has multiple CPU cores
- Check for background processes affecting performance

## 🎓 Academic Context

This implementation demonstrates:
- **Thread synchronization** techniques
- **Performance measurement** with statistical rigor
- **Concurrent data structures** design
- **Lock granularity** trade-offs
- **Read-write optimization** strategies

Perfect for understanding concurrent programming concepts and performance analysis!
