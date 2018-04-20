#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
class StudentWorld;
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject
{
public:
	Actor(StudentWorld *w, int health, int imageID, int startX, int startY, Direction dir = right, unsigned int depth = 0);
	virtual void doSomething() = 0; 
	void modifyHealth(int health);
	//mutator functions
	void setAlive(bool status);
	//accessor functions
	bool alive() { return _isAlive; };
	int getHealth() { return _health; };
	StudentWorld* getWorld() { return world; };

private:
	int _health;
	int _bite;	
	bool _isAlive;
	StudentWorld* world;
};
#endif // ACTOR_H_
