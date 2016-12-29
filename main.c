#include "shell.h"


int main(void)
{
    char *buf = NULL;
    int len;
    char *args[2 * MAXARGS];
    struct parse_info info;
    char prompt[PROMPTLEN];

    sig_setup();
    while(1){

        write_prompt(prompt);
        buf = readline(prompt);

        if((len = split(buf, args)) == 0){  /* no commands */
            free(buf);
            continue;
        }
        free(buf);

        info_init(&info);
        parse(&info, args, len);

        fork_to_exec(&info);
        free_info(&info);

        /* debug
        char **pos;
        char **param;


        pos = args;
        while(*pos){
            printf("%s ", *pos++);
        }
        printf("\n");

        printf("command1 is: %s\n", info.command1);
        param = info.params1;
        printf("params1: ");
        while(*param){
            printf(" %s", *param++);
        }
        printf("\n");

        if(info.flag & SH_ISOUT_REDIRECT){
            printf("out redirect, out file is: %s\n", info.out_file);
        }

        if(info.flag & SH_ISOUT_REDIRECT_APPEND){
            printf("out redirect append, out file is: %s\n", info.out_file);
        }

        if(info.flag & SH_ISIN_REDIRECT){
            printf("in redirect, in file is %s:\n", info.in_file);
        }

        if(info.flag & SH_ISPIPED){
            printf("piped, coomand2 is %s:\n", info.command2);
            param = info.params2;
            printf("params2: ");
            while(*param){
                printf(" %s", *param++);
            }
            printf("\n");
        }

        if(info.flag & SH_BACKGROUND){
            printf("back ground job\n");
        }
        */
    }

    exit(0);
}
