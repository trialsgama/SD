#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#define MAX_NUM_WORKERS 8
#define MAX_SIZE 2000

int size; /*tamaño del vector*/
int numWorkers; /*numero de workers*/
int stripSize; /*tamaño del strip; size = numWorkers*stripSize*/

int array[MAX_SIZE];
int sum=0;

pthread_mutex_t lock;

void * worker (void * ptr);
double timeval_diff(struct timeval *a, struct timeval *b);



int main(int argc, char* argv[]){

	/*Comenzamos a medir el tiempo*/
	  struct timeval t_ini, t_fin;
	  double secs;
	  gettimeofday(&t_ini, NULL);
	/*------------------------*/

	int i, ids[MAX_NUM_WORKERS];
	pthread_attr_t attr;
	pthread_t workers[MAX_NUM_WORKERS];
	pthread_attr_init(&attr);
	pthread_mutex_init(&lock,NULL);

	/*Lee parámetros de la línea de comando*/
	size = atoi(argv[1]);
	size = (size < MAX_SIZE ? size : MAX_SIZE);
	numWorkers = atoi(argv[2]);
	numWorkers = (numWorkers < MAX_NUM_WORKERS ? numWorkers : MAX_NUM_WORKERS);
	stripSize = size / numWorkers;
	
	/*Inicializa vector*/
	for (i = 0; i < size; i++)
		array[ i ] = i+1;

	/* crea los hilos */
	for (i = 0; i < numWorkers; i++) {
		ids[i] = i;
		pthread_create(&workers[i], &attr, worker, &ids[i]);
	}

	/*Espera a que los hilos terminen*/
	for (i = 0; i < numWorkers; i++)
		pthread_join(workers[i], NULL);

	/*Detiene el contador de tiempo*/
	gettimeofday(&t_fin, NULL);
	secs = timeval_diff(&t_fin, &t_ini);
	/*-------------------------------*/

	/*Imprime en pantall el valor final */
	printf("El resultado es: %d. \n",sum);
	printf("%.16g milliseconds\n", secs * 1000.0);
	return 0;
}

void * worker (void *arg) {
	int *ptr_id = (int*) arg;
	int id = *ptr_id;
	int local_sum, i, first, last;

	/*Determina la primera y ultima posición del bloque a procesar*/
	first = id*stripSize;
	last = first + stripSize - 1;
	printf("Primer bloque %d ,%d \n",first,id);
	printf("ultimo bloque %d ,%d \n",last,id);
	/*Suma los valores de su bloque*/
	local_sum = 0;
	for (i = first; i <= last; i++)
		local_sum += array[i];
		//printf("SUMA %d ,%d \n\n",local_sum,id);
	/*Suma la cuenta local en la cuenta global*/
	pthread_mutex_lock(&lock);
	sum += local_sum;
	pthread_mutex_unlock(&lock);

	pthread_exit (0);

}

/*función para medir el tiempo retorna "a -b" en segundos*/
double timeval_diff(struct timeval *a, struct timeval *b)
{
	return
	  (double)(a->tv_sec + (double)a->tv_usec/1000000) -
	  (double)(b->tv_sec + (double)b->tv_usec/1000000);
}
