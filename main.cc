#define WIN_WIDTH 64
#define WIN_HEIGHT 32

#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstdlib>

#include "chip8.h"

using namespace std;

chip8 c8;
int scale = 10;

static void error_callback(int error, const char* description) {
  fputs(description, stderr);
}

int main(int argc, char **argv) {
  GLFWwindow* window;
  glfwSetErrorCallback(error_callback);
  if(!glfwInit())
    exit(EXIT_FAILURE);
  window = glfwCreateWindow(WIN_WIDTH*scale, WIN_HEIGHT*scale, "Chip8-Emulator", NULL, NULL);
  if(!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(window);

  //initialize c8 and load game
  if(argc < 2) {
    printf("Usage: ./chip8 *.rom\n");
    return 0;
  }
  if(!c8.loadApp(argv[1]))
    return 0;
  printf("Game loaded.\n");

  //loop
  float ratiox = 2.0f/WIN_WIDTH;
  float ratioy = 2.0f/WIN_HEIGHT;
  while(!glfwWindowShouldClose(window)) {
    c8.emulateCycle();
    //int width, height;
    //glfwGetFramebufferSize(window, &width, &height);
    //glClearColor(0,0,0,1);
    if(!c8.draw)
      continue;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    for(int y = 0; y <= WIN_HEIGHT; y++) {
      for(int x = 0; x <= WIN_WIDTH; x++) {
        if(c8.gfx[(y*64)+x] == 0)
          glColor3f(0.0f,0.0f,0.0f);
        else
          glColor3f(1.0f,1.0f,1.0f);

        glBegin(GL_QUADS);
        glVertex3f((x*ratiox)-1.0f, 1.0f-(y*ratioy), 0.0f);
        glVertex3f((x*ratiox)-1.0f, 1.0f-(y*ratioy)-ratioy, 0.0f);
        glVertex3f((x*ratiox)+ratiox-1.0f, 1.0f-(y*ratioy)-ratioy, 0.0f);
        glVertex3f((x*ratiox)+ratiox-1.0f, 1.0f-(y*ratioy)-0.0f, 0.0f);
        glEnd();
      }
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
