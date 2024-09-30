//Alex Nguyen

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_CACHE_SIZE 500000

// Struct to represent a cache entry
typedef struct {
    int number;
    int steps;
    int frequency;
    int timestamp;
} CacheEntry;

CacheEntry cache[MAX_CACHE_SIZE];
int        cache_size;
int        cache_policy;
int        current_time = 0;

int total_misses = 0;
int total_evictions = 0;
int hits = 0;

// Function to calculate Collatz steps
int collatz_steps(int n) {
    int steps = 0;
    while (n != 1) {
        if (n % 2 == 0) {
            n /= 2;
        } else {
            n = 3 * n + 1;
        }
        steps++;
    }
    return steps;
}

// Cache management
int cache_hit(int number) {
    for (int i = 0; i < cache_size; i++) {
        if (cache[i].number == number) {
            cache[i].frequency++;

            cache[i].timestamp = current_time++;

            return i;
        }
    }
    total_misses++;
    return -1;
}

// LRU
int find_eviction_index() {
    int index = 0;

    if (cache_policy == 0) {
        int oldest = cache[0].timestamp;

        for (int i = 1; i < cache_size; i++) {
            if (cache[i].timestamp < oldest) {
                oldest = cache[i].timestamp;
                index = i;
            }
        }
        // LFU
    } else if (cache_policy == 1) {
        int least_frequent = cache[0].frequency;

        for (int i = 1; i < cache_size; i++) {
            if (cache[i].frequency < least_frequent) {
                least_frequent = cache[i].frequency;
                index = i;
            }
        }
        // FIFO
    } else if (cache_policy == 2) {
        return 0;
        // MRU
    } else if (cache_policy == 3) {
        int newest = cache[0].timestamp;

        for (int i = 1; i < cache_size; i++) {
            if (cache[i].timestamp > newest) {
                newest = cache[i].timestamp;
                index = i;
            }
        }
        // Random
    } else if (cache_policy == 4) {
        index = rand() % cache_size;
    }

    return index;
}

void add_to_cache(int number, int steps) {
    if (cache_size < MAX_CACHE_SIZE) {
        cache[cache_size].number = number;
        cache[cache_size].steps = steps;
        cache[cache_size].frequency = 1;
        cache[cache_size].timestamp = current_time++;
        cache_size++;
    } else {
        int eviction_index = find_eviction_index();
        total_evictions++;
        cache[eviction_index].number = number;
        cache[eviction_index].steps = steps;
        cache[eviction_index].frequency = 1;
        cache[eviction_index].timestamp = current_time++;
    }
}

void run_collatz(int N, int min_val, int max_val, int cache_type, FILE *fp) {
    hits = 0;
    total_misses = 0;
    total_evictions = 0;
    cache_size = 0;
    cache_policy = cache_type;

    fprintf(fp, "Cache_Type,Number,Steps,Cache_Hit\n");

    srand(time(NULL));

    for (int i = 0; i < N; i++) {
        int rand_num = rand() % (max_val - min_val + 1) + min_val;
        int steps;
        int cache_index = cache_hit(rand_num);

        if (cache_index != -1) {
            steps = cache[cache_index].steps;
            hits++;
            fprintf(fp, "%d,%d,%d,1\n", cache_type, rand_num, steps);
        } else {
            steps = collatz_steps(rand_num);
            add_to_cache(rand_num, steps);
            fprintf(fp, "%d,%d,%d,0\n", cache_type, rand_num, steps);
        }
    }

    // Output cache hit percentage
    printf("Cache type %d: Cache hit percentage: %.2f%%\n", cache_type,
           (double)hits / N * 100);
    printf("Cache type %d: Total cache misses: %d\n", cache_type, total_misses);
    printf("Cache type %d: Total evictions: %d\n", cache_type, total_evictions);
}

int main(int argc, char *argv[]) {
    int N = atoi(argv[1]);
    int MIN = atoi(argv[2]);
    int MAX = atoi(argv[3]);

    FILE *fp = fopen("collatz_results.csv", "w");

    for (int cache_type = 0; cache_type <= 1; cache_type++) {
        run_collatz(N, MIN, MAX, cache_type, fp);
    }

    fclose(fp);
    printf("Results saved to collatz_results.csv\n");

    return 0;
}
