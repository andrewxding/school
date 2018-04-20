#include "provided.h"
#include <vector>
#include "MyMap.h"
using namespace std;

class SegmentMapperImpl
{
public:
	SegmentMapperImpl();
	~SegmentMapperImpl();
	void init(const MapLoader& ml);
	vector<StreetSegment> getSegments(const GeoCoord& gc) const;
private:
	MyMap<string, vector<StreetSegment>> map;
	void pushBack(string gc, StreetSegment seg);
};

SegmentMapperImpl::SegmentMapperImpl()
{
}

SegmentMapperImpl::~SegmentMapperImpl()
{
}

void SegmentMapperImpl::init(const MapLoader& ml)
{
	//map each coord to an array of segments
	StreetSegment seg;
	for (int i = 0; i < ml.getNumSegments(); i++)
	{
		if (ml.getSegment(i, seg))
		{
			//add both of segments coords, map them to segment
			pushBack(seg.segment.start.latitudeText + seg.segment.start.longitudeText, seg);
			pushBack(seg.segment.end.latitudeText + seg.segment.end.longitudeText, seg);
			//add each of segment's attraction, map them to segment
			for (int j = 0; j < seg.attractions.size(); j++)
				pushBack(seg.attractions[j].geocoordinates.latitudeText + seg.attractions[j].geocoordinates.longitudeText, seg);
		}
	}
}
void SegmentMapperImpl::pushBack(string gc, StreetSegment seg) 
{//find if map already has the coord mapped to segments
	vector<StreetSegment>* mapped = map.find(gc);
	if (mapped != nullptr)
	{
		// add segment onto vector ogf segments and reassociate to coord (deletes old entry)
		mapped->push_back(seg);
		map.associate(gc, *mapped);
	}
	else
	{
		vector<StreetSegment> ss;
		ss.push_back(seg);
		map.associate(gc, ss);

	}

}
vector<StreetSegment> SegmentMapperImpl::getSegments(const GeoCoord& gc) const
{//use mymap's find
	const vector<StreetSegment>* streets = map.find(gc.latitudeText + gc.longitudeText);
	if (streets != nullptr)
		return *streets;  // This compiles, but may not be correct
	else
		return vector<StreetSegment>();
}

//******************** SegmentMapper functions ********************************

// These functions simply delegate to SegmentMapperImpl's functions.
// You probably don't want to change any of this code.

SegmentMapper::SegmentMapper()
{
	m_impl = new SegmentMapperImpl;
}

SegmentMapper::~SegmentMapper()
{
	delete m_impl;
}

void SegmentMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

vector<StreetSegment> SegmentMapper::getSegments(const GeoCoord& gc) const
{
	return m_impl->getSegments(gc);
}
