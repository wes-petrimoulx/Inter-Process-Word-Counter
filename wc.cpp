#include <iostream>
#include <fstream>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>
#include <sstream>

using namespace std;

//data structure used to send data
//over the pipe from child to parent
struct data {
    string argu1;
    int line;
    int word;
    int byte;
} mine;

int main(int argc, char *argv[]){
    
    //declaration of all my variables
    //sorry we got a lot of variables
    int totalWords = 0;
    int totalLines = 0;
    int totalBytes = 0;
    int p[2];
    int words = 0;
    int lines = 0;
    int bytes = 0;
    string word;
    string line;
    ifstream myfile;
    pid_t pid;


    //goes through all arguments proivded
    for(int x = 1; x < argc; x++){
        //instantiate the pipe and fork
        pipe(p);
        pid = fork();
        

        //error with fork, exit if true
        if(pid < 0){
            perror("fork error");
            exit(1);
        }

        else if(pid == 0){
            
            //close the read pipe because it is not used
            close(p[0]);

            //gets the number of lines in the file
            //and prints it to the terminal
            myfile.open(argv[x]);
            while (getline(myfile, line)){
                lines++;
                totalLines++;
            }
            myfile.close();

            //gets the number of words in the file
            //and prints it to the terminal
            myfile.open(argv[x]);
            while (!myfile.eof()){
                myfile >> word;
                words++;
                totalWords++;
            }
            myfile.close();

            //gets the number of bytes in the file
            //and prints it to the terminal
            myfile.open(argv[x]);
            myfile.seekg(0, ios::end);
            bytes = myfile.tellg();
            totalBytes += bytes;
            myfile.close();
            
            //adds all data used by the child
            //into the struct to be sent to parent
            mine.argu1 = argv[x];
            mine.line = lines;
            mine.word = words;
            mine.byte = bytes;
            
            //sends data to the parent in struct format
            write(p[1], &mine, sizeof(mine));
            close(p[1]);
            exit(13);
        }
        else{
            close(p[1]);
            read(p[0], &mine , sizeof(mine));
            cout << "   " << mine.line << " " << mine.word << " " << mine.byte << " " << mine.argu1 << endl;
            totalLines += mine.line;
            totalWords += mine.word;
            totalBytes += mine.byte;
            int status;
            pid = wait(&status);
            close(p[0]);
        }
        
    }      
    //print total for all files used.
    cout <<  "   " << totalLines << " " << totalWords << " " << totalBytes << " Total" << endl;
    return 0;
}