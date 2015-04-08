/*************
engine.h
Jeff Csicsek
March 2012
Entry point for using Map class to scrape Google Maps images for building data
**************/

#include "map.h"


int main(int argc, char* argv[])
{
	Map map;
	string fileName = argv[1];
	
	//load map image from file
	map.loadImage(fileName);
	
	//scrape building footprints
	map.scrapeFootprints();
	
	//determine building heights
	map.scrapeHeights();
	
	//draw building footprints and area/height data to map image
	map.drawOutput();
	
	//print XML description of buildings to stdout
	printf("%s\n", map.buildXML().c_str());
	
	return 0;
}

