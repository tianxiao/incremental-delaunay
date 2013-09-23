#include "StdAfx.h"
#include "testQuadEdgeTopo.h"
#include "subdivision.h"
#include "quadedge.h"
#include <assert.h>


testQuadEdgeTopo::testQuadEdgeTopo(void)
{
}


testQuadEdgeTopo::~testQuadEdgeTopo(void)
{
}

bool testQuadEdgeTopo::EdgeValide(Edge *e)
{
	Edge *rot = e->Rot();
	assert(rot);
	Edge *rotOnext = rot->Onext();
	assert(rotOnext);
	Edge *rotOnextRot = rotOnext->Rot();
	assert(rotOnextRot);
	if ( e==rotOnextRot ) 
		return true;
	else
		return false;
}

class FakeEdge
{
public:
	int idx;
	FakeEdge *next;
};

class FakeQuadEdge
{
public:
	FakeEdge e[4];
	FakeEdge *InitialEdge()
	{
		for ( int i=0; i<4; ++i ) {
			e[i].idx = i;
		}

		e[0].next = &e[0];
		e[2].next = &e[2];

		e[1].next = &e[3];
		e[3].next = &e[1];

		return &(e[0]);
	}
};

void FoolFakeEdgeTest()
{
	FakeQuadEdge qe;
	qe.InitialEdge();

}

void testQuadEdgeTopo::RunTest()
{
	FoolFakeEdgeTest();

	Point2d p0 = {0,0};
	Point2d p1 = {4,0};
	Point2d p2 = {2,3};

	Point2d *da, *db, *dc;
	da = new Point2d(p0);
	db = new Point2d(p1);
	dc = new Point2d(p2);

	Edge *ea = MakeEdge();
	ea->EndPoints(da,db);
	assert(EdgeValide(ea));

	Edge *eb = MakeEdge();
	Splice(ea->Sym(),eb);
	eb->EndPoints(db,dc);
	assert(EdgeValide(eb));

	Edge *ec = MakeEdge();
	Splice(eb->Sym(),ec);
	ec->EndPoints(dc,da);
	Splice(ec->Sym(),ea);
	assert(EdgeValide(ec));
	//startingEdge = ea;
}
