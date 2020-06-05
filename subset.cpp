// single threaded program that reads in 32-bit integers from
// standard input, and then counds and prints out how many
// subsets of these integers sum up to 0 (zero).
//
// this program uses a trivial exponential time algorithm, by
// testing every possible subset (there are 2^N-1 subsets to
// test for N integers)
//
// the program uses global variables - to make it a bit easier
// to convert to a multi-threaded code for programmers
// that have not used pthreads before


// For this assignment I converted the simple program into a multi threaded one 
#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <math.h>
#include <thread>
#include <unistd.h>
#include <iostream>
#include <pthread.h>

// global variables are bad ...
// but they are acceptable for your first mulithreaded program
std::vector<long> a;
//removed count as a gloabal var as it wasn't needed

// for debugging purposes (if you want to print the combinations)
void print_sol(long comb)
{
  return; // comment this out if you want to see the subsets
  
  for(size_t i = 0 ; i < a.size() ; i ++ ) {
    if( comb & 1)
      printf("%ld ", a[i]);
    comb /= 2;
  }
  printf("\n");
}

// test a combination 'comb'
// the binary representation of comb will be used to decide
// which numbers in a[] will be used to test the sum
void test(long comb, int* count)
{
  long long sum = 0;
  long bits = comb;
  for(size_t i = 0 ; i < a.size() ; i ++ ) {
    if( bits & 1) // check lowest bit
      sum += a[i];
    bits >>= 1; // shift bits to the right
  }
  if( sum == 0) {
    print_sol(comb);
    (*count)++;  // defrence then increment 
  }
}

// test all combinatsion in range [from, to)
void test_range( long from, long to, int* counter)
{
  for( long i = from ; i < to ; i ++)
    test(i, counter);  //passes a counter to test
}

int main( int argc, char ** argv)
{
  //
  // parse command line arguments
  //
  if( argc > 2) {
    printf("Error: expected at most 1 command line argument.\n");
    return -1;
  }
  long nThreads = 1;
  if( argc > 1) {
    // convert argv[1] to 'nThreads', report error if failed
    char * end = 0;
    errno = 0;
    nThreads = strtol( argv[1], & end, 10);
    if( * end != 0 || errno != 0) {
      printf("Expected integer, but you gave me '%s'\n", argv[1]);
      return -1;
    }
  }

  //
  // read all integers one by one and put them into the array
  //
  while(1) {
    long n;
    if( 1 != scanf("%ld", & n)) break;
    a.push_back(n);
  }

  // debug message
  printf( "Using %ld thread(s) on %lu numbers.\n",
	  nThreads, a.size());
	
	//need this to avoid making threads that do no work 
	if(nThreads > a.size())
		nThreads = a.size();
	
	//spilts into 2^n / m where n = #numbers and m = #threads and then uses celling to force an integer
  long spiltSize = ceill((long(1) << a.size()) / nThreads); 
	
	int counters[nThreads];  //an array of count to avoid race conditions 
	
	//set all to 0
	for(int i = 0; i < nThreads; i++)
		counters[i] = 0;
	
	std::thread threadPool[nThreads];  //a thread pool used to run all the threads needed 
	
	//run all the threads needed 
	for (int i = 0; i < nThreads; i++)
		if (i == nThreads - 1) //needed in case it rounds in such a way it goes over or misses one 
			threadPool[i] = std::thread(test_range, i * spiltSize + 1,long(1) << a.size(), &counters[i]);
		else	
			threadPool[i] = std::thread(test_range, i * spiltSize + 1,(i + 1) * spiltSize, &counters[i]);

  //wait for every thread to finish
  for (int i = 0; i < nThreads; i++)    
		threadPool[i].join();    

	//add up all the counters to find the total count 
  long count = 0;
	for (int i = 0; i < nThreads; i++)
	  count += counters[i];

  printf("Subsets found: %ld\n", count);

  return 0;
}
