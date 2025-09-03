CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pthread -O2
TARGET = lab1
SOURCE = lab1.c

# Default target
all: $(TARGET)

# Compile the program
$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

# Clean compiled files
clean:
	rm -f $(TARGET) $(TARGET).exe

# Test with different configurations
test: $(TARGET)
	@echo "Testing with 1000 initial elements, 10000 operations, 4 threads"
	@echo "Configuration: 80% Member, 10% Insert, 10% Delete"
	./$(TARGET) 1000 10000 4 0.8 0.1
	@echo ""
	@echo "Configuration: 90% Member, 5% Insert, 5% Delete"
	./$(TARGET) 1000 10000 4 0.9 0.05
	@echo ""
	@echo "Configuration: 70% Member, 20% Insert, 10% Delete"
	./$(TARGET) 1000 10000 4 0.7 0.2

# Test with different thread counts
test-threads: $(TARGET)
	@echo "Testing with different thread counts (1000 elements, 10000 ops, 80% Member, 10% Insert, 10% Delete)"
	@echo "1 thread:"
	./$(TARGET) 1000 10000 1 0.8 0.1
	@echo ""
	@echo "2 threads:"
	./$(TARGET) 1000 10000 2 0.8 0.1
	@echo ""
	@echo "4 threads:"
	./$(TARGET) 1000 10000 4 0.8 0.1
	@echo ""
	@echo "8 threads:"
	./$(TARGET) 1000 10000 8 0.8 0.1

.PHONY: all clean test test-threads
