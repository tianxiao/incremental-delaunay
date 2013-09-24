#pragma once
#include "FaceAdjMesh.h"
#include <vector>

struct dVertex : public dPoint2dDefault
{

};

struct dTriFace : public dTriFaceDefault
{

};

typedef FaceAdjMesh<dVertex, dTriFace> FaceAdjMeshP;

class dZone;
class dCell
{
	friend class dZone;
public:
	void SetZone(dZone *pzone) { mZone = pzone; };
	void AddPnt(int idx)
	{
		mVIdxList.push_back(idx);
	}

private:
	//std::vector<dVertex> Vertexlist;
	std::vector<int> mVIdxList;
	dZone *mZone;
};

class dZone
{
	friend class DoDelaunayInZone;
public:
	dZone(int id=0) :mZoneId(id) {};
	~dZone() 
	{
		DestroyCelles();
	}

	void Initialize(int row, int colum, double min[], double max[])
	{
		mNumRows = row;
		mNumColumn = colum;
		if ( mCells.size()==0 ) { 
			CreateCellMatrix();
			mZmesh.InitializeBBox(min,max);
		}
	}

	void InsertPointInCell(int cXIdx, int cYIdx, double x, double y);

	void InsertPointInCellDirect(int cXIdx, int cYIdx, double x, double y);

	void IncrementalDelaunay();

	void DumpFemFile( const char *filename)
	{
		mZmesh.DumpMeshFem(filename);
	}
	
private:
	void CreateCellMatrix()
	{
		mCells.reserve(mNumRows*mNumColumn);
		dCell *pcell = NULL;
		for ( int row=0; row<mNumColumn; ++row ) {
			for ( int column=0; column<mNumRows; ++column ) {
				pcell = new dCell;
				pcell->SetZone( this );
				mCells.push_back(pcell);
			}
		}
	}

	void DestroyCelles()
	{
		for ( size_t i=0; i<mCells.size(); ++i ) {
			delete mCells[i];
			mCells[i] = NULL;
		}
	}

	
private:
	std::vector<dCell *> mCells;
	FaceAdjMeshP mZmesh;
	int mNumRows;
	int mNumColumn;
	int mZoneId;
};

class DoDelaunayInZone
{
private:
	dZone *mZone;
public:
	DoDelaunayInZone(dZone *pzone) :mZone(pzone) {};
	void operator()()
	{
		mZone->IncrementalDelaunay();
		//char buffer[30];
		//char *filepre = "partion";
		//sprintf(buffer,"%s_%d.fem",filepre,mZone->mZoneId);
		//mZone->DumpFemFile(buffer);
	}
};

class PZoneManager
{
public:
	PZoneManager(void);
	~PZoneManager(void);

	//==================================================================
	// Parameters setting
	void SetRange(double minx, double maxx, double miny, double maxy)
	{
		minX = minx; maxX = maxx;
		minY = miny; maxY = maxy;
	}

	void SetProcessor(int totalunit)
	{
		mTotalProcessors = totalunit;
	}

	void SetPntsNumPerCell( int pntspercell=4)
	{
		mNumPntsPerCell = pntspercell;
	}

	void SetPntsNum( int totalPnts )
	{
		mNumTotalPnts = totalPnts;
	}
	//==================================================================

	void Initialize();

	void InsertPoints(double x, double y);
	void DispatchZone();

	void SingleThreadPartionDelaunay();
	void MultiThreadPartionDelaunay();

	void DumZoneFem( const char *filename );
	
private:
	void CalculatePartitionParameters();
	void CreateZonePool();

	void InsertZone(int zXIdx, int zYIdx, int cInZXIdx, int cInZyIdx, double x, double y);
	
private:
	void DestroyZones()
	{
		for ( size_t i=0; i<mZones.size(); ++i ) {
			delete mZones[i];
			mZones[i] = NULL;
		}
	}

private:
	std::vector<dZone *> mZones;
	int mNumCellX;
	int mNumCellY;

	//================
	double minX;
	double minY;
	double maxX;
	double maxY;

	//================
	int mNumProcessorX; // Zone
	int mNumProcessorY; // Zone
	int mTotalProcessors;
	int numCellInZoneSide;

	//================
	int mNumPntsPerCell;

	int mNumTotalPnts;

	//================
	int zoneIdxX;
	int zoneIdxY;
	int cellInZoneIdxX;
	int cellInZoneIdxY;
	double pntX;
	double pntY;
};

