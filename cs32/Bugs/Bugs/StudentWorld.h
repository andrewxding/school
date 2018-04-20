#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include <list>
#include <vector>

class Actor;
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
using namespace std;

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
	 : GameWorld(assetDir)
	{
	}
	int loadField();
	virtual int init();
	virtual int move();
	
	int getTicks();
	void updateDisplayText();
	void updateTickCount();
	virtual void cleanUp();
	int getNumberOfAntsForAnt(int i);
	void removeDeadSimulationObjects();
	string formatDisplay(int ticks, int antsAnt0, int antsAnt1, int antsAnt2, int antsAnt3, int winningAntNumber);
	void addActor(Actor* act, int x, int y);
	//need an add actor functoin

	//accessor for vector
	list<Actor*> getCoordList(int x, int y)
	{
		return grid[y][x];
	}


	/*stuff you can use 
string assetDirectory() const; returns the name of the directory that contains the simulationassets (image, sound, and field data files).
void setWinner(string name);
vector<string> getFilenamesOfAntPrograms() const;d provides you with a vector containing the filenames */
private:
	int ticks;
	list<Actor*> grid[64][64];
	vector<int> counts;
	//#of ants dictionary
};

#endif // STUDENTWORLD_H_
