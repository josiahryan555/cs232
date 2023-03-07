//
//  commandline.cpp
//  CS232 Command Shell
//
//  Created by Victor Norman on 1/20/15.
//  Copyright (c) 2015 Victor Norman. All rights reserved.
//
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <vector>
#include "commandline.h"
using namespace std;

// #define DEBUGME 1

/**
 * Read in the command line from the user and parse it into argc and argv.
 */
CommandLine::CommandLine(istream &in)
{
    ampersandSeen = false;

    // Read whole line in from the user.
    string line;
    getline(in, line);

    // Make an istringstream which is useful for parsing line into words.
    istringstream iss(line);

    string word;
    // temporary vector of strings that we build up as we parse the cmd line.
    vector<string> tempArgv;
    //vector of good strings to check word against
    vector<string> goodWords = {"cd", "pwd", "exit", "&", ".."};
    //vector<string> goodLocations = {command("ls")}; //surly not the way to do it
    while (getline(iss, word, ' ')) {
#if DEBUGME
        cout << "CmdLine: read word " << word << endl;
#endif
//         // TODO: code here to process/check word. If it is a good word, then:
        // exits if the word is just spaces
        if(word.find_first_not_of(' ') == std::string::npos) { // from: https://stackoverflow.com/questions/6444842/efficient-way-to-check-if-stdstring-has-only-spaces
            cout << "Error: invalid input\n\n" << flush;
            exit(-1);
        }
	    // the code should check if the word is an ampersand, etc.
        if (word == "&") {
            ampersandSeen = true;
        }

        tempArgv.push_back(word);
        //cout << " 1 " << endl; //just for testing
    }

#if DEBUGME
    cout << "CmdLine: done\n";
#endif

    // Copy the vector of strings to a C-style array of char *s.
    copyToArgv(tempArgv);
    tempArgv.clear();
}

/*  this function will strip the first two command line argumets (command, filename) and the ampersand if 
 *  present.  
 *  Return: the stripped argument vecotr
 *  Use:    it is to be used when running the file that they specified, if that file requires arguments
 */
vector<string> CommandLine::CommandLineArgs() const {

    //vector<char *const> returnVec;
    vector<string> returnVec;
    int tempArgC = argc;

    if (ampersandSeen == true) { // arg count -1 if & exists (assumed it is at the end)
        tempArgC -= 1;
    }

    for(int i=0; i < tempArgC; i++)  { // in theory gets all arguments besides the &, if it exists 
        returnVec.push_back(argv[i]);  
    }
    returnVec.push_back(nullptr);  // required last value of vec by execve() in shell.cpp
    return returnVec;
}

void CommandLine::copyToArgv(vector<string> &tempArgv)
{
    argc = (int)tempArgv.size();

    // Malloc space for argv array of char pointers.  Make an extra one for the
    // NULL pointer terminator.
    argv = (char **)malloc(sizeof(char *) * (argc + 1));
    for (int i = 0; i < tempArgv.size(); i++) {
        argv[i] = (char *)malloc(tempArgv[i].size() + 1);
        strcpy(argv[i], tempArgv[i].c_str());
    }
    argv[argc] = NULL; // NULL-terminate the array.
}

CommandLine::~CommandLine()
{
    // free up the space for the argv.
    for (int i = 0; i < argc; i++) {
        free(argv[i]);
    }
    free(argv);
}

//returns a pointer to an array of strings of  key=value
//made to be passed into execve( , , cl.getEnvp())
char **CommandLine::getEnvp() const {
    char **envp;
    extern char **environ;    // code gotten from :https://stackoverflow.com/questions/2085302/printing-all-environment-variables-in-c-c
    int i = 1;
    char *s = *environ;

    //stores all environmental variables in an array 
    for (; s; i++) {  
        envp[i-1] = s;
        s = *(environ+i);
    }
    envp[i-1] = nullptr;  //makes last element of array null (required by execve())
    return envp;
// must end with null ptr
}
