/******************************************************************************
* File:         particleSystem.c
* Brief:        Particle system implementation - fountain and smoke
* Author:       Krzysztof Koch  
* Date created: 04/10/2016
* Last mod:     11/11/2016
*
* Note:         
* Two particle systems implemented, water fountain and smoke emitter. Live particles
* are kept at the beginning of the array which stores them all. There are two 
* separate data structures for water and smoke, as well as two for smoke and water
* particles. It's because of different properties they have.
*
* Two rendering options are available (by changing the value of RENDERING_METHOD)
*   1. Particles are drawn as points
*   2. Water movement is drawn using lines, while smoke is rendered using point
*   textures.
*       
******************************************************************************/
#include "particleSystem.h"



/******************************************************************************
* Main method
******************************************************************************/
int main(int argc, char *argv[])
{
  srand(time(NULL));
  initParticleSystem();
  initGraphics(argc, argv);
  glutMainLoop();
}



/******************************************************************************
* Initialisation function
******************************************************************************/
void initGraphics(int argc, char *argv[])
{
  int index;
  glutInit(&argc, argv);
  glutInitWindowPosition(100, 100);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
  glutCreateWindow("Particle system");
  glutFullScreen();
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(cursor_keys);
  glutReshapeFunc(reshape);
  createMenu();

  // Render points as circles and make them span a few pixels instead of one
  glEnable(GL_POINT_SMOOTH);
  glPointSize(POINT_SIZE);

  /*--------------------------------------------------------------------------
  * Setup for the smoke rendering method using textures
  *-------------------------------------------------------------------------*/
  #if RENDERING_METHOD == 2

    // Make points very large (in pixel terms), set the blending funcion
    glPointSize(POINT_SIZE_TEXTURE);
    // Render antialiased points and lines in arbitrary order, pixel aithmetic
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    //  Specify the drawing mode for point sprites
    glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    // Enable point sprites and 2D textures
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_POINT_SPRITE);

    // Load textures one by one
    for (index = 0; index < SMOKE_TEXTURE_NUMBER; index++) {
      sprintf(stringBuffer, "Textures/smoke%d.png", index);
      smokeEmitter.textures[index] = SOIL_load_OGL_texture(stringBuffer, 
                                                           SOIL_LOAD_RGBA, 
                                                           SOIL_CREATE_NEW_ID, 
                                                           SOIL_FLAG_MIPMAPS);
    }
  #endif
}



/******************************************************************************
* Callback function, called whenever graphics should be redrawn
******************************************************************************/
void display()
{
  setView();
  glClear(GL_COLOR_BUFFER_BIT);         // Clear the screen and depth buffer
  spawnParticles();                     // Generate new particles to replace dead ones
  drawParticles();                      // Render particles
  progressTime();                       // Update particle coordinates and properties
  glutPostRedisplay();                  // Mark the current window to be redisplayed
  calculateFPS();                       // Calculate the frame rate
  
  if (currentView == &DEFAULT_VEW)      // Display simulation parameter values
    displayData();  

  glutSwapBuffers();                    // Double buffering in place
}



/******************************************************************************
* Initialise the particle systems
******************************************************************************/
void initParticleSystem()
{
  // Set the initial values of particle system parameters
  fountain.totalParticles = DEFAULT_NO_OF_PARTICLES;
  fountain.aliveParticles = 0;
  smokeEmitter.totalParticles = DEFAULT_NO_OF_PARTICLES;
  smokeEmitter.aliveParticles = 0;
  smokeEmitter.r = smokeEmitter.g = smokeEmitter.b = SMOKE_SHADE;
  smokeEmitter.chaoticSpeed = SMOKE_CHAOS_SPEED_VAR;
  gravity = DEFAULT_GRAVITY;
  currentView = &DEFAULT_VEW;
  windSpeed = SMOKE_WIND_INIT_SPEED;
  angle = SMOKE_WIND_INIT_DIRECTION;
  computeWind();
}



/******************************************************************************
* Spawn particles
******************************************************************************/
void spawnParticles() 
{
  // Dead particles are stored at the end of array, thus no need for 'alive'
  // parameter for each particle
  int index;

  // Spawn water particles with different horizontal speeds (side splash) and 
  // different vertical speeds. Particles are generated from a single point being
  // the fountain location
  for (index = fountain.aliveParticles; index < fountain.totalParticles; index++) 
  {
    fountain.particles[index].xpos = WATER_FOUNTAIN_X;
    fountain.particles[index].ypos = WATER_FOUNTAIN_Y;
    fountain.particles[index].zpos = WATER_FOUNTAIN_Z;
    fountain.particles[index].xvel = gaussianRandom(0.0, WATER_SIDE_SPLASH_VAR);
    fountain.particles[index].zvel = boxMuller2Rand;
    fountain.particles[index].yvel = gaussianRandom(WATER_SPEED_MEAN, WATER_SPEED_VAR);
    fountain.aliveParticles++;

  }

  // Spawn smoke particles with only vertical speed being nonzero. Set their initial colour
  // according to the current value of colour parameters (with some random noise). Particles 
  // are generated from a square area with linear distribution. 
  for (index = smokeEmitter.aliveParticles; index < smokeEmitter.totalParticles; index++) 
  {
    smokeEmitter.particles[index].xpos = uniformRandom(SMOKE_EMITTER_SIZE) + SMOKE_EMITTER_X;
    smokeEmitter.particles[index].ypos = SMOKE_EMITTER_Y;
    smokeEmitter.particles[index].zpos = uniformRandom(SMOKE_EMITTER_SIZE) + SMOKE_EMITTER_Z;
    smokeEmitter.particles[index].xvel = 0.0;
    smokeEmitter.particles[index].yvel = gaussianRandom(SMOKE_SPEED_MEAN, SMOKE_SPEED_VAR);
    smokeEmitter.particles[index].zvel = 0.0;
    smokeEmitter.particles[index].r = gaussianRandom(smokeEmitter.r, SMOKE_SHADE_INIT_VAR);
    smokeEmitter.particles[index].g = gaussianRandom(smokeEmitter.g, SMOKE_SHADE_INIT_VAR);
    smokeEmitter.particles[index].b = gaussianRandom(smokeEmitter.b, SMOKE_SHADE_INIT_VAR);
    smokeEmitter.aliveParticles++;
    smokeEmitter.particles[index].alpha = gaussianRandom(SMOKE_INIT_ALHPA_MEAN, SMOKE_INIT_ALPHA_VAR);
    smokeEmitter.particles[index].textureID = smokeEmitter.textures[index % SMOKE_TEXTURE_NUMBER];
  }
}



/******************************************************************************
* Render the particles
******************************************************************************/
void drawParticles() 
{
  int index;
  int textureID;

  /*--------------------------------------------------------------------------
  * Particles as points
  *-------------------------------------------------------------------------*/
  #if RENDERING_METHOD == 1

    // Draw the fountain
    glBegin (GL_POINTS);
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

  /*--------------------------------------------------------------------------
  * Water as lines, smoke as point textures
  *-------------------------------------------------------------------------*/
  #else
    // Draw the fountain, join the current position and the future one (determined
    // by velocity vector by a line)
    glBegin(GL_LINES);
    glColor3f(WATER_DROP_COLOUR_R , WATER_DROP_COLOUR_G, WATER_DROP_COLOUR_B);
    for (index = 0; index < fountain.aliveParticles; index++) 
    {
      glVertex3f(fountain.particles[index].xpos, fountain.particles[index].ypos, fountain.particles[index].zpos);
      glVertex3f(fountain.particles[index].xpos + fountain.particles[index].xvel, 
                 fountain.particles[index].ypos + fountain.particles[index].yvel, 
                 fountain.particles[index].zpos + fountain.particles[index].zvel);
    }
    glEnd();


    // Draw the smoke. Load the texture depending on particle index, and draw the 
    // texture with alpha blending
    glEnable(GL_POINT_SPRITE);
    glEnable(GL_TEXTURE_2D);
    for (index = 0; index < smokeEmitter.aliveParticles; index++) 
    {
      glBindTexture(GL_TEXTURE_2D, smokeEmitter.particles[index].textureID);
      glBegin (GL_POINTS);
      glColor4f(smokeEmitter.particles[index].r, smokeEmitter.particles[index].g, smokeEmitter.particles[index].b, smokeEmitter.particles[index].alpha);
      glVertex3f(smokeEmitter.particles[index].xpos, smokeEmitter.particles[index].ypos, smokeEmitter.particles[index].zpos);
      glEnd();
    }
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_POINT_SPRITE);

  #endif
}



/******************************************************************************
* Update the display
******************************************************************************/
void progressTime() 
{
  int index;
  double shadeChange;

  // Update each water particle parameters. Water particles maintain
  // their X and Z speeds while the vertical keeps being modified due to
  // gravity
  for (index = 0; index < fountain.aliveParticles; index++) 
  {
    // if particle falls below the fountain Y coordinate it is killed
    if (fountain.particles[index].ypos < WATER_FOUNTAIN_Y || fountain.particles[index].ypos > WINDOW_HEIGHT) {
      fountain.particles[index] = fountain.particles[fountain.aliveParticles - 1];
      fountain.aliveParticles--;
    }
    // Move the particle
    fountain.particles[index].xpos += fountain.particles[index].xvel;
    fountain.particles[index].ypos += fountain.particles[index].yvel;
    fountain.particles[index].zpos += fountain.particles[index].zvel;
    fountain.particles[index].yvel += WATER_DROP_MASS * gravity;
  }

  // Update each smoke particle parameters. 
  for (index = 0; index < smokeEmitter.aliveParticles; index++) 
  {
    // if the particle has faded out, kill it
    if ((smokeEmitter.particles[index].r <= SMOKE_DEATH_THRES && 
      smokeEmitter.particles[index].g <= SMOKE_DEATH_THRES &&
      smokeEmitter.particles[index].b <= SMOKE_DEATH_THRES) || 
      smokeEmitter.particles[index].alpha <= SMOKE_DEATH_THRES) {
      smokeEmitter.particles[index] = smokeEmitter.particles[smokeEmitter.aliveParticles - 1];
      smokeEmitter.aliveParticles--;
    }

    // Otherwise
    else {
      // Move the particle. If smoke hits the ground, make it crawl on it
      smokeEmitter.particles[index].xpos += smokeEmitter.particles[index].xvel;
      smokeEmitter.particles[index].zpos += smokeEmitter.particles[index].zvel;
      smokeEmitter.particles[index].ypos += smokeEmitter.particles[index].yvel;
      if (smokeEmitter.particles[index].ypos < SMOKE_EMITTER_Y)
        smokeEmitter.particles[index].ypos = SMOKE_EMITTER_Y;
  
      // Apart from minor gravitational force each particle has some chaotic 
      // movement in every dimension and is affected by the wind (direction and speed)
      // The vertical chaotic movement is slighlty faster than horizontal one
      smokeEmitter.particles[index].xvel += gaussianRandom(SMOKE_CHAOS_SPEED_MEAN, smokeEmitter.chaoticSpeed) +
                                            smokeEmitter.particles[index].ypos * xWind;;
      smokeEmitter.particles[index].zvel += boxMuller2Rand + smokeEmitter.particles[index].ypos * zWind;
      smokeEmitter.particles[index].yvel += SMOKE_PARTICLE_MASS * gravity + gaussianRandom(SMOKE_CHAOS_SPEED_MEAN, 
                                            smokeEmitter.chaoticSpeed * SMOKE_CHAOS_VERTICAL_MUL);
      
      // Each particle fades away at slighlty different pace. It both becomes 
      // darker and more transparent
      shadeChange = gaussianRandom(SMOKE_SHADE_CHANGE_MEAN, SMOKE_SHADE_CHANGE_VAR);
      smokeEmitter.particles[index].r -= shadeChange;
      smokeEmitter.particles[index].g -= shadeChange;
      smokeEmitter.particles[index].b -= shadeChange;
      smokeEmitter.particles[index].alpha -= SMOKE_ALPHA_CHANGE;
    }
  }
}



/******************************************************************************
* Interactive control of the environment using standard keyboard keys
******************************************************************************/
void keyboard(unsigned char key, int x, int y)
{
  switch(key) 
  {
    // Quit the program
    case 27: exit(0); break;
    
    // Decrease and increase the speed of smoke particle movement
    case 'c': smokeEmitter.chaoticSpeed *= DECREASE_VAL;
              break; 
    case 'C': smokeEmitter.chaoticSpeed *= INCREASE_VAL; break; 
    
    // Decrease and Increase water particle number
    case 'f': if (fountain.totalParticles / 2 >= 1)
                fountain.totalParticles /= 2; 
              break;
    case 'F': fountain.totalParticles *= 2; break;
    
    // Decrease and Increase smoke particle number
    case 's': if (smokeEmitter.totalParticles / 2 >= 1)
                smokeEmitter.totalParticles /= 2; 
              break;
    case 'S': smokeEmitter.totalParticles *= 2; break;

    // Decrease and increase the starting red colour component of smoke particle
    case 'r': if (smokeEmitter.r - SMOKE_COLOUR_CHANGE >= 0.0)
                smokeEmitter.r -= SMOKE_COLOUR_CHANGE; 
              break;
    case 'R': if (smokeEmitter.r + SMOKE_COLOUR_CHANGE <= 1.0)
                smokeEmitter.r += SMOKE_COLOUR_CHANGE; 
              break;

    // Decrease and increase the starting green colour component of smoke particle
    case 'g': if (smokeEmitter.g - SMOKE_COLOUR_CHANGE >= 0.0)
                smokeEmitter.g -= SMOKE_COLOUR_CHANGE; 
              break;
    case 'G': if (smokeEmitter.g + SMOKE_COLOUR_CHANGE <= 1.0)
                smokeEmitter.g += SMOKE_COLOUR_CHANGE; 
              break;
    
    // Decrease and increase the starting blue colour component of smoke particle
    case 'b': if (smokeEmitter.b - SMOKE_COLOUR_CHANGE >= 0.0)
                smokeEmitter.b -= SMOKE_COLOUR_CHANGE; 
              break;
    case 'B': if (smokeEmitter.b + SMOKE_COLOUR_CHANGE <= 1.0)
                smokeEmitter.b += SMOKE_COLOUR_CHANGE; 
              break;

    // Decrease and increase the wind speed
    case 'w': windSpeed *= DECREASE_VAL; 
              computeWind();
              break;
    case 'W': windSpeed *= INCREASE_VAL; 
              computeWind();
              break;
  }
  glutPostRedisplay();
}



/******************************************************************************
* Interactive control of  the enviroment using special keys
******************************************************************************/
void cursor_keys(int key, int x, int y) 
{
  switch (key) {
    
    // Increase and decrease gravitational force
    case GLUT_KEY_UP: gravity *= DECREASE_VAL; break;
    case GLUT_KEY_DOWN: gravity *= INCREASE_VAL; break; 

    // Change the wind direction by WIND_DIRECTION_CHANGE degrees
    case GLUT_KEY_LEFT: 
      angle += SMOKE_WIND_DIRECTION_CHANGE % 360; 
      computeWind();
      break;

    case GLUT_KEY_RIGHT: 
      angle -= SMOKE_WIND_DIRECTION_CHANGE % 360; 
      computeWind();
      break;
  }
} // cursor_keys()



/******************************************************************************
* Create manu interface
******************************************************************************/
void createMenu(void) {
  glutCreateMenu (menu);
  glutAddMenuEntry ("Reset simulation", 1);
  glutAddMenuEntry ("", 999);
  glutAddMenuEntry ("Default view", 2);
  glutAddMenuEntry ("Fountain view", 3);
  glutAddMenuEntry ("Smoke view", 4);
  glutAddMenuEntry ("Fountain top view", 5);
  glutAddMenuEntry ("Smoke top view", 6);
  glutAddMenuEntry ("", 999);
  glutAddMenuEntry ("Quit", 7);
  glutAttachMenu (GLUT_RIGHT_BUTTON);
}



/******************************************************************************
* Create menu entries for changing properties of the particle system
******************************************************************************/
void menu (int menuentry) {
  switch (menuentry) 
  {
    // Reset parameters to starting values
    case 1: initParticleSystem(); break;
    case 2: currentView = &DEFAULT_VEW; break;
    case 3: currentView = &FOUNTAIN_VIEW; break;
    case 4: currentView = &SMOKE_VIEW; break;
    case 5: currentView = &FOUNTAIN_TOP_VIEW; break;
    case 6: currentView = &SMOKE_TOP_VIEW; break;
    case 7: exit(0); 
  }
}



/******************************************************************************
* Reshape function called whenever the application window is reshaped
******************************************************************************/
void reshape(int width, int height)
{
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glViewport(0, 0, (GLsizei)width, (GLsizei)height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, (GLfloat)width / (GLfloat)height, 1.0, 10000.0);
  glMatrixMode(GL_MODELVIEW);
}



/******************************************************************************
* Implement various camera views
******************************************************************************/
void setView (void) 
{
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(currentView->eyeX, currentView->eyeY, currentView->eyeZ,  
            currentView->centerX, currentView->centerY, currentView->centerZ,  
            currentView->upX, currentView->upY, currentView->upZ); 
}



/******************************************************************************
* Return uniformly distributed random double within range [-range,range]
******************************************************************************/
double uniformRandom(double range)
{
  return (rand() / (double)RAND_MAX * 2.0 - 1.0) * range; 
}



/******************************************************************************
* Return gaussian random variable with mean "mean" and standard 
* deviation "stdDev". Uses two uniform random variables for generation of
* a normally distributed one (Box-Muller transform)
******************************************************************************/
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



/******************************************************************************
* Calculate the number of frames per second
******************************************************************************/
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
    // Calculate the number of frames per second, set the time and reset
    // the frame count
    fps = frameCount / (timeInterval / 1000.0f);  
    previousTime = currentTime;                   
    frameCount = 0;                         
  }
}



/******************************************************************************
* Display important characteristics of the simulation
******************************************************************************/
void displayData(void) 
{
  glColor3f(1.0, 1.0, 1.0);
  sprintf(stringBuffer, "FPS: %.2f", fps);
  drawString(GLUT_BITMAP_HELVETICA_12, TEXT_X, TEXT_Y, stringBuffer);
  sprintf(stringBuffer, "Water particles: %d", fountain.totalParticles);
  drawString(GLUT_BITMAP_HELVETICA_12, TEXT_X, TEXT_Y - 1 * FONT_HEIGHT, stringBuffer);
  sprintf(stringBuffer, "Smoke particles: %d", smokeEmitter.totalParticles);
  drawString(GLUT_BITMAP_HELVETICA_12, TEXT_X, TEXT_Y - 2 * FONT_HEIGHT, stringBuffer);
  sprintf(stringBuffer, "Gravity: %.2f m/s^2", gravity);
  drawString(GLUT_BITMAP_HELVETICA_12, TEXT_X, TEXT_Y - 3 * FONT_HEIGHT, stringBuffer);
  sprintf(stringBuffer, "Wind speed: %.2f m/s", windSpeed);
  drawString(GLUT_BITMAP_HELVETICA_12, TEXT_X, TEXT_Y - 4 * FONT_HEIGHT, stringBuffer);
}



/******************************************************************************
* Draw string ’str’ in font ’font’, at world (x,y,0)
******************************************************************************/
void drawString(void* font, float x, float y, char* str) 
{
  char* ch;
  glRasterPos3f(x, y, 0.0);
  for (ch = str; *ch; ch++)
    glutBitmapCharacter(font, (int) *ch);
}



/******************************************************************************
* Calculate the wind vector based on wind angle and speed
******************************************************************************/
void computeWind(void) 
{
  xWind = SMOKE_PARTICLE_MASS * windSpeed * cos(DEG_TO_RAD * angle) / 10;
  zWind = SMOKE_PARTICLE_MASS * windSpeed * sin(DEG_TO_RAD * angle) / 10;
}

