/*
 * StripR.h
 *
 *  Created on: Mar 3, 2018
 *      Author: hinxx
 */

#ifndef STRIPR_H_
#define STRIPR_H_

#include "imgui.h"
#include <pthread.h>

struct StripR {
	pthread_t mThrId;
	int mCount;

	StripR();
	~StripR();
	void Draw(const char* title, bool* p_open);
};

void ShowStripR(bool* p_open);

#endif /* STRIPR_H_ */
