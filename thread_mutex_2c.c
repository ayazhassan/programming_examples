#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

void *thread_function(void *arg);
pthread_mutex_t work_mutex;

#define WORK_SIZE 1024
struct work_struct{
	int readcount;
	int lastreadby;
	char work_area[WORK_SIZE];
};
struct work_struct ws;
int time_to_exit = 0;

int main()
{
	int res;
	pthread_t a_thread;
	pthread_t a_thread1;
	void *thread_result;
	int thread_ids[] = {1,2};
	res = pthread_mutex_init(&work_mutex, NULL);
	if(res != 0){
		perror("Mutex Initialization failed");
		exit(EXIT_FAILURE);
	}
	res = pthread_create(&a_thread, NULL, thread_function, &thread_ids[0]);
	if(res != 0){
		perror("Thread creation failed");
		exit(EXIT_FAILURE);
	}
	res = pthread_create(&a_thread1, NULL, thread_function, &thread_ids[1]);
	if(res != 0){
		perror("Thread creation failed");
		exit(EXIT_FAILURE);
	}
	pthread_mutex_lock(&work_mutex);
	while(!time_to_exit){
		printf("Input some text. Enter 'end' to finish\n");
		fgets(ws.work_area, WORK_SIZE, stdin);
		ws.readcount = 0;
		ws.lastreadby = 0;
		pthread_mutex_unlock(&work_mutex);
		while(1){
			pthread_mutex_lock(&work_mutex);
			if(ws.readcount < 2){
				pthread_mutex_unlock(&work_mutex);
				sleep(3);
			}
			else{
				break;
			}
		}
	}
	pthread_mutex_unlock(&work_mutex);
	printf("\nWaiting for thread to finish...\n");
	res = pthread_join(a_thread, &thread_result);
	if(res != 0){
		perror("Thread join failed");
		exit(EXIT_FAILURE);
	}
	res = pthread_join(a_thread1, &thread_result);
	if(res != 0){
		perror("Thread join failed");
		exit(EXIT_FAILURE);
	}
	printf("Thread joined\n");
	pthread_mutex_destroy(&work_mutex);
	exit(EXIT_SUCCESS);
}

void *thread_function(void *arg)
{
	int myid = *(int *)arg;
	sleep(1);
	pthread_mutex_lock(&work_mutex);
	while(1){
		printf("You input %d characters\n", strlen(ws.work_area) -1);
		ws.readcount++;
		ws.lastreadby = myid;
		if(strncmp("end", ws.work_area, 3) == 0) break;
		while(ws.lastreadby >= myid){
			pthread_mutex_unlock(&work_mutex);
			printf("Sleeping...%d\n", myid);
			sleep(1);
			pthread_mutex_lock(&work_mutex);
		}
		
	}
	time_to_exit = 1;
	pthread_mutex_unlock(&work_mutex);
	printf("child thread exit\n");
	pthread_exit(0);
}

