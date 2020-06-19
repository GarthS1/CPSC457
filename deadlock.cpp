/**********************************************
 * Last Name:   Slaney 
 * First Name:  Garth 
 * Course:      CPSC 457
 * Tutorial:    04
 * Assignment:  1
 *
 * File name: deadlock.cpp
 *********************************************/
// detects deadlocks by being feed in all edges
#include <stdio.h>
#include <ctype.h>
#include <vector>
#include <string>
#include <cassert>
#include <unordered_map>
#include <set>
#include <list>
#include <queue>
#include <iostream> 


typedef std::vector<std::string> vs_t;

vs_t words; // made an global variable so that it could print out last edge 

enum Edge {
  Allocation, Request
};

// split string p_line into vector of strings (words)
// the delimiters are 1 or more whitespaces
vs_t
split( const std::string & p_line)
{
  auto line = p_line + " ";
  vs_t res;
  bool in_str = false;
  std::string curr_word = "";
  for( auto c : line) {
    if( isspace(c)) {
      if( in_str)
        res.push_back(curr_word);
      in_str = false;
      curr_word = "";
    }
    else {
      curr_word.push_back(c);
      in_str = true;
    }
  }
  return res;
}

// read in a line from stdin and parse it
// parsed results are passed back via parameter references
// return true on success, false on EOF
// aborts program on bad input (should throw exception)
bool
parseLine( std::string & proc_name, std::string & res_name, Edge & etype)
{
  char buff[4096];

  // read in a line and parse it into words
  while(1) { // loop to skip empty lines
    if( ! fgets( buff, sizeof(buff), stdin))
      return false;
    words = split(buff);
    if( words.size() != 0) break;
  }

  if( words.size() != 3) {
    printf("Bad line: %s\n", buff);
    exit( -1); // exception would be nicer
  }
	
  proc_name = words[0];
  res_name = words[2];
  etype = Edge::Request;

  if( words[1] == "->") {
    return true;
  }
  if( words[1] == "<-") {
    etype = Edge::Allocation;
    return true;
  }

  printf("Bad line: %s\n", buff);
  exit( -1); // exception would be nicer
};

// utility class you can use to convert words to unique integers
// get(word) returns the same number given the same word
//           the numbers will start at 0, then 1, 2, ...
//
// example:
//   Word2Int w2i
//   w2i.get("hello") = 0
//   w2i.get("world") = 1
//   w2i.get("hello") = 0
//
// HINT: this "may" help you get a bit more performance
// in your cycle finding algorithm, since indexed arrays are faster
// than hash tables... if you don't know what that means,
// safely ignore this hint :)
struct Word2Int {
  int
  get(const std::string & w) {
    auto f = myset.find(w);
    if( f == myset.end()) {
      myset[w] = counter;
      counter ++;
      return counter - 1;
    }
    return f-> second;
  }
	// function for returning the string assocaited with the integer 
	std:: string getString (int n)
	{
		for(const auto &e: myset) // iterate over map
			if(e.second == n)
				return e.first;
		
		return "";
	}
 private:
  int counter = 0;
  std::unordered_map<std::string,int> myset;
};

// class that imlements the graph needed to run the cycle detection algorithm 
// stores all revelant information as well
struct Graph {

  std::unordered_map<int, std::vector<int>> adj_list; // an reverse adjacency list 
  std::unordered_map<int, int> out_counts; // an list which says how many nodes are leaving this node 
  std::set<int> all_nodes; // contains all nodes 
	std::vector<int> deadlocks; // if there is a deadlock this will store all processes deadlocked 
	struct Word2Int intRep; // the struct which converts all words to ints 
  
	//Default constructor 
	Graph() {
  }

  // add edge n1 -> n2
  void
  add( std::string n1 , std::string n2) {
		// convert string to int and store 
		int n3 = intRep.get(n1);
		int n4 = intRep.get(n2);
		
		all_nodes.insert(n3);
    all_nodes.insert(n4);
    adj_list[n4].push_back(n3);
    out_counts[n3] ++;
		out_counts[n4] += 0;  // does nothing but is needed to solve some issues were the out_counts is not intalized  
  }

  // run cycle detection
  bool
  deadlock() 
	{
		std::unordered_map<int, int> temp_out_counts = out_counts; // temp copy 
		std::queue<int> myqueue; // queue objective used to store nodes 
		
		// look for all nodes that have no nodes leaving and add to queue 
	  for( const auto &e : out_counts) 
			if(e.second == 0)
				myqueue.push(e.first);
	
		// while there are still nodes in the queue run 
		while(!myqueue.empty())
		{
			int current = myqueue.front(); // set current to front of queue 
			myqueue.pop(); // remove from queue 
					
			// decrease all in list by one 
			for( const auto & e : adj_list[current])
			{
				temp_out_counts[e] --;
				if( temp_out_counts[e] == 0)
					myqueue.push(e); // if 0 now add to queue 
			}
		}
		
		// look for all nodes that have nodes leaving and add to cycle 
	  for( const auto & e : temp_out_counts) 
			if( e.second > 0)
				if( 'p' == intRep.getString(e.first)[0]) //only processes should be added 
					deadlocks.push_back(e.first);
			
		// if no nodes left no cycle so no deadlock
		if(deadlocks.empty())
			return false;
		// if cycle then return true 
		else
			return true;		
  }

  // report cycle
  void
  report() {
		// if no process no deadlock 
		if (deadlocks.empty())
			std::cout<<"No deadlock"<< "\n";
		else {
			std::cout<<"Deadlock on edge: " << words[0] << words[1] << words[2] <<"\n" ; 
			std::cout<<"Deadlocked processes: ";
			bool print = false; // used for printing 
			// iterate over all in deadlocks and print 
			for( auto & n : deadlocks){
				if(print)
					std::cout << ",";

				std::cout << intRep.getString(n).erase(0, 2); //remove the P_ 
				print = true;
			}
			std::cout << "\n";	
		}
  }
};

int
main()
{
  
  Graph g; //make the graph object 
  
  while(true) {
    std::string pname, rname;
    Edge e;
    if( ! parseLine(pname, rname, e)) break; // if there are no more new edges to read leave the loop 

    if( e == Edge::Request)
      g.add( "p_" + pname, "r_" + rname);  // add to the graph as request 
    else
      g.add( "r_" + rname, "p_" + pname); // add to the graph as holding 

    if( g.deadlock()) break;  // if graph detects a loop leave the loop 
  }

  g.report(); // print the deadlocked resources or none if no deadlock 
  
  return 0;
}
