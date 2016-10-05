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
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
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
  if(axisEnabled) glCallList(axisList);    // If enabled, draw coordinate axis
  spawnParticles();
  drawParticles();
  progressTime();
  glutPostRedisplay();
  calculateFPS();
  glutSwapBuffers();                        // Double buffering in place
}


/*********************************************************************
* Spawn particles
**********************************************************************/
void spawnParticles() 
{
  int index;

  for (index = 0; index < noOfWaterdrops; index++) 
  {
    if (water[index].alive == 0) 
    {
      water[index].xpos = FOUNTAIN_X;
      water[index].ypos = FOUNTAIN_Y;
      water[index].zpos = FOUNTAIN_Z;
      water[index].xvel = gaussianRandom(0.0, SIDE_SPLASH_VAR);
      water[index].yvel = gaussianRandom(WATER_SPEED_MEAN, WATER_SPEED_VAR);
      water[index].zvel = gaussianRandom(0.0, SIDE_SPLASH_VAR);
      water[index].r = WATER_DROP_COLOUR_R;
      water[index].g = WATER_DROP_COLOUR_G;
      water[index].b = WATER_DROP_COLOUR_B;
      water[index].alive = 1;
      water[index].mass = WATER_DROP_MASS;
    }
  }

  for (index = 0; index < noOfSmokeParticles; index++) 
  {
    if (smoke[index].alive <= 0) 
    {
      smoke[index].xpos = gaussianRandom(SMOKE_EMITTER_X, SMOKE_EMITTER_SIZE);
      smoke[index].ypos = SMOKE_EMITTER_Y;
      smoke[index].zpos = gaussianRandom(SMOKE_EMITTER_Z, SMOKE_EMITTER_SIZE);
      smoke[index].xvel = 0.0;
      smoke[index].yvel = gaussianRandom(SMOKE_SPEED_MEAN, SMOKE_SPEED_VAR);
      smoke[index].zvel = 0.0;
      smoke[index].r = smokeR;
      smoke[index].g = smokeG;
      smoke[index].b = smokeB;
      smoke[index].alive = (int) gaussianRandom(SMOKE_PARTICLE_LIFETIME_MEAN, SMOKE_PARTICLE_LIFETIME_STD);
      smoke[index].mass = SMOKE_PARTICLE_MASS;
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
  // Draw the fountain
  for (index = 0; index < noOfWaterdrops; index++) 
  {
    glColor3f(water[index].r , water[index].g, water[index].b);
    glVertex3f(water[index].xpos, water[index].ypos, water[index].zpos);
  }

  // Draw the smoke
  for (index = 0; index < noOfSmokeParticles; index++) 
  {
    glColor3f(smoke[index].r , smoke[index].g, smoke[index].b);
    glVertex3f(smoke[index].xpos, smoke[index].ypos, smoke[index].zpos);
  }
  glEnd();
}


/*********************************************************************
* Update the display
**********************************************************************/
void progressTime() 
{
  int index;
  double shadeChange;

  // Update each water particle parameters. Water particles maintain
  // their X and Z speeds while the vertical keeps being modified due to
  // gravity
  for (index = 0; index < noOfWaterdrops; index++) 
  {
    water[index].xpos += water[index].xvel;
    water[index].ypos += water[index].yvel;
    water[index].zpos += water[index].zvel;
    water[index].yvel += water[index].mass * gravity;
    // if particle falls below the fountain position it is killed
    if (water[index].ypos < FOUNTAIN_Y || water[index].ypos > WINDOW_HEIGHT)
      water[index].alive = 0;
  }

  // Update each smoke particle parameters. 
  for (index = 0; index < noOfSmokeParticles; index++) 
  {
    smoke[index].xpos += smoke[index].xvel;
    smoke[index].ypos += smoke[index].yvel;
    smoke[index].zpos += smoke[index].zvel;
    // Apart from minor gravitational force each particle has some chaotic 
    // movement in every dimension
    smoke[index].xvel += gaussianRandom(SMOKE_CHAOS_SPEED_MEAN, chaoticSpeed);
    smoke[index].yvel += smoke[index].mass * gravity + gaussianRandom(SMOKE_CHAOS_SPEED_MEAN, chaoticSpeed);
    smoke[index].zvel += gaussianRandom(SMOKE_CHAOS_SPEED_MEAN, chaoticSpeed);
    // Each particle fades away at slighlty different pace
    shadeChange = gaussianRandom(SMOKE_SHADE_CHANGE_MEAN, SMOKE_SHADE_CHANGE_VAR);
    smoke[index].r -= shadeChange;
    smoke[index].g -= shadeChange;
    smoke[index].b -= shadeChange;
    // Count down to particle death, he he 
    smoke[index].alive -= 1;
    // If smoke hits the ground, make it crawl on it
    if (smoke[index].ypos < SMOKE_EMITTER_Y)
      smoke[index].ypos = SMOKE_EMITTER_Y;
  }
}



/*********************************************************************
* Key definitions for controlling the simulation of particle system
**********************************************************************/
void keyboard(unsigned char key, int x, int y)
{
  switch(key) 
  {
    case 27: exit(0); break;
    case 'f': gravity += 0.01; break;
    case 'F': gravity -= 0.01; break; 
    case 'c': if (chaoticSpeed - 0.001 > 0.0)
                chaoticSpeed -= 0.001;
              break; 
    case 'C': chaoticSpeed += 0.001; break; 
    case 'w': if (noOfWaterdrops / 2 >= 1)
                noOfWaterdrops /= 2; 
              break;
    case 'W': noOfWaterdrops *= 2; break;
    case 's': if (noOfSmokeParticles / 2 >= 1)
                noOfSmokeParticles /= 2; 
              break;
    case 'S': noOfSmokeParticles *= 2; break;
    case 'r': if (smokeR - 0.05 >= 0.0)
                smokeR -= 0.05; 
              break;
    case 'R': if (smokeR + 0.05 <= 1.0)
                smokeR += 0.05; 
              break;
    case 'g': if (smokeG - 0.05 >= 0.0)
                smokeG -= 0.05; 
              break;
    case 'G': if (smokeG + 0.05 <= 1.0)
                smokeG += 0.05; 
              break;
    case 'b': if (smokeB - 0.05 >= 0.0)
                smokeB -= 0.05; 
              break;
    case 'B': if (smokeB - 0.05 >= 0.0)
                smokeB -= 0.05; 
              break;
    case 'e': gravity = DEFAULT_GRAVITY;
              noOfWaterdrops = DEFAULT_NO_OF_WATERDROPS;
              noOfSmokeParticles = DEFAULT_NO_OF_SMOKE_PARTICLES;
              chaoticSpeed = SMOKE_CHAOS_SPEED_VAR;
              smokeR = smokeG = smokeB = SMOKE_SHADE;
              break;

  }
  glutPostRedisplay();
}


/*********************************************************************
* Reshape function called whenever the application window is reshaped
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
* Return uniformly distributed random double within range [-range,range]
**********************************************************************/
double uniformRandom(double range)
{
  return (rand() / (double)RAND_MAX * 2.0 - 1.0) * range; 
}


/*********************************************************************
* Return gaussian random variable with mean "mean" and standard 
* deviation "stdDev". Uses to uniform random variables for generation of
* a normally distributed one
**********************************************************************/
double gaussianRandom(double mean, double stdDev)
{
  double uniform1, uniform2, w;
 
  do {
    uniform1 = uniformRandom(1.0);
    uniform2 = uniformRandom(1.0);
    w = uniform1 * uniform1 + uniform2 * uniform2;
  } while ( w >= 1.0 );

  w = sqrt((-2.0 * log(w)) / w);
  return uniform1 * w * stdDev + mean;
}


/*********************************************************************
* Calculate the number of frames per second
**********************************************************************/
void calculateFPS()
{
  //  Increase frame count
  frameCount++;

  //  Get the number of milliseconds since glutInit called (or first call
  //  to glutGet(GLUT ELAPSED TIME)) and calculate time passed
  currentTime = glutGet(GLUT_ELAPSED_TIME);
  int timeInterval = currentTime - previousTime;

  if(timeInterval > 1000)
  {
    fps = frameCount / (timeInterval / 1000.0f);  //  calculate the number of frames per second
    previousTime = currentTime;                   //  Set time
    frameCount = 0;                               //  Reset frame count       
  }
}


/*********************************************************************
* Draw string ’str’ in font ’font’, at world (x,y,0)
**********************************************************************/
void drawString (void* font, float x, float y, char* str) 
{
  char* ch;
  glRasterPos3f(x, y, 0.0);
  for (ch = str; *ch; ch++)
    glutBitmapCharacter(font, (int) *ch);
}


