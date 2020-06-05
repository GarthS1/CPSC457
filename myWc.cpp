/**********************************************
 * Last Name:   Slaney 
 * First Name:  Garth 
 * Course:      CPSC 457
 * Tutorial:    04
 * Assignment:  1
 *
 * File name: simple_wc.cpp
 *********************************************/
// Assignment for making the word count program on linux more effiecent by having a buffer 
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <cstdint>

int my_getchar(void){
	//our buffer can be changed
	static char buffer[1048576];
	//index currently at 
	static int index = 0;
	
	if(index == 0)
	{
		index = read(STDIN_FILENO, buffer, 1048576); //need to set index to how many items it reads so that it doesn't go over 
	}
	
	if(index < 1) //when the file is done reading 
		return EOF;
	
	return buffer[--index];		
}

int main( int, char **)
{
  // our boolean mini-state, true = we are in a middle of a word
  bool inside_word = false;
  // setup 3 counters
  int word_count = 0;
  int line_count = 0;
  int char_count = 0;
  // read file character by character and count words
  while(1) {
    // read in a single character
    int c = my_getchar();
    if( c == EOF) break; // quit loop on EOF
    // count every character
    char_count ++;
    // count new lines
    if( c == '\n') {
      line_count ++;
    }
    // update our state
    if( isspace(c)) {
      inside_word = false;
    }
    else {
      // update word count if starting a new word
      if( ! inside_word) word_count ++;
      inside_word = true;
    }
  }

  // print results
  printf( "%7d %7d %7d\n",
	  line_count,
	  word_count,
	  char_count
	  );

  return 0;
}