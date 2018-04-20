#include "provided.h"
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

class MapLoaderImpl
{
public:
	MapLoaderImpl();
	~MapLoaderImpl();
	bool load(string mapFile);
	size_t getNumSegments() const;
	bool getSegment(size_t segNum, StreetSegment& seg) const;
private:
	bool isNum(string str);
	void StrToAttraction(string str, Attraction &a);
	void StrToGeoSegment(string coords, GeoSegment &geo);
	vector<StreetSegment> streets;
};
bool MapLoaderImpl::isNum(string str) {
	for (int i = 0; i < str.length(); i++)
	{
		if (!isdigit(str[i]))
			return false;
	}
	return true;
}

MapLoaderImpl::MapLoaderImpl()
{
}

MapLoaderImpl::~MapLoaderImpl()
{
}

bool MapLoaderImpl::load(string mapFile)
{
	ifstream infile(mapFile);
	if (!infile) {
		cerr << "Cannot open file" << endl;
		return false;
	}
	string line;
	while (getline(infile, line)) {
		//first line is streetname
		StreetSegment newSegment;
		newSegment.streetName = line;
		//second line is geocoordinates
		getline(infile, line);
		GeoSegment geo;
		StrToGeoSegment(line, geo);
		newSegment.segment = geo;
		//get number of attractions
		getline(infile, line);
		vector<Attraction> attvec;
		for (int i = 0; i < stoi(line); i++) {
			//build each attractoin, push onto vector
			string attraction;
			getline(infile, attraction);
			Attraction newAt;
			StrToAttraction(attraction, newAt);
			attvec.push_back(newAt);
		}
		newSegment.attractions = attvec;
		streets.push_back(newSegment);
	}

	return true;
}

size_t MapLoaderImpl::getNumSegments() const
{
	return streets.size(); // This compiles, but may not be correct
}

bool MapLoaderImpl::getSegment(size_t segNum, StreetSegment &seg) const
{//load into vector, access by index
	int sn = static_cast<int>(segNum);
	if (sn >= 0 && sn < streets.size())
	{
		seg = streets[sn];
		return true;
	}
	return false;  // This compiles, but may not be correct
}

//******************** MapLoader functions ************************************

// These functions simply delegate to MapLoaderImpl's functions.
// You probably don't want to change any of this code.

MapLoader::MapLoader()
{
	m_impl = new MapLoaderImpl;
}

MapLoader::~MapLoader()
{
	delete m_impl;
}

bool MapLoader::load(string mapFile)
{
	return m_impl->load(mapFile);
}

size_t MapLoader::getNumSegments() const
{
	return m_impl->getNumSegments();
}

bool MapLoader::getSegment(size_t segNum, StreetSegment& seg) const
{
   return m_impl->getSegment(segNum, seg);
}
void MapLoaderImpl::StrToGeoSegment(string coords, GeoSegment &geo) 
{
	string coordStr[4];
	int index = 0;
	bool next = false;

	//look for the separator for the two coordinates
	for (std::string::iterator it = coords.begin(); it != coords.end(); it++) 
	{
		if (*it != ',' && *it != ' ') 
		{//still a number
			next = false;
			coordStr[index] += (*it);
		}
		else if ((*it == ',' || *it == ' ') && !next) 
		{//next coordinate
			next = true;
			index++;
		}
		
	}

	//copy into geocoords
	geo.start = GeoCoord(coordStr[0], coordStr[1]);
	geo.end = GeoCoord (coordStr[2], coordStr[3]);
}

void MapLoaderImpl::StrToAttraction(string str, Attraction &a) {
	size_t index = str.find("|");//splits the name and coordinates
	string sname = str.substr(0, index);//name
	a.name = sname;
	string coords = str.substr(index + 1);
	int pos = coords.find(",");
	string lat = coords.substr(0, pos);
	//find where second coordinate begins
	for (; pos < coords.length(); pos++)
	{
		if (isdigit(coords[pos]) || coords[pos] == '-')
			break;
	}
	string longi = coords.substr(pos);
	
	a.geocoordinates = GeoCoord(lat, longi);
}
