//Monte Carlo Cpp Project. I've use Pthreads and OpenMP to parallelize the process//
#include <iostream>
#include <unordered_map>
#include <cstdlib>
#include <ctime>
#include <omp.h>
#include <pthread.h>

using namespace std;

// Hash function options
int simpleModuloHash(int x, int m) {
    return x % m;
}

int bitwiseXORHash(int x, int m) {
    return (x ^ (x >> 3)) % m;
}

int multiplicativeHash(int x, int m) {
    const int A = 2654435761; // Knuth's multiplicative constant
    return ((x * A) >> 16) % m;
}

// Function pointer for selected hash function
int (*hashFunction)(int, int);

// Monte Carlo Simulation to estimate collision probability using OpenMP
double monteCarloHashCollision(int numSamples, int hashSize) {
    unordered_map<int, int> hashTable;
    int collisions = 0;
    
    #pragma omp parallel for reduction(+:collisions)
    for (int i = 0; i < numSamples; i++) {
        int randomValue = rand(); // Generate random number
        int hashValue = hashFunction(randomValue, hashSize);
        
        #pragma omp critical
        {
            if (hashTable[hashValue] > 0) {
                collisions++; // A collision occurred
            }
            hashTable[hashValue]++;
        }
    }
    
    return (double)collisions / numSamples; // Collision probability
}

// Struct for pthreads function
struct ThreadData {
    int start, end, hashSize;
    int collisions;
};

void* pthreadMonteCarlo(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    unordered_map<int, int> localHashTable;
    data->collisions = 0;
    
    for (int i = data->start; i < data->end; i++) {
        int randomValue = rand();
        int hashValue = hashFunction(randomValue, data->hashSize);
        
        if (localHashTable[hashValue] > 0) {
            data->collisions++;
        }
        localHashTable[hashValue]++;
    }
    
    pthread_exit(NULL);
}

int main() {
    srand(time(0)); // Seed random number generator
    
    int numSamples = 100000; // Hardcoded reasonable sample size
    int hashSize;
    int choice;
    
    cout << "Enter hash table size: ";
    cin >> hashSize;
    
    cout << "Choose a hash function:\n";
    cout << "1. Simple Modulo Hash\n";
    cout << "2. Bitwise XOR Hash\n";
    cout << "3. Multiplicative Hash\n";
    cin >> choice;
    
    switch (choice) {
        case 1: hashFunction = simpleModuloHash; break;
        case 2: hashFunction = bitwiseXORHash; break;
        case 3: hashFunction = multiplicativeHash; break;
        default: cout << "Invalid choice. Using default (Simple Modulo).\n";
                 hashFunction = simpleModuloHash;
    }
    
    // OpenMP version
    double collisionProbabilityOMP = monteCarloHashCollision(numSamples, hashSize);
    cout << "[OpenMP] Estimated Collision Probability: " << collisionProbabilityOMP * 100 << "%\n";
    
    // Pthreads version
    int numThreads = 4;
    pthread_t threads[numThreads];
    ThreadData threadData[numThreads];
    int chunkSize = numSamples / numThreads;
    int totalCollisions = 0;
    
    for (int i = 0; i < numThreads; i++) {
        threadData[i].start = i * chunkSize;
        threadData[i].end = (i == numThreads - 1) ? numSamples : (i + 1) * chunkSize;
        threadData[i].hashSize = hashSize;
        pthread_create(&threads[i], NULL, pthreadMonteCarlo, (void*)&threadData[i]);
    }
    
    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
        totalCollisions += threadData[i].collisions;
    }
    
    double collisionProbabilityPthread = (double)totalCollisions / numSamples;
    cout << "[Pthreads] Estimated Collision Probability: " << collisionProbabilityPthread * 100 << "%\n";
    
    return 0;
}
