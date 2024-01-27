#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <thread> 

#define N_THREADS 8 

int sieveOfEratosthenes(int n);
int sieveOfEratosthenesMultiThreaded(int n, int *count);
void setArr(int start, int end, bool *arr); 
void sumPrimes(int start, int end, bool *arr, int *ret, int *sum); 

// this is oddly looking more like c than cpp
int main (int argc, char **argv) { 
	if(argc < 3) {
		printf("Not enough args. Format is: %s n_primes outputfile", argv[0]);
		return 1;
	}

	int n = atoi(argv[1]);
	FILE *fp = std::fopen(argv[2], "w+");
	// well now it looks kind of like cpp
	int res = sieveOfEratosthenes(n); 
	auto start = std::chrono::steady_clock::now();
	int cnt;
	int res2 = sieveOfEratosthenesMultiThreaded(n, &cnt);
	auto finished = std::chrono::steady_clock::now();
	

	fprintf(fp, "%lf %d %d\n",std::chrono::duration<double, std::milli>(finished-start).count(), cnt, res );
	fclose(fp);

	printf("\ntotal:%d, %d\n", res, res2);
	return 0;
}

int sieveOfEratosthenes(int n) {
	if (n < 4) {
		return 5;
	}
	int res = 0;
	int max = (int)sqrt((double)n);
	// not sure if this gets nulled out to 0/false by default. 
	// if it does I could be lazy and flip my conditions to skip setting
	// everything to true.
	bool *tracker = (bool *)malloc(n * sizeof(bool)); 

	// parallelizable 
	// no mutex
	for(int i=0; i<n; i++) {
		tracker[i] = true;
	}

	for(int i=2; i<max; i++) 
		if(tracker[i]) 
			// parallelizable 
			// no mutex needed either
			for(int j=i*i; j < n; j+=i) 
				tracker[j] = false;
	
	// parallelizable
	// don't even need a mutex which is nice
	for(int i=2; i<n; i++) {
		if (tracker[i]) {
			res += i; 
			printf("%d ", i);
		}
	}

	free(tracker);

	return res; 
}

int sieveOfEratosthenesMultiThreaded(int n, int *count) {
	if (n < 4) {
		return 5;
	}

	int res = 0;
	int max = (int)sqrt((double)n);

	bool *tracker = (bool *)malloc(n * sizeof(bool)); 
	std::thread hands[N_THREADS];

	int split = n / N_THREADS;

	for(int t=0; t < N_THREADS; t++){
		int begin = t * split;
		int end = t == (N_THREADS - 1) ? n : (t+1) * split;
		hands[t] = std::thread([&] {
					for(int i=begin; i < end; i++) 
						tracker[i] = true;
				});
	}
	
	for(int t=0; t < N_THREADS; t++ ) 
		hands[t].join();


	// this loop has to be serial
	for(int i=2; i<max; i++) {
		if(tracker[i]) {
			split = (n - i*i) / N_THREADS;
			for(int t = 0; t < N_THREADS; t++) {
				int begin = t * split + i*i; 
				begin = begin % i == 0 ? begin : (begin - (begin % i)) + i;
				int end = t == (N_THREADS - 1) ? n : (t+1) * split;
				hands[t] = std::thread([&] {
						for(int j=begin; j < end; j+=i) {
								printf("removing %d from thread %d\n", j, t);
								tracker[j] = false;
						}
						});
			}

			for(int t=0; t < N_THREADS; t++ ) 
				hands[t].join();
			printf("finished filtering\n");

		}
	}
	
	// parallelizable
	// don't even need a mutex which is nice
	int counts[N_THREADS];
	int sums[N_THREADS];

	split = (n-2) / N_THREADS;

	for(int t=0; t < N_THREADS; t++){
		int begin = 2 + t * split;
		int end = t == (N_THREADS - 1) ? n : (t+1) * split;

		printf("starting checking thread[%d] \n", t);
		hands[t] = std::thread([&] {
				for(int i=begin; i < end; i++) {
					if(tracker[i])  {
						printf("found %d from thread %d\n", i, t);
						counts[t] += 1;
						sums[t] += i;
					}
				}
				});
	}

	
	printf("finished checking, collecting now\n");
	// collect threads and sum up everything
	for(int t=0; t < N_THREADS; t++)  {
		hands[t].join();
		res += sums[t];
		*count += counts[t];
	}


	free(tracker);

	return res; 
}
