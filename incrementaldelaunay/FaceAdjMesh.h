#pragma once
#include <assert.h>
#include <map>
#include <vector>
#include <queue>
#include "RTree.h"


struct dTriFaceGen
{

};

struct dTriFace
{
	int vIds[3];
	int aIds[3];
};

struct dPoint2d
{
	double x;
	double y;
};

inline dPoint2d MindPoint2d(dPoint2d &c, dPoint2d &n)
{
	dPoint2d p;
	p.x = c.x<n.x?c.x:n.x;
	p.y = c.y<n.y?c.y:n.y;
	return p;
}

inline dPoint2d MaxdPoint2d(dPoint2d &c, dPoint2d &n)
{
	dPoint2d p;
	p.x = c.x>n.x?c.x:n.x;
	p.y = c.y>n.y?c.y:n.y;
	return p;
}

struct dSwapEdge
{
	int fIdx;
	int eIdx;
	int upTopIdx;
};

struct dDynaArrayW
{
	std::vector<int> list;
};

struct txRect2F
{
	txRect2F(double minx, double miny, double maxx, double maxy)
	{
		min[0] = minx;
		min[1] = miny;
		max[0] = maxx;
		max[1] = maxy;
	}

	double min[2];
	double max[2];
};

//bool MySearchCallback(int id, void* arg) 
//{
//	std::vector<int> triIdlist
//  return true; // keep going
//}

inline int GetASlot(dTriFace *tri, int aId)
{
	if ( tri->aIds[0]==aId ) return 0;
	if ( tri->aIds[1]==aId ) return 1;
	if ( tri->aIds[2]==aId ) return 2;
	assert(false);
	return -1;
}



class FaceAdjMesh
{
public:
	FaceAdjMesh(void);
	~FaceAdjMesh(void);

	void InitializeBBox(double min[], double max[]);

	void Initialize(double *p0, double *p1, double *p2);
	void InsertPoint(double x, double y);

	void DumpMesh( const char *filename );
	void DumpMeshFem( const char *filename );

private:
	dTriFace *GetFace(int idx);
	int NewVertex(double x, double y);
	int NewTriFace(dTriFace *&temp);
	void DeleteTriFace( int idx);
	void Splite(int inIdx, int pntIdx);
	void Swap(dSwapEdge *se);
	void Swap(dSwapEdge *se, 
		dSwapEdge &fdownleftse, 
		dSwapEdge &fdownrightse,
		dSwapEdge &fupleftse,
		dSwapEdge &fuprightse);

	void SwapPush(dSwapEdge *se, std::queue<dSwapEdge> &queue);

	bool InTriangle(dTriFace *tri,double x, double y);
	int LocatePoint(double x, double y);
	bool InTriCircle(dTriFace *tri, double x, double y);

	// rtree
	void PushBBox(int fid, dTriFace *tri);

	void TriangleBBox( dPoint2d &pMin, dPoint2d &pMax, dTriFace *tri)
	{
		pMin = *(mVerts[tri->vIds[0]]);
		pMax = *(mVerts[tri->vIds[0]]);
		for ( int i=1; i<3; i++ ) {
			pMin = MindPoint2d(pMin,*(mVerts[tri->vIds[i]]));
			pMax = MaxdPoint2d(pMax,*(mVerts[tri->vIds[i]]));
		}
	}

	void ReleaseMem();

	
private:
	int mTriCount;
	std::map<int, dTriFace*> mTris;
	std::vector<dPoint2d *> mVerts;

	RTree<int, double, 2> mRtree;
};

