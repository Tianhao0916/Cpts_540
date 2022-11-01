// Agent.h
//
// Fall 2021 HW5 solution.
//
// Written by Larry Holder.

#ifndef AGENT_H
#define AGENT_H

#include "Action.h"
#include "Percept.h"
#include "WorldState.h"
#include "Location.h"
#include "Orientation.h"
#include "Search.h"
#include <list>

class MySearchEngine: public SearchEngine {
	virtual int HeuristicFunction (SearchState* state, SearchState* goalState);
};

class Agent
{
public:
	Agent ();
	~Agent ();
	void Initialize ();
	Action Process (Percept& percept);
	void GameOver (int score);
	void safeLocationFacetoWumpus(Orientation orientation, list<Location>& safeLocations, list<Location>& unsafeLocations, list<Location>& visitedLocations, list<Location>& stenchLocations);

	void UpdateState(Action lastAction, Percept& percept, bool gameOver=false);
	Location SafeUnvisitedLocation();
	bool MemberLocation(Location& location, list<Location>& locationList);
	void UpdateSafeLocations(Location& location);
	void RemoveOutsideLocations();
	void AdjacentLocations(Location& location, list<Location>& adjacentLocations);

	WorldState worldState;
	bool worldSizeKnown;
	list<Location> visitedLocations;
	list<Location> safeLocations; // For HW5, means not known to be unsafe
	list<Location> unsafeLocations;
	list<Location> stenchLocations; // For HW9.2, maintain stenchlocations

	Orientation FWorientation;
	Location wumpusLocations; // For HW9.2, maintain wumpuslocation
	Location safeLocationFW;
	bool wumpusAlive;
	bool DetermineDiagonal(int x1, int x2, int y1, int y2);
	void FindWumpus(int x, int y);

	list<Action> actionList;
	MySearchEngine searchEngine;
	Action lastAction;
};

#endif // AGENT_H
