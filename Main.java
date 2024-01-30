import java.io.FileWriter;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class Main {
	private static final int N_THREADS = 8;

	public static void main(String[] args) throws Exception {
		int count = 0;
		if(args.length < 2) {
			System.out.printf("Didn't supply enough vals. Format is ./executable nPrimes filename\n");
			return;
		}

		int n = Integer.parseInt(args[0]);
		
		FileWriter out = new FileWriter(args[1]);

		long start = System.nanoTime();
		long sum = sieveOfEratosthenes(n);
		long end = System.nanoTime();
		System.out.printf("sum (singlethreaded) is %d. Took %d\n", sum, end - start);

		start = System.nanoTime();
		long sum2 = sieveOfEratosthenesMT(n);
		end = System.nanoTime();
		System.out.printf("sum (multithreaded)is %d.   Took %d\n", sum2, end - start);
	}

	public static long sieveOfEratosthenes(int n) {
		if (n < 4) {
			return 5;
		}

		long res = 0;

		int max = (int)Math.sqrt((double)n);
		boolean[] tracker = new boolean[n]; 

		// reversed it on this vers
		// defaults to false
		// trues means you skip
		for(int i=2; i<max; i++) 
			if(!tracker[i]) 
				for(int j=i*i; j < n; j+=i) 
					tracker[j] = true;

		for(int i=2; i<n; i++) {
			if (!tracker[i]) {
				res += i; 
			}
		}
		return res; 
	}

	public static long sieveOfEratosthenesMT(int n) {
		if (n < 4) {
			return 5;
		}

		long res = 0;

		int max = (int)Math.sqrt((double)n);
		boolean[] tracker = new boolean[n]; 
		Thread[] hands = new Thread[N_THREADS];
		// ExecutorService executor = Executors.newFixedThreadPool(N_THREADS);

		// reversed it on this vers
		// defaults to false
		// trues means you skip
		
		/*
		 *
		for(int i=2; i<max; i++) 
			if(!tracker[i]) 
				for(int j=i*i; j < n; j+=i) 
					tracker[j] = true;
		*/

		for(int i=2; i<max; i++)  {
			if(!tracker[i]) {
				int split = (n - i*i) / N_THREADS;
				for(int t = 0; t < N_THREADS; t++) {
					int begin = (int)Math.ceil((double)(t * split) / i) * i + i*i; 
					int end = t == (N_THREADS - 1) ? n : (t+1) * split + i*i;
					int cpy = i;
					hands[t] = new Thread(() -> {
						for(int j=begin; j < end; j += cpy) {
							tracker[j] = true;
						}
					});
					hands[t].start();
				}
				
				for(int t=0; t < N_THREADS; t++) {
					try {
						hands[t].join();
					} catch (Exception e) {
						System.out.printf("whoopdydo it didn't work\n");
					}
				}
			}
		}

		long[] sums = new long[8];

		int split = (n - 2) / N_THREADS;
		for(int t = 0; t < N_THREADS; t++) {
			int begin = t * split + 2; 
			int end = t == (N_THREADS - 1) ? n : (t+1) * split + 2;
			int cpy = t;
			hands[t] = new Thread(() -> {
				for(int i=begin; i < end; i++) {
					if (!tracker[i]) {
						sums[cpy] += i; 
					}
				}
			});
			hands[t].start();
		}

		for(int t=0; t < N_THREADS; t++) {
			try {
				hands[t].join();
				res += sums[t];
			} catch (Exception e) {
				System.out.printf("whoopdydo it didn't work\n");
			}
		}

		return res; 
	}
}

