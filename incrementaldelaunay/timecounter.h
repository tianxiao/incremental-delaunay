#pragma once
class timecounter
{
public:
	timecounter(void);
	~timecounter(void);
};


class dTime
{
public:
	void start();
	unsigned long ellips();

private:
	unsigned long mCount;
};

