//
// shell.cpp
//  CS232 Command Shell
//
//  Created by Josiah Ryan March 1st 2021
//  Copyright (c) 2021 Josiah Ryan. All rights reserved.
//

#include <unistd.h>
#include <stdlib.h>
#include <string>
#include "shell.h"
#include "prompt.cpp"
#include "commandline.cpp"
#include "path.cpp"
#include <sys/wait.h>
#include <sys/stat.h>  // for isValidPath()

// from: https://www.systutorials.com/how-to-test-a-file-or-directory-exists-in-c/#:~:text=To%20test%20whether%20a%20file,and%20check%20its%20return%20value.
// returns: true if the path exists
bool isPathExist(const string &s)
{
  struct stat buffer;
  return (stat (s.c_str(), &buffer) == 0);
}


Shell::Shell()
{
}


/* run() is the main loop of the command shell project.  
 *   It displays the current path + $
 *   gets a user input, and parses that input
 *   if it is a valid command it executes the command
 */
void Shell::run()
{
    string prompt;
    while(1) {
        // gets current working directory and outputs it 
        Prompt p;
        prompt = p.get(); 
        cout << prompt << flush;

        //reads in user command and parses it
        CommandLine cl(cin);    
        string command = cl.getCommand();  // typecasts so the comparison on the next line can work

        if        (command == "pwd") { // prints the working directory
            cout << p.getCwd() << "\n" << flush;

        } else if (command == "exit") { // exit command line
            cl.~CommandLine();
            exit(-1);

        } else if (command == "cd") {  // move to specified path / directory
            string userGivenPath = cl.getArgVector(1);

            if (isPathExist(userGivenPath)) {  // == valid
                const char * path = userGivenPath.c_str();
                chdir(path);

            } else {  //invalid path given
                cout << cl.getArgVector(1) << " Error: invalid path given" << endl;
            }       

        } else {    // treats it as an executable file
            string userGivenPath = cl.getArgVector(0);
            pid_t pid = fork();  // creates a child process and stores its id in pid

            if (pid == 0) {  //child process
                Path path;   // use to execute a file
                int pathLocation = path.find(userGivenPath);  // gets path location in the paths vector
                if (pathLocation != -1) {  // if path exists
                    char * const * argv = cl.getArgVector();
                    char * const * envp = cl.getEnvp();  //envp

                    const char * path = userGivenPath.c_str(); // path to file to execute

                    execve(path, argv, envp);
                    cout << " executed!\n" << flush;

                } else {            // path is invalid
                    // prints out error message, and valid paths
                    cout << "Error: invalid path given" << endl;
                    cout << "valid paths include:" << endl;
                    for (int i=0; i< path.getDirectorySize(); i++) {
                        cout << path.getDirectory(i) << endl;

                    } // end for loop
                } // end else

                //exits child process
                exit(0);  

            } // if (pid == 0)

            // if there is no ampersand, the patent process waits
            if (cl.noAmpersand()) {
                int status;
                waitpid(pid, &status, 0);
            }  // end if 

        } // end else of if (command == "pwd" or "exit" or "cd")
    } // while(1) loop
} // run()
