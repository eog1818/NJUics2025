#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h> 
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// calculate the length of an array
#define ARRLEN(arr) (int)(sizeof(arr) / sizeof(arr[0]))
#define NR_CMD ARRLEN(cmd_table)

#define str_temp(x) #x
#define STR(x) str_temp(x)
typedef __uint8_t uint8_t;

//static int cmd_help(char *args);
static int cmd_Si(char *args){
	return 0;
}

static int cmd_c(char *args) {
//   cpu_exec(-1);
   return 0;
}
 
 
static int cmd_q(char *args) {
   return -1;
 }
 
static int cmd_help(char *args);
 
//static int cmd_Si(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si [N]", "Execute N step and stop, default N = 1 ", cmd_Si }
  /* TODO: Add more commands */

};


static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    //for (i = 0; i < NR_CMD; i ++) {
    for (i = 0; i < 2; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for (i = 0; i < NR_CMD; i ++) {
		if (strcmp(arg, cmd_table[i].name) == 0) {
	        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    	    return 0;
			}

		}
    printf("Unknown command '%s'\n", arg);
	}
  return 0;
}

void cpu_exec(uint8_t n) {
	for (;n > 0; n --) {

		printf("n is %d\n", n);
		
		//execute(n);
	}

}

int main(){
	char *line_read;
	char *str; 
	char *str2;
	//line_read=readline("Enter: ");
	double x;
	printf("step cpu_exec\n");
	cpu_exec(-1);

	//scanf("%lf",&x);

	//str = str_temp(x);
	//str2 = STR(123);
	//printf("%f\n", x);
	//printf("%s\n", STR(x));



	//cmd_help(input);
	//printf("%d\n",*line_read);
	//printf("%s\n",line_read);
	
	//bool x = line_read && *line_read;
	/*
	if (line_read && *line_read) {
    	add_history(line_read);
  	}
	printf("%s\n",line_read);
	
	str=line_read;
	str_end = str + strlen(str);

	char *cmd = strtok(str, " ");

    char *args = cmd + strlen(cmd) + 1;
	
	printf("%s\n",cmd);
	printf("%s\n",args);

	int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { 
			printf("%d", i);
			return 0; 
		}
        break;
      }
    }



	free(line_read);
	*/

	//double a = calc();
  //printf("Here is the result %f.\n", a);
/*
	char *input;
	char *token;
	const char delim[]=" ";


	while (1)	{
		input =	readline("Enter test: ");

		if(input == NULL){
			printf("exit.\n");


		

		}

		if(strlen(input) > 0 ){
			add_history(input);



		}

		//printf("Your entered: %s.\n", input);
		token = strtok(input, delim);
		while(token!=NULL){
			printf("%s\n",token);
			token = strtok(NULL, delim);




		}
		free(input);


	}


*/
	return 0;
}
