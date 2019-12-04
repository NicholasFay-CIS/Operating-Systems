#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>

#define MAXPUBs 5
#define MAXSUBs 5
#define MAXTOPICS 1000

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

typedef struct {
	pthread_t id;
	int flag;
	char *file;
} table_e;

//Init the registry
int cbrs = 100;
TEQ *registry[MAXTOPICS];
table_e pub_t[MAXPUBs];
table_e sub_t[MAXSUBs];
int Delta;

/* Macro initializers for the structs above */
#define init_te(n, url, caption)            \
	topicEntry n = {					    \
		.pubID = 0,					        \
		.photoURL = url,					\
		.photoCaption = caption,			\
		.entryNum = 0					    \
	};

//Macro for struct to init pthread_create arguments
#define enqueue_arg_init(args, name) 				\
	enqueue_arg args = {				   	 		\
		.file = name								\
	}									    		\

#define get_entry_init(init_name, name, entry, topic, th) 	\
	get_entry_arg init_name = {				    			\
		.queuename = name,									\
		.entry_num = entry,									\
		.TE = topic 										\
	}									    				\

//Init of a null topic entry stuct
topicEntry null = {-1};
//Macro that initializes the circular ring buffer
#define init_cb(n, max, queuename)           \
	topicEntry my_bf[max+1];		         \
	TEQ n = {					             \
		.buffer = my_bf,					 \
		.head = 0,						     \
		.tail = 0,						     \
		.e_Num = 1,					         \
		.name = queuename,				     \
		.m_lock = PTHREAD_MUTEX_INITIALIZER, \
		.max_entries = max 				     \
	};										 \
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
	//lock the given buffer
	pthread_mutex_lock(&registry[i]->m_lock);
	//enqueue the topicEntry
	fail = enqueue(TEQ_ID, TE);
	//Unlock the given buffer
	pthread_mutex_unlock(&registry[i]->m_lock);
	//if the enqueue failed return 0
	if(fail	== 0) {
		return 0;
	} else {
		//otherwise return 1 on success
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
		if(te->buffer[k].entryNum >= entrynumber) {
			if(te->buffer[k].entryNum > entrynumber) {
				strcpy(topic->photoCaption, te->buffer[k].photoCaption);
				strcpy(topic->photoURL, te->buffer[k].photoURL);
				topic->pubID = te->buffer[k].pubID;
				topic->entryNum = te->buffer[k].entryNum;
				topic->timeStamp = te->buffer[k].timeStamp;
			}
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
				if(strcmp(registry[i]->buffer[j].photoCaption, "\0") != 0) {
					struct timeval cur_time;
					gettimeofday(&cur_time, NULL);
					double elapsed = (cur_time.tv_sec - registry[i]->buffer[j].timeStamp.tv_sec) + ((cur_time.tv_usec - registry[i]->buffer[j].timeStamp.tv_usec)/1000000.0);
					if(elapsed >= Delta) {
						strcpy(registry[i]->buffer[j].photoCaption, "\0");
						strcpy(registry[i]->buffer[j].photoURL, "\0");
						registry[i]->buffer[j].entryNum = 0;
						registry[i]->buffer[j].pubID = 0;
						if(registry[i]->head == registry[i]->max_entries-1) {
							registry[i]->head = 0;
						} else {
							registry[i]->head++;
						}
					}
				}
			}
		} else {
			continue;
		}
	}
}

/*----------------------------------------------------------------------------------------------*/
void *Publisher(void *args) {
	//printf("%d\n", ((table_e *)args)->id);
	//printf("%s\n", ((table_e *)args)->file);
	pthread_cond_t condition;
	pthread_mutex_t pub_lock;
	pthread_mutex_lock(&pub_lock);
	pthread_cond_wait(&condition, &pub_lock);
	//printf("Publisher Reading from FILE: %s\n", ((enqueue_arg *)args)->file);
	printf("Publisher Thread created::::Thread ID: %d\n", pthread_self());
	pthread_mutex_unlock(&pub_lock);
	return;
}

/*----------------------------------------------------------------------------------------------*/
void *Subscriber(void *args) {
	//printf("%d\n", ((table_e *)args)->id);
	//printf("%s\n", ((table_e *)args)->file);
	pthread_cond_t condition;
	pthread_mutex_t sub_lock;
	pthread_mutex_lock(&sub_lock);
	pthread_cond_wait(&condition, &sub_lock);
	printf("Subscriber Thread created::::Thread ID: %d\n", pthread_self());
	return;
}
/*----------------------------------------------------------------------------------------------*/
void *cleanup(void *args) {
	dequeue();
	return;
}
/*----------------------------------------------------------------------------------------------*/

/* Circular Buffer Enqueue and Dequeue Functions */
int main(int argc, char *argv[]) {
	int l;
	char *line;
	const char dil[2] = " ";
	ssize_t number_read;
	char *token;
	size_t length = 0;
	FILE *config;
	int reg_q_index = 0;
	int pub_ind = 0;
	int sub_ind = 0;
	config = fopen(argv[1], "r");
	number_read = getline(&line, &length, config);
	do {
		int z = 0;
		int count = 0;
		char *line_array[1000];
		token = strtok(line, dil);
		while(token != NULL) {
			line_array[z] = token;
			if(line_array[z][strlen(line_array[z])-1] == '\n') {
				line_array[z][strlen(line_array[z])-1] = 0;
			}
			z++;
			token = strtok(NULL, dil);
			count++;
		}
		if(strcmp(line_array[0], "create") == 0) {
			char topic[50];
			strcpy(topic, line_array[3]);
			int max;
			max = atoi(line_array[4]);
			init_cb(name, max, "mountains");
			registry[reg_q_index] = &name;
			reg_q_index++;
		}
		if(strcmp(line_array[0], "query") == 0 && strcmp(line_array[1], "topics") == 0) {
			int p = 0;
			while(p < reg_q_index) {
				printf("Topic Name: %s\n", registry[p]->name);
				p++;
			}
		}
		if(strcmp(line_array[0], "add") == 0 && strcmp(line_array[1], "publisher") == 0) {
			char filename[1000];
			strcpy(filename, line_array[z-1]);
			pub_t[pub_ind].flag = 0;
			pub_t[pub_ind].file = filename;
			pthread_create(&pub_t[pub_ind].id, NULL, Publisher, (void *)&pub_t[pub_ind]);
			pub_ind++;
		}
		if(strcmp(line_array[0], "add") == 0 && strcmp(line_array[1], "subscriber") == 0) {
			char filename[1000];

			strcpy(filename, line_array[z-1]);
			sub_t[sub_ind].flag = 0;
			sub_t[sub_ind].file = filename;
			pthread_create(&sub_t[sub_ind].id, NULL, Subscriber, (void *)&sub_t[sub_ind]);
			sub_ind++;
		}
		if(strcmp(line_array[0], "query") == 0 && strcmp(line_array[1], "publishers") == 0) {
			int p = 0;
			while(p < pub_ind) {
				printf("Publisher::::Thread ID: %d File Name %s\n", pub_t[p].id, pub_t[p].file);
				p++;
			}
		}
		if(strcmp(line_array[0], "query") == 0 && strcmp(line_array[1], "subscribers") == 0) {
			int p = 0;
			while(p < sub_ind) {
				printf("Subscriber::::Thread ID: %d File Name %s\n", sub_t[p].id, sub_t[p].file);
				p++;
			}
		}
		if(strcmp(line_array[0], "delta") == 0) {
			int delta = atoi(line_array[1]);
			Delta = delta;
		}

	} while((number_read = getline(&line, &length, config) != -1));

	for(l = 0; l < MAXPUBs; l++) {
		pthread_join(pub_t[l].id, NULL);
		pthread_join(sub_t[l].id, NULL);
	}
	return 1;
	/*
	//STEP-1: Create the publisher thread-pool
	for(l = 0; l < MAXPUBs; l++) {
		//enqueue_arg_init(enqueue, file);
		pub_t[l].flag = 0;
		pthread_create(&pub_t[l].id, NULL, Publisher, (void *)&pub_t[l]);
	}

	//STEP-2: Create the subscriber thread-pool
	for(l = 0; l < MAXPUBs; l++) {
		sub_t[l].flag = 0;
		pthread_create(&sub_t[l].id, NULL, Subscriber, (void *)&sub_t[l]);
	}

	//STEP-3: Join the thread-pools
	for(l = 0; l < MAXPUBs; l++) {
		pthread_join(pub_t[l].id, NULL);
		pthread_join(sub_t[l], NULL);
	}
	*/
	return 1;
}
