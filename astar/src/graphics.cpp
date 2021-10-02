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
	bool path = false;
	float globalGoal;
	float localGoal;
	int x;
	int y;
	vector<int> neighboursX;
	vector<int> neighboursY;
	vector<sNode*> neighbours;
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
		glColor3f(1.0, 1.0, 1.0);
	}
	else
	{
		glLineWidth(1.0);
		glColor3f(1.0, 1.0, 1.0);
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

	for (int x = 0; x < 16; x++)
	{
		for (int y = 0; y < 16; y++)
		{
			if (y > 0)
				nodes[x][y].neighbours.push_back(&nodes[x][y - 1]);
			if (y < 15)
				nodes[x][y].neighbours.push_back(&nodes[x][y + 1]);
			if (x > 0)
				nodes[x][y].neighbours.push_back(&nodes[x - 1][y]);
			if (x < 15)
				nodes[x][y].neighbours.push_back(&nodes[x + 1][y]);
		}
	}
	
	nodes[1][7].start = true;
	nodes[14][7].end = true;

	startX = 1;
	startY = 7;

	endX = 14;
	endY = 7;
	
	nodeStart = &nodes[1][7];
	nodeEnd = &nodes[14][7];
}

void solve()
{
	// Reset all nodes to default values
	for (int x = 0; x < 16; x++)
	{
		for (int y = 0; y < 16; y++)
		{
			nodes[x][y].visited = false;
			nodes[x][y].path = false;
			nodes[x][y].globalGoal = INFINITY;
			nodes[x][y].localGoal = INFINITY;
			nodes[x][y].parent = nullptr;
		}
	}
	
	auto distance = [](sNode* a, sNode* b)
	{
		return sqrtf((a->x - b->x)*(a->x - b->x) + (a->y - b->y)*(a->y - b->y));
	};

	auto heuristic = [distance](sNode* a, sNode* b)
	{
		return distance(a, b);
	};

	// starting conditions
	sNode *nodeCurrent = nodeStart;
	nodeStart->localGoal = 0.0f;
	nodeStart->globalGoal = heuristic(nodeStart, nodeEnd);

	// add start node to list to test to ensure it gets tested
	list<sNode*> listNotTestedNodes;
	listNotTestedNodes.push_back(nodeStart);

	// paths but this one will do, it wont be the longest
	while (!listNotTestedNodes.empty() && nodeCurrent != nodeEnd)
	{
		// sort nodes from lowest to highest by global goal
		listNotTestedNodes.sort([](const sNode* lhs, const sNode* rhs){ return lhs->globalGoal < rhs->globalGoal; } );
			
		// front is probably lowest distance node
		// remove nodes which have already been visited
		while(!listNotTestedNodes.empty() && listNotTestedNodes.front()->visited)
			listNotTestedNodes.pop_front();

		// break loop if there ara no nodes to test
		if (listNotTestedNodes.empty())
			break;

		nodeCurrent = listNotTestedNodes.front();
		nodeCurrent->visited = true; // only check node once
			
					
		// Check each of this node's neighbours...
		for (auto nodeNeighbour : nodeCurrent->neighbours)
		{
			// add neighbour to list if it isnt a barrier or hasn't been visited
			if (!nodeNeighbour->visited && nodeNeighbour->barrier == 0)
				listNotTestedNodes.push_back(nodeNeighbour);

			// Calculate the neighbours potential lowest parent distance
			float fPossiblyLowerGoal = nodeCurrent->localGoal + distance(nodeCurrent, nodeNeighbour);

			// If choosing to path through this node is a lower distance than what 
			// the neighbour currently has set, update the neighbour to use this node
			// as the path source, and set its distance scores as necessary
			if (fPossiblyLowerGoal < nodeNeighbour->localGoal)
			{
				nodeNeighbour->parent = nodeCurrent;
				nodeNeighbour->localGoal = fPossiblyLowerGoal;

				// The best path length to the neighbour being tested has changed, so
				// update the neighbour's score. The heuristic is used to globally bias
				// the path algorithm, so it knows if its getting better or worse. At some
				// point the algo will realise this path is worse and abandon it, and then go
				// and search along the next best path.
				nodeNeighbour->globalGoal = nodeNeighbour->localGoal + heuristic(nodeNeighbour, nodeEnd);
			}
		}	
	}
}

void drawPath()
{
	if (nodeEnd != nullptr)
	{
		sNode *p = nodeEnd;

		while (p->parent != nullptr)
		{
			p->parent->path = true;
			p = p->parent;
		}
	}
}

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

void toggleStart(int x, int y)
{
	for (int j = 0; j < 16; j++)
	{
		for (int i = 0; i < 16; i++)
		{
			if (nodes[j][i].x == x && nodes[j][i].y == y && nodes[j][i].barrier == false && nodes[j][i].start == false)
			{
				for (int x = 0; x < 16; x++)
				{
					for (int y = 0; y < 16; y++)
					{
						nodes[x][y].start = false;
					}
				}

				nodes[j][i].start = true;
				nodeStart = &nodes[j][i];
				startX = x;
				startY = y;
			}
		}
	}
}

void toggleEnd(int x, int y)
{
	for (int j = 0; j < 16; j++)
	{
		for (int i = 0; i < 16; i++)
		{
			if (nodes[j][x].x == x && nodes[j][i].y == y && nodes[j][i].barrier == false && nodes[j][i].end == false)
			{
				for (int x = 0; x < 16; x++)
				{
					for (int y = 0; y < 16; y++)
					{
						nodes[x][y].end = false;
					}
				}

				nodes[j][i].end = true;
				nodeEnd = &nodes[j][i];
				endX = j;
				endY = i;
			}
		}
	}
}

void restart()
{
	for (int x = 0; x < 16; x++)
	{
		for (int y = 0; y < 16; y++)
		{
			nodes[x][y].visited = false;
			nodes[x][y].barrier = false;
			nodes[x][y].path = false;
			nodes[x][y].globalGoal = INFINITY;
			nodes[x][y].localGoal = INFINITY;
			nodes[x][y].parent = nullptr;
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
			
			else if (nodes[x][y].path == true)
				pathDraw(nodes[x][y].x, nodes[x][y].y);

			else
				emptyDraw(nodes[x][y].x, nodes[x][y].y);
		}
	}
}

void barrierDraw(int x, int y)
{
	glBegin(GL_QUADS);              // Each set of 4 vertices form a quad
      		glColor3f(1.0f, 1.0f, 0.0f); // Red
      		glVertex2f(x, y);    // x, y
      		glVertex2f(x + 1, y);
      		glVertex2f(x + 1, y + 1);
      		glVertex2f(x, y + 1);
   	glEnd();
}

void emptyDraw(int x, int y)
{
	glBegin(GL_QUADS);              // Each set of 4 vertices form a quad
      		glColor3f(0.0f, 0.0f, 0.0f); // Red
      		glVertex2f(x, y);    // x, y
      		glVertex2f(x + 1, y);
      		glVertex2f(x + 1, y + 1);
      		glVertex2f(x, y + 1);
   	glEnd();
}

void startDraw(int x, int y)
{
	glBegin(GL_QUADS);              // Each set of 4 vertices form a quad
      		glColor3f(0.0f, 1.0f, 0.0f); // Red
      		glVertex2f(x, y);    // x, y
      		glVertex2f(x + 1, y);
      		glVertex2f(x + 1, y + 1);
      		glVertex2f(x, y + 1);
   	glEnd();
}

void endDraw(int x, int y)
{
	glBegin(GL_QUADS);              // Each set of 4 vertices form a quad
      		glColor3f(1.0f, 0.0f, 0.0f); // Red
      		glVertex2f(x, y);    // x, y
      		glVertex2f(x + 1, y);
      		glVertex2f(x + 1, y + 1);
      		glVertex2f(x, y + 1);
   	glEnd();
}

void pathDraw(int x, int y)
{
	glBegin(GL_QUADS);              // Each set of 4 vertices form a quad
      		glColor3f(1.0f, 0.0f, 1.0f); // Red
      		glVertex2f(x, y);    // x, y
      		glVertex2f(x + 1, y);
      		glVertex2f(x + 1, y + 1);
      		glVertex2f(x, y + 1);
   	glEnd();
}
