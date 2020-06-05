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

// ON LINUX MACHINES REQUIRES COMPILING WITH -pthread 

// For this assignment made the single program into multithread, used barriers to reuse threads and ended the loop early if another thread
// found a smaller prime. 
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cinttypes>
#include <cmath>
#include <iostream>
#include <pthread.h>

int64_t smallPrime;  				// global variable to track smallest prime 
pthread_mutex_t primeMutex;	// mutex used to make sure there are no race conditions for smallest prime
pthread_mutex_t lockMutex;	// mutex used to make sure read is succesful
int64_t sum;								// global sum of all the divisors 
int64_t curPrime; 					// global variable to track which number was just read in 
pthread_barrier_t myBarrier;// the barrier to reuse threads 
bool readSuc;								// global varibable to check if read is succesful
int64_t spiltSize;					// global variable for the spiltSize
int nThreads;								// the amount of threads
int64_t max;								// the current maximum of the prime numbers range 

// read in the current prime number and return if read was succesful 
bool read() {
    if( ! (std::cin >> curPrime) )
        return false;
    else
        return true; 
}

// the inner function that caculates the range for the prime numbers 
int64_t getSmallestDivisorInner(int64_t start, int64_t end)
{
	if( curPrime <= 3) return 0; // 2 and 3 are primes
  if( curPrime % 2 == 0) return 2; // handle multiples of 2
  if( curPrime % 3 == 0) return 3; // handle multiples of 3
	
	// loop to check all other possible primes end early if it is no longer possible to find a smaller prime 
	while( start <= end && (smallPrime == 0 || start < smallPrime) ) 
	{	
    if (curPrime % start == 0) return start;
    if (curPrime % (start+2) == 0) return start + 2;
    start += 6;
  }
  return 0;
}

// thread function which runs until all inputs are exhausted 
void* getSmallestDivisor(void* n)
{
	int threadId = (intptr_t) n;  //cast n into proper thread id 
	while(1) 
	{
    // synchronize all threads
    int res = pthread_barrier_wait(&myBarrier);
		// do parallel work 
    if( res == PTHREAD_BARRIER_SERIAL_THREAD)
    {
      pthread_mutex_lock(&lockMutex);
      readSuc = read();  // read into global variable and check to make sure read was succesful
			
			max = sqrt(curPrime);
			spiltSize = 6 * floor(max / nThreads /6);  //split in such a way that spiltsize is divisible by 6
      pthread_mutex_unlock(&lockMutex);
    }
     
    // synchronize again before starting parallel work
    pthread_barrier_wait(&myBarrier);   

    // run tasks in parallel
    if (readSuc == true)        // if read was successful, run threads
    {
			int64_t temp;
			if(curPrime < 500) // for small values no need to multithread create one thread and run it like the original code 
			{
				// synchronize all threads
				int res = pthread_barrier_wait(&myBarrier);
				// do parallel work 
				if( res == PTHREAD_BARRIER_SERIAL_THREAD)
				{
					smallPrime = getSmallestDivisorInner(5, max);
				}
			}
			else
			{
				if( (threadId + 1) * spiltSize < 5) // needed in case a vaule of less than 5 is meant to be the start point 
					temp = getSmallestDivisorInner(5, max);
				else if (threadId == nThreads - 1){
					//needed in case it rounds in such a way it goes over or misses one 
					temp = getSmallestDivisorInner(threadId * spiltSize + 5, max);
				}
				else  // run normally 
					temp = getSmallestDivisorInner(threadId * spiltSize + 5, ((threadId + 1) * spiltSize) + 5);
				
				// for all threads check to see if this thread has the smallest prime 
				pthread_mutex_lock(&primeMutex);
				if( (smallPrime > temp || smallPrime == 0) && temp != 0)
					smallPrime = temp;
				pthread_mutex_unlock(&primeMutex);
			}
			
			// synchronize all threads
			int res = pthread_barrier_wait(&myBarrier);
			// do parallel work
			if( res == PTHREAD_BARRIER_SERIAL_THREAD)
			{
				//std::cout << "Debug: " << curPrime << " --> " << smallPrime << "\n";
				sum += smallPrime;
				smallPrime = 0;
			}
    }
    else //if read was not succesful break loop
    {
			break;
    }
  }

  pthread_exit(0);
}

// fucntion to make the proper threads and barrier
void startThreads()
{
  sum = 0; 
	
	pthread_barrier_init(&myBarrier, NULL, nThreads);

  // create threads
  pthread_t threadPool[nThreads];
  for (int i = 0; i < nThreads; i++)
  {
   pthread_create(&threadPool[i], NULL, getSmallestDivisor, (void *)i);
  }

  // when program is done finish the program 
  for (int i = 0; i < nThreads; i++)
  {
    pthread_join(threadPool[i], NULL);
  }
}

int main( int argc, char ** argv)
{
  /// parse command line arguments
  nThreads = 1;
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

	// call the function to run the threads 
	startThreads();

  /// report results
  std::cout << "Sum of divisors = " << sum << "\n";

  return 0;
}
