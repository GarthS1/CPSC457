// skeleton program for find duplicate files using sha256 digests
//
// - it reads filenames form standard input
// - it then computes sha256 digests for first two files
// - compares the digests
//
// the program uses C++ containers and strings, but otherwise
// uses standard C APIs
// ===================================================================

// made the program from one which could only compare two files into one that can comapre any number of files 
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <cassert>
#include <string>
#include <vector>
#include <utility> // pair
#include <algorithm>
#include <iostream>

using namespace std;

// comapres the second string in the pair
// using the default .compare function 
bool mySort(const pair<string, string> &t1, const pair<string, string> &t2){
	return t1.second < t2.second;
}

// read line from fp
// returns False on EOF
//         True  on success
bool
get_line( std::string & line, FILE * fp = stdin)
{
  line.clear();
  while(1) {
    int c = fgetc(fp);
    if( c == '\n') return true;
    if( c == EOF) return ! line.empty();
    line.push_back(c);
  }
}

// returns SHA256 digest of the contents of file fname
// on failure returns empty string
// uses popen() to call sha256sum binary
std::string
digest( const std::string & fname)
{
  std::string cmdline = "sha256sum " + fname + " 2> /dev/null";
  FILE * fp = popen( cmdline.c_str(), "r");
  if( fp == NULL) return "";
  std::string output;
  if( ! get_line(output, fp)) return "";
  if( pclose(fp) != 0) return "";
  std::string result;
  for( auto c : output)
    if( isspace(c)) break ; else result.push_back(c);
  return result;
}

int
main()
{
  // read a list of filenames from stdin
  std::vector<std::string> fnames;
  std::string line;
  while(1) {
    if( ! get_line(line)) break;
    fnames.push_back(line);
  }

  // for debugging purposes print out the filenames
  //printf("Read %lu filenames:\n", fnames.size());
  //for( const auto & fname : fnames)
  //  printf("  '%s'\n", fname.c_str());

  if( fnames.size() < 2) {
    printf("I could have worked if you gave me 2+ filenames... :(\n");
    return -1;
  }
	
	//make an vector of pairs 
	vector< pair<string, string> > fileList;
	
	//populate the vector with filenames and digest numbers
	for( const auto & fname : fnames)
		fileList.push_back( make_pair(fname , digest(fname)));
	
	//sorts the vector using mySort comparsion 
	sort(fileList.begin(), fileList.end(), mySort);
	
	//for debugging purposes print out digest numbers in sorted order
	//for(const auto &s: fileList) 
	//	cout<< s.second <<endl ;
	
	//keeps track on how many matches have been found 
	int matches = 1;
	
	// iterates over the fileList  
	for(int i = 0; i < fileList.size() - 1; i++){
		
		//make a vector of file names and add the first one 
		vector<string> files;
		files.push_back(fileList.at(i).first);
		
		// iterate over the list till something doesn't match 
		while(fileList.at(i).second == fileList.at(i + 1).second){
			
			//add to the files list to print at the end 
			files.push_back(fileList.at(i + 1).first);
			
			//only increment i if it will not result in a out of bound errror 
			if(i + 2 != fileList.size())
				i++;
			else 
				break;
		}
		
		//print all the matches 
		if(files.size() != 1){
			
			//if unable to caclulate the digest print out that you were unable to find it 
			if(fileList.at(i).second.empty() ) 
					cout<< "Could not get digest for files:" <<endl; 
			else{
				printf("Match %i:\n", matches);
				matches++;
			}
			for(int k = 0; k < files.size(); k++){
				cout <<"  - "<<files.at(k) <<endl;
			}
		}
	}
  return 0;
}

