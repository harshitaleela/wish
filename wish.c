#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/wait.h>

#define DEBUG_LOG 0

struct command 
{
	//structure to store parsed command
	char **tokens;
	int wordLength;
};

/******************************************************************************************************************
                                                DEBUG FUNCTIONS
 ******************************************************************************************************************/


void debug_parse(struct command com)
{
        int i = 0;
        printf("%d", com.wordLength);
        while (i < com.wordLength)
        {
                printf("%s\n", com.tokens[i]);
                i++;
        }
}


/*****************************************************************************************************************
                                              DEBUG FUNCTIONS END
 *****************************************************************************************************************/


struct command parse(char *str)
{
	char *cmd, **words = NULL, *token;
	struct command com;
	cmd = strdup(str);
	int i = 0, j = 0;
	while (token = strsep(&cmd, " "))
	{
		if (strlen(token)>0)
		{
			words = realloc(words, (i+1)*sizeof(char*));
			words[i++] = strdup(token);
		}
	}
	com.tokens = words;
	com.wordLength = i;
	return com;
}


char *buildpath(struct command cmd)
{
	char *path=malloc(sizeof(char));
	path[0]='\0';
	for(int i=1; i<cmd.wordLength; i++)
	{
		if(realloc(path, strlen(cmd.tokens[i]))==NULL)
		{
			fprintf(stderr, "An error occured\n");
			return "error";
		}
		strcat(path, cmd.tokens[i]);
		strcat(path, ":");
	}

	return path;
}


void redirect(struct command *cmd)
{
	for(int i=0; i<cmd->wordLength-1; i++)
	{
		if(cmd->tokens[i][0]== '>')
		{
			if(i!=cmd->wordLength-2)
				fprintf(stderr, "An error occured\n");
			else
			{
				//int fd = open(cmd->tokens[i+1], O_CREAT|O_WRONLY|O_TRUNC, 0777);
				//if(fd < 0)
				//	fprintf(stderr, "File open failed\n");	
				//if(dup2(fd, STDOUT_FILENO<0))	fprintf(stderr, "Redirection failed");

				//fflush(stdout);
				//close(fd);
				
				if(freopen(cmd->tokens[i+1], "w", stdout)==NULL)	 fprintf(stderr, "Cannot write\n");
				cmd->tokens[i]=NULL;
				cmd->tokens[i+1]=NULL;
				cmd->wordLength-=2;
			}

			return;
		}
	}	
}


void executeCommand(struct command com)
{
	 if(com.wordLength>0)
                {
                        int rc = fork();

                        if (rc<0)
                        {
                                fprintf(stderr, "Fork failed\n");
                                exit(1);
                        }

                        else if (rc==0)
                        {
                                char **myargs = malloc ( (com.wordLength+1) * sizeof(char *) );
				
				redirect(&com);

                                for (int i=0; i<com.wordLength; i++)
				        myargs[i]=strdup(com.tokens[i]);
                           	myargs[com.wordLength]='\0';
                                execvp (myargs[0], myargs);
				fprintf(stderr, "An error occured\n");
				exit(1);
                        }

                        else
                        {
                                int rc_wait = wait(NULL);
                        }

                }
}

int executeBuiltin(struct command cmd)
{
	if (strcmp(cmd.tokens[0], "exit")==0)
	{
		if(cmd.wordLength>1)
		{	fprintf(stderr, "An error occured\n");
			return -1;
		}

		else 
			exit(0);
	}

	else if (strcmp(cmd.tokens[0], "cd")==0)
	{
		if(chdir(cmd.tokens[1])<0)
		{
			fprintf(stderr, "An error occured\n");
		}

		return 1;
	}

	else if (strcmp(cmd.tokens[0], "path")==0)
	{
		char *path=buildpath(cmd);

		if(strcmp(path, "error")==0)
			return -1;

		if(setenv("PATH", path, 1)<0)
		{
			fprintf(stderr, "An error occured\n");
		}

		return 1;
	}

	else return 0;
}
		



int main(int argc, char *argv[])
{
	char *cmd;
	size_t size = 50;
	struct command com;

	while(1)
	{
		printf("wish> ");
		getline(&cmd, &size, stdin);
		cmd[strlen(cmd)-1] = '\0';
		com = parse(cmd);

		if(DEBUG_LOG) debug_parse(com);
		
		if(com.wordLength>0)
		{
			if (executeBuiltin(com)==0)	executeCommand(com);
		}
		
	}

	free(cmd);
	free(com.tokens);

	return 0;
}
