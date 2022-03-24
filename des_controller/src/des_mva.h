/*
 * des_mva.h
 *
 *  Created on: Mar 11, 2022
 *  Author: Justain Treblay
 */

#ifndef DES_MVA_H_
#define DES_MVA_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <process.h>
#include <errno.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>

#define DIRECTION 1
typedef enum
{
	LEFT = 0,
	RIGHT = 1
} Direction;

#define NUM_STATES 9
typedef enum
{
	START_STATE = 0,
	IDLE_STATE = 1,
	EXIT_STATE = 2,
	SCANNER_STATE = 3,
	UNLOCK_STATE = 4,
	OPEN_STATE = 5,
	WEIGHT_STATE = 6,
	CLOSE_STATE = 7,
	LOCK_STATE = 8
} State;

#define NUM_INPUTS 12
typedef enum
{
	LEFT_SCAN = 0,
	RIGHT_SCAN = 1,
	WEIGHT_SCALE = 2,
	LEFT_UNLOCK = 3,
	RIGHT_UNLOCK = 4,
	LEFT_OPEN = 5,
	RIGHT_OPEN = 6,
	LEFT_CLOSE = 7,
	RIGHT_CLOSE = 8,
	LEFT_LOCK = 9,
	RIGHT_LOCK = 10,
	EXIT = 11,
} Input;

#define NUM_OUTPUTS 13
typedef enum
{
	IDLE_MSG = 0,
	SCAN_MSG = 1,
	WEIGHT_MSG = 2,
	LEFT_UNLOCK_MSG = 3,
	RIGHT_UNLOCK_MSG = 4,
	LEFT_OPEN_MSG = 5,
	RIGHT_OPEN_MSG = 6,
	LEFT_CLOSE_MSG = 7,
	RIGHT_CLOSE_MSG = 8,
	LEFT_LOCK_MSG = 9,
	RIGHT_LOCK_MSG = 10,
	EXIT_MSG = 11,
	ERR_MSG = 12
} Output;

const char *outMessage[NUM_OUTPUTS] = {
		"Waiting for person...",
		"Person scanned ID, ID =",
		"Person weighed, Weight =",
		"Left door unlocked by Guard",
		"Right door unlocked by Guard",
		"Person opened left door",
		"Person open right door",
		"Left door closed (automatically)",
		"Right door closed (automatically)",
		"Left door locked by Guard",
		"Right door locked by Guard",
		"Exiting Display",
		"Incorrect operation try again..."
};

const char *inMessage[NUM_INPUTS] = {
	"LS",
	"RS",
	"WS",
	"GLU",
	"GRU",
	"LO",
	"RO",
	"LC",
	"RC",
	"GLL",
	"GRL",
	"EXIT"
};

const char *inputEvent = {
		"Enter the event type (ls= left scan, rs= right scan, ws= weight scale, lo =left open, ro=right open, lc = left closed, rc = right closed , gru = guard right unlock, grl = guard, right lock, gll=guard left lock, glu = guard left unlock, exit = exit programs)\n"
};

typedef struct {
	Input input;
	int step;
	Direction dir;
	State state;
	int person_ID;
	int weight;
} Person;

typedef struct {
	int output;
	int person_ID;
	int weight;
} Display;

#endif /* DES_MVA_H_ */
