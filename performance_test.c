#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>

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
void free_list(struct list_node_s** head_pp);
double get_time_diff(struct timeval start, struct timeval end);
void populate_list(int n);
double run_test(int implementation, int threads);

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
    
    unsigned int seed = time(NULL) + my_rank + getpid();
    
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
    
    unsigned int seed = time(NULL) + my_rank + getpid();
    
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
    
    unsigned int seed = time(NULL) + my_rank + getpid();
    
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
    int* values = malloc(n * sizeof(int));
    unsigned int seed = time(NULL) + getpid();
    
    // Generate n unique random values
    for (int i = 0; i < n; i++) {
        int value;
        int unique;
        do {
            unique = 1;
            value = rand_r(&seed) % 65536; // 0 to 2^16 - 1
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

// Function to run a single test
double run_test(int implementation, int threads) {
    pthread_t* thread_handles = malloc(threads * sizeof(pthread_t));
    struct timeval start_time, end_time;
    thread_count = threads;
    
    // Initialize the list
    head_p = NULL;
    populate_list(n);
    
    // Initialize synchronization primitives based on implementation
    if (implementation == 1) { // Mutex
        pthread_mutex_init(&list_mutex, NULL);
    } else if (implementation == 2) { // Read-write lock
        pthread_rwlock_init(&list_rwlock, NULL);
    }
    
    gettimeofday(&start_time, NULL);
    
    if (implementation == 0) { // Serial
        thread_function_serial((void*)0);
    } else if (implementation == 1) { // Mutex
        for (long thread = 0; thread < threads; thread++) {
            pthread_create(&thread_handles[thread], NULL, thread_function_mutex, (void*)thread);
        }
        for (int thread = 0; thread < threads; thread++) {
            pthread_join(thread_handles[thread], NULL);
        }
    } else if (implementation == 2) { // Read-write lock
        for (long thread = 0; thread < threads; thread++) {
            pthread_create(&thread_handles[thread], NULL, thread_function_rwlock, (void*)thread);
        }
        for (int thread = 0; thread < threads; thread++) {
            pthread_join(thread_handles[thread], NULL);
        }
    }
    
    gettimeofday(&end_time, NULL);
    
    double execution_time = get_time_diff(start_time, end_time);
    
    // Clean up
    if (implementation == 1) {
        pthread_mutex_destroy(&list_mutex);
    } else if (implementation == 2) {
        pthread_rwlock_destroy(&list_rwlock);
    }
    
    free_list(&head_p);
    free(thread_handles);
    
    return execution_time;
}

// Function to calculate statistics
void calculate_stats(double* times, int samples, double* mean, double* std_dev) {
    *mean = 0.0;
    for (int i = 0; i < samples; i++) {
        *mean += times[i];
    }
    *mean /= samples;
    
    double variance = 0.0;
    for (int i = 0; i < samples; i++) {
        variance += (times[i] - *mean) * (times[i] - *mean);
    }
    variance /= (samples - 1);
    *std_dev = sqrt(variance);
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        printf("Usage: %s <case_number> <n> <m> <samples>\n", argv[0]);
        printf("case_number: 1, 2, or 3\n");
        printf("n: initial elements\n");
        printf("m: total operations\n");
        printf("samples: number of test runs\n");
        return 1;
    }
    
    int case_number = atoi(argv[1]);
    n = atoi(argv[2]);
    m = atoi(argv[3]);
    int samples = atoi(argv[4]);
    
    // Set fractions based on case number
    switch (case_number) {
        case 1:
            m_member = 0.99;
            m_insert = 0.005;
            m_delete = 0.005;
            break;
        case 2:
            m_member = 0.90;
            m_insert = 0.05;
            m_delete = 0.05;
            break;
        case 3:
            m_member = 0.50;
            m_insert = 0.25;
            m_delete = 0.25;
            break;
        default:
            printf("Invalid case number. Use 1, 2, or 3.\n");
            return 1;
    }
    
    printf("Performance Testing - Case %d\n", case_number);
    printf("n = %d, m = %d, samples = %d\n", n, m, samples);
    printf("Member: %.1f%%, Insert: %.1f%%, Delete: %.1f%%\n\n", 
           m_member * 100, m_insert * 100, m_delete * 100);
    
    int thread_counts[] = {1, 2, 4, 8};
    char* impl_names[] = {"Serial", "Mutex", "Read-Write Lock"};
    
    printf("Implementation\tThreads\tAverage (s)\tStd Dev (s)\n");
    printf("===============================================\n");
    
    for (int impl = 0; impl < 3; impl++) {
        for (int t_idx = 0; t_idx < 4; t_idx++) {
            int threads = thread_counts[t_idx];
            
            // For serial implementation, only use 1 thread
            if (impl == 0 && threads > 1) {
                printf("%s\t\t%d\t-\t\t-\n", impl_names[impl], threads);
                continue;
            }
            
            double* times = malloc(samples * sizeof(double));
            
            printf("Running %s with %d thread(s)... ", impl_names[impl], threads);
            fflush(stdout);
            
            for (int i = 0; i < samples; i++) {
                times[i] = run_test(impl, threads);
                if ((i + 1) % 10 == 0) {
                    printf("%d ", i + 1);
                    fflush(stdout);
                }
            }
            
            double mean, std_dev;
            calculate_stats(times, samples, &mean, &std_dev);
            
            printf("\n%s\t\t%d\t%.6f\t%.6f\n", impl_names[impl], threads, mean, std_dev);
            
            free(times);
        }
    }
    
    return 0;
}
