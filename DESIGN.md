# Concurrent Linked List Implementation - Design Document

## Step 1: Design Solution

### Overview
This implementation uses a thread-based approach where operations are distributed among multiple threads. Each thread performs a subset of the total m operations, ensuring load balancing and proper work distribution.

### Design Approach

#### 1. Operation Distribution Strategy
- **Total Operations**: m operations are divided among `thread_count` threads
- **Load Balancing**: Each thread gets `ops_per_thread = m / thread_count` operations
- **Remainder Handling**: If `m % thread_count != 0`, the first `remainder` threads get one extra operation
- **Operation Type Selection**: Each thread uses a random number generator with thread-specific seeds to determine operation types based on the given fractions

#### 2. Random Number Generation
- **Thread-Safe Seeding**: Each thread uses `time(NULL) + thread_rank + getpid()` as seed
- **Operation Selection**: Use `rand_r()` with thread-local seed for thread safety
- **Value Generation**: Values are generated in range [0, 2^16 - 1] = [0, 65535]
- **Operation Type**: Based on cumulative probability:
  - Member: [0, m_member)
  - Insert: [m_member, m_member + m_insert)  
  - Delete: [m_member + m_insert, 1.0]

#### 3. Three Implementation Approaches

##### a) Serial Implementation
- Single thread performs all operations sequentially
- No synchronization needed
- Baseline for performance comparison

##### b) Mutex Implementation  
- Global mutex protects the entire linked list
- All operations (Member, Insert, Delete) acquire exclusive lock
- Simple but may create bottleneck with many threads

##### c) Read-Write Lock Implementation
- Read-write lock allows concurrent reads but exclusive writes
- Member operations use read lock (concurrent access)
- Insert/Delete operations use write lock (exclusive access)
- Better performance expected when Member operations dominate

#### 4. Data Structure
```c
struct list_node_s {
    int data;
    struct list_node_s* next;
};
```
- Sorted singly linked list maintains order
- Simplifies search, insert, and delete operations
- No duplicate values allowed

#### 5. Synchronization Mechanisms

##### Mutex Implementation:
```c
pthread_mutex_lock(&list_mutex);
// Critical section - list operation
pthread_mutex_unlock(&list_mutex);
```

##### Read-Write Lock Implementation:
```c
// For Member (read operation)
pthread_rwlock_rdlock(&list_rwlock);
// Read operation
pthread_rwlock_unlock(&list_rwlock);

// For Insert/Delete (write operations)
pthread_rwlock_wrlock(&list_rwlock);
// Write operation  
pthread_rwlock_unlock(&list_rwlock);
```

#### 6. Performance Measurement
- High-resolution timing using `gettimeofday()`
- Measures only the time for m operations (excludes initialization)
- Multiple samples for statistical significance
- Calculates mean and standard deviation

### Expected Performance Characteristics

1. **Case 1 (99% Member, 0.5% Insert, 0.5% Delete)**:
   - Read-write locks should perform best due to high read concurrency
   - Mutex implementation will be bottlenecked by exclusive access
   - Serial will be slowest for multiple threads

2. **Case 2 (90% Member, 5% Insert, 5% Delete)**:
   - Read-write locks still advantageous but less than Case 1
   - More write contention reduces read-write lock benefit
   
3. **Case 3 (50% Member, 25% Insert, 25% Delete)**:
   - High write contention
   - Read-write locks may perform similarly to mutex
   - Contention overhead may reduce parallel efficiency

### Thread Safety Considerations
- Each thread uses independent random number generator with `rand_r()`
- Global list protected by appropriate synchronization
- No race conditions in operation counting or value generation
- Proper initialization and cleanup of synchronization primitives

This design ensures fair work distribution, thread safety, and meaningful performance comparisons across the three implementation approaches.
