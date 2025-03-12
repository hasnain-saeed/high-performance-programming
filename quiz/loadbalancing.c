void *check_primes(void *arg) {
    int thread_id = *(int *)arg;
    for (int num = 2 + thread_id; num <= M; num += 8) {
        if (is_prime(num)) {
            add_prime(num);  // safely synchronized prime storage
        }
    }
    return NULL;
}

pthread_t threads[8];
int thread_ids[8];

for (int i = 0; i < 8; i++) {
    thread_ids[i] = i;
    pthread_create(&threads[i], NULL, check_primes, &thread_ids[i]);
}

// Join all threads
for (int i = 0; i < 8; i++) {
    pthread_join(threads[i], NULL);
}
