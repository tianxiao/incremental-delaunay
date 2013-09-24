#pragma once
#include <assert.h>
#include <map>
#include <vector>
#include <queue>
#include "RTree.h"


struct dTriFaceGen
{
	int vIds[3];
	int aIds[3];
};

struct dTriFaceDefault
{
	int vIds[3];
	int aIds[3];
};

struct dPoint2dDefault
{
	double x;
	double y;
};


template<typename dPoint2d> 
inline dPoint2d MindPoint2d(dPoint2d &c, dPoint2d &n)
{
	dPoint2d p;
	p.x = c.x<n.x?c.x:n.x;
	p.y = c.y<n.y?c.y:n.y;
	return p;
}

template<typename dPoint2d>
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

template<typename dTriFace>
inline int GetASlot(dTriFace *tri, int aId)
{
	if ( tri->aIds[0]==aId ) return 0;
	if ( tri->aIds[1]==aId ) return 1;
	if ( tri->aIds[2]==aId ) return 2;
	assert(false);
	return -1;
}

template<typename dPoint2d,typename dTriFace>
struct DataPool
{
	std::map<int, dTriFace*> mTris;
	std::vector<dPoint2d *> mVerts;
};

/**
* Not a good practice! Need to seperate the mesh data and the mesh algorithm
* Especially the mesh topological structure.
*/
template<typename dPoint2d,typename dTriFace>
class FaceAdjMesh
{
public:
	FaceAdjMesh(void);
	~FaceAdjMesh(void);

	//FaceAdjMesh(DataPool<dPoint2d, dTriFace> &datapool);
	//void InitializeDataPool(DataPool<dPoint2d, dTriFace> &datapool);

	void InitializeBBox(double min[], double max[]);

	void Initialize(double *p0, double *p1, double *p2);
	void InsertPoint(double x, double y);

	void DumpMesh( const char *filename );
	void DumpMeshFem( const char *filename );

	int NewVertex(double x, double y);
	void InsertPointByIdx(int idx);

private:
	dTriFace *GetFace(int idx);
	
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



#include "exactpredicator.h"
#include <queue>

bool RtreeSearchCallback(int fid, void* arg);

template<typename dPoint2d,typename dTriFace>
FaceAdjMesh<dPoint2d,dTriFace>::FaceAdjMesh(void)
	:mTriCount(0)
{
}

template<typename dPoint2d,typename dTriFace>
FaceAdjMesh<dPoint2d,dTriFace>::~FaceAdjMesh(void)
{
	ReleaseMem();
}

template<typename dPoint2d,typename dTriFace>
void FaceAdjMesh<dPoint2d,dTriFace>::InitializeBBox(double min[], double max[])
{
	double pntA[2] = {0.5*(min[0]+max[0]),(2*max[1]-min[1])};
	double pntB[2] = {1.5*min[0]-0.5*max[0],min[1]};
	double pntC[2] = {1.5*max[0]-0.5*min[0],min[1]};

	Initialize(pntA, pntB, pntC);
}

template<typename dPoint2d,typename dTriFace>
void FaceAdjMesh<dPoint2d,dTriFace>::Initialize(double *p0, double *p1, double *p2)
{
	dTriFace* boundtri;
	const int boudntriIdx = NewTriFace(boundtri);

	const int v0Idx = NewVertex(p0[0],p0[1]);
	const int v1Idx = NewVertex(p1[0],p1[1]);
	const int v2Idx = NewVertex(p2[0],p2[1]);

	boundtri->vIds[0] = v0Idx;
	boundtri->vIds[1] = v1Idx;
	boundtri->vIds[2] = v2Idx;

	PushBBox(boudntriIdx, boundtri);

}

template<typename dPoint2d,typename dTriFace>
dTriFace *FaceAdjMesh<dPoint2d,dTriFace>::GetFace(int idx)
{
	std::map<int, dTriFace*>::iterator it = mTris.find(idx);
	if ( it!=mTris.end() ) 
		return (it->second);
	else
		return NULL;
	
}

template<typename dPoint2d,typename dTriFace>
int FaceAdjMesh<dPoint2d,dTriFace>::NewVertex(double x, double y)
{
	//int count = mVerts.size();
	dPoint2d *p = new dPoint2d;
	p->x = x;
	p->y = y;
	mVerts.push_back(p);
	return mVerts.size()-1;
}

template<typename dPoint2d,typename dTriFace>
int FaceAdjMesh<dPoint2d,dTriFace>::NewTriFace(dTriFace *&temp)
{
	temp = new dTriFace;
	// Intialize the adjacent face to -1;
	for ( int i=0; i<3; ++i ) {
		temp->aIds[i] = -1;
	}
	mTris[mTriCount] = temp;
	mTriCount++;
	return (mTriCount-1);
}

template<typename dPoint2d,typename dTriFace>
void FaceAdjMesh<dPoint2d,dTriFace>::DeleteTriFace( int idx)
{
	dTriFace *tri = GetFace(idx);
	assert(tri);

	//=================================================================
	dPoint2d pMin;
	dPoint2d pMax;
	TriangleBBox( pMin, pMax, tri );
	mRtree.Remove( &(pMin.x), &(pMax.x), idx);

	delete tri;
	tri = NULL;
	mTris.erase(idx);
}

template<typename dPoint2d,typename dTriFace>
void FaceAdjMesh<dPoint2d,dTriFace>::Splite(int inIdx, int pntIdx )
{
	dTriFace *inTri = GetFace(inIdx);
	assert(inTri);
	dTriFace *adjs[3];
	for ( int i=0; i<3; ++i ) {
		adjs[i] = GetFace(inTri->aIds[i]);
	}

	dTriFace *newTris[3];
	int newIds[3];
	for ( int i=0; i<3; ++i ) {
		newIds[i] =NewTriFace(newTris[i]);
		newTris[i]->vIds[0] = inTri->vIds[i];
		newTris[i]->vIds[1] = inTri->vIds[(i+1)%3];
		newTris[i]->vIds[2] = pntIdx;
		PushBBox(newIds[i],newTris[i]);

		newTris[i]->aIds[0] = inTri->aIds[i];
	}
	

	for ( int i=0; i<3; ++i ) {
		newTris[i]->aIds[1] = newIds[(i+1)%3];
		newTris[i]->aIds[2] = newIds[(i+2)%3];
	}

	int adjsslot[3] = {-1,-1,-1};
	for ( int i=0; i<3; ++i ) {
		if ( adjs[i] ) {
			int slotidx = GetASlot(adjs[i], inIdx);
			adjsslot[i] = slotidx;
			adjs[i]->aIds[slotidx] = newIds[i];
		}
	}

	DeleteTriFace(inIdx);

	std::queue<dSwapEdge> swapedgelist;

	//===================================================================
	for ( int i=0; i<3; ++i ) {
		if ( adjs[i] ) {
			dPoint2d *p = mVerts[adjs[i]->vIds[(adjsslot[i]+2)%3]];
			if ( InTriCircle(newTris[i],p->x, p->y) ) {
				dSwapEdge se;
				// This eIdx is the new edge's index
				// Which is the new edge's boundary the start edge
				se.eIdx = 0;
				// This fIdx should be the new edge
				se.fIdx = newIds[i];
				//Swap(&se);
				SwapPush(&se,swapedgelist);
			}
		}
	}

	while ( !swapedgelist.empty() ) {
		dSwapEdge se = swapedgelist.front();
		dPoint2d *p = mVerts[se.upTopIdx];
		std::map<int, dTriFace*>::iterator fit = mTris.find(se.fIdx);
		if ( fit!=mTris.end()&&InTriCircle(fit->second,p->x,p->y) ) {
			SwapPush(&se,swapedgelist);
		}
		swapedgelist.pop();
	}

	
}

template<typename dPoint2d,typename dTriFace>
void FaceAdjMesh<dPoint2d,dTriFace>::SwapPush(dSwapEdge *se, std::queue<dSwapEdge> &swapedgelist)
{
	dSwapEdge fdownleftse;
	dSwapEdge fdownrightse;
	dSwapEdge fupleftse;
	dSwapEdge fuprightse;
	Swap(se,fdownleftse,fdownrightse,fupleftse,fuprightse);
	if ( fdownleftse.fIdx!=-1 ) 
		swapedgelist.push(fdownleftse);
	if ( fdownrightse.fIdx!=-1 )
		swapedgelist.push(fdownrightse);
	if ( fupleftse.fIdx !=-1 )
		swapedgelist.push(fupleftse);
	if ( fuprightse.fIdx !=-1 )
		swapedgelist.push(fuprightse);
}

template<typename dPoint2d,typename dTriFace>
void FaceAdjMesh<dPoint2d,dTriFace>::Swap(dSwapEdge *se)
{
	const int seidx = se->eIdx;

	const int downfidx = se->fIdx;

	dTriFace *fdown = GetFace(downfidx);

	const int upfidx = fdown->aIds[seidx];
	dTriFace *fup   = GetFace(upfidx);
	if (!fup) return; //  no adjacent face at the se->eIdx side;

	const int fdownleftidx = fdown->aIds[(seidx+1)%3];
	dTriFace *fdownleft = GetFace(fdownleftidx);
	const int fdownrightidx = fdown->aIds[(seidx+2)%3];
	dTriFace *fdownright = GetFace(fdownrightidx);
	const int seinvidx = GetASlot(fup, se->fIdx);
	const int fupleftidx = fup->aIds[(seinvidx+2)%3];
	dTriFace *fupleft	= GetFace(fupleftidx);
	const int fuprightidx = fup->aIds[(seinvidx+1)%3];
	dTriFace *fupright   = GetFace(fuprightidx);

	// create new face
	dTriFace *newleft;
	int newleftIdx = NewTriFace(newleft);
	
	dTriFace *newright;
	int newrightIdx = NewTriFace(newright);
	
	const int v1 = fdown->vIds[(seidx)%3];
	const int v0 = fdown->vIds[(seidx+1)%3];
	const int v3 = fdown->vIds[(seidx+2)%3];
	const int v2 = fup->vIds[(seinvidx+2)%3];
	
	newleft->vIds[0] = v3;
	newleft->vIds[1] = v2;
	newleft->vIds[2] = v0;
	newleft->aIds[0] = newrightIdx ;
	newleft->aIds[1] = fupleftidx;
	newleft->aIds[2] = fdownleftidx;

	newright->vIds[0] = v2;
	newright->vIds[1] = v3;
	newright->vIds[2] = v1;
	newright->aIds[0] = newleftIdx;
	newright->aIds[1] = fdownrightidx ;
	newright->aIds[2] = fuprightidx;
	
	// update outter adj info
	if ( fdownleft ) {
		const int fdownleftslotidx = GetASlot(fdownleft, downfidx);
		fdownleft->aIds[fdownleftslotidx] = newleftIdx;
	}
	if ( fupleft ) {
		const int fupleftslotidx = GetASlot(fupleft, upfidx);
		fupleft->aIds[fupleftslotidx] = newleftIdx;
	}
	if ( fdownright ) {
		const int fdownrightidx = GetASlot(fdownright,downfidx);
		fdownright->aIds[fdownrightidx] = newrightIdx;
	}
	if ( fupright ) {
		const int fuprightidx = GetASlot(fupright, upfidx);
		fupright->aIds[fuprightidx] = newrightIdx;
	}

	PushBBox( newrightIdx, newright );
	PushBBox( newleftIdx, newleft );

	// delete old face
	DeleteTriFace(downfidx);
	DeleteTriFace(upfidx);
}

template<typename dPoint2d,typename dTriFace>
bool FaceAdjMesh<dPoint2d,dTriFace>::InTriangle(dTriFace *tri,double x, double y)
{
	double pc[2];
	pc[0] = x;
	pc[1] = y;
	double p[3][2];
	for ( int i=0; i<3; ++i ) {
		p[i][0] = mVerts[tri->vIds[i]]->x;
		p[i][1] = mVerts[tri->vIds[i]]->y;
	} 

	if ( 
		orient2d(p[0],p[1],pc)>0 &&
		orient2d(p[1],p[2],pc)>0 &&
		orient2d(p[2],p[0],pc)>0
		) {
		return true;
	} else {
		return false;
	}

}

template<typename dPoint2d,typename dTriFace>
int FaceAdjMesh<dPoint2d,dTriFace>::LocatePoint(double x, double y)
{
	double xy[2] = {x,y};
	dDynaArrayW dylist;
	mRtree.Search(xy,xy,RtreeSearchCallback,&dylist);

	for ( size_t i=0; i<dylist.list.size(); ++i ) {
		dTriFace *f = mTris[dylist.list[i]];
		if ( InTriangle(f,x,y) ) {
			return dylist.list[i];
		}
	}

	//assert(false);
	return -1;
}

template<typename dPoint2d,typename dTriFace>
void FaceAdjMesh<dPoint2d,dTriFace>::InsertPoint(double x, double y)
{
	const int infidx = LocatePoint(x,y);
	if (infidx==-1) return ;
	const int newvertidx = NewVertex(x,y);
	Splite(infidx,newvertidx);
}

template<typename dPoint2d,typename dTriFace>
void FaceAdjMesh<dPoint2d,dTriFace>::InsertPointByIdx(int idx)
{
	//assert(mVerts[idx]);
	assert(mVerts.size()>idx&&idx>0);
	const int infidx = LocatePoint(mVerts[idx]->x, mVerts[idx]->y);
	if ( infidx==-1 ) return ;
	Splite(infidx,idx);
}

template<typename dPoint2d,typename dTriFace>
bool FaceAdjMesh<dPoint2d,dTriFace>::InTriCircle(dTriFace *tri, double x, double y)
{
	double plist[3][2];
	for ( int i=0; i<3; ++i ) {
		plist[i][0] = mVerts[tri->vIds[i]]->x;
		plist[i][1] = mVerts[tri->vIds[i]]->y;
	}
	double outp[2] = {x,y};

	return (incircle(plist[0],plist[1],plist[2],outp)>0);
}

template<typename dPoint2d,typename dTriFace>
void FaceAdjMesh<dPoint2d,dTriFace>::DumpMesh( const char *filename )
{
	FILE *fp = fopen(filename,"w");
	srand(NULL);
	fprintf(fp,"OFF\n");
	fprintf(fp,"%d %d 0\n",mVerts.size(),mTris.size());
	for ( size_t i=0; i<mVerts.size(); ++i ) {
		fprintf(fp,"%f %f %f\n",mVerts[i]->x, mVerts[i]->y, 0.0);//(mVerts[i]->x*mVerts[i]->x+mVerts[i]->y*mVerts[i]->y)/64.0);//rand()/(RAND_MAX*1.0)*5.90+0.5);
	}
	std::map<int, dTriFace*>::iterator it = mTris.begin();
	for ( ; it!=mTris.end(); ++it ) {
		fprintf(fp,"%d %d %d %d\n", 3, it->second->vIds[0],it->second->vIds[2],it->second->vIds[1]);
	}
	fclose(fp);
}

template<typename dPoint2d,typename dTriFace>
void FaceAdjMesh<dPoint2d,dTriFace>::DumpMeshFem( const char *filename )
{
	FILE *fp = fopen(filename,"w");
	srand(NULL);

	//fprintf(fp,"%d %d 0\n",mVerts.size(),mTris.size());
	int GRIDcounter = 1;
	for ( size_t i=0; i<mVerts.size(); ++i ) {
		fprintf(fp,"GRID, %d, ,%f,%f,%f\n",GRIDcounter++,mVerts[i]->x, mVerts[i]->y, 0.0);
	}
	int CTRIA3counter = 1;
	std::map<int, dTriFace*>::iterator it = mTris.begin();
	for ( ; it!=mTris.end(); ++it ) {
		fprintf(fp,"CTRIA3,%d,1,%d,%d,%d\n",it->first/*CTRIA3counter++*/,it->second->vIds[0]+1,it->second->vIds[2]+1,it->second->vIds[1]+1);
	}
	fclose(fp);
}

template<typename dPoint2d,typename dTriFace>
void FaceAdjMesh<dPoint2d,dTriFace>::PushBBox(int fid, dTriFace *tri)
{
	//mVerts[tri->vIds[0]];
	dPoint2d pMin;
	dPoint2d pMax;
	TriangleBBox(pMin,pMax,tri);
	mRtree.Insert( &(pMin.x), &(pMax.x), fid);
}

template<typename dPoint2d,typename dTriFace>
void FaceAdjMesh<dPoint2d,dTriFace>::ReleaseMem()
{
	std::map<int, dTriFace*>::iterator it;
	for ( it=mTris.begin(); it!=mTris.end(); ++it ) {
		delete it->second;
		it->second = NULL;
	}

	for ( size_t i=0; i<mVerts.size(); ++i ) {
		delete mVerts[i];
		mVerts[i] = NULL;
	}
}

template<typename dPoint2d,typename dTriFace>
void FaceAdjMesh<dPoint2d,dTriFace>::Swap(dSwapEdge *se, 
	dSwapEdge &fdownleftse, 
	dSwapEdge &fdownrightse,
	dSwapEdge &fupleftse,
	dSwapEdge &fuprightse)
{
	const int seidx = se->eIdx;

	const int downfidx = se->fIdx;

	dTriFace *fdown = GetFace(downfidx);

	const int upfidx = fdown->aIds[seidx];
	dTriFace *fup   = GetFace(upfidx);
	if (!fup) {
		fdownleftse.fIdx = -1;
		fupleftse.fIdx = -1;
		fdownrightse.fIdx = -1;
		fuprightse.fIdx = -1;
		return; //  no adjacent face at the se->eIdx side;
	}

	const int fdownleftidx = fdown->aIds[(seidx+1)%3];
	dTriFace *fdownleft = GetFace(fdownleftidx);
	const int fdownrightidx = fdown->aIds[(seidx+2)%3];
	dTriFace *fdownright = GetFace(fdownrightidx);
	const int seinvidx = GetASlot(fup, se->fIdx);
	const int fupleftidx = fup->aIds[(seinvidx+2)%3];
	dTriFace *fupleft	= GetFace(fupleftidx);
	const int fuprightidx = fup->aIds[(seinvidx+1)%3];
	dTriFace *fupright   = GetFace(fuprightidx);

	// create new face
	dTriFace *newleft;
	int newleftIdx = NewTriFace(newleft);
	
	dTriFace *newright;
	int newrightIdx = NewTriFace(newright);
	
	const int v1 = fdown->vIds[(seidx)%3];
	const int v0 = fdown->vIds[(seidx+1)%3];
	const int v3 = fdown->vIds[(seidx+2)%3];
	const int v2 = fup->vIds[(seinvidx+2)%3];
	
	newleft->vIds[0] = v3;
	newleft->vIds[1] = v2;
	newleft->vIds[2] = v0;
	newleft->aIds[0] = newrightIdx ;
	newleft->aIds[1] = fupleftidx;
	newleft->aIds[2] = fdownleftidx;

	newright->vIds[0] = v2;
	newright->vIds[1] = v3;
	newright->vIds[2] = v1;
	newright->aIds[0] = newleftIdx;
	newright->aIds[1] = fdownrightidx ;
	newright->aIds[2] = fuprightidx;
	
	// update outter adj info
	if ( fdownleft ) {
		const int fdownleftslotidx = GetASlot(fdownleft, downfidx);
		fdownleft->aIds[fdownleftslotidx] = newleftIdx;
		fdownleftse.eIdx = 2;
		fdownleftse.fIdx = newleftIdx;
		fdownleftse.upTopIdx = fdownleft->vIds[(fdownleftslotidx+2)%3];
	} else {
		fdownleftse.fIdx = -1;
	}

	if ( fupleft ) {
		const int fupleftslotidx = GetASlot(fupleft, upfidx);
		fupleft->aIds[fupleftslotidx] = newleftIdx;
		fupleftse.eIdx = 1;
		fupleftse.fIdx = newleftIdx;
		fupleftse.upTopIdx = fupleft->vIds[(fupleftslotidx+2)%3];
	} else {
		fupleftse.fIdx = -1;
	}

	if ( fdownright ) {
		const int fdownrightidx = GetASlot(fdownright,downfidx);
		fdownright->aIds[fdownrightidx] = newrightIdx;
		fdownrightse.eIdx = 1;
		fdownrightse.fIdx = newrightIdx;
		fdownrightse.upTopIdx = fdownright->vIds[(fdownrightidx+2)%3];
	} else {
		fdownrightse.fIdx = -1;
	}

	if ( fupright ) {
		const int fuprightidx = GetASlot(fupright, upfidx);
		fupright->aIds[fuprightidx] = newrightIdx;
		fuprightse.eIdx = 2;
		fuprightse.fIdx = newrightIdx;
		fuprightse.upTopIdx = fupright->vIds[(fuprightidx+2)%3];
	} else {
		fuprightse.fIdx = -1;
	}

	PushBBox( newrightIdx, newright );
	PushBBox( newleftIdx, newleft );

	// delete old face
	DeleteTriFace(downfidx);
	DeleteTriFace(upfidx);
}





