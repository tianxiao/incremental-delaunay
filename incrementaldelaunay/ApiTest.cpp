#include "StdAfx.h"
#include "ApiTest.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include "FaceAdjMesh.h"
#include "PZoneManager.h"

#include "timecounter.h"


ApiTest::ApiTest(void)
{
}


ApiTest::~ApiTest(void)
{
}

double getRandom0_1()
{
	return rand()/(RAND_MAX*1.0);
};

typedef FaceAdjMesh<dPoint2dDefault,dTriFaceDefault> FaceAdjMeshDefault;

void FaceAdjacentDelaunayTest()
{
	FaceAdjMeshDefault fmesh;
	double p0[2] = {-8,0};
	double p1[2] = {8,0};
	double p2[2] = {0,8};
	fmesh.Initialize(p0,p1,p2);
	fmesh.InsertPoint(2,2);
	fmesh.InsertPoint(-2,2);
	fmesh.InsertPoint(-2,1.5);
	fmesh.InsertPoint(1,1.1);
	fmesh.InsertPoint(2,2.3);
	fmesh.InsertPoint(3,2.3);
	fmesh.InsertPoint(5,2.3);
	fmesh.InsertPoint(0.1,4.2);
	fmesh.InsertPoint(0.3,5.1);

	// 3+9

	//srand(time(NULL));
	srand(NULL);
	for ( int i=0; i<300; ++i ) {
		fmesh.InsertPoint( rand()/(RAND_MAX*1.0)*3.90+0.5, rand()/(RAND_MAX*1.0)*3.90+0.5 );
	}

	//srand(NULL);
	//for ( int i=0; i<1000000; ++i ) {
	//	fmesh.InsertPoint( -(rand()/(RAND_MAX*1.0)*3.90+0.5), rand()/(RAND_MAX*1.0)*3.90+0.5 );
	//}

	fmesh.DumpMesh("dmeshl.off");
	fmesh.DumpMeshFem("dmeshl.fem");
}

void FaceAdjacentDelaunayTest2()
{
	FaceAdjMeshDefault fmesh;
	double p0[2] = {-8,0};
	double p1[2] = {8,0};
	double p2[2] = {0,8};
	fmesh.Initialize(p0,p1,p2);
	fmesh.InsertPoint(2,2);
	fmesh.InsertPoint(-2,2);
	fmesh.InsertPoint(-2,1.5);
	fmesh.InsertPoint(1,1.1);
	fmesh.InsertPoint(2,2.3);
	fmesh.InsertPoint(3,2.3);
	fmesh.InsertPoint(5,2.3);
	fmesh.InsertPoint(0.1,4.2);
	fmesh.InsertPoint(0.3,5.1);

	//srand(time(NULL));
	//srand(NULL);
	//for ( int i=0; i<1000000; ++i ) {
	//	fmesh.InsertPoint( rand()/(RAND_MAX*1.0)*3.90+0.5, rand()/(RAND_MAX*1.0)*3.90+0.5 );
	//}

	srand(NULL);
	for ( int i=0; i<816; ++i ) {
		fmesh.InsertPoint( -(rand()/(RAND_MAX*1.0)*3.90+0.5), rand()/(RAND_MAX*1.0)*3.90+0.5 );
	}

	fmesh.DumpMesh("dmeshr.off");
}

void FaceAdjacentDelaunayBBoxAPITest()
{
	double minPnt[2] = {2.0,2.0};
	double maxPnt[2] = {5.0,10.0};
	FaceAdjMeshDefault fmesh;
	fmesh.InitializeBBox(minPnt,maxPnt);
	srand(NULL);
	//const int numpnts = 163*20000;
	const int numpnts = 1*20000;
	for ( int i=0; i<numpnts; ++i ) {
		fmesh.InsertPoint(
			getRandom0_1()*(maxPnt[0]-minPnt[0])+minPnt[0],
			getRandom0_1()*(maxPnt[1]-minPnt[1])+minPnt[1]);
		if ( i%20000==0 ) 
			printf("%d * 20,000 Generated\n", i/20000 );
	}

	fmesh.DumpMeshFem("dmeshbbox.fem");
}

void FaceAdjDelaunayIdxInsert()
{
	double minPnt[2] = {2.0,2.0};
	double maxPnt[2] = {5.0,10.0};
	FaceAdjMeshDefault fmesh;
	fmesh.InitializeBBox(minPnt,maxPnt);
	srand(NULL);
	//const int numpnts = 163*20000;
	const int numpnts = 1*20000;
	std::vector<int> pntsidxlist;
	pntsidxlist.reserve(numpnts);
	double x, y;
	for ( int i=0; i<numpnts; ++i ) {
		//fmesh.InsertPoint(
		//	getRandom0_1()*(maxPnt[0]-minPnt[0])+minPnt[0],
		//	getRandom0_1()*(maxPnt[1]-minPnt[1])+minPnt[1]);
		//if ( i%20000==0 ) 
		//	printf("%d * 20,000 Generated\n", i/20000 );
		x = getRandom0_1()*(maxPnt[0]-minPnt[0])+minPnt[0];
		y = getRandom0_1()*(maxPnt[1]-minPnt[1])+minPnt[1];
		pntsidxlist.push_back(fmesh.NewVertex(x,y));
	}

	for ( int i=0; i<pntsidxlist.size(); ++i ) {
		fmesh.InsertPointByIdx(pntsidxlist[i]);
	}

	fmesh.DumpMeshFem("dmeshbbox.fem");
}

const int NumPerformancePnts = 200*20000;

void TestZonePartitionDelaunay()
{
	srand(NULL);

	dTime timeper;
	timeper.start();

	PZoneManager zonemana;
	double minPnt[2] = {2.0,2.0};
	double maxPnt[2] = {5.0,10.0};
	double xoffset = 0.00001;
	double yoffset = 0.00001;
	const int numpnts = NumPerformancePnts;
	zonemana.SetRange(minPnt[0],maxPnt[0],minPnt[1],maxPnt[1]);
	zonemana.SetProcessor(12);
	zonemana.SetPntsNumPerCell(4);
	zonemana.SetPntsNum( numpnts );
	zonemana.Initialize();

	double x, y;
	for ( int i=0; i<numpnts; ++i ) {
		x = getRandom0_1()*(maxPnt[0]-minPnt[0])+minPnt[0] - xoffset;
		y = getRandom0_1()*(maxPnt[1]-minPnt[1])+minPnt[1] - yoffset;
		zonemana.InsertPoints(x,y);
		zonemana.DispatchZone();
	}

	zonemana.MultiThreadPartionDelaunay();
	

	unsigned long elipsecount = timeper.ellips();
	int intelipse = elipsecount/1000;
	printf("%d s\n", intelipse); 
	//int i;
	//std::cin>>i;

	zonemana.DumZoneFem("zonepartitionmulti");
}

void TestZonePartitionDelaunayWithoutThread()
{
	srand(NULL);

	dTime timeper;
	timeper.start();

	PZoneManager zonemana;
	double minPnt[2] = {2.0,2.0};
	double maxPnt[2] = {5.0,10.0};
	double xoffset = 0.00001;
	double yoffset = 0.00001;
	const int numpnts = NumPerformancePnts;
	zonemana.SetRange(minPnt[0],maxPnt[0],minPnt[1],maxPnt[1]);
	zonemana.SetProcessor(12);
	zonemana.SetPntsNumPerCell(4);
	zonemana.SetPntsNum( numpnts );
	zonemana.Initialize();

	double x, y;
	for ( int i=0; i<numpnts; ++i ) {
		x = getRandom0_1()*(maxPnt[0]-minPnt[0])+minPnt[0] - xoffset;
		y = getRandom0_1()*(maxPnt[1]-minPnt[1])+minPnt[1] - yoffset;
		zonemana.InsertPoints(x,y);
		zonemana.DispatchZone();
	}

	zonemana.SingleThreadPartionDelaunay();
	

	unsigned long elipsecount = timeper.ellips();
	int intelipse = elipsecount/1000;
	printf("%d s\n", intelipse); 
	//int i;
	//std::cin>>i;

	zonemana.DumZoneFem("zonepartitionsingle");
}
