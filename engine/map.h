/*************
map.h
Jeff Csicsek
March 2012
Processing tools for scraping Google Maps images for building data
**************/

#ifndef map_h
#define map_h

#include <cv.h>
#include <highgui.h>
#include <math.h>
#include <stdio.h>
#include <sstream>

using namespace cv;

//scrapes building footprint and height information from a Google Maps image
class Map
{
	private:
		//a building scraped on the map
		struct Building
		{
			//center of mass of building on image
			Point center;
			//area and height in pixels
			double area;
			double height;
			//polynomail description of building footprint
			vector<Point> contour;
			//street address
			string address;
			
			//generates XML description of building
			string getXML();
		};

		//stores the image to be processed
		Mat rgbImage;
		Mat grayImage;
		
		//building data from map image
		vector <Building> buildings;

		//image file name
		string fileName;

	public:
		Map();
		~Map();

		//loads image from file
		void loadImage(string file);
		
		//draws building footprints and area/height data on map image
		void drawOutput();
		
		//scrapes building footprints from map image
		void scrapeFootprints();
		
		//detect building heights
		void scrapeHeights();
		
		//generates XML list of all buildings on map
		string buildXML();
};


#endif

