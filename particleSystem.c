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
  initParticleSystem();
  initGraphics(argc, argv);
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
  glEnable(GL_POINT_SMOOTH);
  glPointSize(POINT_SIZE);
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
  displayData();
  glutSwapBuffers();                        // Double buffering in place
}


/*********************************************************************
* Initialise the particle systems
**********************************************************************/
void initParticleSystem()
{
  // Set the initial values of particle system parameters
  fountain.totalParticles = DEFAULT_NO_OF_PARTICLES;
  fountain.aliveParticles = 0;
  smokeEmitter.totalParticles = DEFAULT_NO_OF_PARTICLES;
  smokeEmitter.aliveParticles = 0;
  smokeEmitter.r = smokeEmitter.g = smokeEmitter.b = SMOKE_SHADE;
  smokeEmitter.chaoticSpeed = SMOKE_CHAOS_SPEED_VAR;
}


/*********************************************************************
* Spawn particles
**********************************************************************/
void spawnParticles() 
{
  int index;

  for (index = fountain.aliveParticles; index < fountain.totalParticles; index++) 
  {
    fountain.particles[index].xpos = FOUNTAIN_X;
    fountain.particles[index].ypos = FOUNTAIN_Y;
    fountain.particles[index].zpos = FOUNTAIN_Z;
    fountain.particles[index].xvel = gaussianRandom(0.0, WATER_SIDE_SPLASH_VAR);
    fountain.particles[index].zvel = boxMuller2Rand;
    fountain.particles[index].yvel = gaussianRandom(WATER_SPEED_MEAN, WATER_SPEED_VAR);
    fountain.aliveParticles++;

  }

  for (index = smokeEmitter.aliveParticles; index < smokeEmitter.totalParticles; index++) 
  {
    smokeEmitter.particles[index].xpos = uniformRandom(SMOKE_EMITTER_SIZE) + SMOKE_EMITTER_X;
    smokeEmitter.particles[index].ypos = SMOKE_EMITTER_Y;
    smokeEmitter.particles[index].zpos = uniformRandom(SMOKE_EMITTER_SIZE) + SMOKE_EMITTER_Z;
    smokeEmitter.particles[index].xvel = 0.0;
    smokeEmitter.particles[index].yvel = gaussianRandom(SMOKE_SPEED_MEAN, SMOKE_SPEED_VAR);
    smokeEmitter.particles[index].zvel = 0.0;
    smokeEmitter.particles[index].r = smokeEmitter.r;
    smokeEmitter.particles[index].g = smokeEmitter.g;
    smokeEmitter.particles[index].b = smokeEmitter.b;
    smokeEmitter.aliveParticles++;
  }
}


/*********************************************************************
* Render the particles
**********************************************************************/
void drawParticles() 
{
  int index;

  /*-----------------------------------------------------------------*/
  #if RENDERING_METHOD == 1

  glBegin (GL_POINTS);
  // Draw the fountain
  glColor3f(WATER_DROP_COLOUR_R , WATER_DROP_COLOUR_G, WATER_DROP_COLOUR_B);
  for (index = 0; index < fountain.aliveParticles; index++) 
    glVertex3f(fountain.particles[index].xpos, fountain.particles[index].ypos, fountain.particles[index].zpos);

  // Draw the smoke
  for (index = 0; index < smokeEmitter.aliveParticles; index++) 
  {
    glColor3f(smokeEmitter.particles[index].r, smokeEmitter.particles[index].g, smokeEmitter.particles[index].b);
    glVertex3f(smokeEmitter.particles[index].xpos, smokeEmitter.particles[index].ypos, smokeEmitter.particles[index].zpos);
  }
  glEnd();


  /*-----------------------------------------------------------------*/
  #elif RENDERING_METHOD == 2

  glBegin(GL_LINES);
  // Draw the fountain
  glColor3f(WATER_DROP_COLOUR_R , WATER_DROP_COLOUR_G, WATER_DROP_COLOUR_B);
  for (index = 0; index < fountain.aliveParticles; index++) 
  {
    glVertex3f(fountain.particles[index].xpos, fountain.particles[index].ypos, fountain.particles[index].zpos);
    glVertex3f(fountain.particles[index].xpos + fountain.particles[index].xvel, 
               fountain.particles[index].ypos + fountain.particles[index].yvel, 
               fountain.particles[index].zpos + fountain.particles[index].zvel);
  }

  // Draw the smoke
  for (index = 0; index < smokeEmitter.aliveParticles; index++) 
  {
    glColor3f(smokeEmitter.particles[index].r, smokeEmitter.particles[index].g, smokeEmitter.particles[index].b);
    glVertex3f(smokeEmitter.particles[index].xpos, smokeEmitter.particles[index].ypos, smokeEmitter.particles[index].zpos);
    glVertex3f(smokeEmitter.particles[index].xpos + smokeEmitter.particles[index].xvel, 
               smokeEmitter.particles[index].ypos + smokeEmitter.particles[index].yvel, 
               smokeEmitter.particles[index].zpos + smokeEmitter.particles[index].zvel);
  }
  glEnd();
  #endif

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
  for (index = 0; index < fountain.aliveParticles; index++) 
  {
    // if particle falls below the fountain position it is killed
    if (fountain.particles[index].ypos < FOUNTAIN_Y || fountain.particles[index].ypos > WINDOW_HEIGHT) {
      fountain.particles[index] = fountain.particles[fountain.aliveParticles - 1];
      fountain.aliveParticles--;
    }
    
    fountain.particles[index].xpos += fountain.particles[index].xvel;
    fountain.particles[index].ypos += fountain.particles[index].yvel;
    fountain.particles[index].zpos += fountain.particles[index].zvel;
    fountain.particles[index].yvel += WATER_DROP_MASS * gravity;
  }

  // Update each smoke particle parameters. 
  for (index = 0; index < smokeEmitter.aliveParticles; index++) 
  {

    if (smokeEmitter.particles[index].r <= SMOKE_DEATH_COLOUR_THRES && 
      smokeEmitter.particles[index].g <= SMOKE_DEATH_COLOUR_THRES &&
      smokeEmitter.particles[index].b <= SMOKE_DEATH_COLOUR_THRES) {

      smokeEmitter.particles[index] = smokeEmitter.particles[smokeEmitter.aliveParticles - 1];
      smokeEmitter.aliveParticles--;
    }
    else {
    
      smokeEmitter.particles[index].xpos += smokeEmitter.particles[index].xvel;
      smokeEmitter.particles[index].ypos += smokeEmitter.particles[index].yvel;
      // If smoke hits the ground, make it crawl on it
      if (smokeEmitter.particles[index].ypos < SMOKE_EMITTER_Y)
        smokeEmitter.particles[index].ypos = SMOKE_EMITTER_Y;
      
      smokeEmitter.particles[index].zpos += smokeEmitter.particles[index].zvel;
      // Apart from minor gravitational force each particle has some chaotic 
      // movement in every dimension
      smokeEmitter.particles[index].xvel += gaussianRandom(SMOKE_CHAOS_SPEED_MEAN, smokeEmitter.chaoticSpeed);
      smokeEmitter.particles[index].zvel += boxMuller2Rand;
      smokeEmitter.particles[index].yvel += SMOKE_PARTICLE_MASS * gravity + gaussianRandom(SMOKE_CHAOS_SPEED_MEAN, smokeEmitter.chaoticSpeed * 2.0);
      
      // Each particle fades away at slighlty different pace
      shadeChange = gaussianRandom(SMOKE_SHADE_CHANGE_MEAN, SMOKE_SHADE_CHANGE_VAR);
      smokeEmitter.particles[index].r -= shadeChange;
      smokeEmitter.particles[index].g -= shadeChange;
      smokeEmitter.particles[index].b -= shadeChange;
    }
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
    case 'f': gravity *= DECREMENT_MULTIPLIER; break;
    case 'F': gravity *= INCREMENT_MULTIPLIER; break; 
    case 'c': smokeEmitter.chaoticSpeed *= 0.9;
              break; 
    case 'C': smokeEmitter.chaoticSpeed *= INCREMENT_MULTIPLIER; break; 
    case 'w': if (fountain.totalParticles / 2 >= 1)
                fountain.totalParticles /= 2; 
              break;
    case 'W': fountain.totalParticles *= 2; break;
    case 's': if (smokeEmitter.totalParticles / 2 >= 1)
                smokeEmitter.totalParticles /= 2; 
              break;
    case 'S': smokeEmitter.totalParticles *= 2; break;
    case 'r': if (smokeEmitter.r - SMOKE_COLOUR_CHANGE >= 0.0)
                smokeEmitter.r -= SMOKE_COLOUR_CHANGE; 
              break;
    case 'R': if (smokeEmitter.r + SMOKE_COLOUR_CHANGE <= 1.0)
                smokeEmitter.r += SMOKE_COLOUR_CHANGE; 
              break;
    case 'g': if (smokeEmitter.g - SMOKE_COLOUR_CHANGE >= 0.0)
                smokeEmitter.g -= SMOKE_COLOUR_CHANGE; 
              break;
    case 'G': if (smokeEmitter.g + SMOKE_COLOUR_CHANGE <= 1.0)
                smokeEmitter.g += SMOKE_COLOUR_CHANGE; 
              break;
    case 'b': if (smokeEmitter.b - SMOKE_COLOUR_CHANGE >= 0.0)
                smokeEmitter.b -= SMOKE_COLOUR_CHANGE; 
              break;
    case 'B': if (smokeEmitter.b - SMOKE_COLOUR_CHANGE >= 0.0)
                smokeEmitter.b -= SMOKE_COLOUR_CHANGE; 
              break;
    case 'e': gravity = DEFAULT_GRAVITY;
              initParticleSystem();
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
* deviation "stdDev". Uses two uniform random variables for generation of
* a normally distributed one (Box-Muller transform)
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
  boxMuller2Rand = uniform2 * w * stdDev + mean;
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
* Display important characteristics of the simulation
**********************************************************************/
void displayData(void) 
{
  glColor3f(1.0, 1.0, 1.0);
  sprintf(stringBuffer, "FPS: %.2f", fps);
  drawString(GLUT_BITMAP_HELVETICA_12, TEXT_X, TEXT_Y, stringBuffer);
  sprintf(stringBuffer, "Water particles: %d", fountain.totalParticles);
  drawString(GLUT_BITMAP_HELVETICA_12, TEXT_X, TEXT_Y - 1 * 12, stringBuffer);
  sprintf(stringBuffer, "Smoke particles: %d", smokeEmitter.totalParticles);
  drawString(GLUT_BITMAP_HELVETICA_12, TEXT_X, TEXT_Y - 2 * 12, stringBuffer);
  sprintf(stringBuffer, "Gravity: %.2f", gravity);
  drawString(GLUT_BITMAP_HELVETICA_12, TEXT_X, TEXT_Y - 3 * 12, stringBuffer);
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


