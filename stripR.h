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
#include <cadef.h>

struct StripRTrace {
	char *mName;
	chid mChId;
    int mStatus;
    long  mDbfType;
    long  mDbrType;
    // True length of data in value
    unsigned long mNumElems;
    // Requested length of data
    unsigned long mReqElems;
    void* mValue;
    epicsTimeStamp tsPreviousC;
    epicsTimeStamp tsPreviousS;
    char firstStampPrinted;
    char mOnceConnected;
    evid mEvId;
    bool mNewName;
    double mLastValue;

    StripRTrace();
    ~StripRTrace();
	void SetTraceName(const char *name);
	int CreatePV();
	void DestroyPV();
	void ConnectionHandler(struct connection_handler_args args);
	void EventHandler(evargs args);
};

struct StripR {
	pthread_t mCAThrId;
	int mCount;
//	pthread_mutex_t mCAMutex;
//	pthread_cond_t mCACond;
	bool mCAExit;

	struct StripRTrace mTraces[2];
	StripR();
	~StripR();
	void Draw(const char* title, bool* p_open);
	void SetTraceName(const int idx, const char *name);
};

int StripRInit();
int StripRShutdown();

void ShowStripR(bool* p_open);

#endif /* STRIPR_H_ */
