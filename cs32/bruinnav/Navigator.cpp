#include "provided.h"
#include <string>
#include <list>
#include <vector>
#include <iostream>
#include <queue>
using namespace std;

class NavigatorImpl
{
public:
    NavigatorImpl();
    ~NavigatorImpl();
    bool loadMapData(string mapFile);
    NavResult navigate(string start, string end, vector<NavSegment>& directions) const;
private:
	MapLoader mapped;
	AttractionMapper atts;
	SegmentMapper segs;
	struct Node
	{
		Node*parent;

		double h;
		double g;
		GeoCoord geocoordinates;
	};
	bool compareTo(Node* p, Node* pp);
	int indexOf(list<Node*> ln, Node *n) const;
	SegmentMapper mapper;
	AttractionMapper am;
	bool GeoCoordsEqual(GeoCoord a, GeoCoord b) const;
};

NavigatorImpl::NavigatorImpl()
{	
}

NavigatorImpl::~NavigatorImpl()
{
}

bool NavigatorImpl::loadMapData(string mapFile)
{
	//load the map files, attraction mapper, and segment mapper
	bool thing = mapped.load(mapFile);
	cerr << "loaded";
	mapper.init(mapped);
	am.init(mapped);
	segs.init(mapped);
	cerr << "naving";
	return thing;
}

NavResult NavigatorImpl::navigate(string start, string end, vector<NavSegment> &directions) const
{
	vector<Node*> toDel;
	list<Node*> SolutionPathList;
	//Create a node containing the goal state node_goal
	Node* node_goal = new Node;
	Node* node_start = new Node;
	GeoCoord gc;
	//get the starting and ending coordinates and initialize their node values
	if (!am.getGeoCoord(end, gc))
		return NAV_BAD_DESTINATION;
	node_goal->geocoordinates = gc;
	if (!am.getGeoCoord(start, gc))
		return NAV_BAD_SOURCE;
	node_start->g = 0;
	node_start->h = distanceEarthMiles(gc, node_goal->geocoordinates);
	node_start->geocoordinates = gc;

	//Create OPEN list of potential next nodes
	list<Node*> OPEN;
	list<Node*> CLOSED;
	OPEN.push_back(node_start);
	bool found = false;
	//while the OPEN list is not empty
	while (OPEN.size() > 0)
	{
		Node* min = *OPEN.begin();
		list<Node*>::iterator it;
		int mini = 0;
		int i = 0;
		//find node of minimum f values
		for (it = OPEN.begin(); it != OPEN.end(); it++)
		{
			if ((*it)->g + (*it)->h < min->g + min->h && ((*it)->g != 0 && (*it)->h != 0))
			{
				mini = i;
				min = *it;
			}
			i++;
		}
		it = OPEN.begin();
		advance(it, mini);
		//get the segments related to node min
		vector <StreetSegment> strts = segs.getSegments(min->geocoordinates);//
		Node* node_current = min;
		//current node being explored has min f value, delete off list of nodes to explore

		OPEN.erase(it);
		//delete node we are on from the list of ndoes to look from

		if (GeoCoordsEqual(node_current->geocoordinates, node_goal->geocoordinates))
		{//if your reached destination(probably never going to be this case because attractions are in hte middle of a segment)
			node_goal = node_current;
			found = true;
			break;
		}
		vector<StreetSegment> tempsegs = segs.getSegments(node_goal->geocoordinates);//attractions gc
		if (GeoCoordsEqual(node_current->geocoordinates, tempsegs[0].segment.end) || GeoCoordsEqual(node_current->geocoordinates, tempsegs[0].segment.start)) //////OR IF THE SEGMENT IT IS ON CONTAINS NODE GOAL
		{//if you reached the segment containing the attraction
			node_goal->parent = node_current;
			found = true;
			break;
		}

		//Generate each state node_successor that can come after node_current
		GeoCoord cur = node_current->geocoordinates;
		vector<StreetSegment>ss = segs.getSegments(cur);//segments associated with geocoord
		vector<Node*> successors;
		for (int i = 0; i < ss.size(); i++)
		{//create a node for each possible successor and keep track of them in a vector of successors
			Node* temp = new Node;
			if (GeoCoordsEqual(cur, ss[i].segment.start))//if the current node matches the segments start, the segments end is the next node
				temp->geocoordinates = ss[i].segment.end;
			else if (GeoCoordsEqual(cur, ss[i].segment.end))
				temp->geocoordinates = ss[i].segment.start;
			else//if node is in the middle of the segment, push both coords of the segment to explore
			{
				temp->geocoordinates = ss[i].segment.start;
				Node* other = new Node;
				other->geocoordinates = ss[i].segment.end;
				other->h = distanceEarthMiles(temp->geocoordinates, node_goal->geocoordinates);
				other->parent = node_current;
				other->g = node_current->g + distanceEarthMiles(temp->geocoordinates, node_current->geocoordinates);
				successors.push_back(other);
			}
			//initialize costs and parent node
			temp->h = distanceEarthMiles(temp->geocoordinates, node_goal->geocoordinates);
			temp->parent = node_current;
			temp->g = node_current->g + distanceEarthMiles(temp->geocoordinates, node_current->geocoordinates);
			successors.push_back(temp);
		}
		Node* node_successor;
		//for each node_successor or node_current
		for (int i = 0; i < successors.size(); i++)
		{
			//cerr << successors[i]->geocoordinates.latitudeText;
			node_successor = successors[i];
			//find node_successor on the OPEN list
			int oFound = indexOf(OPEN, node_successor);
			//if node_successor is on the OPEN list but the existing one is as good
			//or better then discard this successor and continue
			if (oFound > 0)
			{
				list<Node*>::iterator it = OPEN.begin();
				advance(it, oFound);
				Node* existing_node = *it;
				if (existing_node->g + existing_node->h <= node_current->h + node_current->g)
				{
					delete successors[i];
					continue;
				}
			}
			int cFound = indexOf(CLOSED, node_successor);
			if (cFound > 0)
			{
				list<Node*>::iterator it = CLOSED.begin();
				advance(it, cFound);
				Node* existing_node = *it;
				if (existing_node->g + existing_node->h <= node_current->h + node_current->g)
				{
					delete successors[i];
					continue;
				}
			}

			if (oFound != -1)
			{
				list<Node*>::iterator it = OPEN.begin();
				advance(it, oFound);
				toDel.push_back(*it);
				OPEN.erase(it);
			}
			if (cFound != -1)
			{
				list<Node*>::iterator it = CLOSED.begin();
				advance(it, cFound);
				toDel.push_back(*it);
				CLOSED.erase(it);
			}
			OPEN.push_back(node_successor);

		}
		CLOSED.push_back(node_current);
	}
	if (!found)
		return NAV_NO_ROUTE;

	//follow nodes and put into solution list
	Node* tempn = node_goal;
	while (tempn != nullptr)
	{
		SolutionPathList.push_front(tempn);
		if (tempn == node_start)
			break;
		tempn = tempn->parent;
	}
	//decode nodes into directoins
	vector<NavSegment> dirs;
	list<Node*>::iterator it = SolutionPathList.begin();
	Node* prev = node_start;
	StreetSegment lastseg;


	//lastseg.streetName = 
	bool first = true;
	for (advance(it, 1); it != SolutionPathList.end(); it++)
	{
		NavSegment temp;
		vector<StreetSegment> sts = segs.getSegments((*it)->geocoordinates);
		for (int i = 0; i < sts.size(); i++)
		{
			//if the segment coords match with the current node
			if (first)
			{
				temp.m_streetName = segs.getSegments(prev->geocoordinates)[0].streetName;

			}
			if (GeoCoordsEqual(sts[i].segment.end, prev->geocoordinates) || GeoCoordsEqual(sts[i].segment.start, prev->geocoordinates) || first )
			{
				//initialize current navsegment values
				GeoSegment cur(prev->geocoordinates, (*it)->geocoordinates);
				temp.m_geoSegment = sts[i].segment;
				if(!first)
					temp.m_streetName = sts[i].streetName;
				//if street names arent the same, indicates a turn
				if (sts[i].streetName != lastseg.streetName  &&  !first)
				{
					temp.m_command = NavSegment::TURN;
					double ang = angleBetween2Lines(lastseg.segment, cur);
					if (ang < 180)
						temp.m_direction = "left";
					else
						temp.m_direction = "right";
					dirs.push_back(temp);
					//keep track of the last segment you were on
					lastseg = sts[i];
					lastseg.segment = cur;
				}
				first = false;
				temp.m_command = NavSegment::PROCEED;
				string dir = "east";
				temp.m_distance = distanceEarthMiles(prev->geocoordinates, (*it)->geocoordinates);
				double angle = angleOfLine(cur);
				if (angle > 22.5 && angle <= 67.5)
					dir = "northeast";
				else if (angle > 67.5 && angle <= 112.5)
					dir = "north";
				else if (angle > 112.5 && angle <= 157.5)
					dir = "northwest";
				else if (angle > 157.5 && angle <= 202.5)
					dir = "west";
				else if (angle > 202.5 && angle <= 247.5)
					dir = "souhtwest";
				else if (angle > 247.5 && angle <= 292.5)
					dir = "south";
				else if (angle > 292.5 && angle <= 337.5)
					dir = "southeast";
				temp.m_direction = dir;
				//keep track of the last segment
				lastseg = sts[i];
				lastseg.segment = cur;
				dirs.push_back(temp);
				break;
			}
		}
		//keep track of previous node
		prev = *it;
	}
	//clean up
	directions = dirs;
	for (int i = 0; i < toDel.size(); i++)
		delete toDel[i];
	while (!OPEN.empty()) delete OPEN.front(), OPEN.pop_front();
	while (!CLOSED.empty()) delete CLOSED.front(), CLOSED.pop_front();
	while (SolutionPathList.empty()) delete SolutionPathList.front(), SolutionPathList.pop_front();
	cerr << OPEN.size() << CLOSED.size() << endl<< SolutionPathList.size();
	delete node_goal;

	
	return NAV_SUCCESS;
}

//******************** Navigator functions ************************************

// These functions simply delegate to NavigatorImpl's functions.
// You probably don't want to change any of this code.

Navigator::Navigator()
{
    m_impl = new NavigatorImpl;
}

Navigator::~Navigator()
{
    delete m_impl;
}

bool Navigator::loadMapData(string mapFile)
{
    return m_impl->loadMapData(mapFile);
}

NavResult Navigator::navigate(string start, string end, vector<NavSegment>& directions) const
{
    return m_impl->navigate(start, end, directions);
}




bool NavigatorImpl::compareTo(Node* p, Node* pp)
{
	return p->g + p->h >= pp->g + pp->h;
}
int NavigatorImpl::indexOf(list<Node*> ln, Node *n) const
{
	int i = 0;
	for (list<Node*>::iterator it = ln.begin(); it != ln.end(); it++)
	{
		if ((*it)->geocoordinates.longitudeText == n->geocoordinates.longitudeText && (*it)->geocoordinates.latitudeText == n->geocoordinates.latitudeText)
			return i;
		i++;
	}
	return -1;
}

bool NavigatorImpl::GeoCoordsEqual(GeoCoord a, GeoCoord b) const
{
	if (a.longitudeText != b.longitudeText || a.latitudeText != b.latitudeText)
		return false;
	return true;
}