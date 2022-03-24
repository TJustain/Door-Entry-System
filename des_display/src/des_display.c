/*
 * des_display.c
 *
 *  Created on: Mar 11, 2022
 *  Author: Justain Treblay
 */

#include "D:\Users\tjust\cst8244_assign1\des_controller\src\des_mva.h"

Display display;
name_attach_t *attach;

int main(void) {
	int rcvid;        // indicates who we should reply to
	int chid;         // the channel ID

	// create a channel --- Phase I
	attach = name_attach(NULL, "Display", 0);
	printf("Display Server PID is %d\n", getpid());

	// this is typical of a server:  it runs forever --- Phase II
	while (display.output != EXIT_MSG) {
		rcvid = MsgReceive(attach->chid, &display, sizeof(display), NULL);
		MsgReply(rcvid, EOK, &display, sizeof(display));
		switch (display.output) {
		case SCAN_MSG:
			printf("%s %d\n", outMessage[display.output], display.person_ID);
			break;
		case WEIGHT_MSG:
			printf("%s %d\n", outMessage[display.output], display.weight);
			break;
		default:
			printf("%s\n", outMessage[display.output]);
			break;
		}
	}
	name_detach(attach, 0);
	return EXIT_SUCCESS;
}
