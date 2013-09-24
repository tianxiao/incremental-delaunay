#include "StdAfx.h"
#include "PZoneManager.h"
#include <math.h>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>


void dZone::InsertPointInCell(int cXIdx, int cYIdx, double x, double y)
{
	const int pntIdx = mZmesh.NewVertex(x,y);
	const int cellidx = cXIdx + cYIdx*mNumRows;
	mCells[cellidx]->AddPnt(pntIdx);

	// add pnts into the zmesh;
	// mZmesh.InsertPointByIdx( pntIdx );
}

void dZone::InsertPointInCellDirect(int cXIdx, int cYIdx, double x, double y)
{
	const int pntIdx = mZmesh.NewVertex(x,y);
	const int cellidx = cXIdx + cYIdx*mNumRows;
	mCells[cellidx]->AddPnt(pntIdx);

	mZmesh.InsertPointByIdx( pntIdx );
}

void dZone::IncrementalDelaunay()
{
	for ( size_t i=0; i<mCells.size(); ++i ) {
		for ( size_t j=0; j<mCells[i]->mVIdxList.size(); ++j ) {
			mZmesh.InsertPointByIdx( mCells[i]->mVIdxList[j] );
		}
	}
}

PZoneManager::PZoneManager(void)
{
}

PZoneManager::~PZoneManager(void)
{
}

void PZoneManager::InsertPoints(double x, double y)
{
	pntX = x;
	pntY = y;

	const double zoneStepX = (maxX-minX)/mNumProcessorX;
	const double zoneStepY = (maxY-minY)/mNumProcessorY;
	const double cellStepX = (maxX-minX)/(mNumProcessorX*numCellInZoneSide);
	const double cellStepY = (maxY-minY)/(mNumProcessorY*numCellInZoneSide);

	//int zoneIdxX = (int) ((x-minX)/zoneStepX);
	//int zoneIdxY = (int) ((y-minY)/zoneStepY);

	const int cellTIdxX = (int) ((x-minX)/cellStepX);
	const int cellTIdxY = (int) ((y-minY)/cellStepY);

	zoneIdxX = (int) (cellTIdxX/(1.*numCellInZoneSide));
	zoneIdxY = (int) (cellTIdxY/(1.*numCellInZoneSide));
	cellInZoneIdxX = cellTIdxX - zoneIdxX*numCellInZoneSide;
	cellInZoneIdxY = cellTIdxY - zoneIdxY*numCellInZoneSide;

	//InsertZone( zoneIdxX, zoneIdxY, cellInZoneIdxX, cellInZoneIdxY, x, y );

}

void PZoneManager::DispatchZone()
{
	InsertZone( zoneIdxX, zoneIdxY, cellInZoneIdxX, cellInZoneIdxY, pntX, pntY );
}

void PZoneManager::InsertZone(int zXIdx, int zYIdx, int cInZXIdx, int cInZyIdx, double x, double y)
{
	const int zoneIdx = zXIdx+zYIdx*mNumProcessorX;
	mZones[zoneIdx]->InsertPointInCell( cInZXIdx, cInZyIdx, x, y );
}

void PZoneManager::MultiThreadPartionDelaunay()
{
	std::vector<DoDelaunayInZone> dodelaunaylist;
	for ( size_t i=0; i<mZones.size(); ++i ) {
		DoDelaunayInZone de(mZones[i]);
		dodelaunaylist.push_back(de);
	}
	//boost::thread thrd1(CountFun(1));
	std::vector<boost::thread *> threadlist;
	for ( size_t i=0; i<dodelaunaylist.size(); ++i ) {
		threadlist.push_back( new boost::thread(dodelaunaylist[i]) );
	}

	for ( size_t i=0; i<threadlist.size(); ++i ) {
		threadlist[i]->join();
	}

	for ( size_t i=0; i<threadlist.size(); ++i ) {
		delete threadlist[i];
		threadlist[i] = NULL;
	}
}

void PZoneManager::SingleThreadPartionDelaunay()
{
	for ( size_t i=0; i<mZones.size(); ++i ) {
		mZones[i]->IncrementalDelaunay();
	}
}

void PZoneManager::CalculatePartitionParameters()
{
	double rx = maxX-minX;
	double ry = maxY-minY;
	double preNumOfCells = mNumTotalPnts/(1.*mNumPntsPerCell);
	double preNCX = sqrt((rx/ry)* preNumOfCells);
	double preNCY = (ry/rx)*preNCX;

	double preProcX = sqrt((rx/ry)*mTotalProcessors);
	double preProcY = (ry/rx)*preProcX;
	mNumProcessorX = (int) preProcX;
	mNumProcessorY = (int) preProcY;

	numCellInZoneSide = (int) sqrt( (preNCX*preNCY)/(mNumProcessorX*mNumProcessorX) );
	mNumCellX = numCellInZoneSide*mNumProcessorX;
	mNumCellY = numCellInZoneSide*mNumProcessorY;
}

void PZoneManager::CreateZonePool()
{
	double min[2] = {minX, minY};
	double max[2] = {maxX, maxY};

	mZones.reserve( mNumProcessorY*mNumProcessorX );
	dZone *pzone;
	for ( int row=0; row<mNumProcessorY; ++row ) {
		for ( int column=0; column<mNumProcessorX; ++column ) {
			pzone = new dZone(row*mNumProcessorX+column); 
			pzone->Initialize( numCellInZoneSide, numCellInZoneSide, min, max );
			mZones.push_back( pzone );
		}
	}
}

void PZoneManager::Initialize()
{
	CalculatePartitionParameters();
	CreateZonePool();
}

class DoDumFileMulti
{
private:
	const char *mFilename;
	dZone *mZone;
public:
	DoDumFileMulti(const char *filename, dZone *zone) : mFilename(filename ), mZone(zone) {};
	void operator()()
	{
		mZone->DumpFemFile(mFilename);
	}

};

void PZoneManager::DumZoneFem( const char *filename )
{
	const int STR_LENGTH = 30;
	char buffer[STR_LENGTH];

#if 0
	for ( size_t i=0; i<mZones.size(); ++i ) {
		sprintf(buffer,"%s_%d.fem",filename,i);
		mZones[i]->DumpFemFile(buffer);
	}
#else
	std::vector<boost::thread *> threadlist;
	for ( size_t i=0; i<mZones.size(); ++i ) {
		sprintf(buffer,"%s_%d.fem",filename,i);
		DoDumFileMulti ddump(buffer,mZones[i]);
		threadlist.push_back( new boost::thread( ddump ) );
		//threadlist[threadlist.size()-1]->join();
		//mZones[i]->DumpFemFile(buffer);
	}

	for ( size_t i=0; i<threadlist.size(); ++i ) {
		threadlist[i]->join();
	}

	for ( size_t i=0; i<threadlist.size(); ++i ) {
		delete threadlist[i];
		threadlist[i] = NULL;
	}
#endif

}
