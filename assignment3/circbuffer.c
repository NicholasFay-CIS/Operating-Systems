#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>

/*STRUCTS for circular buffer and topic entry*/

typedef struct {
	int entryNum;
	struct timeval timeStamp;
	int pubID;
	char photoURL[1000];
	char photoCaption[1000];
} topicEntry;

typedef struct {
	char name[50];
	int e_Num;
	int head;
	int tail;
	int max_entries;
	topicEntry * const buffer;
	pthread_mutex_t m_lock;
} TEQ;

//Init the registry
int cbrs = 100;
TEQ *registry[100];
int Delta = 5;

/* Macro initializers for the structs above */
#define init_te(n, url, caption)            \
	topicEntry n = {					    \
		.pubID = 0,					        \
		.photoURL = url,					\
		.photoCaption = caption,			\
		.entryNum = 0					    \
	};

topicEntry null = {-1};
#define init_cb(n, max, queuename)          \
	topicEntry my_bf[max+1];		        \
	TEQ n = {					            \
		.buffer = my_bf,					\
		.head = 0,						    \
		.tail = 0,						    \
		.e_Num = 1,					        \
		.name = queuename,				    \
		.max_entries = max 				    \
	};										\
	n.buffer[max] = null;					

/*-------------------------------------------*/
int enqueue(char *TEQ_ID, topicEntry TE) {
	int i;
	int found = -1;
	time_t entrytime;
	TEQ *cb;
	for(i = 0; i < cbrs; i++) {
		if(strcmp(registry[i]->name, TEQ_ID) == 0) {
			cb = registry[i];
			found = i;
			break;
		}
	}
	if(found == -1) {
		return 0;
	}
	if(cb->buffer[cb->tail].entryNum == -1) {
		return 0;

	} else {
		TE.entryNum = cb->e_Num;
		gettimeofday(&TE.timeStamp, NULL);
		cb->buffer[cb->tail] = TE;
		if(cb->tail + 1 > cb->max_entries) {
			cb->tail = 0;
		} else {
			cb->tail++;
		}
		cb->e_Num++;
	}
	return 1;
}
/*--------------------------------------------------*/
int thread_safe_enqueue(char *TEQ_ID, topicEntry TE) {
	int i, fail;
	int found = -1;
	time_t entrytime;
	TEQ *cb;
	for(i = 0; i < cbrs; i++) {
		if(strcmp(registry[i]->name, TEQ_ID) == 0) {
			cb = registry[i];
			found = i;
			break;
		}
	}
	//if the registry does not have the given queue name
	if(found == -1) {
		return 0;
	}
	pthread_mutex_lock(&registry[i]->m_lock);
	fail = enqueue(TEQ_ID, TE);
	pthread_mutex_unlock(&registry[i]->m_lock);
	if(fail	== 0) {
		return 0;
	} else {
		return 1;
	}
}
/*-----------------------------------------------*/
int getEntry(char *TEQ_ID, int entrynumber, topicEntry *topic) {
	int i;
	int found = -1;
	TEQ *te;
	for(i = 0; i < cbrs; i++) {
		if(strcmp(registry[i]->name, TEQ_ID) == 0) {
			te = registry[i];
			found = i;
			break;
		}
	}
	if(found == -1) {
		return 0;
	}
	if(te->tail == te->head) {
		return 0;
	}
	int k = 0;
	for(k; k < te->max_entries; k++) {
		if(te->buffer[k].entryNum == entrynumber) {
			if(k == te->max_entries) {
				strcpy(topic->photoCaption,te->buffer[te->head].photoCaption);
				strcpy(topic->photoURL,te->buffer[te->head].photoURL);
				topic->pubID = te->buffer[te->head].pubID;
				topic->entryNum = te->buffer[te->head].entryNum;
				topic->timeStamp = te->buffer[te->head].timeStamp;
			} else {
				strcpy(topic->photoCaption, te->buffer[k+1].photoCaption);
				strcpy(topic->photoURL, te->buffer[k+1].photoURL);
				topic->pubID = te->buffer[k+1].pubID;
				topic->entryNum = te->buffer[k+1].entryNum;
				topic->timeStamp = te->buffer[k+1].timeStamp;

			}
		}
	}
	return 1;
}
/* ------------------------------------------------------------------------ */
int dequeue() {
	int i = 0;
	for(i; i < cbrs; i++) {
		int j;
		if(registry[i] != NULL) {
			j = registry[i]->head;
			for(j; j < registry[i]->tail; j++) {
				printf("checking position %d\n", j);
				if(strcmp(registry[i]->buffer[j].photoCaption, "\0") != 0) {
					struct timeval cur_time;
					gettimeofday(&cur_time, NULL);
					double elapsed = (cur_time.tv_sec - registry[i]->buffer[j].timeStamp.tv_sec) + ((cur_time.tv_usec - registry[i]->buffer[j].timeStamp.tv_usec)/1000000.0);
					if(elapsed >= Delta) {
						strcpy(registry[i]->buffer[j].photoCaption, "\0");
						strcpy(registry[i]->buffer[j].photoURL, "\0");
						registry[i]->buffer[j].entryNum = 0;
						registry[i]->buffer[j].pubID = 0;
						registry[i]->head++;
					}
				}
			}
		} else {
			continue;
		}
	}
}
/*----------------------------------------------------------------------------------------------*/

/* Circular Buffer Enqueue and Dequeue Functions */
int main(int argc, char *argv[]) {
	int max = 10;
	init_cb(Test, max, "Test");
	init_te(te, "url", "caption");
	init_te(te2, "url2", "caption2");
	registry[0] = &Test;
	enqueue(Test.name, te);
	enqueue(Test.name, te2);
	topicEntry topic;
	getEntry("Test", 1, &topic);
	sleep(6);
	dequeue();
	printf("Head is at %d\n", Test.head);
	printf("Head value is %s\n", Test.buffer[Test.head].photoCaption);
}