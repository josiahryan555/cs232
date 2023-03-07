//
// path.cpp
//  CS232 Command Shell
//
//  Created by Josiah Ryan March 1st 2021
//  Copyright (c) 2021 Josiah Ryan. All rights reserved.
//

#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include "path.h"
#include <sys/types.h>
#include <dirent.h>
#include <string.h>


/* Path() - construct a Path from the PATH environment variable, by storing each directory in PATH 
     * in a vector<string> (you may find the getenv() system call to be of use). 
     */
Path::Path()
{
    //stores all paths
    string tempPathHolder = getenv("PATH");  
    // Make an istringstream which is useful for parsing line into words.
    istringstream iss(tempPathHolder);
    string path;

    //stores all individual paths into the 'paths' vector
    while (getline(iss, path, ':')) {  
        paths.push_back(path);
    }
}

/* return: the index of the directory containing program, or -1 if program is in no directory 
 * param: the name of a path
 */
int Path::find(const string& program) const 
{
    //declare some pointers
    const char* path;  //opendir input type
    DIR * dirp;         //opendir return type
    dirent * file;     //readdir return type

    //searches through the paths looking for a directory with program in it
    for(int i=0; i<paths.size(); i++) {
        path = (paths[i]).c_str();  // gets the name of the path to open
        dirp = opendir(path);        // opens the directory
        file = readdir(dirp);        // reads in a dirent structure 
        while (file != nullptr) {   //should check through the directory for a program named 'program'

            if (strcmp(file->d_name, program.c_str()) == 0) {  //if the program is found, the index of the directory is returned
                closedir(dirp);
                return i;    
            }
            file = readdir(dirp); //reads in next directory

        } // end while loop
    } // end for loop 
  return -1;  // item not found
}   // end find() 

// - return the name of the directory whose index is i.
string Path::getDirectory(int i) const 
{ 
    return paths[i];
}

