#pragma once

class Edge;

class testQuadEdgeTopo
{
public:
	testQuadEdgeTopo(void);
	~testQuadEdgeTopo(void);

	static bool EdgeValide(Edge *e);

	void RunTest();
};

