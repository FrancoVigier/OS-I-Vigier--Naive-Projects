#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>

#define buffer_size 1024
#define max_args 500
const char fallo[] = "Fallo";
const char error_fatal[] = "Error fatal";
const char prompt[] = "$ ";

int fork_and_check(char* filename, char * args[]) {
	pid_t p = fork();
	if(p < 0) {
		perror(error_fatal);
		_exit(-1);
	}

	if (p == 0) {
		execv(filename, args);
		_exit(-1);
		//Fin de ejecucion
	} else {
		int status;
		waitpid(p, &status, 0);
		fflush(stdout);

		if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
			printf("%s\n", fallo);
			printf(prompt);
			fflush(stdout);
			_exit(-1);
			//Fin de ejecucion
		} else {
			printf(prompt);
			fflush(stdout);
			_exit(0);
			//Fin de ejecucion
		}
	}
}

int execute(char *file_to_execute, char * args[], bool is_async) {
	if(access(file_to_execute, X_OK) == 0) {
		if(!is_async) {
			execv(file_to_execute, args);
			//Fin de ejecucion
		} else {
			fork_and_check(file_to_execute, args);
			//Fin de ejecucion
		}

		//Exec no reemplazo la imagen del hijo: exec no ejecuto nada
		_exit(-1);
	}

	//Esto es para ejecutar sin el path completo
	//Equivalente a hacer which file_to_execute y ejecutar el resultado

	char* env_path = getenv("PATH");
	char* copy_env_path = malloc(strlen(env_path) + 1);
	strcpy(copy_env_path, env_path);

	char* possible_file_path = strtok(copy_env_path, ":");
	while (possible_file_path != NULL) {
		char* filename = malloc(strlen(possible_file_path) + 1 + strlen(file_to_execute) + 1);
		strcpy(filename, possible_file_path);
		filename[strlen(possible_file_path)] = '/';
		strcpy(filename + 1 + strlen(possible_file_path), file_to_execute);

		if(access(filename, X_OK) == 0) {
			if(!is_async) {
				execv(filename, args);
				//Fin de ejecucion
				break;
			} else {
				fork_and_check(filename, args);
				//Fin de ejecucion
				break;
			}
		}
		free(filename);

		possible_file_path = strtok(NULL, ":");
	}

	free(copy_env_path);

	//Exec no reemplazo la imagen del hijo: exec no ejecuto nada
	_exit(-1);
}

int main() {
	printf("Creado por Franco Ignacio Vallejos Vigier y Facundo Emmanuel Messulam\n");
	printf("Entrada maxima 1023!\n");

	while(true) {
		printf(prompt);
		fflush(stdout);

		char instruction[buffer_size] = {0};

		scanf("%1024[^\n]", instruction);
		int c;
		while ((c = getchar()) != '\n' && c != EOF);

		if(strncmp(instruction, "exit", strlen("exit")) == 0) {
			_exit(0);
			//Fin de ejecucion
		}

		if(instruction[0] == '\0') {
			continue;
		}

		char ** args = calloc(max_args, sizeof(char*));
		int iterador_args = 0;

		char* arg = strtok(instruction, " ");

		while (arg != NULL) {
			char * copyarg = calloc(strlen(arg) + 1, sizeof (char));
			strcpy(copyarg, arg);
			args[iterador_args++] = copyarg;

			arg = strtok(NULL, " ");
		}

		bool async_instruction = false;

		if(*(args[iterador_args - 1]) == '&') {
			iterador_args--;
			free(args[iterador_args]);
			args[iterador_args] = NULL;
			async_instruction = true;
		}

		pid_t p = fork();
		if(p < 0) {
			perror(error_fatal);
			_exit(-1);
		}

		if(p == 0) {
			execute(instruction, args, async_instruction);
			//Fin de ejecucion
		} else {
			for(int i = 0; i < iterador_args; i++) {
				free(args[i]);
			}
			free(args);

			if(!async_instruction) {
				int status;
				waitpid(p, &status, 0);
				if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
					printf("%s\n", fallo);
				}
			}
		}
	}
}
