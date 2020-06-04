/// reads in a positive integers in range (2 .. 2^63-1) from standard input
/// for each number on input:
///   if the number is prime, it is ignored
///   determines the smallest factor of the composite number
/// prints out the sum of all smallest factors
///
/// if no composite numbers are found, prints out "0"
///
/// compile with:
///   $ g++ sumFactors.cpp -O3 -o gcd -lm

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cinttypes>
#include <cmath>
#include <iostream>
#include <pthread.h>

int64_t smallPrime;  				  //global variable to track smallest prime 
pthread_mutex_t  primeMutex;	//mutex used to make sure there are no race conditions 

// structure used to pass arguements into threads 
struct args{
	int64_t n;			//the number being tested 
	int64_t start;	//the start of the range 
	int64_t end;		//the end of the range 
};

// the inner function that caculates the range for the prime numbers 
void* getSmallestDivisorInner(void* input)
{
	struct args my_data = *(struct args*) input; //cast args into local data 
	
	while(my_data.start <= my_data.end) 
	{
    if (my_data.n % my_data.start == 0){
			pthread_mutex_lock(&primeMutex);  //lock mutex 
			if(smallPrime > my_data.start || smallPrime == 0)  //change if smallPrime is 0 or if current is less than																											
				smallPrime = my_data.start;											 //current value 
			pthread_mutex_unlock(&primeMutex); //unlock mutex
			pthread_exit(0);
		}
    if (my_data.n % ( my_data.start + 2) == 0){
			pthread_mutex_lock(&primeMutex); //lock mutex 
			if(smallPrime > (my_data.start + 2) || smallPrime == 0)  //change if smallPrime is 0 or if current is less than																											
				smallPrime = my_data.start + 2;											   //current value 
			pthread_mutex_unlock(&primeMutex); //unlock mutex 
			pthread_exit(0);
		}
    my_data.start += 6;
  }
	pthread_exit(0);
}

/// calculates smallest divisor of n that is less than n
/// takes in amount of threads used 
/// returns 0 if n is prime or n < 2
///
/// examples:
///    n =  1         --> result = 0
///    n =  2         --> result = 0
///    n =  9         --> result = 3
///    n = 35         --> result = 5
///    n = 8589934591 --> result = 7
int64_t getSmallestDivisor(int64_t n, int nThreads)
{
  if( n <= 3) return 0; // 2 and 3 are primes
  if( n % 2 == 0) return 2; // handle multiples of 2
  if( n % 3 == 0) return 3; // handle multiples of 3
	
	smallPrime = 0;  
	int64_t max = sqrt(n);
	int64_t spiltSize = 6 * floor(max / nThreads /6);  //split in such a way that spiltsize is divisible by 6 
	pthread_t threadPool[nThreads];
	struct args temp[nThreads];		// have an array of args to avoid issues with passing them 
	
	//loop which constructs all of the pthreads needed 
	for(int i = 0; i < nThreads; i++)
	{ 
		if( (i + 1) * spiltSize < 5) // needed in case a vaule of less than 5 is meant to be the start point 
			temp[i] = {n, 5, max};
		else if (i == nThreads - 1) //needed in case it rounds in such a way it goes over or misses one 
			temp[i] = {n, i * spiltSize + 5, max};
		else
			temp[i] = {n, i * spiltSize + 5, ((i + 1) * spiltSize) + 5};
		pthread_create(&threadPool[i], NULL, getSmallestDivisorInner, (void *) &temp[i]);
	}
	
	//join all the threads 
	for(int i = 0; i < nThreads; i++)
		pthread_join(threadPool[i],NULL);
	
  return smallPrime;
}


int main( int argc, char ** argv)
{
  /// parse command line arguments
  int nThreads = 1;
  if( argc != 1 && argc != 2) {
    std::cout << "Usage: " << argv[0] << " [nThreads]\n";
    exit(-1);
  }
  if( argc == 2) nThreads = atoi( argv[1]);

  /// handle invalid arguments
  if( nThreads < 1 || nThreads > 256) {
    std::cout << "Warning: Bad arguments. 1 <= nThreads <= 256!\n";
  }

  std::cout << "Using " << nThreads
            << " thread" << (nThreads == 1 ? "" : "s")
            << ".\n";

  /// sum up the divisors
  int64_t sum = 0;
  while( 1) {
    int64_t num;
    if( ! (std::cin >> num) ) break;
    int64_t div = getSmallestDivisor(num, nThreads);
    // the line below may help you with debugging
    std::cout << "Debug: " << num << " --> " << div << "\n";
    sum += div;
  }

  /// report results
  std::cout << "Sum of divisors = " << sum << "\n";

  return 0;
}

// barriers then thread cancellation 