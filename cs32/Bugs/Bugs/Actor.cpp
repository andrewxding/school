#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp
Actor::Actor(StudentWorld *w, int health, int imageID, int startX, int startY, Direction dir, unsigned int depth) :
	GraphObject(imageID, startX, startY, dir, depth)//, world(this)
{
	world = w;
	_health = health;
}


//mutator functions
void Actor::modifyHealth(int health)
{
	_health += health;
}
void Actor::setAlive(bool status) 
{ 
	_isAlive = status;

}
