#include "StdAfx.h"
#include "subdivision.h"
#include "quadedge.h"

subdivision::~subdivision(void)
{
}


subdivision::subdivision(const Point2d &a, const Point2d &b, const Point2d &c )
{
	Point2d *da, *db, *dc;
	da = new Point2d(a);
	db = new Point2d(b);
	dc = new Point2d(c);

	Edge *ea = MakeEdge();
	ea->EndPoints(da,db);
	Edge *eb = MakeEdge();
	Splice(ea->Sym(),eb);
	eb->EndPoints(db,dc);
	Edge *ec = MakeEdge();
	Splice(eb->Sym(),ec);
	ec->EndPoints(dc,da);
	Splice(ec->Sym(),ea);
	startingEdge = ea;
}

Edge *subdivision::Locate(const Point2d &x)
{
	Edge *e = startingEdge;

	while (true )
	{
		if (x.Equal(e->Org2d()) || x.Equal(e->Dest2d()))
			return e;
		else if ( RightOf(x,e) )
			e = e->Sym();
		else if ( !RightOf(x,e->Onext()) )
			e = e->Onext();
		else if ( !RightOf(x,e->Dprev()) )
			e = e->Dprev();
		else 
			return e;
	}
}
	
void subdivision::InsertSite( const Point2d &x)
{
	Edge *e = Locate(x);
	if ( (x.Equal(e->Org2d())) || (x.Equal(e->Dest2d())) ) 
		return;
	else if (OnEdge(x,e)) {
		e = e->Oprev();
		DeleteEdge(e->Onext());
	}

	Edge *base = MakeEdge();
	base->EndPoints(e->Org(),new Point2d(x));
	Splice(base,e);
	startingEdge = base;
	do {
		base = Connect(e,base->Sym());
		e = base->Oprev();
	} while(e->Lnext()!=startingEdge);

	do {
		Edge *t = e->Oprev();
		if (RightOf(t->Dest2d(),e)&&
			InCircle(e->Org2d(), t->Dest2d(), e->Dest2d(),x)) {
			Swap(e);
			e = e->Oprev();
		} else if ( e->Onext()==startingEdge)
			return;
		else
			e = e->Onext()->Lprev();
	} while(true);
}