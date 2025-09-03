#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

// Linked list node structure
struct list_node_s {
    int data;
    struct list_node_s* next;
};

// Global variables
struct list_node_s* head_p = NULL;
pthread_mutex_t list_mutex;
pthread_rwlock_t list_rwlock;

int n; // Initial number of elements
int m; // Total number of operations
int thread_count;
double m_member, m_insert, m_delete; // Fractions of operations

// Function prototypes
int Member(int value, struct list_node_s* head_p);
int Insert(int value, struct list_node_s** head_pp);
int Delete(int value, struct list_node_s** head_pp);
int Member_mutex(int value);
int Insert_mutex(int value);
int Delete_mutex(int value);
int Member_rwlock(int value);
int Insert_rwlock(int value);
int Delete_rwlock(int value);
void* thread_function_serial(void* rank);
void* thread_function_mutex(void* rank);
void* thread_function_rwlock(void* rank);
void print_list(struct list_node_s* head_p);
void free_list(struct list_node_s** head_pp);
double get_time_diff(struct timeval start, struct timeval end);

// Serial implementation of Member function
int Member(int value, struct list_node_s* head_p) {
    struct list_node_s* curr_p = head_p;
    
    while (curr_p != NULL && curr_p->data < value)
        curr_p = curr_p->next;
    
    if (curr_p == NULL || curr_p->data > value) {
        return 0;
    } else {
        return 1;
    }
}

// Serial implementation of Insert function
int Insert(int value, struct list_node_s** head_pp) {
    struct list_node_s* curr_p = *head_pp;
    struct list_node_s* pred_p = NULL;
    struct list_node_s* temp_p;
    
    while (curr_p != NULL && curr_p->data < value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }
    
    if (curr_p == NULL || curr_p->data > value) {
        temp_p = malloc(sizeof(struct list_node_s));
        temp_p->data = value;
        temp_p->next = curr_p;
        if (pred_p == NULL) // New first node
            *head_pp = temp_p;
        else
            pred_p->next = temp_p;
        return 1;
    } else { // Value already in list
        return 0;
    }
}

// Serial implementation of Delete function
int Delete(int value, struct list_node_s** head_pp) {
    struct list_node_s* curr_p = *head_pp;
    struct list_node_s* pred_p = NULL;
    
    while (curr_p != NULL && curr_p->data < value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }
    
    if (curr_p != NULL && curr_p->data == value) {
        if (pred_p == NULL) { // Deleting first node in list
            *head_pp = curr_p->next;
            free(curr_p);
        } else {
            pred_p->next = curr_p->next;
            free(curr_p);
        }
        return 1;
    } else { // Value isn't in list
        return 0;
    }
}

// Mutex-based implementations
int Member_mutex(int value) {
    int result;
    pthread_mutex_lock(&list_mutex);
    result = Member(value, head_p);
    pthread_mutex_unlock(&list_mutex);
    return result;
}

int Insert_mutex(int value) {
    int result;
    pthread_mutex_lock(&list_mutex);
    result = Insert(value, &head_p);
    pthread_mutex_unlock(&list_mutex);
    return result;
}

int Delete_mutex(int value) {
    int result;
    pthread_mutex_lock(&list_mutex);
    result = Delete(value, &head_p);
    pthread_mutex_unlock(&list_mutex);
    return result;
}

// Read-write lock implementations
int Member_rwlock(int value) {
    int result;
    pthread_rwlock_rdlock(&list_rwlock);
    result = Member(value, head_p);
    pthread_rwlock_unlock(&list_rwlock);
    return result;
}

int Insert_rwlock(int value) {
    int result;
    pthread_rwlock_wrlock(&list_rwlock);
    result = Insert(value, &head_p);
    pthread_rwlock_unlock(&list_rwlock);
    return result;
}

int Delete_rwlock(int value) {
    int result;
    pthread_rwlock_wrlock(&list_rwlock);
    result = Delete(value, &head_p);
    pthread_rwlock_unlock(&list_rwlock);
    return result;
}

// Thread function for serial execution (single thread)
void* thread_function_serial(void* rank) {
    long my_rank = (long) rank;
    int ops_per_thread = m / thread_count;
    int remainder = m % thread_count;
    int my_ops = ops_per_thread + (my_rank < remainder ? 1 : 0);
    
    unsigned int seed = time(NULL) + my_rank;
    
    for (int i = 0; i < my_ops; i++) {
        double operation = (double) rand_r(&seed) / RAND_MAX;
        int value = rand_r(&seed) % 65536; // 0 to 2^16 - 1
        
        if (operation < m_member) {
            Member(value, head_p);
        } else if (operation < m_member + m_insert) {
            Insert(value, &head_p);
        } else {
            Delete(value, &head_p);
        }
    }
    
    return NULL;
}

// Thread function for mutex-based execution
void* thread_function_mutex(void* rank) {
    long my_rank = (long) rank;
    int ops_per_thread = m / thread_count;
    int remainder = m % thread_count;
    int my_ops = ops_per_thread + (my_rank < remainder ? 1 : 0);
    
    unsigned int seed = time(NULL) + my_rank;
    
    for (int i = 0; i < my_ops; i++) {
        double operation = (double) rand_r(&seed) / RAND_MAX;
        int value = rand_r(&seed) % 65536; // 0 to 2^16 - 1
        
        if (operation < m_member) {
            Member_mutex(value);
        } else if (operation < m_member + m_insert) {
            Insert_mutex(value);
        } else {
            Delete_mutex(value);
        }
    }
    
    return NULL;
}

// Thread function for read-write lock execution
void* thread_function_rwlock(void* rank) {
    long my_rank = (long) rank;
    int ops_per_thread = m / thread_count;
    int remainder = m % thread_count;
    int my_ops = ops_per_thread + (my_rank < remainder ? 1 : 0);
    
    unsigned int seed = time(NULL) + my_rank;
    
    for (int i = 0; i < my_ops; i++) {
        double operation = (double) rand_r(&seed) / RAND_MAX;
        int value = rand_r(&seed) % 65536; // 0 to 2^16 - 1
        
        if (operation < m_member) {
            Member_rwlock(value);
        } else if (operation < m_member + m_insert) {
            Insert_rwlock(value);
        } else {
            Delete_rwlock(value);
        }
    }
    
    return NULL;
}

// Utility function to print the list
void print_list(struct list_node_s* head_p) {
    struct list_node_s* curr_p = head_p;
    printf("List: ");
    while (curr_p != NULL) {
        printf("%d ", curr_p->data);
        curr_p = curr_p->next;
    }
    printf("\n");
}

// Utility function to free the list
void free_list(struct list_node_s** head_pp) {
    struct list_node_s* curr_p = *head_pp;
    struct list_node_s* temp_p;
    
    while (curr_p != NULL) {
        temp_p = curr_p;
        curr_p = curr_p->next;
        free(temp_p);
    }
    *head_pp = NULL;
}

// Utility function to calculate time difference
double get_time_diff(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
}

// Function to populate the list with n unique random values
void populate_list(int n) {
    srand(time(NULL));
    int* values = malloc(n * sizeof(int));
    
    // Generate n unique random values
    for (int i = 0; i < n; i++) {
        int value;
        int unique;
        do {
            unique = 1;
            value = rand() % 65536; // 0 to 2^16 - 1
            for (int j = 0; j < i; j++) {
                if (values[j] == value) {
                    unique = 0;
                    break;
                }
            }
        } while (!unique);
        values[i] = value;
    }
    
    // Insert values into the list
    for (int i = 0; i < n; i++) {
        Insert(values[i], &head_p);
    }
    
    free(values);
}

int main(int argc, char* argv[]) {
    if (argc != 6) {
        printf("Usage: %s <n> <m> <thread_count> <m_member> <m_insert>\n", argv[0]);
        printf("Note: m_delete = 1.0 - m_member - m_insert\n");
        return 1;
    }
    
    // Parse command line arguments
    n = atoi(argv[1]);
    m = atoi(argv[2]);
    thread_count = atoi(argv[3]);
    m_member = atof(argv[4]);
    m_insert = atof(argv[5]);
    m_delete = 1.0 - m_member - m_insert;
    
    if (m_delete < 0) {
        printf("Error: m_member + m_insert cannot exceed 1.0\n");
        return 1;
    }
    
    printf("Configuration:\n");
    printf("Initial elements (n): %d\n", n);
    printf("Total operations (m): %d\n", m);
    printf("Number of threads: %d\n", thread_count);
    printf("Member fraction: %.2f\n", m_member);
    printf("Insert fraction: %.2f\n", m_insert);
    printf("Delete fraction: %.2f\n", m_delete);
    printf("\n");
    
    pthread_t* thread_handles = malloc(thread_count * sizeof(pthread_t));
    struct timeval start_time, end_time;
    
    // Test 1: Serial Implementation
    printf("=== Serial Implementation ===\n");
    head_p = NULL;
    populate_list(n);
    printf("Initial list size: %d\n", n);
    
    gettimeofday(&start_time, NULL);
    for (long thread = 0; thread < 1; thread++) {
        thread_function_serial((void*)thread);
    }
    gettimeofday(&end_time, NULL);
    
    double serial_time = get_time_diff(start_time, end_time);
    printf("Serial execution time: %.6f seconds\n", serial_time);
    free_list(&head_p);
    printf("\n");
    
    // Test 2: Mutex Implementation
    printf("=== Mutex Implementation ===\n");
    head_p = NULL;
    populate_list(n);
    pthread_mutex_init(&list_mutex, NULL);
    printf("Initial list size: %d\n", n);
    
    gettimeofday(&start_time, NULL);
    for (long thread = 0; thread < thread_count; thread++) {
        pthread_create(&thread_handles[thread], NULL, thread_function_mutex, (void*)thread);
    }
    for (int thread = 0; thread < thread_count; thread++) {
        pthread_join(thread_handles[thread], NULL);
    }
    gettimeofday(&end_time, NULL);
    
    double mutex_time = get_time_diff(start_time, end_time);
    printf("Mutex execution time: %.6f seconds\n", mutex_time);
    printf("Speedup over serial: %.2fx\n", serial_time / mutex_time);
    pthread_mutex_destroy(&list_mutex);
    free_list(&head_p);
    printf("\n");
    
    // Test 3: Read-Write Lock Implementation
    printf("=== Read-Write Lock Implementation ===\n");
    head_p = NULL;
    populate_list(n);
    pthread_rwlock_init(&list_rwlock, NULL);
    printf("Initial list size: %d\n", n);
    
    gettimeofday(&start_time, NULL);
    for (long thread = 0; thread < thread_count; thread++) {
        pthread_create(&thread_handles[thread], NULL, thread_function_rwlock, (void*)thread);
    }
    for (int thread = 0; thread < thread_count; thread++) {
        pthread_join(thread_handles[thread], NULL);
    }
    gettimeofday(&end_time, NULL);
    
    double rwlock_time = get_time_diff(start_time, end_time);
    printf("Read-write lock execution time: %.6f seconds\n", rwlock_time);
    printf("Speedup over serial: %.2fx\n", serial_time / rwlock_time);
    printf("Speedup over mutex: %.2fx\n", mutex_time / rwlock_time);
    pthread_rwlock_destroy(&list_rwlock);
    free_list(&head_p);
    printf("\n");
    
    // Performance Summary
    printf("=== Performance Summary ===\n");
    printf("Serial time:     %.6f seconds\n", serial_time);
    printf("Mutex time:      %.6f seconds (%.2fx speedup)\n", mutex_time, serial_time / mutex_time);
    printf("RW-Lock time:    %.6f seconds (%.2fx speedup)\n", rwlock_time, serial_time / rwlock_time);
    
    free(thread_handles);
    return 0;
}
