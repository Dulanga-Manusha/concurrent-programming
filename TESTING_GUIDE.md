# Performance Testing Guide

## Quick Testing (for development/debugging)
```bash
# Compile everything
make

# Run a quick test with small sample size
make quick-test

# Test a specific case manually
./performance_test 1 1000 10000 10  # Case 1 with 10 samples
```

## Full Performance Testing (for final results)
```bash
# Run comprehensive performance tests with proper statistical sampling
make performance

# This will:
# 1. Show system information
# 2. Run all 3 cases with 50 samples each
# 3. Generate formatted tables for your report
# 4. Save detailed results to case1_results.txt, case2_results.txt, case3_results.txt
```

## Manual Performance Testing
```bash
# Compile the performance test program
make performance_test

# Run specific cases manually:
./performance_test 1 1000 10000 50  # Case 1: 99% Member, 0.5% Insert, 0.5% Delete
./performance_test 2 1000 10000 50  # Case 2: 90% Member, 5% Insert, 5% Delete  
./performance_test 3 1000 10000 50  # Case 3: 50% Member, 25% Insert, 25% Delete
```

## Understanding the Output

### Performance Test Output Format:
```
Implementation  Threads Average (s)     Std Dev (s)
===============================================
Serial          1       0.008050        0.001758
Mutex           1       0.007513        0.000422
Mutex           2       0.017539        0.001780
Read-Write Lock 1       0.007776        0.000651
Read-Write Lock 2       0.006370        0.000290
```

### Statistical Significance
- **Samples**: 50 samples per configuration for statistical reliability
- **Confidence Level**: 95% confidence level
- **Accuracy**: ±5% accuracy as required
- **Standard Deviation**: Shows variability in measurements

## Expected Performance Patterns

### Case 1 (99% Member operations):
- **Read-Write Locks**: Should perform best due to concurrent reads
- **Mutex**: Will be bottlenecked by exclusive access
- **Speedup**: More pronounced with higher thread counts

### Case 2 (90% Member operations):
- **Read-Write Locks**: Still advantageous but less than Case 1
- **Mutex**: Shows more contention
- **Balanced**: Moderate performance differences

### Case 3 (50% Member operations):
- **High Contention**: Write operations dominate
- **Similar Performance**: Mutex and Read-Write locks may perform similarly
- **Overhead**: Synchronization overhead more apparent

## System Requirements
- **CPU**: At least 4 physical cores (not hyperthreaded)
- **Memory**: Sufficient RAM for concurrent execution
- **OS**: Linux with pthread support
- **Compiler**: GCC with C99 support

## Troubleshooting

### Compilation Issues:
```bash
# If you get pthread errors, ensure you have:
sudo apt-get install build-essential
sudo apt-get install libc6-dev
```

### Performance Issues:
```bash
# Check CPU frequency scaling
cat /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor

# Disable CPU frequency scaling for consistent results (optional)
sudo cpupower frequency-set --governor performance
```

### Memory Issues:
```bash
# Check available memory
free -h

# Monitor memory usage during tests
top -p $(pgrep performance_test)
```

## Files Generated
- `case1_results.txt`: Detailed results for Case 1
- `case2_results.txt`: Detailed results for Case 2  
- `case3_results.txt`: Detailed results for Case 3
- Formatted tables printed to console for direct use in report
