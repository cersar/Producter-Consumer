/*
 * main.c
 *
 *  Created on: Jun 17, 2018
 *      Author: yangsen
 */
#include "buffer.h"
#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<stdlib.h>
#include <semaphore.h>
/* the buffer */
buffer_item buffer[BUFFER_SIZE];
int in = 0, out = 0;

pthread_mutex_t mutex;
sem_t empty, full;

int insert_item(buffer_item item) {
	/* insert item into buffer
	 return 0 if successful, otherwise
	 return -1 indicating an error condition */
	buffer[(in++) % BUFFER_SIZE] = item;
	return 0;
}
int remove_item(buffer_item *item) {
	/* remove an object from buffer
	 placing it in item
	 return 0 if successful,otherwise
	 return -1 indicating an error condition */
	*item = buffer[(out++) % BUFFER_SIZE];
	return 0;
}

void *producer(void *param) {
	int ind = *((int *) param);
	buffer_item item;
	int cnt = 0;
	while (cnt < 5) {
		cnt++;
		item = rand() % 10 + 1;
		/* sleep for a random period of time */
		sleep(item);
		/* generate a random number */

		sem_wait(&empty);
		pthread_mutex_lock(&mutex);
		if (insert_item(item))
			printf("report error condition");
		else
			printf("producer %d produced %d ,cost %ds\n", ind,item,item);
		printf("producer %d cnt %d\n", ind,cnt);
		pthread_mutex_unlock(&mutex);
		sem_post(&full);
	}
	pthread_exit(0);
}
void *consumer(void *param) {
	int ind = *((int *) param);
	buffer_item item;
	int time;
	int cnt = 0;
	while (cnt < 2) {
		cnt++;
		sem_wait(&full);
		pthread_mutex_lock(&mutex);
		if (remove_item(&item))
			printf("report error condition");
		else
			printf("consumer %d start consume %d \n", ind,item);
		pthread_mutex_unlock(&mutex);
		sem_post(&empty);
		/* sleep for a random period of time */
		time = rand() % 5 + 1;
		sleep(time);
		printf("consumer %d consumed %d ,cost %ds\n", ind,item,time);
		printf("consumer %d cnt %d\n", ind,cnt);
	}
	pthread_exit(0);
}

int main() {
	pthread_t tid_c[5], tid_p[2]; /* the thread identifier */
	pthread_attr_t attr; /* set of thread attributes */
	int index_c[5], index_p[2];

	sem_init(&empty, 0, BUFFER_SIZE);
	sem_init(&full, 0, 0);
	pthread_mutex_init(&mutex, NULL);
	pthread_attr_init(&attr);

	srand(time(NULL));

	for (int i = 0; i < 5; i++) {
		index_c[i] = i + 1;
		pthread_create(&tid_c[i], &attr, consumer, &index_c[i]);
	}

	for (int i = 0; i < 2; i++) {
		index_p[i] = i + 1;
		pthread_create(&tid_p[i], &attr, producer, &index_p[i]);
	}

	for (int i = 0; i < 5; ++i) {
		pthread_join(tid_c[i], NULL);
	}

	for (int i = 0; i < 2; ++i) {
		pthread_join(tid_p[i], NULL);
	}

	return 0;
}
