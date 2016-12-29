#include "shell.h"

int write_prompt(char *prompt)
{
    struct passwd *pwd;
    char hostname[HOSTLEN];
    char pathname[PATHNAMELEN];
    int length;

    pwd = getpwuid(getuid());
    getcwd(pathname, PATHNAMELEN);
    if(gethostname(hostname, HOSTLEN)==0)
        sprintf(prompt,"%s@%s:", pwd -> pw_name, hostname);
    else
        sprintf(prompt,"%s@unknown:",pwd -> pw_name);
    
    length = strlen(prompt);
    if(strlen(pathname) < strlen(pwd->pw_dir) || 
            strncmp(pathname,pwd->pw_dir,strlen(pwd->pw_dir))!=0)
        sprintf(prompt+length,"%s",pathname);
    else
        sprintf(prompt+length,"~%s",pathname+strlen(pwd->pw_dir));

    length = strlen(prompt);
    if(geteuid()==0)
        sprintf(prompt+length,"# ");  /* super process */
    else
        sprintf(prompt+length,"$ ");
    return 0;
    
}

/***************************
int main(void)
{
    char prompt[50];

    write_prompt(prompt);

    printf("%s\n", prompt);

    return 0;
}
****************************/
