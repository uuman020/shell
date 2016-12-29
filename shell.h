#include<readline/readline.h>
#include<sys/wait.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<signal.h>
#include<fcntl.h>
#include<unistd.h>
#include<pwd.h>

#define MAXARGS       20
#define MAXBACKJOB    256

#define HOSTLEN       256
#define PATHNAMELEN   256
#define PROMPTLEN     512

#define SH_ISPIPED                1
#define SH_ISOUT_REDIRECT         2
#define SH_ISOUT_REDIRECT_APPEND  4
#define SH_ISIN_REDIRECT          8
#define SH_BACKGROUND             16

struct parse_info{
    int flag;
    char *command1;
    char *params1[MAXARGS];
    char *command2;
    char *params2[MAXARGS];
    char *out_file;
    char *in_file;
};

int get_line(char *); 
/*
 * return numbers of chars
 */


int split(char *, char **);
    /*
     * args : line which be split, address of args to save
     * return: number of args
     */

int info_init(struct parse_info *);
/*
 * initiallize struct parse_info
 */

int parse(struct parse_info *, char **, int);
/*
 * get information for exec
 * input : address of parse_info, args, number of args
 */

int free_info(struct parse_info *);
/*
 * free memory 
 */


int fork_to_exec(struct parse_info *);
/*
 * fork and execvp
 */

int sig_setup();
/*
 * set sigint and sigquit ign
 * set sigchild to deal with background process
 */

int write_prompt(char *);
/* 
 * get pathname, hostname... ,like bash
 */



