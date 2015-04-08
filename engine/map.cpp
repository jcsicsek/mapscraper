/*************
map.cpp
Jeff Csicsek
March 2012
Processing tools for scraping Google Maps images for building data
**************/

#include "map.h"

//empty constructor, destructor
Map::Map()
{

}

Map::~Map()
{

}


//loads image data from file
void Map::loadImage(string file)
{
	//isolate filename from full path
	size_t pos = file.find_last_of("/");
	if(pos != std::string::npos)
	    fileName = file.substr(pos + 1);
	else
		fileName = file;
	//load image
	rgbImage = imread(file, 1);
	//convert image to grayscale
	cvtColor(rgbImage, grayImage, CV_RGB2GRAY);
}


//scrapes building footprints from image
void Map::scrapeFootprints()
{
	Mat footprints;
	int pixel_val;
	double area;
	vector<vector<Point> > footContours;
	vector<Point> poly;
	vector<Point> contour;
	double ctrX, ctrY;
	Building bldg;
		
	//isolate building roof pixels by color
	threshold(grayImage, footprints, 234, 255, THRESH_TOZERO_INV);
	threshold(footprints, footprints, 233.5, 255, THRESH_BINARY);
		
	//find building footprint contours
	findContours(footprints, footContours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	
	for (int i = 0; i < footContours.size(); i++)
	{
		//fit polynomial to contour and get area, discard noise too small to be a building
		contour = footContours[i];		
		approxPolyDP(contour, poly, 3, true);
		area = contourArea(poly);
		if (poly.size() >= 3 && area > 400)
		{
			//calculate center of mass of building
			ctrX = 0;
			ctrY = 0;
			for (int j = 0; j < poly.size(); j++)
			{
				ctrX += poly[j].x;
				ctrY += poly[j].y;
			}
			ctrX /= (double)poly.size();
			ctrY /= (double)poly.size();
			bldg.center = Point(ctrX, ctrY);
			bldg.area = area;
			bldg.height = 0.0;
			bldg.contour = contour;
			bldg.address = "address";
			buildings.push_back(bldg);
		}
	}
}

//detects building heights
void Map::scrapeHeights()
{
	Mat sidewalls;
	vector<vector<Point> > sideContours;
	vector<int> sideFootprintIndexes;
	int bldgIndex;
	vector<Point> poly;
	vector<Point> contour;
	double area;
	double ctrX, ctrY;
	double height;
	int verticalCount;
	vector<double> distances;
	double adjCtrX;
	double distance;
	
	
	//isolate building sidewall pixels by color
	threshold(grayImage, sidewalls, 216, 255, THRESH_TOZERO_INV);
	threshold(sidewalls, sidewalls, 212, 255, THRESH_BINARY);
	
	//find building sidewall contours
	findContours(sidewalls, sideContours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	for (int i = 0; i < sideContours.size(); i++)
	{	
		contour = sideContours[i];
		//fit polynomial to contour and get area, discard noise too small to be a building sidewall
		approxPolyDP(contour, poly, 3, true);
		area = contourArea(poly);
		if (poly.size() >= 3 && area > 400)
		{
			//calculate center of mass of sidewall
			ctrX = 0;
			ctrY = 0;
			height = 0;
			verticalCount = 0;
			//go through lines which form polynomail
			for (int j = 1; j < poly.size(); j++)
			{
				ctrX += poly[j].x;
				ctrY += poly[j].y;
				//if line is near-vertical, use it to determine building height
				if (abs(poly[j].x - poly[j - 1].x) <= 1)
				{
					height = (height * (double)verticalCount + abs(poly[j].y - poly[j - 1].y)) / ((double)verticalCount + 1);
					verticalCount++;
				}
			}
			ctrX += poly[0].x;
			ctrY += poly[0].y;	
			if (abs(poly[poly.size() - 1].x - poly[0].x) <= 1)
			{
				height = (height * (double)verticalCount + abs(poly[poly.size() - 1].y - poly[0].y))/ ((double)verticalCount + 1);
				verticalCount++;
			}
			
			ctrX /= (double)poly.size();
			ctrY /= (double)poly.size();
			//iterate through all buildings.
			//sidewall belongs to the building such that building_area / distance_from_building ^ 2 is maximized
			//when a sidewall is associated with a building, use it to set that building's height
			for (int l = 0; l < buildings.size(); l++)
			{
				adjCtrX = ctrX - sqrt(buildings[l].area);
				distance = sqrt((adjCtrX - buildings[l].center.x)
					* (adjCtrX - buildings[l].center.x) + (ctrY - buildings[l].center.y) * (ctrY - buildings[l].center.y));
				distances.push_back(buildings[l].area / (distance * distance));
			}
			bldgIndex = max_element(distances.begin(), distances.end()) - distances.begin();
			buildings[bldgIndex].height = height;
			distances.clear();
		}
	}
}

//draws building footprints and area/height data on map image
void Map::drawOutput()
{
	Mat outputImage;
	int drawnBuildings;
	int heightInFloors;
	int areaInSqFt;
	char dispAddress[100];
	char dispArea[100];
	char dispHeight[100];
	Building bldg;
	int r, g, b;
	
	drawnBuildings = 0;
	for (int i = 0; i < buildings.size(); i++)
	{
		bldg = buildings[i];
		
		//only draw a building if it is either non-zero height, or larger than an area threshold
		if (bldg.height > 0 || bldg.area > 3000)
		{
			drawnBuildings++;
			//draw each building footprint and data in a different, random color
			r = rand() % 255;
			g = rand() % 255;
			b = rand() % 255;
			
			//draw lines representing the building footprint on map image
			for (int j = 1; j < bldg.contour.size(); j++)
			{
				line(rgbImage, bldg.contour[j - 1], bldg.contour[j], Scalar(r, g, b), 2, 8);
			}
			line(rgbImage, bldg.contour[bldg.contour.size() - 1], bldg.contour[0], Scalar(r, g, b), 2, 8);
			
			//scaling factor for pixels-to-floors and pixels-to-square-feet, by experimentation
			heightInFloors = bldg.height / 2;
			areaInSqFt = (int)bldg.area / 4;
			sprintf(dispAddress, "bldg %d", drawnBuildings);
			buildings[i].address = dispAddress;
			sprintf(dispArea, "a = %d sq ft", areaInSqFt);
			sprintf(dispHeight, "h = %d fl", heightInFloors);
			
			//draw text of building data on map image
			putText(rgbImage, dispAddress, bldg.center, FONT_HERSHEY_SIMPLEX, .8, Scalar(r, g, b), 2);
			putText(rgbImage, dispArea, Point(bldg.center.x, bldg.center.y + 30), FONT_HERSHEY_SIMPLEX, .8, Scalar(r, g, b), 2);
			//the algorithm may be unable to calculate the height of some buildings
			if (bldg.height > 0)
				putText(rgbImage, dispHeight, Point(bldg.center.x, bldg.center.y + 60), FONT_HERSHEY_SIMPLEX, .8, Scalar(r, g, b), 2);
		}
	}
	
	//downsize image to match web client map size
	resize(rgbImage, outputImage, Size(), .5, .5);
	//save image
	imwrite("public/images/" + fileName, outputImage);
}

//generates XML description of a building
string Map::Building::getXML()
{
	std::stringstream xml;
	
	xml << "<building address=\"" << address <<  "\" area=\"" << (int)area / 4 << "\" height=\"" << (int)height / 2 << "\"></building>";
	
	return xml.str();
}

//generates XML of all buildings on map
string Map::buildXML()
{
	std::stringstream xml;
	xml << "<buildings>\n";
	for (int i = 0; i < buildings.size(); i++)
	{
		if (buildings[i].height > 0 || buildings[i].area > 3000)
		{
			xml << buildings[i].getXML() << "\n";
		}
	}
	xml << "</buildings>\n";
	
	return xml.str();
}
