/*
 * des_controller.c
 *
 *  Created on: Mar 11, 2022
 *  Author: Justain Treblay
 */

#include "des_mva.h"

typedef void *(*StateFun)();
void *StartState();
void *IdleState();
void *ExitState();
void *ScannerState();
void *UnlockState();
void *OpenState();
void *WeightState();
void *CloseState();
void *LockState();

Person person;
Display display;

int main(int argc, char *argv[]) {
	int rcvid;        // indicates who we should reply to
	int chid;         // the channel ID
	int coid;
	name_attach_t *attach;
	StateFun statePointer = StartState;

	//Connect --- Phase I
	//Client
	coid = name_open("Display", 0);
	if (coid == -1) {
		perror("Controller failed to connect to the Display!\n");
		exit(EXIT_FAILURE);
	}
	//Sever
	attach = name_attach(NULL, "Controller", 0);

	printf("Controller Server PID is %d\n", getpid());

	// this is typical of a server:  it runs forever --- Phase II
	while (1) {
		//Read from Inputs
		rcvid = MsgReceive(attach->chid, &person, sizeof(person), NULL);
		//Function pointer for state transition
		statePointer = (StateFun) (*statePointer)();
		//send to Display
		if (MsgSend(coid, &display, sizeof(display) + 1, &display,
				sizeof(display)) == -1L) {
			perror("Error during MsgSend\n");
			exit(EXIT_FAILURE);
		}
		//Replay to Inputs
		MsgReply(rcvid, EOK, &person, sizeof(person));
		//EXIT Condition
		if (person.input == EXIT) {
			printf("EXIT Controller...\n");
			break;
		}
	}
	//Disconnect --- Phase III
	name_close(coid);
	name_detach(attach,0);
	return EXIT_SUCCESS;
}

/**
 * General comment for the following functions:
 * Each function represents a state and must return itself or another state function.
 * It selects what state to go next with the following logic:
 * Depending on the input, direction, and step counter: the state will choose the appropriate transition.
 * The step counter increments after every state and is reset when returning to the idle state.
 * This is used in state that are accessed multiple times throughout the cycle.
 */
void *StartState() {
	person.state = IDLE_STATE;
	display.output = IDLE_MSG;
	person.step++;
	return IdleState;
}
void *IdleState() {
	if (person.input == LEFT_SCAN) {
		person.dir = LEFT;
		person.state = SCANNER_STATE;
		display.output = SCAN_MSG;
		display.person_ID = person.person_ID;
		person.step++;
		return ScannerState;
	} else if (person.input == RIGHT_SCAN) {
		person.dir = RIGHT;
		person.state = SCANNER_STATE;
		display.output = SCAN_MSG;
		display.person_ID = person.person_ID;
		person.step++;
		return ScannerState;
	} else if (person.input == EXIT) {
		person.state = EXIT_STATE;
		display.output = EXIT_MSG;
		return ExitState;
	} else if (person.state == IDLE_STATE) {
		display.output = IDLE_MSG;
		return IdleState;
	} else {
		display.output = ERR_MSG;
		return IdleState;
	}
}
void *ExitState() {
	person.state = EXIT_STATE;
	display.output = EXIT_MSG;
	return ExitState;
}
void *ScannerState() {
	if (person.input == LEFT_UNLOCK && person.dir == LEFT) {
		person.state = UNLOCK_STATE;
		display.output = LEFT_UNLOCK_MSG;
		person.step++;
		return UnlockState;
	} else if (person.input == RIGHT_UNLOCK && person.dir == RIGHT) {
		person.state = UNLOCK_STATE;
		display.output = RIGHT_UNLOCK_MSG;
		person.step++;
		return UnlockState;
	} else if (person.input == EXIT) {
		person.state = EXIT_STATE;
		display.output = EXIT_MSG;
		return ExitState;
	} else {
		display.output = ERR_MSG;
		return ScannerState;
	}
}
void *UnlockState() {
	if ((person.input == LEFT_OPEN && person.dir == LEFT && person.step == 3) || (person.input == LEFT_OPEN && person.dir == RIGHT && person.step == 8)) {
		person.state = OPEN_STATE;
		display.output = LEFT_OPEN_MSG;
		person.step++;
		return OpenState;
	} else if ((person.input == RIGHT_OPEN && person.dir == RIGHT && person.step == 3) || (person.input == RIGHT_OPEN && person.dir == LEFT && person.step == 8)) {
		person.state = OPEN_STATE;
		display.output = RIGHT_OPEN_MSG;
		person.step++;
		return OpenState;
	} else if (person.input == EXIT) {
		person.state = EXIT_STATE;
		display.output = EXIT_MSG;
		return ExitState;
	} else {
		display.output = ERR_MSG;
		return UnlockState;
	}
}
void *OpenState() {
	if (person.input == WEIGHT_SCALE && person.step == 4) {
		person.state = WEIGHT_STATE;
		display.output = WEIGHT_MSG;
		display.weight = person.weight;
		person.step++;
		return WeightState;
	} else if (person.input == LEFT_CLOSE && person.dir == RIGHT && person.step == 9) {
		person.state = CLOSE_STATE;
		display.output = LEFT_CLOSE_MSG;
		person.step++;
		return CloseState;
	} else if (person.input == RIGHT_CLOSE && person.dir == LEFT && person.step == 9) {
		person.state = CLOSE_STATE;
		display.output = RIGHT_CLOSE_MSG;
		person.step++;
		return CloseState;
	} else if (person.input == EXIT) {
		person.state = EXIT_STATE;
		display.output = EXIT_MSG;
		return ExitState;
	} else {
		display.output = ERR_MSG;
		return OpenState;
	}
}
void *WeightState() {
	if (person.input == LEFT_CLOSE && person.dir == LEFT) {
		person.state = CLOSE_STATE;
		display.output = LEFT_CLOSE_MSG;
		person.step++;
		return CloseState;
	} else if (person.input == RIGHT_CLOSE && person.dir == RIGHT) {
		person.state = CLOSE_STATE;
		display.output = RIGHT_CLOSE_MSG;
		person.step++;
		return CloseState;
	} else if (person.input == EXIT) {
		person.state = EXIT_STATE;
		display.output = EXIT_MSG;
		return ExitState;
	} else {
		display.output = ERR_MSG;
		return WeightState;
	}
}
void *CloseState() {
	if ((person.input == LEFT_LOCK && person.dir == LEFT && person.step == 6) || (person.input == LEFT_LOCK && person.dir == RIGHT && person.step == 10)) {
		person.state = LOCK_STATE;
		display.output = LEFT_LOCK_MSG;
		person.step++;
		return LockState;
	}else if ((person.input == RIGHT_LOCK && person.dir == RIGHT && person.step == 6) || (person.input == RIGHT_LOCK && person.dir == LEFT && person.step == 10)) {
		person.state = LOCK_STATE;
		display.output = RIGHT_LOCK_MSG;
		person.step++;
		return LockState;
	}else if(person.input == EXIT){
		person.state = EXIT_STATE;
		display.output = EXIT_MSG;
		return ExitState;
	}else{
		display.output = ERR_MSG;
		return CloseState;
	}
}
void *LockState() {
	if(person.step == 11){
		person.state = IDLE_STATE;
		display.output = IDLE_MSG;
		person.step = 1;
		return IdleState;
	}else if(person.input == EXIT){
		person.state = EXIT_STATE;
		display.output = EXIT_MSG;
		return ExitState;
	}else if(person.input == LEFT_UNLOCK && person.dir == RIGHT){
		person.state = UNLOCK_STATE;
		display.output = LEFT_UNLOCK_MSG;
		person.step++;
		return UnlockState;
	}else if(person.input == RIGHT_UNLOCK && person.dir == LEFT){
		person.state = UNLOCK_STATE;
		display.output = RIGHT_UNLOCK_MSG;
		person.step++;
		return UnlockState;
	}else{
		display.output = ERR_MSG;
		return LockState;
	}
}
