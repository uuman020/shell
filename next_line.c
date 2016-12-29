#include "shell.h"

static char *mollac_to_cpy(char *source){

    char *dest;
    
    dest = (char *)malloc(sizeof(source));
    if(dest == (char *)NULL){
        fprintf(stderr,"no memory");
        exit(1);
    }

    strcpy(dest, source);
    return dest;
}

static char *makestring(char *start, int len)
{
    char *str;

    str = (char *)malloc((len + 1) * sizeof(char));
    if(str == (char *)NULL){
        fprintf(stderr, "no memory");
        exit(1);
    }

    strncpy(str, start, len);
    str[len] = '\0';

    return str;
}

int info_init(struct parse_info *info)
{
    int i;
    
    info -> flag = 0;
    info -> command1 = (char *)NULL;
    info -> command2 = (char *)NULL;
    info-> out_file = (char *)NULL;
    info -> in_file = (char *)NULL;

    for(i = 0; i < MAXARGS; i++){
        info -> params1[i] = (char *)NULL;
        info -> params2[i] = (char *)NULL;
    }
    return 0;
}


int split(char *line, char **args)
{
    char *start, *pos_line;
    int len, count;

    if(*line == '\0'){
        return 0;
    }
    
    count = 0;
    pos_line = line;
    while(*pos_line != '\0'){

        while(*pos_line == ' ' || *pos_line == '\t'){
            pos_line++;
        }
        
        if(*pos_line == '\0'){
            args[count] = (char *)NULL;
            return count;
        }

        len = 0; /* reset */
        start = pos_line;
        while(*pos_line != '\0' && *pos_line != ' ' && *pos_line != '\t'){
            pos_line++;
            len++;
        }
        args[count++] = makestring(start, len);
        
        if(*pos_line == '\0'){
            args[count] = (char *)NULL;
            return count;
        }
    }
}


int parse(struct parse_info *info, char **args, int args_num)
{
    int i;
    char **pos_pram1, **pos_pram2;

    if(strcmp(args[args_num - 1], "&") == 0){
        info -> flag |= SH_BACKGROUND;
        args[args_num - 1] = (char *)NULL;
        args_num--;     /*drop out "&" */
    }

    i = 0;
    pos_pram1 = info -> params1;
    pos_pram2 = info -> params2;
    while(i < args_num){

        if(i == 0){
            info -> command1 = mollac_to_cpy(args[i]);
            *pos_pram1++ = mollac_to_cpy(args[i]);
            free(args[i]);

            i += 1;
        }

        else if(strcmp(args[i], ">") == 0){

            info -> flag |= SH_ISOUT_REDIRECT;
            info -> out_file = mollac_to_cpy(args[i + 1]);

            free(args[i]);
            free(args[i + 1]);

            i += 2;
        }

        else if(strcmp(args[i], ">>") == 0){

            info -> flag |= SH_ISOUT_REDIRECT_APPEND;
            info -> out_file = mollac_to_cpy(args[i + 1]);

            free(args[i]);
            free(args[i + 1]);

            i += 2;
        }

        else if(strcmp(args[i], "<") == 0){

            info -> flag |= SH_ISIN_REDIRECT;
            info -> in_file= mollac_to_cpy(args[i + 1]);
            
            free(args[i]);
            free(args[i + 1]);

            i += 2;
        }
        
        else if(strcmp(args[i], "|") == 0){

            info -> flag |= SH_ISPIPED;
            free(args[i]);

            info -> command2 = mollac_to_cpy(args[i + 1]);
            *pos_pram2++ = mollac_to_cpy(args[i + 1]);
            free(args[i + 1]);

            i += 2;

            while(i < args_num){
                *pos_pram2++ = mollac_to_cpy(args[i]);
                free(args[i]);
                *pos_pram2 = (char *)NULL;

                i += 1;
            }
        }

        else{
            *pos_pram1++ = mollac_to_cpy(args[i]);
            free(args[i]);
            *pos_pram1 = (char *)NULL;

            i += 1;
        }
    }
    return 0;
}

int free_info(struct parse_info *info)
{
    char **str;

    free(info -> command1);
    free(info -> command2);

    info -> flag = 0;

    free(info -> out_file);
    free(info -> in_file);

    str = info -> params1;
    while(*str++){
        free(*str);
    }

    str = info -> params2;
    while(*str++){
        free(*str);
    }

    return 0;
}
