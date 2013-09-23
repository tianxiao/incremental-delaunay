#pragma once
#include <math.h>

typedef double Reall ;

#define PRECISION 0.00000000000000001

struct Point2d
{
	Reall x, y;
	Reall norm() { return sqrt(x*x+y*y); };
	Point2d operator-(const Point2d &r) const
	{
		Point2d p = {(x-r.x), (y-r.y)};
		return p;
	}
	Reall Distance( const Point2d &linestart, const Point2d &lineend ) const
	{
		Point2d linedir = lineend-linestart;
		return (x*linedir.x+y*linedir.y)/linedir.norm();
	}

	bool Equal( const Point2d &p ) const
	{
		return ( abs(p.x-x)<PRECISION && abs(p.y-y)<PRECISION );
	}
};

class Edge
{
	friend class Quadedge;
	friend void Splice( Edge *, Edge *);

private:
	int num;
	Edge *next;
	Point2d *data;

public:
	Edge() { data=0; };
	Edge *Rot();
	Edge *invRot();
	Edge *Sym();
	Edge *Onext();
	Edge *Oprev();
	Edge *Dnext();
	Edge *Dprev();
	Edge *Lnext();
	Edge *Lprev();
	Edge *Rnext();
	Edge *Rprev();
	Point2d *Org();
	Point2d *Dest();
	const Point2d &Org2d() const;
	const Point2d &Dest2d() const;
	void EndPoints( Point2d* p0, Point2d* p1)
	{
		data = p0;
		this->Sym()->data = p1;
	}

	Quadedge *Qedge() { return (Quadedge*)(this-num); };
};

class Quadedge
{
	friend Edge *MakeEdge();
private:
	Edge e[4];
public:
	Quadedge(void);
	~Quadedge(void);
};

inline Quadedge::Quadedge(void)
{
	for ( int i=0; i<4; ++i ) {
		e[i].num = i;
	}

	e[0].next = &(e[0]); 
	e[2].next = &(e[2]); // primal edge is point to it self

	e[1].next = &(e[3]);
	e[3].next = &(e[1]); // dual edge is reference to the opposite side (face side)
}




inline Edge *Edge::Rot()
{
	return (num<3)? this+1:this-3;
}	

inline Edge *Edge::invRot()
{
	return (num>0)? this-1:this+3;
}

inline Edge *Edge::Sym()
{
	return (num>2)? this-2:this+2;
}

inline Edge *Edge::Onext()
{
	return this->next;
}

inline Edge *Edge::Oprev()
{
	return this->Rot()->Onext()->Rot();
}

//inline Edge *Edge::Dnext()
//{
//	return this->Sym()->Onext();
//}

//inline Edge *Edge::Dprev()
//{
//	return this->Sym()->Oprev();
//}

// Return the next ccw edge around the (into) the destination of the current edge
inline Edge *Edge::Dnext()
{
	return this->Sym()->Onext()->Sym();
}

// Return the next cw edge around the (into) the destination of the current edge
inline Edge *Edge::Dprev()
{
	return this->invRot()->Onext()->invRot();
}

inline Edge *Edge::Lnext()
{
	return this->invRot()->Onext()->Rot();
}

inline Edge *Edge::Lprev()
{
	//return this->invRot()->Oprev()->Rot();
	return this->Onext()->Sym();
}

inline Edge *Edge::Rnext()
{
	return this->Rot()->Onext()->invRot();
}

inline Edge *Edge::Rprev()
{
	//return this->Rot()->Oprev()->invRot();
	return this->Sym()->Onext();
}

inline Point2d *Edge::Org()
{
	return data;
}

inline Point2d *Edge::Dest()
{
	return this->Sym()->Org();
}

inline const Point2d &Edge::Org2d() const
{
	return *data;
}

inline const Point2d &Edge::Dest2d() const
{
	return num<2? *(this+2)->data : *(this-2)->data;
}

/**
* This is a convention that
* ql's index 0, 2 is the primal edge ?
*/
inline Edge *MakeEdge()
{
	Quadedge *ql = new Quadedge;
	return ql->e;
}

inline void Splice( Edge *a, Edge *b ) 
{
	Edge *alpha = a->Onext()->Rot();
	Edge *beta  = b->Onext()->Rot();
	Edge *t1 = b->Onext();
	Edge *t2 = a->Onext();
	Edge *t3 = beta->Onext();
	Edge *t4 = alpha->Onext();

	// swap
	a->next = t1;
	b->next = t2;
	alpha->next = t3;
	beta->next = t4;
}

// conect the dest of a to the oringinal of b
inline Edge * Connect( Edge *a, Edge *b )
{
	Edge *e = MakeEdge();
	e->EndPoints( a->Dest(), b->Org() );
	//Splice( a->Sym(), e );
	Splice(e, a->Lnext() );
	Splice( e->Sym(), b );
	return e;
}

inline void DeleteEdge( Edge *e )
{
	Splice( e, e->Oprev() );
	Splice( e->Sym(), e->Sym()->Oprev() );
	delete e->Qedge();
}

inline void Swap( Edge *e )
{
	Edge *a = e->Oprev();
	Edge *b = e->Sym()->Oprev();
	Splice(e,a);
	Splice(e->Sym(),b);
	Splice(e,a->Lnext());
	Splice(e->Sym(),b->Lnext());
	e->EndPoints(a->Dest(),b->Dest());
}

// Geometry Predictator
inline Reall TriArea( const Point2d &a, const Point2d &b, const Point2d &c )
{
	return (b.x-a.x)*(c.y-a.y)-(b.y-a.y)*(c.x-a.x);
}

#define VECTOR_SQUARE(a) (a.x*a.x + a.y*a.y )

inline Reall VectorSquare( const Point2d &a )
{
	return ( a.x*a.x+a.y*a.y );
}

inline int InCircle( const Point2d &a, const Point2d &b, const Point2d &c, const Point2d &d )
{
	return VectorSquare(a) * TriArea(b,c,d) -
		VectorSquare(b) * TriArea( c,d,a) +
		VectorSquare(c) * TriArea( d,a,b) -
		VectorSquare(d) * TriArea(a, b,c) > 0;
}

inline int ccw(const Point2d &a, const Point2d b, const Point2d &c )
{
	return (TriArea(a,b,c)>0);
}

inline int RightOf( const Point2d &x, Edge *e )
{
	//return (ccw(*e->Org(),*e->Dest(),x));
	return ( ccw( x,*e->Dest(),*e->Org() ) );
}

inline int LeftOf( const Point2d &x, Edge *e )
{
	return ( ccw( x, *e->Org(), *e->Dest() ) );
}

inline int OnEdge( const Point2d &x, Edge *e )
{
	Reall t1, t2, t3;
	t1 = (x-e->Org2d()).norm();
	t2 = (x-e->Dest2d()).norm();
	if ( t1<PRECISION || t2<PRECISION )
		return 1;
	t3 = (e->Org2d()-e->Dest2d()).norm();
	if ( t1>t3 || t2>t3 )
		return -1;
	return (x.Distance(e->Org2d(),e->Dest2d())<PRECISION);
}



