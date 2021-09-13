#include <iostream>

using namespace std;

#define HEIGHT 20
#define WIDTH 40

void renderScreen(int markerX, int markerY, int targetX, int targetY)
{
	for (int i = 0; i < HEIGHT; i++)
	{
		cout << "*";

    		for (int j = 0; j < WIDTH; j++)
    		{
      			if (i == 0 || i == (HEIGHT - 1))
      			{
        			cout << "*";
      			}
			
			else if (i == markerY && j == markerX)
			{
				cout << "@";
			}

			else if (i == targetY && j == targetX)
			{
				cout << "0";
			}

      			else
      			{
        			cout << " ";
      			}	
    		}

    		cout << "*\n";
	}
}

void placeMarker(int x, int y)
{

}

int main(int argc, char** argv)
{
	renderScreen(atoi(argv[1]) * 2, atoi(argv[2]), atoi(argv[3]) * 2, atoi(argv[4]));
}
