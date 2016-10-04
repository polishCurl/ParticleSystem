/*********************************************************************
* File:         particleSystem.c
* Author:       Krzysztof Koch  
* Date created: 04/10/2016
* Last mod:     04/10/2016
* Brief:        COMP37111 Assignment, particle system implementation.
*               Simulation of a fountain and smoke 
**********************************************************************/

#include "particleSystem.h"

/*********************************************************************
* Main method
**********************************************************************/
int main(int argc, char *argv[])
{
  double f;
  srand(time(NULL));
  initGraphics(argc, argv);
  glEnable(GL_POINT_SMOOTH);
  glPointSize(POINT_SIZE);
  glutMainLoop();
}


/*********************************************************************
* Initialisation function
**********************************************************************/
void initGraphics(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitWindowSize(800, 600);
  glutInitWindowPosition(100, 100);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
  glutCreateWindow("COMP37111 Fountain");
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutReshapeFunc(reshape);
  makeAxes();
}


/*********************************************************************
* Callback function, called whenever graphics should be redrawn
**********************************************************************/
void display()
{
  glLoadIdentity();
  gluLookAt(EYE_X, EYE_Y, EYE_Z,
            CENTER_X, CENTER_Y, CENTER_Z,
            0.0, 1.0, 0.0);
  
  glClear(GL_COLOR_BUFFER_BIT);  // Clear the screen and depth buffer
  if(AXIS_ENABLED) glCallList(axisList);    // If enabled, draw coordinate axis
  spawnParticles();
  drawParticles();
  progressTime();
  glutPostRedisplay();
  glutSwapBuffers();                        // Double buffering in place
}


/*********************************************************************
* Spawn particles
**********************************************************************/
void spawnParticles() 
{
  int index;
  for (index = 0; index < NO_OF_WATERDROPS; index++) 
  {
    if (water[index].alive == 0) 
    {
      water[index].xpos = FOUNTAIN_X;
      water[index].ypos = FOUNTAIN_Y;
      water[index].zpos = FOUNTAIN_Z;
      water[index].xvel = myRandom(SIDE_SPLASH_MAX);
      water[index].yvel = myRandom(WATER_SPEED_MAX);
      water[index].zvel = myRandom(SIDE_SPLASH_MAX);
      water[index].r = WATER_DROP_COLOUR_R;
      water[index].g = WATER_DROP_COLOUR_G;
      water[index].b = WATER_DROP_COLOUR_B;
      water[index].alive = 1;
      water[index].mass = WATER_DROP_MASS;
    }
  }
}


/*********************************************************************
* Render the particles
**********************************************************************/
void drawParticles() 
{
  int index;
  glBegin (GL_POINTS);
  for (index = 0; index < NO_OF_WATERDROPS; index++) 
  {
    glColor3f(water[index].r , water[index].g, water[index].b);
    glVertex3f(water[index].xpos, water[index].ypos, water[index].zpos);
  }
  glEnd ();
}


/*********************************************************************
* Update the display
**********************************************************************/
void progressTime() 
{
  int index;
  for (index = 0; index < NO_OF_WATERDROPS; index++) 
  {
    water[index].xpos += water[index].xvel;
    water[index].ypos += water[index].yvel;
    water[index].zpos += water[index].zvel;
    water[index].yvel += water[index].mass * GRAVITY;
    if (water[index].ypos < FOUNTAIN_Y)
      water[index].alive = 0;
  }
}



/*********************************************************************
* Key definitions for controlling the simulation of particle system
**********************************************************************/
void keyboard(unsigned char key, int x, int y)
{
  if(key == 27) exit(0);
  glutPostRedisplay();
}


/*********************************************************************
* Reshape function called whenever the application windos is reshaped
**********************************************************************/
void reshape(int width, int height)
{
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glViewport(0, 0, (GLsizei)width, (GLsizei)height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, (GLfloat)width / (GLfloat)height, 1.0, 10000.0);
  glMatrixMode(GL_MODELVIEW);
}


/*********************************************************************
* Create a display list for drawing coord axis
**********************************************************************/
void makeAxes() 
{
  axisList = glGenLists(1);
  glNewList(axisList, GL_COMPILE);
      glLineWidth(2.0);
      glBegin(GL_LINES);
      glColor3f(1.0, 0.0, 0.0);       // X axis - red
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(AXIS_SIZE, 0.0, 0.0);
      glColor3f(0.0, 1.0, 0.0);       // Y axis - green
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(0.0, AXIS_SIZE, 0.0);
      glColor3f(0.0, 0.0, 1.0);       // Z axis - blue
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(0.0, 0.0, AXIS_SIZE);
    glEnd();
  glEndList();
}





/*********************************************************************
* Return random double within range [-range,range]
**********************************************************************/
double myRandom(double range)
{
  return (rand() / (double)RAND_MAX * 2.0 - 1.0) * range;
}



/*********************************************************************
* Draw the fountain from which the water will originate
**********************************************************************
void drawFountain()
{
  glPushMatrix();
    glColor3f(0.7, 0.7, 0.7);
    glBegin(GL_POLYGON);
      glVertex3f(  50, -50, -50 );      
      glVertex3f(  50,  50, -50 );   
      glVertex3f( -50,  50, -50 );      
      glVertex3f( -50, -50, -50 );   
    glEnd();

    glBegin(GL_POLYGON);
      glVertex3f(  50, -50, 50 );
      glVertex3f(  50,  50, 50 );
      glVertex3f( -50,  50, 50 );
      glVertex3f( -50, -50, 50 );
    glEnd();
 
    glBegin(GL_POLYGON);
      glVertex3f( 50, -50, -50 );
      glVertex3f( 50,  50, -50 );
      glVertex3f( 50,  50,  50 );
      glVertex3f( 50, -50,  50 );
    glEnd();
 
    glBegin(GL_POLYGON);
      glVertex3f( -50, -50,  50 );
      glVertex3f( -50,  50,  50 );
      glVertex3f( -50,  50, -50 );
      glVertex3f( -50, -50, -50 );
    glEnd();
 
    glBegin(GL_POLYGON);
      glVertex3f(  50,  50,  50 );
      glVertex3f(  50,  50, -50 );
      glVertex3f( -50,  50, -50 );
      glVertex3f( -50,  50,  50 );
    glEnd();
 
    glBegin(GL_POLYGON);
      glVertex3f(  50, -50, -50 );
      glVertex3f(  50, -50,  50 );
      glVertex3f( -50, -50,  50 );
      glVertex3f( -50, -50, -50 );
    glEnd();
  glPopMatrix();
}*/