/**********************************************
 * Last Name:   Slaney 
 * First Name:  Garth 
 * Course:      CPSC 457
 * Tutorial:    04
 * Assignment:  1
 *
 * File name: deadlock.cpp
 *********************************************/

#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <algorithm>
using namespace std;

// structure used to store information about each job 
struct info{
	int process; // process index 
	int64_t time_start; // time which the process starts 
	int64_t time_needed; // time left to run 
};

// Simulation of round robin scheluding
void startLoopRR(std::vector< std::pair<int64_t, int64_t> > &data, int64_t time)
{
	int64_t curr_time = 0; // the time counter 
	int jobs_remaining = data.size(); // keeps track of all the jobs 
	
	std::vector< struct info > readyQ; // the queue of all the jobs 
	int current_index = 0; // index which keeps track of what time the next job starts 
	bool cpuIdle = true; // boolean for if the cpu is idle 
	bool start = true; // for printing 
	int64_t rr = 0; // used for keeping track of how much time done on current round 
	bool skipped = true; // for printing 
	
	while(1) 
	{
    // If no more jobs left in file exit the loop 
    if (jobs_remaining == 0) 
			break;
		
		// If the queue is empty and the process is done remove from queue  
    if(!readyQ.empty())
		{
			if(readyQ.at(0).time_needed == 0)
			{
        readyQ.erase(readyQ.begin());
				cpuIdle = true; // set CPU to idle 
        jobs_remaining--;
        continue;
			}
		}
		
		// if there are still jobs remaining to add to the queue and the current time is the time the next job 
		// should start add job to queue 
		if(current_index < data.size())
		{
			if(data.at(current_index).first == curr_time)
			{
				// add as structure info to keep track of all needed info 
				readyQ.push_back({current_index, data.at(current_index).first, data.at(current_index).second});
				current_index++;
				continue;
			}
		}
		
		//need in case of false switches 
		if(cpuIdle && !readyQ.empty() )
		{
			rr = 0; // set time on slice to 0 now
			cpuIdle = false;
			continue;			
		}
		
		// if the cpu is idle or RR time slice exceeded and the queue is not empty 
    if (rr == time && !readyQ.empty())
		{
			int64_t temp = readyQ.at(0).process;
			readyQ.push_back(readyQ.at(0)); // make a copy at the back 
			readyQ.erase(readyQ.begin());
			rr = 0;
			if(readyQ.at(0).process != temp)
				skipped = true;
			else
				skipped = false;
			cpuIdle = false;
			continue;
		}

		// if cpu idle output - 
		if(cpuIdle)
		{
			if(start)
			{
				std::cout << "-";
				start = false;
			}
			else				
				std::cout << ",-";
		}
		// else output the process running 
		else
		{
			if(start)
			{
				std::cout << "P" << readyQ.at(0).process;
				start = false;
			}	
			else if(skipped) // only print when job is begining 
				std::cout << ",P" << readyQ.at(0).process;
		}
		
    // figure out how much to increment the time 
		if(!cpuIdle)
		{
			// if there are jobs left to add
			if(current_index < data.size())
			{
				// find which comes first the next job coming or the ending of the current job due to time or ending naturally
				if( readyQ.at(0).time_needed < (data.at(current_index).first - curr_time) || (time -rr) < (data.at(current_index).first - curr_time) )
				{				
					// if time_needed exceeds time slice 
					if(readyQ.at(0).time_needed > (time -rr))
					{
						readyQ.at(0).time_needed -= (time -rr); // one time slice done on current job  
						curr_time += (time -rr); // increment by one time slice 
						rr = time; // will finish the job						
					}
					// otherwise go to when job finishes  
					else 
					{
						curr_time += readyQ.at(0).time_needed; // skip to when job is finished 
						readyQ.at(0).time_needed = 0; // job is now finished 
						rr = time; // will finish the job 
					}
					skipped = true; // can print 
				}
				else
				{
					rr += data.at(current_index).first - curr_time; // increase time slice by amount skipped
					curr_time = data.at(current_index).first; // go to time next job arrives
					readyQ.at(0).time_needed -= data.at(current_index).first - curr_time; // decrease the amount left by time skipped 
					skipped = false; //can't print 
				}
			}
			// no more jobs to add figure out how much time to add to either finish job or timeslice 
			else 
			{
				// if time_needed exceeds time slice 
				if(readyQ.at(0).time_needed > time - rr)
				{
					readyQ.at(0).time_needed -= time - rr; // one time slice done on current job - what has already run   
					curr_time += time - rr; // increment by one time slice - what has already run
					rr = time; // wil finish the job 
				}
				// otherwise go to when job finishes  
				else 
				{
					curr_time += readyQ.at(0).time_needed; // skip to when job is finished 
					readyQ.at(0).time_needed = 0; // job is now finished 
				}
				skipped = true; // can print 
			}
		}
		// idle 
		else 
			curr_time = data.at(current_index).first; // go to time next job arrives
	}
}

// Simulation of Shortest job first scheluding algorithm
void startLoopSJF(std::vector< std::pair<int64_t, int64_t> > &data)
{
	int64_t curr_time = 0; // the time counter 
	int jobs_remaining = data.size(); // keeps track of all the jobs 
	
	std::vector< struct info > readyQ; // the queue of all the jobs 
	int current_index = 0; // index which keeps track of what time the next job starts 
	bool cpuIdle = true; // boolean for if the cpu is idle 
	bool skipped = true; // boolean for printing
	bool start = true; // for printing 
	
	while(1) 
	{
		// If no more jobs left in file exit the loop 
    if (jobs_remaining == 0) 
			break;
		
		// If the queue is empty and the process is done remove from queue  
    if(!readyQ.empty())
		{
			if(readyQ.at(0).time_needed == 0)
			{
        readyQ.erase(readyQ.begin());
				cpuIdle = true; // set CPU to idle 
        jobs_remaining--;
        continue;
			}
		}
		
		// if there are still jobs remaining to add to the queue and the current time is the time the next job 
		// should start add job to queue 
		if(current_index < data.size())
		{
			if(data.at(current_index).first == curr_time)
			{
				// add as structure info to keep track of all needed info 
				readyQ.push_back({current_index, data.at(current_index).first, data.at(current_index).second});
				current_index++;
				continue;
			}
		}
		
		// if the cpu is idle and the queue is not empty 
    if (cpuIdle  && !readyQ.empty())
		{
			int64_t smallest = readyQ.at(0).time_needed;  // start value 
			int index = 0;
			// look for the shortest job in the ready queue
			for( int i = 1; i < readyQ.size(); i++)
				if( smallest > readyQ.at(i).time_needed)
				{
					smallest = readyQ.at(i).time_needed;
					index = i;
				}
			// move the shortest job to the front of the ready queue 
			std::iter_swap(readyQ.begin(), readyQ.begin() + index);
			cpuIdle = false; // no longer idle 
			continue;
		}
		
		// if cpu idle output - 
		if(cpuIdle)
		{
			if(start)
			{
				std::cout << "-";
				start = false;
			}
			else 
				std::cout << ",-";
		}
		// else output the process running 
		else
		{
			if(start)
			{
				std::cout << "P" << readyQ.at(0).process;
				start = false;
			}	
			else if(skipped) // only print when job is begining 
				std::cout << ",P" << readyQ.at(0).process;
		}

		
		// figure out how much to increment the time 
		if(!cpuIdle)
		{
			// if there are jobs left to add
			if(current_index < data.size())
			{
				// find which comes first the next job coming or the ending of the current job 
				if( readyQ.at(0).time_needed < (data.at(current_index).first - curr_time) )
				{				
					curr_time += readyQ.at(0).time_needed; // go to when the job ends  
					readyQ.at(0).time_needed = 0; //done now
					skipped = true; // can print 
				}
				else
				{
					curr_time = data.at(current_index).first; // go to time next job arrives
					readyQ.at(0).time_needed -= data.at(current_index).first - curr_time; // decrease the amount left by time skipped 
					skipped = false; //can't print 
				}
			}
			// no more jobs to add so skip to when the job should be done 
			else 
			{
				curr_time += readyQ.at(0).time_needed; // go to when the job ends  
				readyQ.at(0).time_needed = 0; //done now
				skipped = true; // can print 
			}
		}
		// idle 
		else 
			curr_time = data.at(current_index).first; // go to time next job arrives
	}
}

// function to read the file and store it in a vector of pairs 
int readData(std::vector< std::pair<int64_t, int64_t> > &x, std::string file) 
{
	ifstream inObj;
	
	inObj.open(file.c_str());
	
	// if issue opening file exit program 
	if( !inObj )
	{
		cout << "Error: can not read file";
		exit(1);
	}
	
	// define temp variables and read first line 
	int64_t i, j;
	inObj >> i;
	inObj >> j;
	while( !inObj.eof() )
	{
		x.push_back( std::make_pair(i, j) ); // add a pair to vector 
		inObj >> i;
		inObj >> j;
	}
	
	inObj.close();	
}

// The main function of the program which reads in the arguements and performs error catching before calling 
// the scheluding simulation 
int main( int argc, char ** argv)
{
	// Error catching for inproper number of arguements entered
	if( argc != 3 && argc != 4) {
    std::cout << "Improper arguements entered";
    exit(-1);
  }
	
	// Error catching for trying to specfy a timeslice for shortest job first 
  if( argc == 4){
		if( 'S' == argv[2][0]){
			std::cout << "SJF does not accept timeslice." << "\n";
			exit(-1);
		}
	}
	
	// boolean for if round robin is true 
	bool roundRobin;
	
	// Condiontal logic for figuring out which algorthim to use 
	if('S' == argv[2][0])
		roundRobin = false;
	else if('R' == argv[2][0])
		roundRobin = true;
	else{
		std::cout << "Improper scheluding algorthim." << "\n";
		exit(-1);
	}
	
	//read the data into a vector of pairs 
	std::vector<std::pair<int64_t, int64_t>> data;
	readData(data, argv[1]);
	
	std::cout << "Seq: ";
	if(roundRobin)
	{
		//If round robin define the time slice 
		int64_t time = 1;
		time = strtoll(argv[3], NULL, 10);
		startLoopRR(data, time ); //start loop 
	}
	else
		startLoopSJF(data); // start loop 
}
