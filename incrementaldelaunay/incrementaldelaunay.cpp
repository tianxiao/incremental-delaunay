// incrementaldelaunay.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "subdivision.h"
#include "quadedge.h"
#include "testQuadEdgeTopo.h"

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>

#include "FaceAdjMesh.h"
#include "exactpredicator.h"

#include "ApiTest.h"


void sampleSubdivision()
{
	Point2d p0 = {0,0};
	Point2d p1 = {4,0};
	Point2d p2 = {2,3};
	subdivision sub(p0,p1,p2);

	Point2d pi0 = {2,0.75};
	Point2d pi1 = {2,1.5};
	Point2d pi2 = {1,0.75};
	Point2d pi3 = {3,0.75};

	sub.InsertSite(pi0);
	sub.InsertSite(pi1);
	sub.InsertSite(pi2);
	sub.InsertSite(pi3);

	return;
}

void testInCirclePredicate()
{
	double pa[2] = {-2,0};
	double pb[2] = {2,0};
	double pc[2] = {0,3};
	double pd[2] = {0,1.5};
	double pdp[2] = {0,-1.5};
	double pbc[2] = {3,2};
	double pca[2] = {-3,-3};

	double rtn = incircle(pa,pb,pc,pd);
	double rtn1 = incircle(pa,pb,pc,pdp);
	double rtnbc = incircle(pa,pb,pc,pca);

	// The following test case is the incircle test!
	double p25_4_41_v0[2] = {0.657466,2.696793};
	double p25_4_41_v1[2] = {2.000000,2.000000};
	double p25_4_41_v2[2] = {1.354103,1.967425};

	double p25_4_25_v[2] = {0.657466,2.696793};
	double p25_4_25_incircle = incircle(p25_4_41_v0,p25_4_41_v1,p25_4_41_v2,p25_4_25_v);

	double p641_1996_1069_v0[2] = {1.784130,2.094302};
	double p641_1996_1069_v1[2] = {1.285546,2.383407};
	double p641_1996_1069_v2[2] = {1.400998,2.373647};

	double p641_1996_2333_v[2] = {1.400165,2.265218};
	double p641_1996_1069_incircle = incircle(p641_1996_1069_v0,p641_1996_1069_v1,p641_1996_1069_v2,p641_1996_2333_v);
}



void smapletestQuadEdgeTopo()
{
	testQuadEdgeTopo test;
	test.RunTest();
}

struct FakeElements
{
	int a[4];
};

void TestHugeNew( int num )
{
	FakeElements ini;
	ini.a[0] = 1; ini.a[1] = 2; ini.a[2] = 3; ini.a[3] = 4;
	FakeElements *elemlist = new FakeElements[num];
	for ( int i=0; i<num; ++i ) {
		memcpy(elemlist[i].a,&ini,sizeof(FakeElements));
	}
	delete []elemlist;
}

void TestHugeMalloc(int num)
{
	FakeElements ini;
	ini.a[0] = 1; ini.a[1] = 2; ini.a[2] = 3; ini.a[3] = 4;
	printf("FakeElements size %d \n", sizeof(FakeElements));
	FakeElements *elemlist = (FakeElements*)malloc(sizeof(FakeElements)*num);
	assert(elemlist);
	for ( int i=0; i<num; ++i ) {
		memcpy(elemlist[i].a,&ini,sizeof(FakeElements));
	}


	//FILE *fp = fopen("file","w");
	//for ( int i=0; i<num; ++i ) {
	//	fprintf(fp,"%d\t\t%d,%d,%d,%d\n",
	//		i, elemlist[i].a[0], elemlist[i].a[1],elemlist[i].a[2],elemlist[i].a[3]);
	//}
	//fclose(fp);

	free(elemlist);
}

int _tmain(int argc, _TCHAR* argv[])
{

	//smapletestQuadEdgeTopo();

	//sampleSubdivision();

	//FaceAdjacentDelaunayTest();
	//FaceAdjacentDelaunayTest2();
	//FaceAdjacentDelaunayBBoxAPITest();
	FaceAdjDelaunayIdxInsert();
	//testInCirclePredicate();

	//TestHugeMalloc(316081674);
	//TestHugeMalloc(500000);
	//TestHugeNew(316081674);

	return 0;
}

