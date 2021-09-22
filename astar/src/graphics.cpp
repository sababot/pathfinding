#include <GL/glut.h>
#include "../include/graphics.h"
#include <vector>
#include <array>
#include <cmath>
#include <list>
#include <algorithm>

using namespace std;

int gridX, gridY;

struct sNode
{
	bool barrier = false;
	bool visited = false;
	bool start = false;
	bool end = false;
	float globalGoal;
	float localGoal;
	int x;
	int y;
	vector<int> neighboursX;
	vector<int> neighboursY;
	sNode* parent = nullptr;
};

sNode nodes[16][16];
int startX, startY;
int endX, endY;
sNode *nodeStart = nullptr;
sNode *nodeEnd = nullptr;

void initGrid(int x, int y)
{
	gridX = x;
	gridY = y;
}

void unit(int, int);

void drawGrid()
{
	for (int x = 0; x < gridX; x++)
	{
		for (int y = 0; y < gridY; y++)
		{
			unit(x, y);
		}
	}
}

void unit(int x, int y)
{
	if (x == 0 || y == 0 || x == gridX - 1 || y == gridY - 1)
	{
		glLineWidth(1.0);
		glColor3f(1.0, 0.5, 1.0);
	}
	else
	{
		glLineWidth(1.0);
		glColor3f(1.0, 0.5, 1.0);
	}

	glBegin(GL_LINE_LOOP);
		glVertex2f(x, y);
		glVertex2f(x + 1, y);
		glVertex2f(x + 1, y + 1);
		glVertex2f(x, y + 1);
	glEnd();
}

void nodeInit()
{
	// Create a node for each box of the grid and assign their positions
	for (int x = 0; x < 16; x++)
	{
		for (int y = 0; y < 16; y++)
		{
			nodes[x][y].x = x;
			nodes[x][y].y = y;
		}
	}
	
	// Create connections between the nodes
	for (int x = 0; x < 16; x++)
	{
		for (int y = 0; y < 16; y++)
		{
			if (y > 0)
			{
				nodes[x][y].neighboursX.push_back(x);
				nodes[x][y].neighboursY.push_back(y - 1);
			}
			
			if (y < 15)
			{
				nodes[x][y].neighboursX.push_back(x);
				nodes[x][y].neighboursY.push_back(y + 1);
			}
			
			if (x > 0)
			{
				nodes[x][y].neighboursX.push_back(x - 1);
				nodes[x][y].neighboursY.push_back(y);
			}
			
			if (x < 15)
			{
				nodes[x][y].neighboursX.push_back(x + 1);
				nodes[x][y].neighboursY.push_back(y);
		
			}
		}
	}
	
	nodes[1][7].start = true;
	nodes[14][7].end = true;

	startX = 1;
	startY = 7;

	endX = 14;
	endY = 7;
}

void Solve_AStar()
{
	auto distance = [](sNode* a, sNode* b) // For convenience
	{
		return sqrtf((a->x - b->x)*(a->x - b->x) + (a->y - b->y)*(a->y - b->y));
	};

	auto heuristic = [distance](sNode* a, sNode* b) // So we can experiment with heuristic
	{
		return distance(a, b);
	};

	// Setup starting conditions
	sNode *nodeCurrent = nodeStart;
	nodeStart->localGoal = 0.0f;
	nodeStart->globalGoal = heuristic(nodeStart, nodeEnd);

	// Add start node to not tested list - this will ensure it gets tested.
	// As the algorithm progresses, newly discovered nodes get added to this
	// list, and will themselves be tested later
	list<sNode*> listNotTestedNodes;
	listNotTestedNodes.push_back(nodeStart);

	// if the not tested list contains nodes, there may be better paths
	// which have not yet been explored. However, we will also stop 
	// searching when we reach the target - there may well be better
	// paths but this one will do - it wont be the longest.
	while (!listNotTestedNodes.empty() && nodeCurrent != nodeEnd)// Find absolutely shortest path // && nodeCurrent != nodeEnd)
	{
		// Sort Untested nodes by global goal, so lowest is first
		listNotTestedNodes.sort([](const sNode* lhs, const sNode* rhs){ return lhs->fGlobalGoal < rhs->fGlobalGoal; } );
			
		// Front of listNotTestedNodes is potentially the lowest distance node. Our
		// list may also contain nodes that have been visited, so ditch these...
		while(!listNotTestedNodes.empty() && listNotTestedNodes.front()->bVisited)
			listNotTestedNodes.pop_front();

		// ...or abort because there are no valid nodes left to test
		if (listNotTestedNodes.empty())
			break;

		nodeCurrent = listNotTestedNodes.front();
		nodeCurrent->bVisited = true; // We only explore a node once
			
					
		// Check each of this node's neighbours...
		for (auto nodeNeighbour : nodeCurrent->vecNeighbours)
		{
			// ... and only if the neighbour is not visited and is 
			// not an obstacle, add it to NotTested List
			if (!nodeNeighbour->bVisited && nodeNeighbour->bObstacle == 0)
				listNotTestedNodes.push_back(nodeNeighbour);

			// Calculate the neighbours potential lowest parent distance
			float fPossiblyLowerGoal = nodeCurrent->fLocalGoal + distance(nodeCurrent, nodeNeighbour);

			// If choosing to path through this node is a lower distance than what 
			// the neighbour currently has set, update the neighbour to use this node
			// as the path source, and set its distance scores as necessary
			if (fPossiblyLowerGoal < nodeNeighbour->fLocalGoal)
			{
				nodeNeighbour->parent = nodeCurrent;
				nodeNeighbour->fLocalGoal = fPossiblyLowerGoal;

				// The best path length to the neighbour being tested has changed, so
				// update the neighbour's score. The heuristic is used to globally bias
				// the path algorithm, so it knows if its getting better or worse. At some
				// point the algo will realise this path is worse and abandon it, and then go
				// and search along the next best path.
				nodeNeighbour->fGlobalGoal = nodeNeighbour->fLocalGoal + heuristic(nodeNeighbour, nodeEnd);
			}
		}	
	}

	return true;
}

/*
void solve()
{
	auto distance = [] (int x1, int x2, int y1, int y2)
	{
		return sqrtf((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y1));
	};

	auto heuristic = [distance] (int x1, int x2, int y1, int y2)
	{
		return distance(x1, x2, y1, y2);
	};

	// Setup starting conditions
	int current;
	nodes[startX][startY].localGoal = 0.0f;
	nodes[endX][endY].globalGoal = heuristic(startX, endX, startY, endY);

	// Add starting node to the to test list so that it gets tested first
	vector<std::array<int, 2>> listToTest;
	int startNode[2] = {startX, startY};
	int endNode[2] = {endX, endY};
	listToTest.push_back(startNode);
	
	// While the tested list contains nodes there could be better paths. It will stop searching once the target has been reached
	while (!listToTest.empty() && listToTest[current] != endNode)
	{
		// Sort untested nodes by global goal from lowest to largest
		for (int i = 0; i < (int)listToTest.size(); i++)
		{
			// Code
		}
		// Ditch nodes which have already been visited
	}

}
*/

void toggleBarrier(int x, int y)
{
	for (int j = 0; j < 16; j++)
	{
		for (int i = 0; i < 16; i++)
		{
			if (nodes[j][i].x == x && nodes[j][i].y == y && nodes[j][i].barrier == false)
				nodes[j][i].barrier = true;
			else if (nodes[j][i].x == x && nodes[j][i].y == y && nodes[j][i].barrier == true)
				nodes[j][i].barrier = false;
		}
	}
}

void drawNodes()
{
	for (int x = 0; x < 16; x++)
	{
		for(int y = 0; y < 16; y++)
		{
			if (nodes[x][y].barrier == true)
				barrierDraw(nodes[x][y].x, nodes[x][y].y);
			
			else if (nodes[x][y].start == true)
				startDraw(nodes[x][y].x, nodes[x][y].y);
			
			else if (nodes[x][y].end == true)
				endDraw(nodes[x][y].x, nodes[x][y].y);

			else
				emptyDraw(nodes[x][y].x, nodes[x][y].y);
		}
	}
}

void barrierDraw(int x, int y)
{
	glColor3f(1.0, 1.0, 1.0);
	glRectd(x, y, x + 1, y + 1);
}

void emptyDraw(int x, int y)
{
	glColor3f(0.0, 0.0, 0.0);
	glRectd(x, y ,x + 1, y + 1);
}

void startDraw(int x, int y)
{
	glColor3f(0.0, 1.0, 0.0);
	glRectd(x, y, x + 1, y + 1);
}

void endDraw(int x, int y)
{
	glColor3f(1.0, 0.0, 0.0);
	glRectd(x, y, x + 1, y + 1);
}
