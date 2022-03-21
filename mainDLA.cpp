#include <GL/glut.h>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <math.h>
#include <string>
#include <experimental/filesystem>
#include <ctime>


#include "DLASystem.h"
#include "Window.h"

using namespace std;
namespace fs = std::experimental::filesystem;

// functions which are needed for openGL go into a namespace so that we can identify them
namespace drawFuncs {
  void handleKeypress(unsigned char key, int x, int y);
  void display(void);
  void update(int val);
  void introMessage();
}

// this is a global pointer, which is how we access the system itself
DLASystem *sys;

int main(int argc, char **argv) {
  // turn on glut
	glutInit(&argc, argv);

  int window_size[] = { 480,480 };
  string window_title("simple DLA simulation");

  // create a window
  Window *win = new Window(window_size,window_title);

  // create the system
  sys = new DLASystem(win);

  // this is the seed for the random numbers
  int seed = time(NULL);
  cout << "setting seed " << seed << endl;
  sys->setSeed(seed);

  // print the "help" message to the console
  drawFuncs::introMessage();

  // tell openGL how to redraw the screen and respond to the keyboard
	glutDisplayFunc(  drawFuncs::display );
	glutKeyboardFunc( drawFuncs::handleKeypress );

  // tell openGL to do its first update after waiting 10ms
  int wait = 10;
  int val = 0;
	glutTimerFunc(wait, drawFuncs::update, val);

  // start the openGL stuff
 	glutMainLoop();

  return 0;
}

// this is just a help message
void drawFuncs::introMessage() {
        cout << "Keys (while in graphics window):" << endl << "  q or e to quit (or exit)" << endl;
        cout << "  h to print this message (help)" << endl;
        cout << "  u for a single update" << endl;
        cout << "  g to start running (go)" << endl;
        cout << "  p to pause running" << endl;
        cout << "  s to run in slow-mode" << endl;
        cout << "  f to run in fast-mode" << endl;
        cout << "  r to clear everything (reset)" << endl;
        cout << "  z to pause and zoom in" << endl;
        cout << "  w or b to change background colour to white or black" << endl;
        cout << "  0 to print size and number of particles" << endl;
        cout << "  n to delete particle after unsuccessful stick" << endl;
        cout << "  d to allow particles to stick diagonally as well as cardinally" << endl;
        cout << "  v to adjust sticking probability" << endl;
        cout << "  c to change minimum number of successful sticks required before particle stops" << endl;
        cout << "  m to simulate inter-molecular forces causing particles to attract" << endl;
}

// openGL function deals with the keyboard
void drawFuncs::handleKeypress(unsigned char key, int x, int y) {
	switch (key) {
  case 'h':
    drawFuncs::introMessage();
    break;
	case 'q':
	case 'e':
		cout << "Exiting..." << endl;
    // delete the system
    delete sys;
		exit(0);
		break;
  case 'p':
    cout << "pause" << endl;
    sys->pauseRunning();
    break;
  case 'g':{
    cout << "go" << endl;
    int fileNum = 0;
    fs::path checkF;
    bool ex;
    do{
        //Ensures new file made each run so no data overwritten
        fileNum++;
        sys->filename = "";
        sys->filename.append("fractDim").append(to_string(fileNum)).append(".csv");
        fs::path checkF{sys->filename};
        ex = fs::exists(checkF);
    }while (ex);
    sys->setRunning();
    glutTimerFunc(0, drawFuncs::update, 0);
    break;
  }
  case 's':
    cout << "slow" << endl;
    sys->setSlow();
    break;
  case 'w':
    cout << "white" << endl;
    sys->setWinBackgroundWhite();
    break;
  case 'b':
    cout << "black" << endl;
    sys->setWinBackgroundBlack();
    break;
  case 'f':
    cout << "fast" << endl;
    sys->setFast();
    break;
  case 'r':
    cout << "reset" << endl;
    sys->Reset();
    break;
  case 'z':
    cout << "zoom" << endl;
    sys->pauseRunning();
    sys->viewAddCircle();
    break;
  case 'u':
    cout << "upd" << endl;
    sys->Update();
    break;
  case '0':
    cout << "print size" << endl;
    sys->printSize();
    break;
  case 'n':
    sys->delNoStick = not sys->delNoStick;
    cout << "delete on no stick: " << sys->delNoStick << endl;
    break;
  case 'd':
    sys->diagonalStick=not sys->diagonalStick;
    cout << "diagonal stick: " << sys->diagonalStick << endl;
    break;
  case 'v':
    cout << "Enter a probability between 0 and 1: ";
    cin >> sys->stickProb;
    break;
  case 'c':
    cout << "Enter number of collisions before stick (min 1): ";
    cin >> sys->minColls;
    break;
  case 'm':
    cout << "Enter the max separation for particle to be attracted (>=1, 0 disables): ";
    cin >> sys->attrSeparation;
	}
  // tell openGL to redraw the window
	glutPostRedisplay();
}

// this function gets called whenever the algorithm should do its update
void drawFuncs::update(int val) {
  int wait;  // time to wait between updates (milliseconds)

  if (( sys->running ) && (sys->numParticles < sys->endNum)) {
    if ( sys->slowNotFast == 1)
      wait = 10;
    else
      wait = 0;

    sys->Update();

    // tell openGL to call this funtion again after "wait" milliseconds
    glutTimerFunc(wait, drawFuncs::update, 0);
  }
}

// this function redraws the window when necessary
void drawFuncs::display() {
    	//  Clear the window or more specifically the frame buffer...
      //  This happens by replacing all the contents of the frame
      //  buffer by the clear color (black in our case)
      glClear(GL_COLOR_BUFFER_BIT);

      // this puts the camera at the origin (not sure why) with (I think) z axis out of page and y axis up
      // there is also the question of the GL perspective which is not set up in any clear way at the moment
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      gluLookAt(0.0, 0.0, 1.0,   /* camera position */
        0.0, 0.0, -1.0,        /* point to look at */
        0.0, 1.0, 0.0);		   /* up direction */

      //sys->DrawSpheres();
      sys->DrawSquares();

      //  Swap contents of backward and forward frame buffers
      glutSwapBuffers();
}

