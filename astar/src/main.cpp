#include <GL/gl.h>
#include <GL/glut.h>
#include <vector>
#include "../include/graphics.h"

#define COLUMNS 16
#define ROWS 16
#define FPS 60

void timer_callback(int);
void display_callback();
void reshape_callback(int, int);
void mouse_callback(int, int, int, int);

void init()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	initGrid(COLUMNS, ROWS);
	nodeInit();
	solve();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(500, 500);
	glutCreateWindow("A* Pathfinding Visualizer");
	glutDisplayFunc(display_callback);
	glutReshapeFunc(reshape_callback);
	glutTimerFunc(0, timer_callback, 0);
	glutMouseFunc(mouse_callback);
	init();
	glutMainLoop();

	return 0;
}

void display_callback()
{
	glClear(GL_COLOR_BUFFER_BIT);
	
	drawNodes();	
	drawGrid(); // Grid
	
	glutSwapBuffers();
}

void reshape_callback(int w, int h)
{
	glViewport(0, 0, (GLsizei)w,(GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, COLUMNS, 0.0, ROWS, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
}

void timer_callback(int)
{
	glutPostRedisplay();
	glutTimerFunc(1000/FPS, timer_callback, 0);
}

void mouse_callback(int button, int state, int x, int y)
{
	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
	{
		toggleBarrier(x / (glutGet(GLUT_WINDOW_WIDTH) / 16), 15 - y / (glutGet(GLUT_WINDOW_HEIGHT) / 16));
	}
}
