#include "StudentWorld.h"
#include <string>
#include "Field.h"
#include "Pebble.h"
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

int StudentWorld::init()//page19	
{
	ticks = 0;
	return 0;
	//load compiler
//	Compiler c;
//	if (c.compile("dumbant.bug"))
//	{
		// Successfully compiled! Woot!
//		cout << "Compiled ant named : " << c.getColonyName() << endl;
		//create anthill at random loc, save the loc of anthill to access in stl vector
//	}

}

void StudentWorld::cleanUp()
{
/*	for (int i = 0; i < 64; i++)
	{
		for (int j = 0; j < 64; j++)
		{
			list<Actor*>::iterator it = grid[i][j].begin();	
			for (int i = 0; i < grid[i][j].size(); i++ )
			{	
				delete *it;
				it = grid[i][j].erase(it);
			}
				
		}
	}*/
}
int StudentWorld::loadField()
{
	string fieldFileName;
	Field f;
	string fieldFile = getFieldFilename();

	if (f.loadField(fieldFile) != Field::LoadResult::load_success)
		return false; // something bad happened!

	for(int x = 0; x < 64; x++)
	{
		for(int y = 0; y < 64; y++)
		{
			Field::FieldItem item = f.getContentsOf(x, y); // note it’s x,y and not y,x!!!
			if (item == Field::FieldItem::rock)
			{
				cout << "A pebble should be placed at " << y << x << " in the field\n";
				Actor *temp = new Pebble(this, x, y);
				grid[y][x].push_back(temp);
			}
/*			case Field::FieldItem::food:
					cout << "A Food should be placed at " << y << x << " in the field\n";
					Actor *temp = new Food(this, x, y);
					grid[y][x].push_back(temp);
					break;*/
			if (item == Field::FieldItem::grasshopper)
			{
				cout << "A Grasshopper should be placed at " << y << x << " in the field\n";
				Actor *temp = new BabyGrasshopper(this, x, y);
				grid[y][x].push_back(temp);
			}
//			case Field::FieldItem::anthill0:
//					cout << "The anthill for the first Ant should be at " << y << x << ",7 in the field\n";
//					break;
							//	Actor *temp = new Anthill(x, y, "Anthill0", );
				//	grid[y][x].push_back(temp);
				//	break;
		
			
		}
	}
	//*/
	return 0;
}
int StudentWorld::getNumberOfAntsForAnt(int i)
{
	if (i < counts.size())
		return counts[i];
	return -1;
}
void StudentWorld::updateTickCount()
{
	ticks++;
}
int StudentWorld::getTicks()
{
	return ticks;
}
int StudentWorld::move()//more on pg 22
{
	updateTickCount(); // update the current tick # in the simulation
					   // The term "actors" refers to all ants, anthills, poison, pebbles,
					   // baby and adult grasshoppers, food, pools of water, etc.
					   // Give each actor a chance to do something
	for (int i = 0; i < 64; i++)
	{
		for (int j = 0; j < 64; j++)
		{
			list<Actor*>::iterator q = grid[i][j].begin();
			while (q != grid[i][j].end())
			{
				Actor* temp = *q;
				int oldX = temp->getX(), oldY = temp->getY();
				if (temp->alive())
				{
					// ask each actor to do something (e.g. move)
					temp->doSomething();
				}
				if (temp->getX() != oldX || temp->getY() != oldY)
				{
					grid[(*q)->getY()][(*q)->getX()].push_back(*q);//check if need to decrement by 1
					q = grid[i][j].erase(q);//not sure how to delete properly
				}
				else
					q++;
			}

		}
	}
	removeDeadSimulationObjects(); // delete dead simulation objectsS
	updateDisplayText(); // update the ticks/ant stats text at screen top
						 // If the simulation’s over (ticks == 2000) then see if we have a winner
	if (getTicks() == 2000)
	{
	//	if (weHaveAWinningAnt())
	//	{
			//setWinner(getWinningAntsName());
	//		return GWSTATUS_Ant_WON;
	//	}
	//	else
			return GWSTATUS_NO_WINNER;
	}
	// the simulation is not yet over, continue!
	return GWSTATUS_CONTINUE_GAME;

}
//string StudentWorld::getWinningAntsName()
//{
//}

void StudentWorld::removeDeadSimulationObjects() 
{
	for (int i = 0; i < 64; i++)
	{
		for (int j = 0; j < 64; j++)
		{
			list<Actor*>::iterator q = grid[i][j].begin();
			while (q != grid[i][j].end())
			{
				Actor* temp = *q;
				if (!temp->alive())
				{
					delete *q;
					q = grid[i][j].erase(q);
				}
				else
					q++;
			}

		}
	}

	//iterate through all the actors;
}
void StudentWorld::updateDisplayText()
{
	int ticks = getTicks();
	int antsAnt0, antsAnt1, antsAnt2, antsAnt3;
	int winningAntNumber = 0;////////////////////////////////////
	antsAnt0 = getNumberOfAntsForAnt(0);
	antsAnt1 = getNumberOfAntsForAnt(1);
	antsAnt2 = getNumberOfAntsForAnt(2);
	antsAnt3 = getNumberOfAntsForAnt(3);
//	winningAntNumber = getWinningAntNumber();//need to define
		// Create a string from your statistics, of the form:
		// Ticks: 1134 - AmyAnt: 32 BillyAnt: 33 SuzieAnt*: 77 IgorAnt: 05
	string s = formatDisplay(ticks,
		antsAnt0,
		antsAnt1,
		antsAnt2,
		antsAnt3,
		winningAntNumber
	);
		// Finally, update the display text at the top of the screen with your
		// newly created stats
	setGameStatText(s); // calls our provided GameWorld::setGameStatText
}

string StudentWorld::formatDisplay(int ticks, int antsAnt0, int antsAnt1, int antsAnt2, int antsAnt3, int winningAntNumber)
{
	string base = "Ticks: ";
	return base;
}
// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
void StudentWorld::addActor(Actor* act, int x, int y)
{
	grid[y][x].push_back(act);
}