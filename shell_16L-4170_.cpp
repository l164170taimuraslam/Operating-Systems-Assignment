#include <iostream>
#include <sys/wait.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fstream>
#define KYEL  "\x1B[33m"
#define LINE_LEN 80
#define MAX_ARGS 64
#define MAX_ARGS_LEN 16
#define MAX_PATHS 64
#define MAX_PATH_LEN 16
#define WHITESPACE "\t\n"
using namespace std;
int dir_count;
struct command_t{
		char *name;
		int argc;
		char * argv[MAX_ARGS];
};
int parsePath(char * dirs[])
{
	char * pathEnvVar;
	char * thePath;	
	for (int i=0;i<MAX_ARGS;i++)
	{
		dirs[i]=NULL;
	}
	pathEnvVar=(char *) getenv("PATH");
	thePath=(char *) malloc(strlen(pathEnvVar) +1);
	strcpy(thePath, pathEnvVar);
	int x = 0;
	char *arg = strtok(thePath,":");
	dirs[x++] = arg;
	while(arg != NULL)
	{	
		arg = strtok(NULL,":");
		dirs[x++] = arg;
	}
	dir_count = x - 1;
	return 0;
}
char * lookupPath(char **argv, char **dir)
{
	char * result;
	char * pName = new char[MAX_PATH_LEN];
	//check to see if file name is already an absolute pathname
	if(*argv[0] == '/')
	{
		return argv[0];		
	}

	//Look in PATH directories
	// use acess() to see if the file is in a dir.

	char * buffer;
	bool flag = false;
	for(int i=0;i<dir_count;i++)
	{
		buffer = new char[255];
		strcpy(buffer,dir[i]);
	        strcat(buffer,"/");
		strcat(buffer,argv[0]);
		
		if(access(buffer,F_OK) == 0)
		{	
			//cout << buffer;
			flag = true;
			break;
		}
		
	}
		if(flag == false){cout << "not found";}
		if(flag == true)
		{
			return buffer;
		}
	return NULL;
}

void read(char * buffer)
{
	//gets_s(buffer);
	cout << buffer;
}


void printPrompt()
{
//	cout << "YOU HAVE NOW ENTERED INTO THE SHELL   :)  \n";
	 printf("%sYOU HAVE NOW ENTERED INTO THE SHELL   :)  \n Enter 'quit' to exit\n", KYEL);
}

void parseCommand(char cmdLine[],command_t & comm)
{
	for(int i=0;i<MAX_ARGS;i++)
	{
		comm.argv[i] = NULL;
	}
	int x = 0;	
	char *arg = strtok(cmdLine," ");
	comm.argv[x++] = arg;
	while(arg != NULL)
	{	
		arg = strtok(NULL," ");
		comm.argv[x++] = arg;
	}
	x = x - 1;
	comm.argc = x;
}
int main()
{
	dir_count = 0;
	char * commandLine;
	command_t myCommand;
	char cmdLine[200];
	char * dirs[50];
	parsePath(dirs);
	
	while(1)
	{	

		printPrompt();

		char cwd[1024];//current working directory
   		if (getcwd(cwd, sizeof(cwd)) != NULL)
       		{
			fprintf(stdout, "Current Directory: %s\n", cwd);
		}
	        else
       			perror("getcwd() error");
		
		//intializing command line array to null		
		for(int i=0;i<200;i++)
		{
			cmdLine[i] = '\0';
		}
		cin.getline(cmdLine,200);
		bool flag = false;
		// A check to see if no coomand is given and simple "enter key" is pressed
		for(int i=0;i<200;i++)
		{
			if(cmdLine[i] != '\0')
			{
				flag = true;
				break;
			}
		}		
		if(flag == false){continue;}
		// if all is well then command is passed to be parsed.....
		parseCommand(cmdLine,myCommand);
		if(strcmp(myCommand.argv[0],"cd") == 0)
		{
			chdir(myCommand.argv[1]);
			continue;
		}		
		// A user defined check for exit condition
		if(strcmp(myCommand.argv[0],"quit") == 0)
		{
			break;
		}
		
		//get the full pathname for the file
		
		myCommand.name = lookupPath(myCommand.argv,dirs);
		
		//if command is wrong or doesn't exist then execution is skipped		
		if(myCommand.name == NULL)
		{
			cout << "FILE DOESNT EXISTS \n";
			continue;				
		}
			
		//create a child process and execute it
		int pid = fork();
		if(pid == 0)
		{	
			// command execution
			execv(myCommand.name,myCommand.argv);
		}
		wait(NULL);
		cout << "\n Command Executed Successfully\n";
		//wait for the child to terminate	
	}
	cout << "\n- - - - - - - - - T H E    S H E L L    I S    E X I T I N G - - - - - - - - - \n";
	//shell termination
}
