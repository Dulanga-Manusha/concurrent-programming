# Detailed System Specifications for Performance Testing

## Complete Hardware Specifications

### CPU Detailed Information
```
Architecture:             x86_64
CPU op-mode(s):          32-bit, 64-bit
Address sizes:           39 bits physical, 48 bits virtual
Byte Order:              Little Endian
CPU(s):                  8
On-line CPU(s) list:     0-7
Vendor ID:               GenuineIntel
Model name:              11th Gen Intel(R) Core(TM) i5-1135G7 @ 2.40GHz
CPU family:              6
Model:                   140
Thread(s) per core:      2
Core(s) per socket:      4
Socket(s):               1
Stepping:                1
BogoMIPS:                4838.40
```

### CPU Features and Instructions
```
Flags: fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 
clflush mmx fxsr sse sse2 ss ht syscall nx pdpe1gb rdtscp lm constant_tsc 
arch_perfmon rep_good nopl xtopology tsc_reliable nonstop_tsc cpuid 
tsc_known_freq pni pclmulqdq vmx ssse3 fma cx16 pdcm pcid sse4_1 sse4_2 
x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand hypervisor 
lahf_lm abm 3dnowprefetch ssbd ibrs ibpb stibp ibrs_enhanced tpr_shadow ept 
vpid ept_ad fsgsbase tsc_adjust bmi1 avx2 smep bmi2 erms invpcid avx512f 
avx512dq rdseed adx smap avx512ifma clflushopt clwb avx512cd sha_ni avx512bw 
avx512vl xsaveopt xsavec xgetbv1 xsaves vnmi avx512vbmi umip avx512_vbmi2 
gfni vaes vpclmulqdq avx512_vnni avx512_bitalg avx512_vpopcntdq rdpid movdiri 
movdir64b fsrm avx512_vp2intersect md_clear flush_l1d arch_capabilities
```

### Cache Hierarchy
```
L1d:                     192 KiB (4 instances) - 48 KiB per core
L1i:                     128 KiB (4 instances) - 32 KiB per core  
L2:                      5 MiB (4 instances) - 1.25 MiB per core
L3:                      8 MiB (1 instance) - shared across all cores
```

### Memory Information
```
Total Physical Memory:   3,854,560 KB (3.7 GiB)
Available Memory:        2,311,420 KB (2.3 GiB)
Buffers:                 1,088 KB
Cached:                  277,268 KB
Swap Total:              1,048,576 KB (1.0 GiB)
Swap Used:               0 KB
```

### Virtualization Details
```
Virtualization:          VT-x
Hypervisor vendor:       Microsoft
Virtualization type:     full
Platform:                WSL2 (Windows Subsystem for Linux 2)
```

## Software Environment

### Operating System
```
Distribution:            Ubuntu 22.04.2 LTS (Jammy Jellyfish)
Kernel:                  Linux 6.6.87.2-microsoft-standard-WSL2
Kernel Type:             SMP PREEMPT_DYNAMIC
Architecture:            x86_64
```

### Compiler and Build Tools
```
Compiler:                GCC (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0
C Standard:              C99
Optimization:            -O2
Warning Flags:           -Wall -Wextra
Threading Support:       -pthread
Math Library:            -lm
```

### Library Versions
```
pthread Library:         libpthread.so.0 (glibc implementation)
Math Library:           libm.so.6 (GNU Math Library)
C Runtime:              glibc 2.35
GNU Binutils:           2.37
```

## Performance Testing Configuration

### Timing Methodology
```
Timer Function:          gettimeofday()
Resolution:              Microsecond (1e-6 seconds)
Measurement Scope:       Operation execution only (excludes initialization)
Statistical Method:      Mean and standard deviation calculation
```

### Experimental Design
```
Sample Size:             50 independent runs per configuration
Random Seeds:            time(NULL) + thread_rank + getpid()
Seed Strategy:           Unique per thread per execution
Confidence Level:        95%
Target Accuracy:         ±5%
```

### Test Parameters
```
Initial List Size (n):   1,000 elements
Operations per Test (m): 10,000 operations
Thread Counts:           1, 2, 4, 8
Value Range:             0 to 65,535 (2^16 - 1)
```

### Workload Characteristics
```
Case 1 - Read Heavy:     99% Member, 0.5% Insert, 0.5% Delete
Case 2 - Moderate Mix:   90% Member, 5% Insert, 5% Delete  
Case 3 - Write Heavy:    50% Member, 25% Insert, 25% Delete
```

## System State During Testing

### System Load
```
Load Average:            0.10 - 0.33 (low load)
Active Processes:        ~70 total system processes
CPU Utilization:         Dedicated to test processes during measurement
Memory Pressure:         None (2.3 GiB available)
```

### Performance Optimizations
```
CPU Frequency Scaling:   Not available (WSL2 limitation)
Background Services:     Minimal Ubuntu services only
Swap Usage:              0 KB (no swapping during tests)
File System:             WSL2 virtual file system
```

### Security and Mitigations
```
Spectre v1:              Mitigation enabled
Spectre v2:              Enhanced IBRS enabled  
Meltdown:                Not affected (11th gen Intel)
L1TF:                    Not affected
MDS:                     Not affected
```

This comprehensive specification ensures reproducibility and provides context for performance result interpretation.
