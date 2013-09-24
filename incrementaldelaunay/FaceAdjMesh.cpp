#include "StdAfx.h"
#include "FaceAdjMesh.h"
#include "exactpredicator.h"
#include <queue>

bool RtreeSearchCallback(int fid, void* arg)
{
	//std::vector<int> *fidlist = dynamic_cast<std::vector<int>*>(arg);
	dDynaArrayW *listw = (dDynaArrayW*)(arg);
	listw->list.push_back(fid);
	return true;
}