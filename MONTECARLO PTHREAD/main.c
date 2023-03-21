#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/* Cantidad de hilos a crear */
#define NTHS 8
#define N 1000000

struct Punto {
	float x, y;
};

/* Procedimiento a ejecutar como hilo */
void * crear_punto(void * memarg){
	/* La memoria que nos mandan es de un entero */
	unsigned int* seed = (unsigned int *) memarg;
	unsigned int * puntos = malloc(sizeof(unsigned int));

	float max = ((float) RAND_MAX)/2.0f;

	for (int i = 0; i < N; ++i) {
		struct Punto punto = (struct Punto) {
			.x = ((float) rand_r(seed) / max) - 1,
			.y = ((float) rand_r(seed) / max) - 1
		};

		if(punto.x * punto.x + punto.y * punto.y < 1) {
			(*puntos)++;
		}
	}

	pthread_exit(puntos);
}

int main(int argc, char **argv) {
	/* Nombre de los threads */
	pthread_t ths[NTHS];

	/* Argumentos de los hilos */
	int args[NTHS];

	for(int i = 0; i < NTHS; i++)
		args[i] = i;

	/* Crear NTHS hilos */
	for(int i = 0; i < NTHS; i++)
		if(pthread_create(&ths[i], NULL, crear_punto, (void *) (&args[i]))
		   != 0){
			perror("Falló la creación de un hilo");
			exit(EXIT_FAILURE);
		}

	unsigned int cant_adentro = 0;

	/* Esperamos a que todos los threads terminen */
	for(int i = 0; i < NTHS; i++){
		unsigned int * puntos;

		if(pthread_join(ths[i] , (void **) &puntos)
		   != 0){
			perror("Falló la espera de un hilo");
			exit(EXIT_FAILURE);
		}

		cant_adentro += *puntos;
	}

	float pi = 4*(cant_adentro / (float)(NTHS * N));

	printf("PI: %f", pi);

	return 0;
}