/*
 * stripR.cpp
 *
 *  Created on: Mar 3, 2018
 *      Author: hinxx
 */


#include "imgui.h"
#include <stdio.h>
#include <pthread.h>
#include "stripR.h"


StripR::StripR() :
	mThrId(0),
	mCount(0)
{
	printf("Called %s\n", __func__);
}

StripR::~StripR() {
	printf("Called %s\n", __func__);
}

void StripR::Draw(const char* title, bool* p_open) {
//	printf("Called %s\n", __func__);

	mCount++;

    ImGui::Begin(title, p_open);
    ImGui::Text("Hello from main window for %d time..", mCount);
    if (ImGui::Button("Close Me")) {
        *p_open = false;
    	printf("Will close window %s\n", __func__);
    }

    ImGui::End();
}

void ShowStripR(bool* p_open)
{
    static StripR stripr;
    stripr.Draw("StripR", p_open);
}
