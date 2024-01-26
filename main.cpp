#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <thread> 

#define N_THREADS 8 

struct Boundaries {
	int start;
	int end;
	int val;
};

// this is oddly looking more like c than cpp
int main (int argc, char **argv) { 
	if(argc < 3) {
		printf("Not enough args. Format is: ./%s n_primes outputfile", argv[0]);
		return 1;
	}

	// well now it looks kind of like cpp
	auto start = std::chrono::steady_clock::now();

	auto finished = std::chrono::steady_clock::now();

	printf("%d", 69);
	return 0;
}

/** original sieve in go. don't question why I returned a hashmap
 * // if only we could use go :moyai:
 * func SieveOfEratosthenes(n int) map[int]bool {
 *     res := make(map[int]bool)
 *     tracker := make([]bool, n)
 *     max := int(math.Sqrt(float64(n)))
 *  
 *     for i := 0; i < n; i++ {
 *     	tracker[i] = true
 *     }
 *  
 *     for i := 2; i < max; i++ {
 *     		if tracker[i] == true {
 *  
 *     		for j := i * i; j < n; j += i {
 *     			tracker[j] = false
 *     		}
 *     	}
 *     }
 *  
 *     for i := 0; i < n; i++ {
 *     	if tracker[i] == true {
 *     		res[i] = true
 *     	}
 *     }
 *  
 *     return res
 *  
 */

int sieveOfEratosthenes(int n) {
	int res = 0;
	int max = (int)sqrt((double)n);
	// not sure if this gets nulled out to 0/false by default. 
	// if it does I could be lazy and flip my conditions to skip setting
	// everything to true.
	bool *tracker = (bool *)malloc(n * sizeof(bool)); 

	// parallelizable 
	for(int i=0; i<n; i++) {
		tracker[i] = true;
	}

	for(int i=2; i<max; i++) 
		if(tracker[i]) 
			// parallelizable 
			for(int j=i*i; j < n; j+=i) 
				tracker[j] = false;
	
	// parallelizable
	// don't even need a mutex which is nice
	for(int i=2; i<n; i++) {
		res += tracker[i] ? i : 0; 
	}

	free(tracker);

	return res; 
}

