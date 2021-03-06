//$Id: ext_numb.cpp 3126 2013-12-04 17:52:52Z jsibert $
/** \file ext_numb.cpp
Functions for manipulating file name extensions for .par files.
Also contains functions for processing directory entries.
The functions contain a lot of system- and compiler- dependent
code and data structures.
\todo Update these functions to use C++ strtream in place of sprintf.
Change from char[] variables to adstring.
Include option to switch between 2 and 3 byte extensions.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#ifdef __GNUC__
  #if (__GNUC__ >=3)
    #include <iostream>
    using namespace std;
  #else
    #include <iostream>
  #endif
  #include <dirent.h>
  int findfirst(const char* mask, DIR** dirp, struct dirent** direntp);
  int findnext(const char* mask, DIR* dirp, struct dirent** direntp);
#endif

/** Finds next unused extension number.
  Valid for hex or decimal extension numbers. 
  Be cautious of placing graphics files such as .pdf or .png files in a run directory. 
  The string "df" is a valid hexidecimal number equal to decicmal 223.
 \param root Character string containing the file name root; the contents of file_nam.tmp
 \param letter First character of file name extension, for example 'p'
 \return The last used (ie highest) file extension number for a specified
 first letter in the file name extension.
*/
int last_ext_number(const char* root, const char letter)
{
  int file_num = 0;
  int test_num = 0;
  char mask[80];
  char name[80];
  int  number_pos = 0;
 
  sprintf(mask,"%s.%c%c",root, letter,'\0');
  //cout << "mask: " << mask << endl;

  DIR* dirp = NULL;
  struct dirent* ffb = NULL;
  int find_result = findfirst(mask, &dirp, &ffb);
  //cout << find_result << endl;
  //cout << ffb->d_name << endl;

  if (find_result < 0)
  {
    file_num = -1;  // return -1 if no numbered files found
  }
  else
  {
    do
    {
      strcpy(name, ffb->d_name);
      //cout << "name: " << name << endl;
      number_pos = strlen(name) - 3;
      //cout << "number_pos: " <<  number_pos << endl;
      if ( isxdigit(name[number_pos]) &&
	      isxdigit(name[number_pos+1]) &&
	         isxdigit(name[number_pos+2]) )
      {
	sscanf(&name[number_pos],"%3x",&test_num);
        //cout << "test_num: " << test_num << endl;
	if ((test_num <= 0xfff) && (test_num > file_num) )
	  file_num = test_num;
      }
      find_result = findnext(mask, dirp, &ffb);
      //cout << "findnext returned " << find_result << endl;
    } while (find_result == 0);
  }
  return file_num;
}

/** Make numbered file name extension.
\param c char containing single letter prefix for extension.
\param n int containing number. Will be converted to hex in extension.
\param[out] ext char array containing the extension on return.
*/
void make_ext(const char c, const int n, char * ext)
{
  if ((n < 0) || (n > 0xfff))
  {
    cerr << "** File extensions numbers must be in the range 0 .. 0xff\n";
    cerr << "   make_ext was passed " << std::hex << n << endl;
    exit(1);
  }
  sprintf(ext, ".%c%03x", c , n);
}

int findfirst(const char* mask, DIR** dirp, struct dirent** _direntp)
{
  struct dirent* direntp = *_direntp;
  *dirp = opendir(".");
  if (!dirp)
  {
    perror("opendir() failed");
    //exit(1);
    return(-1);
  }
  do
  {
    direntp = readdir(*dirp);
    if (direntp && (strstr(direntp->d_name, mask)) )
    {
       *_direntp = direntp;
       return(0);
    }
  } while  (direntp);
  *_direntp = direntp;

  closedir(*dirp);
  return(-1);
}

int findnext(const char* mask, DIR* dirp, struct dirent** _direntp)
{
  struct dirent* direntp = *_direntp;
  do
  {
    direntp = readdir(dirp);
    if (direntp && (strstr(direntp->d_name, mask)) )
    {
      *_direntp = direntp;
       return(0);
    }
  } while  (direntp);

  *_direntp = direntp;
  closedir(dirp);
  return(-1);
}

#undef TEST_CODE
#ifdef TEST_CODE
int main(int argc, char *argv[])
{
  int ext_no = last_ext_number(argv[1], 'p');
  cout << "last ext number for " << argv[1] << " = " 
       << ext_no << endl;
}

#endif  // TEST_CODE
