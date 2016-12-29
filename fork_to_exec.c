#include "shell.h"


static int bg_table[MAXBACKJOB];  /* record of back jobs*/

static void sig_chld_handler(int sig) /* once a back process exit, parent process get a signal, this handler be called */
{
    int i;
    pid_t pid;

    for(i = 0; i < MAXBACKJOB; i++){
        if(bg_table[i] > 0){  /* only concern a back job which doesn't be retrieve */
            if((pid = waitpid(bg_table[i], NULL, WNOHANG)) != 0){
                printf("job %u done\n", pid);
                bg_table[i] = 0; /* clear */
            }
        }
    }
}

static int clear_bg_table() /* should be called in the init state of shell */
{
    int i;

    for(i = 0; i < MAXBACKJOB; i++){
        bg_table[i] = 0;
    }

    return 0;
}

int sig_setup()
{
    clear_bg_table();

    if(signal(SIGINT, SIG_IGN) == SIG_ERR){    /* shell ign signal from keybord */
        perror("set sigint error: ");
        exit(1);
    }
    if(signal(SIGQUIT, SIG_IGN) == SIG_ERR){
        perror("set sigquit error: ");
        exit(1);
    }
    if(signal(SIGCHLD, sig_chld_handler) == SIG_ERR){
        perror("set sigchild error: ");
        exit(1);
    }

    return 0;
}

int fork_to_exec(struct parse_info *info)
{
    int pipe_fd[2];
    pid_t pid, pid2;
    int in_fd, out_fd;
    int i;

    if(strcmp(info -> command1, "exit") == 0){
        printf("about to quit..\n");
        fflush(stdout);  /* not necessary*/
        exit(0);
    }
    
    if((pid = fork()) == -1){
        perror("fork error :");
        exit(1);
    }
    /* process for exec new programs */
    else if(pid == 0){

        /* no matter one or two commands, all of them need to be interupt by the signal from keybord */
        if(signal(SIGINT, SIG_DFL) == SIG_ERR){
            perror("sigint reset error: ");
            exit(1);
        }
        if(signal(SIGQUIT, SIG_DFL) == SIG_ERR){
            perror("sigchild reset error: ");
            exit(1);
        }

        /* deal with two commands */
        if(info -> flag & SH_ISPIPED && !(info -> flag & SH_ISOUT_REDIRECT) && !(info -> flag & SH_ISOUT_REDIRECT_APPEND)){
            if(pipe(pipe_fd) == -1){              /* case out redirect, we don't do pipe*/
                perror("pipe error: ");
                exit(1);
            }

            if((pid2 = fork()) == -1){
                perror("fork2 error: ");
                exit(1);
            }

            else if(pid2 == 0){
                close(pipe_fd[0]); /*command1 doesn't read from pipe*/
                
                if(dup2(pipe_fd[1], STDOUT_FILENO) == -1){
                    perror("dup2 error: ");
                    exit(1);
                }
                close(pipe_fd[1]);

                if(info -> flag & SH_ISIN_REDIRECT){
                    in_fd = open(info -> in_file, O_RDONLY);
                    if(dup2(in_fd, STDIN_FILENO) == -1){
                        perror("dup2 error: ");
                        exit(1);
                    }

                    close(in_fd);
                }

                execvp(info -> command1, info -> params1);
                perror("error: ");
                exit(1);
            }
            else {
                close(pipe_fd[1]); /*command2 doesn't write to pipe*/

                if(dup2(pipe_fd[0], STDIN_FILENO) == -1){
                    perror("dup2 error: ");
                    exit(1);
                }

                close(pipe_fd[0]);

                execvp(info -> command2, info -> params2);
                perror("error: ");
                exit(1);
            }
        }

        /* just one command*/
        else{

            if(info -> flag & SH_ISIN_REDIRECT){
                in_fd = open(info -> in_file, O_RDONLY);
                if(dup2(in_fd, STDIN_FILENO) == -1){
                    perror("dup2 error: ");                       
                    exit(1);
                }

                close(in_fd);
            }
            
            if(info -> flag & SH_ISOUT_REDIRECT || info -> flag & SH_ISOUT_REDIRECT_APPEND){
                if(info -> flag &SH_ISOUT_REDIRECT)
                    out_fd = open(info -> out_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                else
                    out_fd = open(info -> out_file, O_WRONLY | O_CREAT | O_APPEND, 0666);

                if(dup2(out_fd, STDOUT_FILENO) == -1){
                    perror("dup2 error: ");
                    exit(1);
                }

                close(out_fd);
            }

            
            execvp(info -> command1, info -> params1);
            perror("error: ");
            exit(1);
        }
    }

    /* shell process */
    else{
        if(info -> flag & SH_BACKGROUND){
            printf("pid is %u\n", pid); /* only show the pid of last command */
            fflush(stdout);  /* not necessary */
            for(i = 0; i < MAXBACKJOB; i++){
                if(bg_table[i] == 0){
                    bg_table[i] = pid;   /* register a back job */
                }
            }            
        }

        else{
            waitpid(pid, NULL, 0);
        }
    }

    return 0;
}
