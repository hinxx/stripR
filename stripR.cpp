/*
 * stripR.cpp
 *
 *  Created on: Mar 3, 2018
 *      Author: hinxx
 */


#include "imgui.h"

#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include <cadef.h>

#include "stripR.h"

void *ca_fn( void *ptr ) {
	struct StripR *stripr;
	stripr = (struct StripR *) ptr;
//	int i;

//	i = 0;
	while (! stripr->mCAExit) {

		// Lock mutex and then wait for signal to relase mutex
//		pthread_mutex_lock(&stripr->mCAMutex);
		// Wait while functionCount2() operates on count
		// mutex unlocked if condition varialbe in functionCount2() signaled.
//		pthread_cond_wait( &condition_var, &count_mutex );

//		sleep(1.0);
//		printf("%s: Thread IS #%ld iter %d..\n", __func__, stripr->mCAThrId, i++);

		for (int i = 0; i < 2; i++) {
			StripRTrace *t = (struct StripRTrace *) &stripr->mTraces[i];
			// printf("%s: PV%i=%s\n", __func__, i, t->mName);
			// XXX: we have to create and handle PVs from this thread
			//      otherwise no CA updates are seen!!!!!!
			if (t->mNewName) {
				t->DestroyPV();
				t->CreatePV();
			}
		}
	    ca_pend_event(0.01);
//	    fflush(stdout);
	}
	return NULL;
}

StripR::StripR() :
	mCAThrId(0),
	mCount(0)
{
	int ret;

	printf("Called %s\n", __func__);

    struct StripRTrace trace1, trace2;

	mTraces[0] = trace1;
	mTraces[1] = trace2;

//	pthread_mutex_init(&mCAMutex, NULL);
//	pthread_cond_init(&mCACond, NULL);
	mCAExit = false;
    ret = pthread_create(&mCAThrId, NULL, ca_fn, (void*) this);
    if (ret) {
    	printf("ERR: pthread_create() %s\n", strerror(errno));
    }
}

StripR::~StripR() {
	printf("Called %s\n", __func__);
	mCAExit = true;
	pthread_join(mCAThrId, NULL);
	sleep(2.0);
	printf("Done %s\n", __func__);
}

void StripR::Draw(const char* title, bool* p_open) {
//	printf("Called %s\n", __func__);

	mCount++;
    static char PV0[256], PV1[256];

    ImGui::Begin(title, p_open);
    ImGui::Text("Hello from main window for %d time..", mCount);

    if (ImGui::InputText("PV0", PV0, IM_ARRAYSIZE(PV0),ImGuiInputTextFlags_EnterReturnsTrue)) {
    	printf("PV0: %s\n", PV0);
    	SetTraceName(0, PV0);
    }

    if (mTraces[0].mName) {
    	ImGui::Text("%s = %g", mTraces[0].mName, mTraces[0].mLastValue);
    }

    if (ImGui::InputText("PV1", PV1, IM_ARRAYSIZE(PV1), ImGuiInputTextFlags_EnterReturnsTrue)) {
    	printf("PV1: %s\n", PV1);
    	SetTraceName(1, PV1);
    }

    if (mTraces[1].mName) {
    	ImGui::Text("%s = %g", mTraces[1].mName, mTraces[1].mLastValue);
    }

    if (ImGui::Button("Close Me")) {
        *p_open = false;
    	printf("Will close window %s\n", __func__);
    }

    ImGui::End();
}

void StripR::SetTraceName(const int idx, const char *name) {
	StripRTrace *trace = &mTraces[idx];
	trace->SetTraceName(name);
//	trace->DestroyPV();
//	trace->CreatePV();
}


StripRTrace::StripRTrace() :
	mName(NULL),
	mChId(0),
	mOnceConnected(0),
	mEvId(0),
	mNewName(false) {

}

StripRTrace::~StripRTrace() {

}

static void ca_event_handler (evargs args) {
//    pv* pv_ = (pv *)args.usr;
	struct StripRTrace *t = (struct StripRTrace *) (args.usr);
	t->EventHandler(args);
}

void StripRTrace::EventHandler(evargs args) {

    mStatus = args.status;
    if (args.status == ECA_NORMAL) {
    	printf("%s: ECA_NORMAL for PV %s\n", __func__, mName);

        mDbrType = args.type;
        mNumElems = args.count;
        /* casting away const */
//        mValue = (void *) args.dbr;
//        unsigned base_type = args.type % (LAST_TYPE+1);
        void *val_ptr = dbr_value_ptr(args.dbr, args.type);
        mLastValue = *(dbr_double_t*) val_ptr;

//        sprintf(str, dblFormatStr, ((dbr_double_t*) val_ptr)[index]);

//        print_time_val_sts(pv_, reqElems);
//        fflush(stdout);

        mValue = NULL;
    }
}

static void ca_conn_handler(struct connection_handler_args args) {
	printf("enter %s..\n", __func__);
	struct StripRTrace *t = (struct StripRTrace *) ca_puser(args.chid);
	t->ConnectionHandler(args);
}


void StripRTrace::ConnectionHandler (struct connection_handler_args args) {

	printf("enter %s..\n", __func__);

//    pv *ppv = ( pv * ) ca_puser ( args.chid );
    if (args.op == CA_OP_CONN_UP) {
//        nConn++;
        if (! mOnceConnected) {
            mOnceConnected = 1;
			/* Set up pv structure */
			/* ------------------- */

			/* Get natural type and array count */
            mDbfType = ca_field_type(mChId);
            /* Use native type */
            mDbrType = dbf_type_to_DBR_TIME(mDbfType);
            // XXX: This is to get PV data as string
//            /* Enums honour -n option */
//            if (dbr_type_is_ENUM(mDbrType)) {
//                if (enumAsNr) mDbrType = DBR_TIME_INT;
//                else          mDbrType = DBR_TIME_STRING;
//            }
//            else if (floatAsString &&
//                     (dbr_type_is_FLOAT(ppv->dbrType) || dbr_type_is_DOUBLE(ppv->dbrType)))
//            {
//                ppv->dbrType = DBR_TIME_STRING;
//            }

                                /* Set request count */
            mNumElems = ca_element_count(mChId);
            mReqElems = mReqElems > mNumElems ? mNumElems : mReqElems;

                                /* Issue CA request */
                                /* ---------------- */
            /* install monitor once with first connect */
            mStatus = ca_create_subscription(mDbrType,
                                                mReqElems,
                                                mChId,
												DBE_VALUE | DBE_ALARM,
                                                ca_event_handler,
                                                (void*)this,
                                                &mEvId);
    		printf("%s: created event ID %p\n", __func__, mEvId);
       }
    }
    else if ( args.op == CA_OP_CONN_DOWN ) {
//        nConn--;
        mStatus = ECA_DISCONN;
//        print_time_val_sts(ppv, reqElems);
    }
}


void StripRTrace::SetTraceName(const char *name) {
	if (mName) {
		free(mName);
	}
	mName = strdup(name);
	mNewName = true;
}

int StripRTrace::CreatePV() {
//    int n;
    int result;
    int returncode = 0;

//    if (!tsInitC)                /* Initialize start timestamp */
//    {
//        epicsTimeGetCurrent(&tsStart);
//        tsInitC = 1;
//    }

    /* Issue channel connections */
	result = ca_create_channel (mName,
								ca_conn_handler,
								this,
								0,
								&mChId);
	if (result != ECA_NORMAL) {
		fprintf(stderr, "CA error %s occurred while trying "
				"to create channel '%s'.\n", ca_message(result), mName);
		mStatus = result;
		returncode = 1;
	}

	mNewName = false;
	if (returncode == 0) {
		printf("%s: SUCCESS creating channel for PV %s (ID %p)\n", __func__, mName, mChId);
	} else {
		printf("%s: FAILED creating channel for PV %s\n", __func__, mName);
	}
    return returncode;
}

void StripRTrace::DestroyPV() {
	if (mEvId) {
		ca_clear_subscription(mEvId);
		printf("%s: canceled event ID %p\n", __func__, mEvId);
		mEvId = 0;
	}
	if (mChId) {
		ca_clear_channel(mChId);
		printf("%s: destroyed channel ID %p\n", __func__, mChId);
		mChId = 0;
	}
	mOnceConnected = 0;
}

void ShowStripR(bool* p_open)
{
    static struct StripR stripr;
    stripr.Draw("StripR", p_open);
}

int StripRInit() {
	int result;

    result = ca_context_create(ca_disable_preemptive_callback);
    if (result != ECA_NORMAL) {
        fprintf(stderr, "CA error %s occurred while trying "
                "to start channel access.\n", ca_message(result));
        return 1;
    }

    return 0;
}

int StripRShutdown() {
	ca_context_destroy();
	return 0;
}
