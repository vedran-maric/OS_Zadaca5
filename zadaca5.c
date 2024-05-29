#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

sem_t semafor[5]; //semafor za filozofe
sem_t mutex; //semafor medjusobnog iskljucivanja
int radnja[3]; //1 == gladan, 2 == jede, 3 == misli

void* filozof(void* id_filozofa){
	/*Vrtimo u krug filozofa i njegove radnje*/
	int broj_filozofa = *((int *)id_filozofa);
	while(1){
		int broj_filozofa = *((int *)id_filozofa);
		sleep(2);
		uzima_vilicu(broj_filozofa);
		sleep(0);
		vrati_vilice(broj_filozofa);
	}
}

void moze_li_jesti(int broj_filozofa){

	/*Provjerava jedu li susjedni filozofi*/
	if(radnja[broj_filozofa] == 1 && radnja[(broj_filozofa + 4) % 5] != 2 && radnja[(broj_filozofa + 1) % 5] != 2 ){
		radnja[broj_filozofa] = 2;
		printf("X - Filozof %d uzima vilice i jede.\n", broj_filozofa + 1);
		fflush(stdin);
		sleep(1);
		sem_post(&semafor[broj_filozofa]);
	}
}

void uzima_vilicu(int broj_filozofa){
	sem_wait(&mutex);
	radnja[broj_filozofa] = 1;

	/*Postaje gladan*/
	printf("o - Filozof %d je gladan.\n", broj_filozofa + 1);
	moze_li_jesti(broj_filozofa);
	sem_post(&mutex);
	sem_wait(&semafor[broj_filozofa]);
	fflush(stdin);
	sleep(1);
}

void vrati_vilice(int broj_filozofa){
	sem_wait(&mutex);

	/*Filozof nastavlja misliti*/
	printf("O - Filozof %d ostavlja vilice i nastavlja misliti.\n", broj_filozofa + 1);
	fflush(stdin);
	sleep(1);
	radnja[broj_filozofa] = 3; 

	/*Provjera za susjedne filozofe mogu li sada jesti*/
	moze_li_jesti((broj_filozofa + 4 ) % 5);
	moze_li_jesti((broj_filozofa + 1 ) % 5);
	sem_post(&mutex);	
}


void main(){
	/*Kreiranje threadova*/
	pthread_t thread_id[5];
	sem_init(&mutex, 0, 1);
	int broj_filozofa;

	/*Kreiranje semafora*/
	for(int i=0; i < 5; i++){
		sem_init(&semafor[i],0,0);
		radnja[i] = 3; //svi filozofi se postavljaju da misle
	}
	/*Kreiranje filozofa*/
	for(int i = 0; i < 5; i++){
		int *broj_filozofa = (int *) malloc(sizeof(int));
		*broj_filozofa = i;
		pthread_create(&thread_id[i], NULL, filozof,  broj_filozofa);
		printf("O - Filozof %d misli.\n", i + 1);
	}
	/*Gasenje threadova*/
	for(int i = 0; i < 5; i++){
		pthread_join(thread_id[i], NULL);
	}
	
}