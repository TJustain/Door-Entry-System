/*
 * des_inputs.c
 *
 *  Created on: Mar 11, 2022
 *  Author: Justain Treblay
 */

#include "D:\Users\tjust\cst8244_assign1\des_controller\src\des_mva.h"

Person person;
int validateInput(char* input);
char* upperCase(char* input);
void promptID();
void promptWeight();
void Idle();
int coid;

int main(int argc, char *argv[]) {
	char input[4] = "";

	// establish a connection --- Phase I
	coid = name_open("Controller", 0);
	if (coid == -1) {
		fprintf(stderr, "Couldn't ConnectAttach\n");
		perror(NULL);
		exit(EXIT_FAILURE);
	}
	printf("Client PID is %d\n", getpid());

	//Inputs --- Phase 2
	while (1) {
		Idle(); //check for idle conditions
		printf(inputEvent);
		do {
			scanf("%4s", input);
			upperCase(input);
		} while (validateInput(input));
		switch (person.input) {
		case LEFT_SCAN:
			promptID();
			break;
		case RIGHT_SCAN:
			promptID();
			break;
		case WEIGHT_SCALE:
			promptWeight();
			break;
		default:
			break;
		}
		if (MsgSend(coid, &person, sizeof(person) + 1, &person, sizeof(person))
				== -1L) {
			perror("Error during MsgSend\n");
			exit(EXIT_FAILURE);
		}
		if (person.input == EXIT) {
			break;
		}
	}
	//Disconnect from the channel --- Phase III
	name_close(coid);
	return EXIT_SUCCESS;
}

//Checks if the input is valid
int validateInput(char *input) {
	for (int i = 0; i < NUM_INPUTS; i++) {
		if (!strcmp(input, inMessage[i])) {
			person.input = i;
			return 0;
		}
	}
	return 1;
}

// Changes the input to upper case
char* upperCase(char* input) {
	for (int i = 0; i < strlen(input); i++) {
		input[i] = toupper(input[i]);
	}
	//input[strlen(input)-1] = '\0';
	return input;
}

void promptID() {
	printf("Enter the Person's ID;\n");
	scanf("%d", &person.person_ID);
}

void promptWeight() {
	printf("Enter the Person's weight;\n");
	scanf("%d", &person.weight);
}

//Does the proper messaging for the change to idle
void Idle() {
	if (person.state == START_STATE
			|| (person.dir == LEFT && person.input == RIGHT_LOCK
					&& person.state == LOCK_STATE)
			|| (person.dir == RIGHT && person.input == LEFT_LOCK
					&& person.state == LOCK_STATE)) {
		if (MsgSend(coid, &person, sizeof(person) + 1, &person, sizeof(person))
				== -1L) {
			perror("Error during MsgSend\n");
			exit(EXIT_FAILURE);
		}
	}
	return;
}

