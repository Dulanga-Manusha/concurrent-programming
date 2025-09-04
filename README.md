# Concurrent Linked List

Concurrent linked list implementation with three synchronization approaches: serial, mutex, and read-write locks.

## How to Run

### Build
```bash
make
```

### Interactive Demo
```bash
./lab1 <initial_size> <operations> <threads> <member_ratio> <insert_ratio>
```

Example:
```bash
./lab1 1000 10000 4 0.8 0.1
```

### Performance Tests
```bash
make performance
```

### Individual Performance Test
```bash
./performance_test <case> <threads> [samples]
```

Example:
```bash
./performance_test 1 4 50
```

## Test Cases

- **Case 1**: 99% Member, 0.5% Insert, 0.5% Delete
- **Case 2**: 90% Member, 5% Insert, 5% Delete  
- **Case 3**: 50% Member, 25% Insert, 25% Delete

## Clean Up
```bash
make clean
```
