#include <iostream>
#include <algorithm>
#include <list>
#include <vector>
#include "Agent.h"
#include "Action.h"

using namespace std;

int MySearchEngine::HeuristicFunction(SearchState* state, SearchState* goalState) {
	int cityBlock = abs(state->location.X - goalState->location.X) + abs(state->location.Y - goalState->location.Y);
	return cityBlock;
	//return 0; // not a good heuristic
}

Agent::Agent ()
{
	// Initialize new agent based on new, unknown world
	worldState.agentLocation = Location(1,1);
	worldState.agentOrientation = RIGHT;
	worldState.agentHasGold = false;
	lastAction = CLIMB; // dummy action
	worldState.worldSize = 3; // HW5: somewhere between 3x3 and 9x9
	worldSizeKnown = false;
	wumpusAlive = true;
	worldState.goldLocation = Location(0,0); // unknown
	Orientation FWorientation;
}

Agent::~Agent ()
{

}

void Agent::Initialize ()
{
	// Initialize agent back to the beginning of the world
	worldState.agentLocation = Location(1,1);
	worldState.agentOrientation = RIGHT;
	worldState.agentAlive = true;
	worldState.agentHasGold = false;
	lastAction = CLIMB; // dummy action
	actionList.clear();

	safeLocationFW = Location(0,0); //initialize for hw9 requirements
	Orientation FWorientation = UP;
	//If agent still need to try after got gold, revive wumpus
	if (wumpusAlive == false){
		wumpusAlive = true;
		searchEngine.RemoveSafeLocation(wumpusLocations.X,wumpusLocations.Y); 
	}
}

Action Agent::Process (Percept& percept)
{
	list<Action> actionList2;
		UpdateState(lastAction, percept);
		if (actionList.empty()) {
			if (percept.Glitter) {
				// HW5.4: If there is gold, then GRAB it
				cout << "Found gold. Grabbing it.\n";
				actionList.push_back(GRAB);
			} 
			else if (worldState.agentHasGold && (worldState.agentLocation == Location(1,1))) {
				// HW5.5: If agent has gold and is in (1,1), then CLIMB
				cout << "Have gold and in (1,1). Climbing.\n";
				actionList.push_back(CLIMB);
			}
			else if (!worldState.agentHasGold && !(worldState.goldLocation == Location(0,0))) {
				// HW5.6: If agent doesn't have gold, but knows its location, then navigate to that location
				cout << "Moving to known gold location (" << worldState.goldLocation.X << "," << worldState.goldLocation.Y << ").\n";
				actionList2 = searchEngine.FindPath(worldState.agentLocation, worldState.agentOrientation, worldState.goldLocation, worldState.agentOrientation);
				//hw9
				if (!(actionList2.empty())){
					actionList.splice(actionList.end(), actionList2);
				}
				else{
					cout<<"Wumpus is on the path from (1,1) to the gold location."<<endl;
					safeLocationFacetoWumpus(FWorientation, safeLocations, unsafeLocations, visitedLocations, stenchLocations);
					if(!(safeLocationFW == Location(0,0))){
						actionList2 = searchEngine.FindPath(worldState.agentLocation, worldState.agentOrientation, safeLocationFW, FWorientation);
						if(!(actionList2.empty())){
							actionList.splice(actionList.end(), actionList2);
							actionList.push_back(SHOOT);
					}
						if(safeLocationFW == worldState.agentLocation)
							actionList.push_back(SHOOT);
					}
				}
			}
			else if (worldState.agentHasGold && !(worldState.agentLocation == Location(1,1))) {
				// HW5.7: If agent has gold, but isn't in (1,1), then navigate to (1,1)
				cout << "Have gold. Moving to (1,1).\n";
				actionList2 = searchEngine.FindPath(worldState.agentLocation, worldState.agentOrientation, Location(1,1), worldState.agentOrientation);
				actionList.splice(actionList.end(), actionList2);
			} 
				// HW5.8: If safe unvisited location, then navigate there (should be one)
			else{
				Location safeUnvisitedLocation = SafeUnvisitedLocation();
				cout << "safe unvisited location: "<<safeUnvisitedLocation.X << "," << safeUnvisitedLocation.Y << endl;
				//Print all current safe locations
				/*
				list<Location>::iterator safelocation;
				cout << "safe locations: ";
				for (safelocation = safeLocations.begin(); safelocation != safeLocations.end(); ++safelocation)
				{
					Location safeLoc = *safelocation;
					cout << safeLoc.X << "," << safeLoc.Y << endl;
				}
				*/
				if(!(safeUnvisitedLocation == Location(0,0))){
					cout << "Moving to safe unvisited location (" << safeUnvisitedLocation.X << "," << safeUnvisitedLocation.Y << ").\n";
					actionList2 = searchEngine.FindPath(worldState.agentLocation, worldState.agentOrientation, safeUnvisitedLocation, worldState.agentOrientation);
					if (!(actionList2.empty())) {
					actionList.splice(actionList.end(), actionList2);
					} else {
						cout << "ERROR: no path to safe unvisited location\n"; // for debugging
						exit(1);
					}
				}
				else{
					cout << "wumpus location: " << wumpusLocations.X << "," << wumpusLocations.Y << endl; 
					//HW9.4a  If the agent knows the location of the live Wumpus, and there is a safe location facing the Wumpus, 
					//then the agent should move there and shoot the Wumpus.
					if (!(wumpusLocations == Location(0,0)) && wumpusAlive == true){
						list<Location> adj_wumpus;
						AdjacentLocations(wumpusLocations, adj_wumpus);
						list<Location>::iterator adj;

						for (adj = adj_wumpus.begin(); adj != adj_wumpus.end(); ++adj){
							if(!(MemberLocation(*adj,unsafeLocations))){
								safeLocations.push_back(*adj);
								searchEngine.AddSafeLocation(adj->X,adj->Y);
							}
						}
						cout << "wumpus" << wumpusLocations.X << "," << wumpusLocations.Y << endl;
						safeLocationFacetoWumpus(FWorientation, safeLocations, unsafeLocations, visitedLocations, stenchLocations);
						cout << "orientation" << FWorientation << endl;
						if(!(safeLocationFW == Location(0,0))){
							cout << "agentLocation:" << worldState.agentLocation.X << "," << worldState.agentLocation.Y << "agentOrientation:" << FWorientation << endl;
							cout << "safe location face to wumpus" << safeLocationFW.X << "," << safeLocationFW.Y << "OrientationFw" << FWorientation << endl;
							actionList2 = searchEngine.FindPath(worldState.agentLocation,worldState.agentOrientation,safeLocationFW,FWorientation);
							if(!(actionList2.empty())){
								actionList.splice(actionList.end(), actionList2);
								actionList.push_back(SHOOT);
							}
							if(safeLocationFW == worldState.agentLocation)
							actionList.push_back(SHOOT);
						}
					}
					else{	
						//hw9.4b If the Wumpus is dead or cannot be killed, then the agent should move to an unvisited location 
						//that is not known to be unsafe
						if (safeUnvisitedLocation == Location(0,0)){
							cout << "Moving to safe unvisited location (" << safeUnvisitedLocation.X << "," << safeUnvisitedLocation.Y << ").\n";
							actionList2 = searchEngine.FindPath(worldState.agentLocation, worldState.agentOrientation, safeUnvisitedLocation, worldState.agentOrientation);
							if(!(actionList2.empty())){
								actionList.splice(actionList.end(), actionList2);
							}
						else{
							cout<<"There is no path to safe unvisited location.";
							exit(1);
							}	
						}
					}
				}
			}
		}
		Action action = actionList.front();
		actionList.pop_front();
		lastAction = action;
		return action;
}
//hw9.3 Find appropriate location and orientation to shoot wumpus
void Agent::safeLocationFacetoWumpus(Orientation FWorientation, list<Location>& safeLocations, list<Location>& unsafeLocations, list<Location>& visitedLocations, list<Location>& stenchLocations){
	int x = wumpusLocations.X;
	int y = wumpusLocations.Y;
	int x1=x-1, x2=x+1, y1=y-1, y2=y+1;
	Location location1 = Location (x1,y);
	Location location2 = Location (x2,y);
	Location location3 = Location (x,y1);
	Location location4 = Location (x,y2);
	if(MemberLocation(location1, safeLocations) && !(MemberLocation(location1,unsafeLocations)) && MemberLocation(location1, visitedLocations) && MemberLocation(location1, stenchLocations)){
		FWorientation = RIGHT;
		safeLocationFW = location1;
		//cout << "FWorientation" <<FWorientation << endl;
	}
	else if(MemberLocation(location2, safeLocations) && !(MemberLocation(location2,unsafeLocations)) && MemberLocation(location2, visitedLocations) && MemberLocation(location2, stenchLocations)){
		FWorientation = LEFT;
		safeLocationFW = location2;
		//cout << "FWorientation" <<FWorientation << endl;
	}
	else if(MemberLocation(location3, safeLocations) && !(MemberLocation(location3,unsafeLocations)) && MemberLocation(location3, visitedLocations) && MemberLocation(location3, stenchLocations)){
		FWorientation = UP;
		safeLocationFW = location3;
		//cout << "FWorientation" <<FWorientation << endl;
	}
	else if(MemberLocation(location4, safeLocations) && !(MemberLocation(location4,unsafeLocations)) && MemberLocation(location4, visitedLocations) && MemberLocation(location4, stenchLocations)){
		FWorientation = DOWN;
		safeLocationFW = location4;
		//cout << "FWorientation" <<FWorientation << endl;
	}
}
//hw9.3 Used to find diagonal
bool Agent::DetermineDiagonal(int x1, int x2, int y1, int y2){
	if(abs(x1-x2)==1 && abs(y1-y2)==1){
	//cout<< "diagonal found!" <<endl;
	return true;
	}
}

//hw9 For finding wumpus
void Agent::FindWumpus(int x, int y){
	cout << "Find wumpus began!" << endl;
	list<Location>::iterator itr;
	for(itr = stenchLocations.begin(); itr != stenchLocations.end(); ++itr){
		Location stenchLocation = *itr;
		if(DetermineDiagonal(x, y, stenchLocation.X, stenchLocation.Y)){
			cout << "diagonal begin" <<endl;
			Location adj_1 = Location(x,stenchLocation.Y);
			Location adj_2 = Location(stenchLocation.X,y);
			if (MemberLocation(adj_1, safeLocations) && MemberLocation(adj_1, visitedLocations) && !(MemberLocation(adj_1, unsafeLocations))){
				wumpusLocations = adj_2;
				cout << "Wumpus location founded!" << wumpusLocations.X << "," << wumpusLocations.Y <<endl;
				unsafeLocations.push_back(wumpusLocations);
				searchEngine.RemoveSafeLocation(wumpusLocations.X,wumpusLocations.Y);
				break;
			}
			else{
				wumpusLocations = adj_1;
				unsafeLocations.push_back(wumpusLocations);
				searchEngine.RemoveSafeLocation(wumpusLocations.X,wumpusLocations.Y);
			}
		}
	}
}

void Agent::GameOver (int score)
{
	if (score < -1000) {
		// Agent died by going forward into pit or Wumpus
		Percept percept; // dummy, values don't matter
		UpdateState(GOFORWARD, percept, true);
		int X = worldState.agentLocation.X;
		int Y = worldState.agentLocation.Y;
		if (!(MemberLocation(worldState.agentLocation, unsafeLocations))) {
			unsafeLocations.push_back(worldState.agentLocation);
		}
		searchEngine.RemoveSafeLocation(X,Y);
		cout << "Found unsafe location at (" << X << "," << Y << ")\n";
	}
}

void Agent::UpdateState(Action lastAction, Percept& percept, bool gameOver) {
	int X = worldState.agentLocation.X;
	int Y = worldState.agentLocation.Y;
	Orientation orientation = worldState.agentOrientation;

	if (lastAction == TURNLEFT) {
		worldState.agentOrientation = (Orientation) ((orientation + 1) % 4);
	}
	if (lastAction == TURNRIGHT) {
		if (orientation == RIGHT) {
			worldState.agentOrientation = DOWN;
		} else {
			worldState.agentOrientation = (Orientation) (orientation - 1);
		}
	}
	if (lastAction == GOFORWARD) {
		if (percept.Bump) {
			if ((orientation == RIGHT) || (orientation == UP)) {
				cout << "World size known to be " << worldState.worldSize << "x" << worldState.worldSize << endl;
				worldSizeKnown = true;
				RemoveOutsideLocations();
			}
		} else {
			switch (orientation) {
			case UP:
				worldState.agentLocation.Y = Y + 1;
				break;
			case DOWN:
				worldState.agentLocation.Y = Y - 1;
				break;
			case LEFT:
				worldState.agentLocation.X = X - 1;
				break;
			case RIGHT:
				worldState.agentLocation.X = X + 1;
				break;
			}
		}
	}
	if (lastAction == GRAB) { // Assume GRAB only done if Glitter was present
		worldState.agentHasGold = true;
	}
	if (lastAction == CLIMB) {
		// do nothing; if CLIMB worked, this won't be executed anyway
	}
	// HW5 requirement 3a
	if (percept.Glitter) {
		worldState.goldLocation = worldState.agentLocation;
		cout << "Found gold at (" << worldState.goldLocation.X << "," << worldState.goldLocation.Y << ")\n";
	}
	// HW5 clarification: track world size
	int new_max = max(worldState.agentLocation.X, worldState.agentLocation.Y);
	if (new_max > worldState.worldSize) {
		worldState.worldSize = new_max;
	}
	// HW5 requirement 3b
	if (!gameOver) {
		UpdateSafeLocations(worldState.agentLocation);
	}
	// HW5 requirement 3c
	if (!(MemberLocation(worldState.agentLocation, visitedLocations))) {
		visitedLocations.push_back(worldState.agentLocation);
	}
	//hw9.3 
	if (percept.Stench){
		if(!(MemberLocation(worldState.agentLocation,stenchLocations)) && wumpusLocations == Location(0,0)){
			FindWumpus(worldState.agentLocation.X, worldState.agentLocation.Y);
			cout << "Wumpus is located at" << wumpusLocations.X << "," << wumpusLocations.Y << endl;
			stenchLocations.push_back(worldState.agentLocation);
			cout << "stench location:" << worldState.agentLocation.X << "," << worldState.agentLocation.Y << endl;
		}
	}
	if (percept.Scream){
		cout<<"Wumpus is dead!"<< endl;
		wumpusAlive = false;
		safeLocations.push_back(wumpusLocations);
		searchEngine.AddSafeLocation(wumpusLocations.X,wumpusLocations.Y);
	}
}

bool Agent::MemberLocation(Location& location, list<Location>& locationList) {
	if (find(locationList.begin(), locationList.end(), location) != locationList.end()) {
		return true;
	}
	return false;
}

Location Agent::SafeUnvisitedLocation() {
	// Find and return safe unvisited location.
	list<Location>::iterator loc_itr;
	for (loc_itr = safeLocations.begin(); loc_itr != safeLocations.end(); ++loc_itr) {
		if (!(MemberLocation(*loc_itr, visitedLocations))) {
			return *loc_itr;
		}
	}
	return Location(0,0);
}

void Agent::UpdateSafeLocations(Location& location) {
	// HW5 requirement 3b, and HW5 clarification about not known to be unsafe locations
    // Add current and adjacent locations to safe locations, unless known to be unsafe.
	if (!(MemberLocation(location, safeLocations))) {
		safeLocations.push_back(location);
		searchEngine.AddSafeLocation(location.X, location.Y);
	}
	list<Location> adj_locs;
	AdjacentLocations(location, adj_locs);
	list<Location>::iterator loc_itr;
	for (loc_itr = adj_locs.begin(); loc_itr != adj_locs.end(); ++loc_itr) {
		if ((!(MemberLocation(*loc_itr, safeLocations))) && (!(MemberLocation(*loc_itr, unsafeLocations)))) {
			safeLocations.push_back(*loc_itr);
			searchEngine.AddSafeLocation(loc_itr->X, loc_itr->Y);
		}
	}
}

void Agent::RemoveOutsideLocations() {
	// Know exact world size, so remove locations outside the world.
	int boundary = worldState.worldSize + 1;
	for (int i = 1; i < boundary; ++i) {
		safeLocations.remove(Location(i,boundary));
		searchEngine.RemoveSafeLocation(i,boundary);
		safeLocations.remove(Location(boundary,i));
		searchEngine.RemoveSafeLocation(boundary,i);
	}
	safeLocations.remove(Location(boundary,boundary));
	searchEngine.RemoveSafeLocation(boundary,boundary);
}

void Agent::AdjacentLocations(Location& location, list<Location>& adjacentLocations) {
	// Append locations adjacent to given location on to give locations list.
	// One row/col beyond unknown world size is okay. Locations outside the world
	// will be removed later.
	int X = location.X;
	int Y = location.Y;
	if (X > 1) {
		adjacentLocations.push_back(Location(X-1,Y));
	}
	if (Y > 1) {
		adjacentLocations.push_back(Location(X,Y-1));
	}
	if (worldSizeKnown) {
		if (X < worldState.worldSize) {
			adjacentLocations.push_back(Location(X+1,Y));
		}
		if (Y < worldState.worldSize) {
			adjacentLocations.push_back(Location(X,Y+1));
		}
	} else {
		adjacentLocations.push_back(Location(X+1,Y));
		adjacentLocations.push_back(Location(X,Y+1));
	}
}
