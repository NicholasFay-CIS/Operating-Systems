#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>
#include <regex.h>

#define MAXPUBs 50
#define MAXSUBs 50
#define MAXTOPICS 1000
#define MAXENTRIES 100
//Condition Variable
pthread_cond_t condition;

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
	int TEQ_ID;
	topicEntry *buffer;
	pthread_mutex_t m_lock;
} TEQ;

//struct for the thread pools
typedef struct {
	pthread_t id;
	int flag;
	char file[1000];
} table_e;

//Init the registry
int cbrs = MAXTOPICS;
TEQ registry[MAXTOPICS];
//used for the TEQ buffers mainly
topicEntry arr[MAXTOPICS][MAXENTRIES+1];
table_e pub_t[MAXPUBs];
table_e sub_t[MAXSUBs];
int Delta;

/* Macro initializers for the structs above Used to test parts before part 4
Including just to show some of the stuff I used in some of my
 */
#define init_te(n, url, caption)            \
	topicEntry n = {					    \
		.pubID = 0,					        \
		.photoURL = url,					\
		.photoCaption = caption,			\
		.entryNum = 0					    \
	};

#define get_entry_init(init_name, name, entry, topic, th) 	\
	get_entry_arg init_name = {				    			\
		.queuename = name,									\
		.entry_num = entry,									\
		.TE = topic 										\
	}									    				\

//Init of a null topic entry stuct
topicEntry null = {-1};
//Macro that initializes the circular ring buffer
#define init_TQ(n, max, pos, ID)          \
	strcpy(registry[pos].name, n),		  \
	registry[pos].buffer = arr[pos],	  \
	registry[pos].max_entries = max, 	  \
	registry[pos].TEQ_ID = ID,			  \
	registry[pos].buffer[max] = null;					

/*-------------------------------------------*/
int enqueue(int TEQ_ID, topicEntry TE) {
	int i;
	int found = -1;
	time_t entrytime;
	TEQ cb;
	for(i = 0; i < cbrs; i++) {
		if(registry[i].TEQ_ID == TEQ_ID) {
			cb = registry[i];
			found = i;
			break;
		}
	}
	if(found == -1) {
		return 0;
	}
	if(cb.buffer[cb.tail].entryNum == -1) {
		return 0;

	} else {
		//insert the topic into the queue and get the time stamp
		TE.entryNum = cb.e_Num;
		gettimeofday(&TE.timeStamp, NULL);
		cb.buffer[cb.tail] = TE;
		//increment the tail according to its position
		if(cb.tail + 1 > cb.max_entries) {
			cb.tail = 0;
		} else {
			cb.tail++;
		}
		cb.e_Num++;
	}
	return 1;
}
/*--------------------------------------------------*/
int thread_safe_enqueue(int TEQ_ID, topicEntry TE) {
	int i, fail;
	int found = -1;
	time_t entrytime;
	for(i = 0; i < cbrs; i++) {
		if(registry[i].TEQ_ID == TEQ_ID) {
			found = i;
			break;
		}
	}
	//if the registry does not have the given queue name
	if(found == -1) {
		return 0;
	}
	//lock the given buffer
	pthread_mutex_lock(&registry[i].m_lock);
	//enqueue the topicEntry
	fail = enqueue(TEQ_ID, TE);
	//Unlock the given buffer
	pthread_mutex_unlock(&registry[i].m_lock);
	//if the enqueue failed return 0
	if(fail	== 0) {
		return 0;
	} else {
		//otherwise return 1 on success
		return 1;
	}
}
/*-----------------------------------------------*/
int getEntry(int TEQ_ID, int entrynumber, topicEntry *topic) {
	int i;
	int found = -1;
	TEQ te;
	//find the queue in the registry
	for(i = 0; i < cbrs; i++) {
		if(registry[i].TEQ_ID == TEQ_ID) {
			te = registry[i];
			found = i;
			break;
		}
	}
	if(found == -1) {
		return 0;
	}
	if(te.tail == te.head) {
		return 0;
	}
	int k = 0;
	//find the next topic and save it to the topic entry given
	for(k; k < te.max_entries; k++) {
		//if the entry number is greater than the number we want to grab these
		if(te.buffer[k].entryNum > entrynumber) {
			if(te.buffer[k].entryNum > entrynumber) {
				strcpy(topic->photoCaption, te.buffer[k].photoCaption);
				strcpy(topic->photoURL, te.buffer[k].photoURL);
				topic->pubID = te.buffer[k].pubID;
				topic->entryNum = te.buffer[k].entryNum;
				topic->timeStamp = te.buffer[k].timeStamp;
			}
			if(k == te.max_entries) {
				strcpy(topic->photoCaption,te.buffer[te.head].photoCaption);
				strcpy(topic->photoURL,te.buffer[te.head].photoURL);
				topic->pubID = te.buffer[te.head].pubID;
				topic->entryNum = te.buffer[te.head].entryNum;
				topic->timeStamp = te.buffer[te.head].timeStamp;
			} else {
				strcpy(topic->photoCaption, te.buffer[k+1].photoCaption);
				strcpy(topic->photoURL, te.buffer[k+1].photoURL);
				topic->pubID = te.buffer[k+1].pubID;
				topic->entryNum = te.buffer[k+1].entryNum;
				topic->timeStamp = te.buffer[k+1].timeStamp;
			}
		}
	}
	return 1;
}

int thread_safe_get_Entry(int TEQ_ID) {
	int i, fail;
	int found = -1;
	for(i = 0; i < cbrs; i++) {
		if(registry[i].TEQ_ID, TEQ_ID) {
			found = i;
			break;
		}
	}
	if(found == -1) {
		return 0;
	}
	pthread_mutex_lock(&registry[i].m_lock);
	//enqueue the topicEntry
	topicEntry *topic;
	int current_entry = registry[i].e_Num;
	int l = 0;
	for(l; l <= current_entry; l++) {
		getEntry(TEQ_ID, l, topic);
	}
	//Unlock the given buffer
	pthread_mutex_unlock(&registry[i].m_lock);
	return 1;
}

/* ------------------------------------------------------------------------ */
int dequeue() {
	int i = 0;
	//cbrs is the MAXTOPICS
	for(i; i < cbrs; i++) {
		int j;
		if(registry[i].name != NULL) {
			j = registry[i].head;
			for(j; j < registry[i].tail; j++) {
				if(strcmp(registry[i].buffer[j].photoCaption, "\0") != 0) {
					struct timeval cur_time;
					gettimeofday(&cur_time, NULL);
					double elapsed = (cur_time.tv_sec - registry[i].buffer[j].timeStamp.tv_sec) + ((cur_time.tv_usec - registry[i].buffer[j].timeStamp.tv_usec)/1000000.0);
					if(elapsed >= Delta) {
						strcpy(registry[i].buffer[j].photoCaption, "\0");
						strcpy(registry[i].buffer[j].photoURL, "\0");
						registry[i].buffer[j].entryNum = 0;
						registry[i].buffer[j].pubID = 0;
						if(registry[i].head == registry[i].max_entries-1) {
							registry[i].head = 0;
						} else {
							registry[i].head++;
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
	char *line;
	const char dil[2] = " ";
	ssize_t number_read;
	size_t length = 0;
	FILE *config;
	pthread_mutex_t pub_lock;
	pthread_mutex_lock(&pub_lock);
	while(((table_e *)args)->file == "\0") {
		pthread_cond_wait(&condition, &pub_lock);
	}
	config = fopen(((table_e *)args)->file, "r");
	number_read = getline(&line, &length, config);
	do {
		int i = 0;
		char *token;
		char *rem;
		char string[100];
		int z = 0;
		char *line_array[1000];
		token = strtok_r(line, dil, &rem);
		while(token != NULL) {
			if(token[0] == '"') {
				int i;
				int put = 0;
				for(i = 1; token[i] != '"'; i++) {
					string[put] = token[i];
					put++;
				}
				line_array[z] = string;		
			} else {
				line_array[z] = token;
			}
			if(line_array[z][strlen(line_array[z])-1] == '\n') {
				line_array[z][strlen(line_array[z])-1] = 0;
			}
			z++;
			token = strtok_r(NULL, dil, &rem);
		}
		if(strcmp(line_array[0], "put") == 0) {
			int id;
			topicEntry TE;
			id = atoi(line_array[1]);
			strcpy(TE.photoURL, line_array[2]);
			strcpy(TE.photoCaption, line_array[3]);
			thread_safe_enqueue(id, TE);
			printf("Proxy Thread: %d - type: Publisher- Executed Command: put\n", pthread_self());
		}
		if(strcmp(line_array[0], "sleep") == 0) {
			int sleep_time;
			sleep_time = atoi(line_array[1]);
			usleep(sleep_time);
			printf("Proxy Thread: %d - type: Publisher- Executed Command: sleep\n", pthread_self());
		}
		if(strcmp(line_array[0], "stop") == 0) {
			printf("Proxy Thread: %d - type: Publisher- Executed Command: stop\n", pthread_self());
			break;
		}
	} while((number_read = getline(&line, &length, config)) != -1);
	printf("Proxy Thread: %d - type: Publisher\n", pthread_self());
	strcpy(((table_e *)args)->file, "\0");
	fclose(config);
	pthread_mutex_unlock(&pub_lock);
	return;
}

/*----------------------------------------------------------------------------------------------*/
void *Subscriber(void *args) {
	char *line;
	const char dil[2] = " ";
	ssize_t number_read;
	size_t length = 0;
	FILE *config;
	pthread_mutex_t sub_lock;
	pthread_mutex_lock(&sub_lock);
	while(((table_e *)args)->file == "\0") {
		pthread_cond_wait(&condition, &sub_lock);
	}
	config = fopen(((table_e *)args)->file, "r");
	number_read = getline(&line, &length, config);
	do {
		int i = 0;
		char *token;
		char *rem;
		char string[100];
		int z = 0;
		char *line_array[1000];
		token = strtok_r(line, dil, &rem);
		while(token != NULL) {
			if(token[0] == '"') {
				int i;
				int put = 0;
				for(i = 1; token[i] != '"'; i++) {
					string[put] = token[i];
					put++;
				}
				line_array[z] = string;		
			} else {
				line_array[z] = token;
			}
			if(line_array[z][strlen(line_array[z])-1] == '\n') {
				line_array[z][strlen(line_array[z])-1] = 0;
			}
			z++;
			token = strtok_r(NULL, dil, &rem);
		}
		if(strcmp(line_array[0], "get") == 0) {
			int id = atoi(line_array[1]);
			thread_safe_get_Entry(id);
			printf("Proxy Thread: %d - type: Subscriber - Executed Command: get\n", pthread_self());
		}
		if(strcmp(line_array[0], "sleep") == 0) {
			int sleep_time;
			sleep_time = atoi(line_array[1]);
			usleep(sleep_time);
			printf("Proxy Thread: %d - type: Subscriber - Executed Command: sleep\n", pthread_self());
		}
		if(strcmp(line_array[0], "stop") == 0) {
			printf("Proxy Thread: %d - type: Subscriber - Executed Command: stop\n", pthread_self());
			break;
		}
	} while((number_read = getline(&line, &length, config)) != -1);
	printf("Proxy Thread: %d - type: Subscriber\n", pthread_self());
	strcpy(((table_e *)args)->file, "\0");
	fclose(config);
	pthread_mutex_unlock(&sub_lock);
	return;
}
/*----------------------------------------------------------------------------------------------*/
void *CleanUp(void *args) {
	//offset the clean up time so its not the same as the time delts
	int cleanup_time = Delta + 5;
	double elapsed;
	struct timeval start, current_time;
	//get current start
	gettimeofday(&start, NULL);
	while(1) {
		//get current time
		gettimeofday(&current_time, NULL);
		elapsed = (current_time.tv_sec - start.tv_sec) + ((current_time.tv_usec - start.tv_usec)/1000000.0);
		if(elapsed >= cleanup_time) {
			//if the elapsed time is greater or equal to cleanuptime dequeue and get the new start time
			dequeue();
			gettimeofday(&start, NULL);
		} else {
			//if its not time yet, then schedule yield
			sched_yield();
		}
	}
	return;
}
/*----------------------------------------------------------------------------------------------*/

/* Circular Buffer Enqueue and Dequeue Functions */
int main(int argc, char *argv[]) {
	int l;
	char *line;
	const char dil[2] = " ";
	ssize_t number_read;
	size_t length = 0;
	FILE *config;
	int reg_q_index = 0;
	config = fopen(argv[1], "r");
	number_read = getline(&line, &length, config);
	do {
		char *token;
		char *rem;
		char strings[100];
		int z = 0;
		int kill = 0;
		char *line_array[1000];
		//Regex search with sscanf
		sscanf(line, "%*[^\"]\"%31[^\"]\"", strings);
		token = strtok_r(line, dil, &rem);
		while(token != NULL) {
			//if the token starts with the string
			//iterate through the rest of the tokens
			if(token[0] == '"') {
				while(token != NULL && token[strlen(token)-1] != '"') {
					token = strtok_r(NULL, dil, &rem);
				}
				//place regex match
				line_array[z] = strings;
			} else {
				line_array[z] = token;
			}
			if(line_array[z][strlen(line_array[z])-1] == '\n') {
				line_array[z][strlen(line_array[z])-1] = 0;
			}
			z++;
			token = strtok_r(NULL, dil, &rem);
		}
		if(strcmp(line_array[0], "create") == 0) {
			int max, ID;
			max = atoi(line_array[4]);
			ID = atoi(line_array[2]);
			init_TQ(line_array[3], max, reg_q_index, ID);
			reg_q_index++;
		}
		if(strcmp(line_array[0], "query") == 0 && strcmp(line_array[1], "topics") == 0) {
			int p = 0;
			while(p < MAXTOPICS) {
				if(registry[p].max_entries != 0) {
					printf("Topic Name: %s - Length: %d\n", registry[p].name, registry[p].max_entries);
				}
				p++;
			}
		}
		if(strcmp(line_array[0], "add") == 0 && strcmp(line_array[1], "publisher") == 0) {
			char filename[1000];
			strcpy(filename, line_array[z-1]);
			int p = 0;
			int found = 0;
			for(p; p < MAXPUBs; p++) {
				//if the flag is 1 then that thread is now free;
				if(pub_t[p].flag == 1) {
					found = 1;
					break;
				}
			}
			if(found == 0) {
				for(p; p < MAXPUBs; p++) {
					//if the flag is 0 then that thread has not yet been used
					if(pub_t[p].flag == 0) {
						found = 1;
						break;
					}
				}
			}
			//set the flag to 2 showing it is in use
			pub_t[p].flag = 2;
			strcpy(pub_t[p].file, line_array[z-1]);
			pthread_create(&pub_t[p].id, NULL, Publisher, (void *)&pub_t[p]);
			//free the flag
			pub_t[p].flag = 1;
	
		}
		if(strcmp(line_array[0], "add") == 0 && strcmp(line_array[1], "subscriber") == 0) {
			int p = 0;
			int found;
			found = 0;
			for(p; p < MAXSUBs; p++) {
				//if the flag is 1 then that thread is free
				if(sub_t[p].flag == 1) {
					found = 1;
					break;
				}
			}
			if(found == 0) {
				for(p = 0; p < MAXSUBs; p++) {
					//if the flag is 0 then that thread has not yet been used
					if(sub_t[p].flag == 0) {
						found = 1;
						break;
					}
				}
			}
			//set the flag to 2 showing it is in use
			sub_t[p].flag = 2;
			strcpy(sub_t[p].file, line_array[z-1]);
			pthread_create(&sub_t[p].id, NULL, Subscriber, (void *)&sub_t[p]);
			//free the flag
			sub_t[p].flag = 1;
		}
		if(strcmp(line_array[0], "query") == 0 && strcmp(line_array[1], "publishers") == 0) {
			int p = 0;
			while(p < MAXPUBs) {
				if(pub_t[p].flag != 0) {
					printf("Proxy Thread: %d - type: Publisher - Command File Name %s\n", pub_t[p].id, pub_t[p].file);
				}
				p++;
			}
		}
		if(strcmp(line_array[0], "query") == 0 && strcmp(line_array[1], "subscribers") == 0) {
			int p = 0;
			while(p < MAXSUBs) {
				if(pub_t[p].flag != 0) {
					printf("Proxy Thread: %d - type: Subsciber - Command File Name %s\n", sub_t[p].id, sub_t[p].file);
				}
				p++;
			}
		}
		if(strcmp(line_array[0], "delta") == 0) {
			int delta = atoi(line_array[1]);
			Delta = delta;
		}
		if(strcmp(line_array[0], "start") == 0) {
			pthread_t cleanup_t;
			pthread_create(&cleanup_t, NULL, CleanUp, NULL);
			pthread_join(cleanup_t, NULL);
			//Broadcast to threads
			pthread_cond_broadcast(&condition);
		}

	} while((number_read = getline(&line, &length, config) != -1));

	sleep(5);

	for(l = 0; l < MAXPUBs; l++) {
		if(pub_t[l].flag != 1) {
			pthread_join(pub_t[l].id, NULL);
		}
	}
	//Join subscriber threads
	for(l = 0; l < MAXSUBs; l++) {
		if(sub_t[l].flag != 1) {
			pthread_join(sub_t[l].id, NULL);
		}
	}
	//broadcast to all the threads
	fclose(config);
	return 1;
}
