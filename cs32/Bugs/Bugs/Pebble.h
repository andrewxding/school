#ifndef PEBBLE_H
#define PEBBLE_H

#include "Actor.h"
#include "GameConstants.h"
#include <list>
using namespace std;
//class Food;

class Insect : public Actor
{
public:
	Insect(StudentWorld *world, int health, int b, int IID, int x, int y, Direction dir = right, unsigned int depth = 0) 
		: Actor(world, health, IID, x, y, dir, depth)
	{
		biteSize = b;
	}
	void bite(Actor *act) 
	{
		act->modifyHealth(-1 * biteSize);
	};
private:
	int biteSize;
};
class Grasshopper : public Insect {
public:
	Grasshopper(StudentWorld *sw, int startX, int startY, int hp, int imageID, int bite)
		:Insect(sw, hp, bite, imageID, startX, startY, none) {
		setRandDirection();
		m_distanceToWalk = randInt(2, 10);
		stunned = 0;
	};
	virtual void doSomething() = 0;
	void Grasshopper::setRandDirection()
	{
		int rand = randInt(1, 4);
		switch (rand)
		{
		case 1:
			setDirection(up);
			break;
		case 2:
			setDirection(down);
			break;
		case 3:
			setDirection(left);
			break;
		case 4:
			setDirection(right);
			break;
		}
	}

//	virtual void bittenPoisonedOrStunned() = 0; MIGHT NEED TO BE NON VIRTUAL
	int Grasshopper::getDistance() {
		return m_distanceToWalk;
	}
	void walk() {
		int rand = randInt(1, 10);
					if (rand == 1)
					{
						//check radius 10  and walk in direction
						//check if can eat 200 food
						int x = randInt(1, 2);
						if (x == 1);
						//fal asleep immediatrely
						else;
						//keep walking
						//reread and redo!!!!!!!!!!!
					}
	}
	void setStunned(int val)
	{
		stunned = val;
	}
	void die()
	{
		setAlive(false);
//		Actor* temp = new Food(getWorld(), getX(), getY(), 100);
//		getWorld()->addActor(temp, getX(), getY());
	}
	int modifyStunned(int val)
	{
		stunned += val;
	}
	int getStunned()
	{
		return stunned;
	}
	

private:
	int m_distanceToWalk;
	int stunned;
};

class AdultGrasshopper : public Grasshopper {
public:
	AdultGrasshopper(StudentWorld *sw, int startX, int startY)
		:Grasshopper(sw, startX, startY, 1600, IID_ADULT_GRASSHOPPER, 50) {};
	virtual void doSomething()
	{
		modifyHealth(-1);
		if (getHealth() <= 0)
			die();
		else
		{
			if (getStunned() == 0)
			{
				int rand = randInt(1, 3);
				if (rand == 1)//if insect on this square too
				{
					//bite everything onsquare, if multiple randomly choose 1
					list<Actor*> temp = getWorld()->getCoordList(getX(), getY());
					if (!temp.empty())
					{
						int rand = randInt(0, temp.size() - 1);
						list<Actor*>::iterator it = temp.begin();
						advance(it, rand);
						bite(*it);
					}

				}
				else
				{
					//walk();
				}
				setStunned(2);
			}
			else
			{
				setStunned(-1);
				//return
			}

		}

	}
};
class BabyGrasshopper : public Grasshopper {
public:
	BabyGrasshopper(StudentWorld *sw, int startX, int startY)
		:Grasshopper(sw, startX, startY, 500, IID_BABY_GRASSHOPPER, 0) {};
	virtual void doSomething()
	{
		modifyHealth(-1);
		if (getHealth() <= 0)
			die();
		else
		{
			if (getStunned() == 0)
			{
				if (getHealth() >= 1600)//if insect on this square too
				{
					Actor* temp = new AdultGrasshopper(getWorld(), getX(), getY());
					getWorld()->addActor(temp, getX(), getY());
					die();
					//return 
				}
				else
				{
					//walk();
				}
				setStunned(2);
			}
			else
			{
				setStunned(-1);
				//return
			}

		}

	}
};
class Pebble :public Actor
{
public:
	Pebble(StudentWorld *world, int x, int y) : Actor(world, -1, IID_ROCK, x, y)
	{
	}
	virtual void doSomething() {};
};

/*class Food : public Actor
{
public:
	Food(StudentWorld *world, int x, int y, int health = 6000) : Actor(world, health, IID_FOOD, x, y, right, 2)
	{};
	virtual void doSomething() {};
};
class Pheromone : public Actor
{
public:
	Pheromone(StudentWorld *world, int x, int y, int IID, int health = 256) : Actor(world, health, IID, x, y, right, 2) {};
	virtual void doSomething()
	{
		modifyHealth(-1);
		if (getHealth() == 0)
			setAlive(false);
	}
};
/*class Anthill : public Actor
{
public:
	Anthill(StudentWorld *world, int x, int y, string name, Compiler comp) : Actor(world, 8999, 0, IID_ANT_HILL, x, y, none, 2), _name(name), instruct(comp)
	{}
	void giveBirth() // used to give birth to a new ant
	{
		// allocate a new ant, and pass in a pointer to the
		// Compiler object for this AntHill. The
		// m_pointerToMyCompilerObject below points to the Compiler
		// object that created in the StudentWorld::init() function
		// above. By passing this into each ant as it’s born, the ant
		// knows how to get hold of its instructions that govern it and
		// other members of its colony 
	//	34
	//		Ant* newAnt = new Ant(..., m_pointerToMyCompilerObject);
		// now add our new ant to our simulation data structures
		// so it can be tracked and asked to do something during each
		// tick by our virtual world
	//	addObjectToSimulation(newAnt);
	//	…
	}
	virtual void doSomething()
	{
		modifyHealth(-1);
		if (getHealth() == 0)
			setAlive(false);
		else
		{
			//check if food on square
			//eat up to 10000
			//return
			if (getHealth() >= 2000)
			{
				//create ant with compiler
				modifyHealth(-1500);
			//	getWorld()->addActor();
			}
		}
	}
private:
	string _name;
	Compiler instruct;
};
*/

		
	


#endif

