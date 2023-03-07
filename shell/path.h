//
//  path.h
//  CS232 Command Shell
//
//  Created by Josiah Ryan on March 3rd 2021
//  Copyright (c) 2021 Josiah Ryan. All rights reserved.
//

#ifndef __CS232_Command_Shell__path__
#define __CS232_Command_Shell__path__

#include <iostream>
//#include <string>
using namespace std;

class Path
{
public:
    Path(); // construct a Path from the PATH environment variable, 
    int find(const string& program) const;  // - return the index of the directory containing program, or -1 if non-existant
    string getDirectory(int i) const;  // - return the name of the directory whose index is i.
    int getDirectorySize() {return paths.size();}  // returns size of the vector paths

private:
    vector<string> paths;
};

#endif /* defined(__CS232_Command_Shell__path__) */