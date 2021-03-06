/************************************************************************
Program:		FindPath
Author:			Joe Prosapio
Date Edited:	5/26/2018
Use:			This program uses the A Star method of finding the path on a grid from a defined starting location to an ending location.
				Blocker squares are ignored in the path.
*************************************************************************/

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

/************************************************************************
Class:				gridSquare
Members:
	fScore:			Estimated Hueristic Score to get to end from the current location + Real Score to get to the square location from the start
	gScore:			Real Score to get to the square location from the start
	hScore:			Hueristic Score to get to the end from the current location
	xCoordinate:	X Coordinate of the square on the grid
	yCoordinate:	Y Coordinate of the square on the grid
	cameFromX:		X Coordinate of the square that is cheapest to get from the starting square to this square
	cameFromY:		Y Coordinate of the square that is cheapest to get from the starting square to this square
Operators:
	==:				Two gridSquares are considered the same if they have the same x and y coordinates
*************************************************************************/
class gridSquare {
public:
	gridSquare(int xCoordinate, int yCoordinate, int gscore, int hscore) {
		this->hScore = hscore;
		this->gScore = gscore;
		this->fScore = hscore + gscore;
		this->xCoordinate = xCoordinate;
		this->yCoordinate = yCoordinate;
	}
	int fScore;
	int hScore;
	int gScore;
	int xCoordinate;
	int yCoordinate;
	int cameFromX;
	int cameFromY;

	bool operator==(const gridSquare & rhs) const { 
		if (rhs.xCoordinate == this->xCoordinate && rhs.yCoordinate == this->yCoordinate)
			return true;
		else
			return false;
	}
};

/************************************************************************
Class:						grid
Members:
	height:					Height of the grid
	width:					Width of the grid
	pMap:					Map of the grid, 1 means the square is traversible, anything else is not
	targetX:				X coordinate of the ending square
	targetY:				Y coordinate of the ending square
	startX:					X coordinate of the starting square
	startY:					Y coordinate of the starting square
Methods:
	getLocationFromGrid:	Gets the numeric location of a square from its x and y coordinates
	isTraversible:			Determines if a given square is blocked
	getManhattan:			Gets the hueristic Manhattan score from a given location to the ending location
	getNeighbors:			Returns a vector of grid locations adjacent to the location given.  This will take into account map edges and blocked squares.
	retracePath:			Returns a vector of the path from the starting location to the end, starting location is not included.
*************************************************************************/
class grid {
public:
	int height;
	int width;
	const unsigned char* pMap;
	int targetX;
	int targetY;
	int startX;
	int startY;

	grid(const int height, const int width, const unsigned char* pMap, const int targetX, const int targetY, const int startX, const int startY) {
		this->height = height;
		this->width = width;
		this->pMap = pMap;
		this->targetX = targetX;
		this->targetY = targetY;
		this->startX = startX;
		this->startY = startY;
	}

	vector<gridSquare> retracePath(gridSquare currentLocation, vector<gridSquare> currentPath, vector<gridSquare> closedList) {
		if (currentLocation.xCoordinate == this->startX && currentLocation.yCoordinate == this->startY) {
			currentPath.push_back(currentLocation);
			return currentPath;
		}			
		else {
			currentPath.push_back(currentLocation);
			vector<gridSquare>::iterator it;
			gridSquare tmpSquare = gridSquare(currentLocation.cameFromX, currentLocation.cameFromY, 0, 0);
			it = find(closedList.begin(), closedList.end(), tmpSquare);
			return this->retracePath(*it, currentPath, closedList);
		}
		
			
	}

	int getLocationFromGrid(int xCoordinate, int yCoordinate) {
		return ((yCoordinate * this->width) + xCoordinate);
	}

	bool isTraversible(int xCoordinate, int yCoordinate) {
		int location = getLocationFromGrid(xCoordinate, yCoordinate);
		if ((int)this->pMap[location] == 1)
			return true;
		else
			return false;
	}

	int getManhattan(const int nStartX, const int nStartY) {
		return (abs(nStartX - this->targetX) + abs(nStartY - this->targetY));
	}

	vector <gridSquare> getNeighbors(gridSquare currentLocation) {
		vector <gridSquare> retVector;

		if (((currentLocation.yCoordinate + 1) < this->height) && this->isTraversible(currentLocation.xCoordinate, currentLocation.yCoordinate + 1)) {
			gridSquare neighbor = gridSquare(currentLocation.xCoordinate, currentLocation.yCoordinate + 1, currentLocation.gScore + 1, this->getManhattan(currentLocation.xCoordinate, currentLocation.yCoordinate + 1));
			retVector.push_back(neighbor);
		}

		if (((currentLocation.yCoordinate - 1) >= 0) && this->isTraversible(currentLocation.xCoordinate, currentLocation.yCoordinate - 1)) {
			gridSquare neighbor = gridSquare(currentLocation.xCoordinate, currentLocation.yCoordinate - 1, currentLocation.gScore + 1, this->getManhattan(currentLocation.xCoordinate, currentLocation.yCoordinate - 1));
			retVector.push_back(neighbor);
		}

		if ((((currentLocation.xCoordinate + 1) % this->width) != 0) && ((currentLocation.xCoordinate + 1) < (this->height * this->width)) && (this->isTraversible(currentLocation.xCoordinate + 1, currentLocation.yCoordinate))) {
			gridSquare neighbor = gridSquare(currentLocation.xCoordinate + 1, currentLocation.yCoordinate, currentLocation.gScore + 1, this->getManhattan(currentLocation.xCoordinate + 1, currentLocation.yCoordinate));
			retVector.push_back(neighbor);
		}

		if ((((currentLocation.xCoordinate - 1) % this->width) != (this->width - 1)) && ((currentLocation.xCoordinate - 1) >= 0) && (this->isTraversible(currentLocation.xCoordinate - 1, currentLocation.yCoordinate))) {
			gridSquare neighbor = gridSquare(currentLocation.xCoordinate - 1, currentLocation.yCoordinate, currentLocation.gScore + 1, this->getManhattan(currentLocation.xCoordinate - 1, currentLocation.yCoordinate));
			retVector.push_back(neighbor);
		}
		return retVector;
	}
};


/************************************************************************
Operator:
	<			This is used to sort grid squares by their fScores
*************************************************************************/
inline bool operator<(const gridSquare &s1, const gridSquare &s2) {
	return s1.fScore < s2.fScore;
}

/************************************************************************
Function:						FindPath
Inputs:
	const int nStartX:			Starting X Coordinate	
	const int nStartY:			Starting Y Coordinate
	const int nTargetX:			Target X Coordinate
	const int nTargetY:			Target Y Coordinate
	const unsigned char* pMap:	This is the map of the grid we are traversing.  A '1' means the square is traversible, anything else means it is not
	const int nMapWidth:		Width of the map
	const int nMapHeight:		Height of the map
	int* pOutBuffer:			This is the list of locations visited in the shortest path found.  Excludes starting location but includes ending location.
	const int nOutBufferSize:	The maximum size allowed for the output buffer.  If the size of the path is bigger than this, return a failure.
*************************************************************************/
int FindPath(const int nStartX, const int nStartY,
	const int nTargetX, const int nTargetY,
	const unsigned char* pMap, const int nMapWidth, const int nMapHeight,
	int* pOutBuffer, const int nOutBufferSize) {

	//Initialize lists, starting location and the map
	grid myMap = grid(nMapHeight, nMapWidth, pMap, nTargetX, nTargetY, nStartX, nStartY);
	gridSquare startingLocation = gridSquare(nStartX, nStartY, 0, myMap.getManhattan(nStartX, nStartY));
	vector <gridSquare> openList;
	vector <gridSquare> closedList;

	//The initial open location is the starting location
	openList.push_back(startingLocation);

	while (!openList.empty()) {
		//Sort the open list so we get the lowest fScore, make that the current location
		sort(openList.begin(), openList.end());
		gridSquare currentLocation = openList[0];

		//Mark the current location as closed (visited) and remove it from the open list (known, but not yet visited)
		closedList.push_back(currentLocation);
		openList.erase(openList.begin());

		//Check if we hit the goal (hooray!)
		if (currentLocation.xCoordinate == nTargetX && currentLocation.yCoordinate == nTargetY) {
			//Retrace the path to get back to the start
			vector<gridSquare> path;
			path = myMap.retracePath(currentLocation, path, closedList);
			//Remove the starting square from the path, reverse it so that we begin at the start
			path.pop_back();
			reverse(path.begin(), path.end());
			//Only assign and return the path if the size is less than what we are expecting, otherwise fail
			if (path.size() <= (size_t)nOutBufferSize) {
				for (size_t i = 0; i < path.size(); i++)
					pOutBuffer[i] = myMap.getLocationFromGrid(path[i].xCoordinate, path[i].yCoordinate);
				return path.size();
			}
			else
				return -1;

		}

		//Get the list of neighbors from the current location, add them to lists as needed
		vector <gridSquare> adjacentSqaures = myMap.getNeighbors(currentLocation);
		for (size_t i = 0; i < adjacentSqaures.size(); i++) {
			//Set the location for where we are coming from to the adjacent square
			//adjacentSqaures[i].cameFrom = myMap.getLocationFromGrid(currentLocation.xCoordinate, currentLocation.yCoordinate);
			adjacentSqaures[i].cameFromX = currentLocation.xCoordinate;
			adjacentSqaures[i].cameFromY = currentLocation.yCoordinate;

			//Check if the neighbor is in the closed list.  If it is, we don't need to worry about it again
			vector<gridSquare>::iterator it;
			it = find(closedList.begin(), closedList.end(), adjacentSqaures[i]);
			if (it != closedList.end())
				continue;

			//Check if the neighbor is in the open list.  If it isn't, add it.  
			//If it is in the open list and the new G score for the neighbor is better than the old G score, update the F and G scores as well as where we're coming from for the neighbor
			it = find(openList.begin(), openList.end(), adjacentSqaures[i]);
			if (it == openList.end()) 
				openList.push_back(adjacentSqaures[i]);
			else {
				if ((currentLocation.gScore + 1) < adjacentSqaures[i].gScore) {
					adjacentSqaures[i].gScore = currentLocation.gScore + 1;
					adjacentSqaures[i].fScore = currentLocation.fScore + 1;
					replace(openList.begin(), openList.end(), *it, adjacentSqaures[i]);
				}

			}

		}

	}
	//If we exhausted the open set, there is no path available
	return -1;
}

int main()
{
	//unsigned char pMap[] = { 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1 };
	//int pOutBuffer[12];
	//int k = FindPath(0, 0, 1, 2, pMap, 4, 3, pOutBuffer, 12);
	//cout << "First:\t" << k << endl;

	unsigned char pMap[] = { 0, 0, 1, 0, 1, 1, 1, 0, 1 };
	int pOutBuffer[7];
	int k = FindPath(2, 0, 0, 2, pMap, 3, 3, pOutBuffer, 7);
	cout << "Second:\t" << k << endl;

    return k;
}

