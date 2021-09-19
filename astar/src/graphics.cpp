#include <GL/glut.h>
#include "../include/graphics.h"
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

int gridX, gridY;

struct sNode
{
	bool barrier = false;
	bool visited = false;
	float globalGoal;
	float localGoal;
	int x;
	int y;
	std::vector<sNode> neighbours;
	sNode* parent = nullptr;
};

sNode nodes[16][16];
sNode nodeStart;
sNode nodeEnd;

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
				// Code
			if (y < 15)
				// Code
			if (x > 0)
				// Code
			if (x < 15)
				// Code
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

void drawNodes()
{
	for (int x = 0; x < 16; x++)
	{
		for(int y = 0; y < 16; y++)
		{
			if (nodes[x][y].barrier == true)
			{
				barrierDraw(nodes[x][y].x, nodes[x][y].y);
			}
	
			else
			{
				emptyDraw(nodes[x][y].x, nodes[x][y].y);
			}
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
