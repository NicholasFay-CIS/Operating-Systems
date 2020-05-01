#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//Author: Nicholas David Fay
//Lab 8
//CIS 415 Operating systems

/*--------------------------*/
/* Structs */
typedef struct {
	int ticketNum;
	char dish[50];
} mealTicket;

typedef struct {
	char name[1000];
	mealTicket * const buffer;
	int head; //used to access elements in the buffer
	int tail; //if tail == head: buffer = full or empty
	const int length;
} MTQ;

int TICKET = 0;
MTQ *registry[4];

/* Circular Buffer Functions */
mealTicket init_meal(int tick, char *n) {
	mealTicket mt;
	mt.ticketNum = tick;
	strcpy(mt.dish, n);
	return mt;
}

MTQ init_crb(char *n, const int len, mealTicket null) {
	MTQ cb;
	strcpy(cb.name, n);
	cb.head = 0;
	cb.tail = 0;
	*(int *)&cb.length = len;
	*(mealTicket **)&cb.buffer = malloc(sizeof(mealTicket) * (len + 1));
	cb.buffer[cb.length] = null;
	return cb;
}

int enqueue(char *MTQ_ID, mealTicket MT) {
	int i;
	int found = -1;
	MTQ *cb;
	for(i = 0; i < 4; i++) {
		if(strcmp(registry[i]->name, MTQ_ID) == 0) {
			cb = registry[i];
			found = i;
			break;
		}
	}
	if(found == -1) {
		return 0;
	}
	if(cb->buffer[cb->tail].ticketNum == -1) {
		return 0;
	} else {
		MT.ticketNum = TICKET;
		//printf("Ticket Num: %d\n", MT.ticketNum);
		cb->buffer[cb->tail] = MT;
		//cb->buffer[cb->tail].ticketNum = TICKET;
		//printf("Adding Dish (%s) with Ticket num (%d)\n", cb->buffer[cb->tail].dish, cb->buffer[cb->tail].ticketNum);
		if(cb->tail + 1 > cb->length) {
			cb->tail = 0;

		} else {
			cb->tail++;
		}
		TICKET = TICKET + 1;
	}
	return 1;
}
int dequeue(char *MTQ_ID, int number, mealTicket *MT) {
	int i;
	int found = -1;
	MTQ *cb;
	for(i = 0; i < 4; i++) {
		if(strcmp(registry[i]->name, MTQ_ID) == 0) {
			cb = registry[i];
			found = i;
			break;
		}
	}
	//printf("The CRB name is %s\n", registry[found]->name);
	if(found == -1) {
		return 0;
	}
	if(cb->tail == cb->head) {
		return 0;
	}
	if(cb->buffer[cb->head].ticketNum != number) {
		return 0;
	} else {
		MT->ticketNum = cb->buffer[cb->head].ticketNum;
		strcpy(MT->dish, cb->buffer[cb->head].dish);
		//printf("Removing ticketnumber (%d)  Dish (%s) from Queue %s\n",cb->buffer[cb->head].ticketNum, cb->buffer[cb->head].dish, cb->name);
		if(cb->head == 0) {
			cb->buffer[cb->head].ticketNum = 0;
			strcpy(cb->buffer[cb->head].dish, "\0");
		} else {
			cb->buffer[cb->head-1].ticketNum = 0;
			strcpy(cb->buffer[cb->head-1].dish, "\0");
		}
		if(cb->head + 1 > cb->length) {
			cb->head = 0;
		} else {
			cb->head++;
		}
	}
	return 1;
}

/* Main */
int main(int argc, char *argv[]) {
	mealTicket null = init_meal(-1, "\0");
	MTQ breakfast = init_crb("Breakfast", 5, null);
	MTQ Lunch = init_crb("Lunch", 5, null);
	MTQ Dinner = init_crb("Dinner", 5, null);
	MTQ Bar = init_crb("Bar", 5, null);
	registry[0] = &breakfast;
	registry[1] = &Lunch;
	registry[2] = &Dinner;
	registry[3] = &Bar;
	mealTicket eggs = init_meal(TICKET, "eggs");
	int fail;
	fail = dequeue("Breakfast", 0, &eggs);
	if(fail == 0) {
		printf("Test Case A Dequeue when dequeue is empty: Success\n");
	}
	fail = enqueue("Breakfast", eggs);
	if(fail == 1) {
		printf("Test Case D Enqueue when a queue is empty: Success\n");
	}
	printf("Queue Breakfast - Ticket Number: %d - Dish: %s\n", eggs.ticketNum, eggs.dish);
	mealTicket bacon = init_meal(TICKET, "bacon");
	enqueue("Breakfast", bacon);
	printf("Queue Breakfast - Ticket Number: %d - Dish: %s\n", bacon.ticketNum, bacon.dish);
	mealTicket ham = init_meal(TICKET, "ham");
	enqueue("Breakfast", ham);
	printf("Queue Breakfast - Ticket Number: %d - Dish: %s\n", ham.ticketNum, ham.dish);
	mealTicket sausage = init_meal(TICKET, "sausage");
	enqueue("Breakfast", sausage);
	printf("Queue Breakfast - Ticket Number: %d - Dish: %s\n", sausage.ticketNum, sausage.dish);
	mealTicket cereal = init_meal(TICKET, "cereal");
	enqueue("Breakfast", cereal);
	printf("Queue Breakfast - Ticket Number: %d - Dish: %s\n", cereal.ticketNum, cereal.dish);
	fail = enqueue("Breakfast", cereal);
	if(fail == 0) {
		printf("Test Case C enqueue when a queue is full: Success\n");
	}
	mealTicket testb;
	fail = dequeue("Breakfast", 0, &testb);
	if(fail == 1) {
		printf("Test Case B dequeue when a queue is full: Success\n");
	}
	enqueue("Breakfast", cereal);
	//printf("After dequeue: first item is %s\n", breakfast.buffer[0].dish);
	mealTicket soup = init_meal(TICKET, "soup");
	fail = dequeue("Lunch", 0, &soup);
	if(fail == 0) {
		printf("Test Case A Dequeue when dequeue is empty: Success\n");
	}
	fail = enqueue("Lunch", soup);
	if(fail == 1) {
		printf("Test Case D Enqueue when a queue is empty: Success\n");
	}
	/*------------------------------*/
	//mealTicket soup = init_meal(TICKET, "soup");
	enqueue("Lunch", soup);
	printf("Queue Lunch - Ticket Number: %d - Dish: %s\n", soup.ticketNum, soup.dish);
	mealTicket taco = init_meal(TICKET, "taco");
	enqueue("Lunch", taco);
	printf("Queue Lunch - Ticket Number: %d - Dish: %s\n", taco.ticketNum, taco.dish);
	mealTicket ham2 = init_meal(TICKET, "ham2");
	enqueue("Lunch", ham2);
	printf("Queue Lunch - Ticket Number: %d - Dish: %s\n", ham2.ticketNum, ham2.dish);
	mealTicket sandwich = init_meal(TICKET, "sandwich");
	enqueue("Lunch", sandwich);
	printf("Queue Lunch - Ticket Number: %d - Dish: %s\n", sandwich.ticketNum, sandwich.dish);
	mealTicket bread = init_meal(TICKET, "bread");
	enqueue("Lunch", bread);
	printf("Queue Lunch - Ticket Number: %d - Dish: %s\n", bread.ticketNum, bread.dish);
	fail = enqueue("Lunch", bread);
	if(fail == 0) {
		printf("Test Case C enqueue when a queue is full: Success\n");
	}
	mealTicket test;
	fail = dequeue("Lunch", 5, &test);
	if(fail == 1) {
		printf("Test Case B dequeue when a queue is full: Success\n");
	}

	fail = dequeue("Dinner", 0, &eggs);
	if(fail == 0) {
		printf("Test Case A Dequeue when dequeue is empty: Success\n");
	}
	mealTicket pig = init_meal(TICKET, "pig");
	enqueue("Dinner", pig);
	if(fail == 1) {
		printf("Test Case D Enqueue when a queue is empty: Success\n");
	}
	printf("Queue Dinner - Ticket Number: %d - Dish: %s\n", pig.ticketNum, pig.dish);
	mealTicket steak = init_meal(TICKET, "steak");
	enqueue("Dinner", steak);
	printf("Queue Dinner - Ticket Number: %d - Dish: %s\n", steak.ticketNum, steak.dish);
	mealTicket duck = init_meal(TICKET, "duck");
	enqueue("Dinner", duck);
	printf("Queue Dinner - Ticket Number: %d - Dish: %s\n", duck.ticketNum, duck.dish);
	mealTicket cheesebread = init_meal(TICKET, "cheesebread");
	enqueue("Dinner", cheesebread);
	printf("Queue Dinner - Ticket Number: %d - Dish: %s\n", cheesebread.ticketNum, cheesebread.dish);
	mealTicket turkey = init_meal(TICKET, "turkey");
	enqueue("Dinner", turkey);
	printf("Queue Dinner - Ticket Number: %d - Dish: %s\n", turkey.ticketNum, turkey.dish);
	fail = enqueue("Dinner", cereal);
	if(fail == 0) {
		printf("Test Case C enqueue when a queue is full: Success\n");
	}
	mealTicket testc;
	fail = dequeue("Dinner", 10, &testc);
	if(fail == 1) {
		printf("Test Case B dequeue when a queue is full: Success\n");
	}

	/* Free all buffers */
	free(breakfast.buffer);
	free(Lunch.buffer);
	free(Dinner.buffer);
	free(Bar.buffer);
	return 0;
}