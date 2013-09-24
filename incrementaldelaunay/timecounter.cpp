#include "StdAfx.h"
#include "timecounter.h"
#include <Windows.h>


timecounter::timecounter(void)
{
}


timecounter::~timecounter(void)
{
}

void dTime::start()
{
	mCount = GetTickCount();
}

unsigned long dTime::ellips()
{
	return GetTickCount()-mCount;
}
