// The main.cpp you can use for testing will replace this file soon.
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <iostream>
#include "provided.h"
#include "MyMap.h"
#include <cassert>

using namespace std;
void printDirections(string start, string end, const vector<NavSegment>& directions);
int main()
{/*
	MapLoader map;
	map.load("mapdata.txt");
	AttractionMapper am;
	GeoCoord gc;
	am.init(map);
	am.getGeoCoord("triangle fraternity", gc);
	cout << gc.latitudeText << endl << gc.longitudeText;
	SegmentMapper segs;
	segs.init();
	vector<StreetSegment> aa;
	aa = segs.getSegments(gc);
	*/
	//MyMap<string, int> mapper;
	//mapper.associate("Caleb", 1);
	//mapper.associate("Veronica", 2);
	//mapper.associate("Nathan", 3);
	//mapper.associate("Chiu", 4);
	//mapper.associate("Pusheen", 5);
	//mapper.associate("Feibi", 6);

	//cout << "Caleb = " << *mapper.find("Caleb") << endl;
	//cout << "Chiu = " << *mapper.find("Chiu") << endl;
	//cout << "Feibi = " << *mapper.find("Feibi") << endl;
	//cout << "Nathan = " << *mapper.find("Nathan") << endl;
	//cout << "Pusheen = " << *mapper.find("Pusheen") << endl;
	//cout << "Veronica = " << *mapper.find("Veronica") << endl;

	//mapper.clear();

	//assert(mapper.size() == 0);
	//assert(mapper.find("Caleb") == nullptr);
	//assert(mapper.find("Chiu") == nullptr);
	//assert(mapper.find("Feibi") == nullptr);
	//assert(mapper.find("Nathan") == nullptr);
	//assert(mapper.find("Pusheen") == nullptr);
	//assert(mapper.find("Veronica") == nullptr);

	//MapLoader ml;
	//ml.load("mapdata.txt");
	//cout << "hi";
	//StreetSegment seg1;
	//ml.getSegment(0, seg1);
	//assert(seg1.streetName == "10th Helena Drive");
	//assert(seg1.segment.start.latitudeText == "34.0547000");
	//assert(seg1.segment.start.longitudeText == "-118.4794734");
	//assert(seg1.segment.end.latitudeText == "34.0544590");
	//assert(seg1.segment.end.longitudeText == "-118.4801137");
	//assert(seg1.attractions.size() == 0);

	//ml.getSegment(ml.getNumSegments() - 1, seg1);
	//assert(seg1.streetName == "access road for water tank");
	//assert(seg1.segment.start.latitudeText == "34.0853742");
	//assert(seg1.segment.start.longitudeText == "-118.4955413");
	//assert(seg1.segment.end.latitudeText == "34.0852898");
	//assert(seg1.segment.end.longitudeText == "-118.4954341");
	//assert(seg1.attractions.size() == 0);
	//cout << "here";
	//AttractionMapper am;
	//am.init(ml);
	//GeoCoord geo;
	//assert(am.getGeoCoord("Broxton Plaza", geo) && geo.latitudeText == "34.0635414" && geo.longitudeText == "-118.4474561");
	//assert(am.getGeoCoord("BROXTON PLAZA", geo) && geo.latitudeText == "34.0635414" && geo.longitudeText == "-118.4474561");
	//assert(am.getGeoCoord("Geffen Playhouse", geo) && geo.latitudeText == "34.0634558" && geo.longitudeText == "-118.4445262");
	//assert(am.getGeoCoord("Starbucks Coffee", geo) && geo.latitudeText == "34.0594135" && geo.longitudeText == "-118.4447466");
	//assert(am.getGeoCoord("USC", geo) == false);
	//cout << "attraction";
	//SegmentMapper sm;
	//sm.init(ml);
	//vector<StreetSegment> streets;
	//streets = sm.getSegments(geo); // Starbucks Coffee from previous test
	//assert(streets.size() == 1);
	//assert(streets[0].streetName == "Westwood Boulevard");
	//assert(streets[0].segment.start.latitudeText == "34.0596259" && streets[0].segment.start.longitudeText == "-118.4446352");
	//assert(streets[0].segment.end.latitudeText == "34.0595394" && streets[0].segment.end.longitudeText == "-118.4445598");
	//cout << "amapper";
	//geo = GeoCoord("34.0564200", "-118.4799368");
	//streets = sm.getSegments(geo); // 16th Helena Drive end of 1st segment
	//assert(streets.size() == 2);
	//assert(streets[0].streetName == "16th Helena Drive");
	//assert(streets[0].segment.start.latitudeText == "34.0564067" && streets[0].segment.start.longitudeText == "-118.4798349");
	//assert(streets[0].segment.end.latitudeText == "34.0564200" && streets[0].segment.end.longitudeText == "-118.4799368");
	//assert(streets[1].streetName == "16th Helena Drive");
	//assert(streets[1].segment.start.latitudeText == "34.0564200" && streets[1].segment.start.longitudeText == "-118.4799368");
	//assert(streets[1].segment.end.latitudeText == "34.0564109" && streets[1].segment.end.longitudeText == "-118.4805269");
	//cout << "geo";
	//geo = GeoCoord("10", "20"); // unknown coordinate
	//streets = sm.getSegments(geo);
	//assert(streets.size() == 0);

	//cout << "Passed all tests" << endl;

	//Navigator nav;
	//nav.loadMapData("mapdata.txt");
	//vector<NavSegment> dir;
	//nav.navigate("diddy riese", "triangle fraternity", dir);
	//cout << "whatup";//*/
	Navigator navigator;
	if (!navigator.loadMapData("mapdata.txt")) {
		cerr << "Error initializing map data" << endl;
		return 1;
	}

	vector<NavSegment> directions;
	navigator.navigate("Triangle Fraternity", "riviera country club", directions);
	printDirections("Triangle Fraternity", "riviera country club", directions);
	navigator.navigate("Triangle Fraternity", "Diddy Riese", directions);
	printDirections("Triangle Fraternity", "Diddy Riese", directions);
	_CrtDumpMemoryLeaks();
}

void printDirections(string start, string end, const vector<NavSegment>& directions) {
	double distance = 0;
	cerr << "You are starting at: " << start << endl;
	for (auto dir : directions) {
		if (dir.m_command == NavSegment::NavCommand::PROCEED) {
			cerr << "Proceed " << dir.m_distance << " miles " << dir.m_direction << " on " << dir.m_streetName << endl;
			distance += dir.m_distance;
		}
		else {
			cerr << "Turn " << dir.m_direction << " onto " << dir.m_streetName << endl;
		}
	}
	cerr << "You have reached your destination: " << end << endl;
	cerr << "Total travel distance: " << distance << " miles" << endl;
}