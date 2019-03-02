/* ch.c.
 * auteur: Mohamed Sarr & Kevin P. Kombate
 * date: 8 Février 2019
 * problèmes connus:
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

size_t BUFFER_SIZE = 1024;
int procedure(char* string);

/**
 * Parse l'input en liste d'arguments.
 * @param input
 * @param sep
 * @return
 */
char **parseInput(char *input, char * sep){

    int buffersize = BUFFER_SIZE;
    int position = 0;
    char **args = malloc (sizeof(char)*buffersize);
    char *token;

    if (args == NULL){
        printf("malloc failed !!!\n");
        exit(-1);
    }

    /* Le premier mot */
    token = strtok (input, sep);

    while(token != NULL){
        args[position] = token;
        position++;

        if(position >= buffersize){
            buffersize += BUFFER_SIZE;
            args = realloc (args, buffersize * sizeof(char));

            if(args == NULL){
                printf("realloc failed !!!\n");
                free(args);
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, sep);
    }
    args[position] = NULL;

    return args;
}


/**
 * Fonction d'exécution inspiré de la source cité ci-dessous.
 * Source: https://brennan.io/2015/01/16/write-a-shell-in-c/
 * @param args
 * @return
 */
int execute(char **args){

    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            return 0;
        }
        exit(EXIT_FAILURE);
    }else if (pid < 0) {
        // Error forking
        return 1;
    }else {
        // Parent process
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        }while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    
    if(status == 0){
    	return 0 ;
    }
    else {
    	return 1;
    }
}


/**
 * Exécute les commandes dans le tableau d'arguments.
 * @param args
 * @return
 */
int launcher(char **args){
    return execute(args);
}


/**
 * Vérifie si il y a un && ou || dans la string.
 * @param string
 * @return
 */
int et_ou_validator(char *string){

    if((strstr(string, "||")!= NULL) || strstr(string, "&&")){
        return 0;
    }
    return -1;
}


/**
 * Verifie si il y a un IF dans la string.
 * @param string
 * @return
 */
int if_validator(char *string){
    if(strstr(string, "if")!= NULL){
        return 0;
    }
    return -1;
}



/**
 * Fonction pour ||
 * @param string
 */
void ou_procedure(char *string){

    char* debut_command = strtok(string, "|");
    char* autre_command = strtok(NULL, "|")+1;

    int error_message = procedure(debut_command);

    if(error_message != 0){
        procedure(autre_command);
    }

}


/**
 * Fonction pour &&
 * @param string
 */
void et_procedure(char *string){

    char* debut_command = strtok(string, "&");
    char* autre_command = strtok(NULL, "&")+1;

    int error_message = procedure(debut_command);

    if(error_message == 0){
        procedure(autre_command);
    }

}

/**
 * Fonction qui compte le nombre de virgule 
 * @param input
 * @return x
 */
int nbreVirgule (char *input){

    int x = 0;
    char *token;
    token = strtok (input, ";");

    while(token != NULL){
        token = strtok(NULL, ";");
        x++;
    }
    
    return x;
}


/**
 * Fonction pour IF
 * @param input
 */
void if_execution(char *input) {

    char* position_done;

    while(1){
        position_done = strrchr(input, ';');

        if(strcmp(position_done+2,"done")==0){
            break;
        }
    }

    char* debut_if_string = strdup(input);
    char* milieu_if_string = strdup(input);
    char* milieu2_if_string = strdup(input);

    debut_if_string = strtok(debut_if_string, ";");
    milieu_if_string = strtok(NULL, ";");
    milieu2_if_string = strtok(NULL, ";");


    debut_if_string += 3; // Pour enlever le if
    milieu_if_string += 4; // Pour enlever le do
    milieu2_if_string += 1; // Pour enlever l'espace


    if (strstr(milieu2_if_string,"done") ){
        milieu2_if_string = NULL;
    }

    position_done = NULL; // Done == NULL

    /*On verifie si le if est true*/
    int status = procedure(debut_if_string);

    if (status == 0){
        procedure(milieu_if_string);
    }

}




/**
 * Fonction qui s'occupe de chaque procédure développé par notre Shell.
 * @param string
 * @return
 */
int procedure(char* string){

	char *debut_string = strdup(string);
    char *milieu_string = strdup(string);
    char *milieu2_string = strdup(string);
    char *milieu3_string = strdup(string);
	

	if((if_validator(string) == 0) && et_ou_validator(string) != 0){
        if_execution(string);
    }
    else if(et_ou_validator(string) == 0 && if_validator(string)== 0){
    	
    	int i = nbreVirgule(string);
		
		if (i == 3) {
			debut_string = strtok(debut_string, ";");
    		milieu_string = strtok(NULL, ";");
    		milieu2_string = strtok(NULL, "\n");

    		debut_string += 3; // Pour enlever le if
    		milieu_string += 4; // Pour enlever le do
    		milieu2_string += 1; // Pour enlever l'espace


    		

    		int status = procedure(debut_string);

    		if(status == 0){
                    procedure(milieu_string);

    		}
    		else if (status == 1){
    		    
    		}
           
		}

		if (i == 4){
			debut_string = strtok(debut_string, ";");
            milieu_string = strtok(NULL, ";");
            milieu2_string = strtok(NULL, ";");
            milieu3_string = strtok(NULL, "\n");

            debut_string += 3; // Pour enlever le if
            milieu_string += 4; // Pour enlever le do
            milieu2_string += 1; // Pour enlever l'espace
            milieu3_string += 1; // Pour enlever l'espace

            int status = procedure(debut_string);
            if(status == 0){
            	procedure(milieu_string);

            	int status2 = procedure(milieu2_string);
            	if(strcmp(milieu2_string,"true") == 0){
            		procedure(milieu3_string);
            	}

            }
            if(strcmp(debut_string,"false") == 0){

            }

		}
	}
	else if(et_ou_validator(string) == 0){

        char* et = strstr(string, "&&");
        char* ou = strstr(string, "||");

        if(!et){
            ou_procedure(string);
        }

        else if(!ou){
            et_procedure(string);
        }

        else{

            if(et<ou){
                et_procedure(string);
            }
            else{
                ou_procedure(string);
            }

        }
    } 
    else{
        char **args = parseInput(string, " ");
        return launcher(args);
    }
    return 0;
}


/**
 * Fonction main du Shell
 * @return
 */
int main (void){

    char input[BUFFER_SIZE];
    char *pointeur_input = input;

    while(1){
        printf(">_");
        getline(&pointeur_input, &BUFFER_SIZE, stdin);
        strtok(input, "\n");
        procedure(input);
    }

}