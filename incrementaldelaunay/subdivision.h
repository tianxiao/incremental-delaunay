#pragma once
//#include "quadedge.h"
class Edge;
struct Point2d;


class subdivision
{
private:
	Edge *startingEdge;
	Edge *Locate(const Point2d &x);
public:
	subdivision(const Point2d &a, const Point2d &b, const Point2d &c );
	void InsertSite( const Point2d &x);
	void Draw();
	~subdivision(void);
};

