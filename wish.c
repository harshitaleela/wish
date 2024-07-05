#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>

#define DEBUG_LOG 0

struct command 
{
	//structure to store parsed command
	char **tokens;
	int wordLength;
};

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

int main(int argc, char *argv[])
{
	char *cmd;
	size_t size = 50;
	struct command com;
	int rc;

	do
	{
		printf("wish> ");
		getline(&cmd, &size, stdin);
		cmd[strlen(cmd)-1] = '\0';
		com = parse(cmd);
		
		if (DEBUG_LOG)
		{
			int i = 0;
			while (i < com.wordLength)
			{
				printf("%s\n", com.tokens[i]);
				i++;
			}
		}
		if(com.wordLength>0)
		{
			rc = fork();

			if (rc<0)
			{	
				fprintf(stderr, "Fork failed\n");
				exit(1);
			}

			else if (rc==0)
			{
				char **myargs = malloc ( (com.wordLength+1) * sizeof(char *) );
				for (int i=0; i<com.wordLength; i++)
					myargs[i]=strdup(com.tokens[i]);
				myargs[com.wordLength]='\0';
				execvp (myargs[0], myargs);
			}

			else 
			{
				int rc_wait = wait(NULL);
			}

		}
		
	} while (strcmp(com.tokens[0], "exit"));
	free(cmd);
	free(com.tokens);

	return 0;
}
