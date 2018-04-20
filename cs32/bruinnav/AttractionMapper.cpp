#include "provided.h"
#include <string>
#include "MyMap.h"
#include <iostream>
#include <locale>
using namespace std;

class AttractionMapperImpl
{
public:
	AttractionMapperImpl();
	~AttractionMapperImpl();
	void init(const MapLoader& ml);
	bool getGeoCoord(string attraction, GeoCoord& gc) const;
private:
	MyMap<string, GeoCoord> map;
};

AttractionMapperImpl::AttractionMapperImpl()
{
}

AttractionMapperImpl::~AttractionMapperImpl()
{
}

void AttractionMapperImpl::init(const MapLoader& ml)
{//bst
	
	for (int i = 0; i < ml.getNumSegments(); i++)
	{
		StreetSegment seg;
		if (ml.getSegment(i, seg))
		{
			for (int j = 0; j < seg.attractions.size(); j++)
			{
				string lower = "";
				for (int x = 0; x < seg.attractions[j].name.length(); x++)
				{
					lower += tolower(seg.attractions[j].name[x]);
				}
				map.associate(lower, seg.attractions[j].geocoordinates);
			}
		}
	}
	
}

bool AttractionMapperImpl::getGeoCoord(string attraction, GeoCoord& gc) const
{
	string lower; 
	for (int i = 0; i < attraction.length(); i++)
		lower += tolower(attraction[i]);
	const GeoCoord* temp = map.find(lower);
	if (temp != nullptr)
	{
		gc = *temp;
		return true;
	}

	return false;  // This compiles, but may not be correct
}

//******************** AttractionMapper functions *****************************

// These functions simply delegate to AttractionMapperImpl's functions.
// You probably don't want to change any of this code.

AttractionMapper::AttractionMapper()
{
	m_impl = new AttractionMapperImpl;
}

AttractionMapper::~AttractionMapper()
{
	delete m_impl;
}

void AttractionMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

bool AttractionMapper::getGeoCoord(string attraction, GeoCoord& gc) const
{
	return m_impl->getGeoCoord(attraction, gc);
}
