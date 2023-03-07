#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>

/* author: Josiah Ryan
 * date  : Feb/22/2021
 * purpose: this program takes an input file and a none-existant ouput file.  Makes the output file and then copies the contents of the input file into the output file.  
 ***************************************************************************/ 

// from: https://stackoverflow.com/questions/4553012/checking-if-a-file-is-a-directory-or-just-a-file
/* description: takes a file and determines if it is a regular file
 * parameters: a file path
 * returns: 1 if the file is regular, and 0 if it is not
 **************************************************************************/
int is_regular_file(const char *path) 
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

/* description: takes an input file and non-existant output file, and copies contents of the input into the output.
 * parameters: an existant source file 'src' & a non-existant file dest
 * returns: 0 if successful, and -1 if unsuccessful
 ****************************************************************************/
int copy(char* src, char* dest) {
  FILE *srcp;
  FILE *destp;

  //opens source file
  // from: https://www.tutorialspoint.com/c_standard_library/c_function_fopen.htm
  srcp = fopen(src, "r");  
  if (srcp == NULL) {//checks if source file exists
    perror("Error: ");      
    exit(-1);
  } else if (is_regular_file(src) != 1) {// checks if it is an accessible file.
    perror("Error: source file entered was not a accessible file");
    exit(-1);
  }
  
  //checks to make sure a file 'dest' does not exist
  if ( access(dest, F_OK) == 0 ) { // if file exists, it exits
    fprintf(stderr, "Error: destination file already exists\n");
    exit(-1);
  } else { //if file does not exist, it makes and opens a new file  
    destp = fopen(dest, "w");
  }
  
  //code taken from 'https://www.geeksforgeeks.org/fgetc-fputc-c/'
  do 
  {
    // Taking input single character at a time
    char c = fgetc(srcp);
 
    // Checking for end of file
    if (feof(srcp))
      break ;
    fputc(c, destp); // writes single character into dest
   }  while(1);
  
  //clean up (closes files)
  fclose(srcp);
  fclose(destp);
  return 0;
}

int main(int argc, char *argv[]) {
  return copy(argv[1], argv[2]);
}

/* homework02.c */
