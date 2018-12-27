#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <GLFW/glfw3.h>

#include "chip8.h"

chip8::chip8() {}
chip8::~chip8() {}

bool chip8::loadApp(const char * filename) {
  initialize();
  printf("Loading: %s\n", filename);

  FILE * f = fopen(filename, "rb");
  if(f == NULL)
    return 0;

  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  rewind(f);
  printf("Size: %d\n", (int)size);
  // allocate memory
  char * buffer = (char*)malloc(sizeof(char) * size);
  if(buffer == NULL)
    return 0;
  // copy to buffer
  size_t res = fread(buffer, 1, size, f);
  if(res != size)
    return 0;
  // copy buffer to chip8 mem
  if((4096-512) > size) {
    for(int i = 0; i <= size; i++)
      memory[i+512] = buffer[i];
  } else {
    printf("ROM too big\n");
  }

  fclose(f);
  free(buffer);
  return 1;
}

void chip8::emulateCycle() {
  key[0] = glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_0);
  key[1] = glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_1);
  key[2] = glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_2);
  key[3] = glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_3);
  key[4] = glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_Q);
  key[5] = glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_W);
  key[6] = glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_E);
  key[7] = glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_A);
  key[8] = glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_S);
  key[9] = glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_D);
  key[10] = glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_Z);
  key[11] = glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_C);
  key[12] = glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_4);
  key[13] = glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_R);
  key[14] = glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_F);
  key[15] = glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_V);
  // fetch Opcode
  opcode = memory[pc] << 8 | memory[pc+1];
  //printf("%x\n",memory[pc]);
  //printf("%d\n",pc);
  // process Opcode
  draw = 0;
  switch(opcode & 0xF000) {
    case 0x0000:
      switch(opcode & 0x000F) {
        case 0x0000: // clear screen
          for(int i = 0; i <= 2048; i++)
            gfx[i] = 0;
          draw = 1;
          break;
        case 0x000E:
          pc = stack[--sp];
          pc += 2;
          break;
      }
      break;
    case 0x1000:
      pc = opcode & 0x0FFF;
      break;
    case 0x2000:
      stack[sp++] = pc;
      pc = opcode & 0x0FFF;
      break;
    case 0x3000:
      if(vreg[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
        pc += 2;
      pc += 2;
      break;
    case 0x4000:
      if(vreg[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
        pc += 2;
      pc += 2;
      break;
    case 0x5000:
      if(vreg[(opcode & 0x0F00) >> 8] == vreg[(opcode & 0x00F0) >> 4])
        pc += 2;
      pc += 2;
      break;
    case 0x6000:
      vreg[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
      pc += 2;
      break;
    case 0x7000:
      vreg[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
      pc += 2;
      break;
    case 0x8000:
      switch(opcode & 0x000F) {
        case 0x0000:
          vreg[(opcode & 0x0F00) >> 8] = vreg[(opcode & 0x00F0) >> 4];
          pc += 2;
          break;
        case 0x0001:
          vreg[(opcode & 0x0F00) >> 8] |= vreg[(opcode & 0x00F0) >> 4];
          pc += 2;
          break;
        case 0x0002:
          vreg[(opcode & 0x0F00) >> 8] &= vreg[(opcode & 0x00F0) >> 4];
          pc += 2;
          break;
        case 0x0003:
          vreg[(opcode & 0x0F00) >> 8] ^= vreg[(opcode & 0x00F0) >> 4];
          pc += 2;
          break;
        case 0x0004:
          if(vreg[(opcode & 0x0F00) >> 8] + vreg[(opcode & 0x00F0) >> 4] > 256)
            vreg[0xF] = 1;
          else
            vreg[0xF] = 0;
          vreg[(opcode & 0x0F00) >> 8] += vreg[(opcode & 0x00F0) >> 4];
          pc += 2;
          break;
        case 0x0005:
          if(vreg[(opcode & 0x0F00) >> 8] > vreg[(opcode & 0x00F0) >> 4])
            vreg[0xF] = 1;
          else
            vreg[0xF] = 0;
          vreg[(opcode & 0x0F00) >> 8] -= vreg[(opcode & 0x00F0) >> 4];
          pc += 2;
          break;
        case 0x0006:
          vreg[0xF] = vreg[(opcode & 0x0F00) >> 8] & 0x1;
          vreg[(opcode & 0x0F00) >> 8] /= 2;
          pc += 2;
          break;
        case 0x0007:
          if(vreg[(opcode & 0x00F0) >> 4] > vreg[(opcode & 0x0F00) >> 8])
            vreg[0xF] = 1;
          else
            vreg[0xF] = 0;
          vreg[(opcode & 0x0F00) >> 8] = vreg[(opcode & 0x00F0) >> 4] -
                                          vreg[(opcode & 0x0F00) >> 8];
          pc += 2;
          break;
        case 0x000E:
          vreg[0xF] = vreg[(opcode & 0x0F00) >> 8] >> 7;
          vreg[(opcode & 0x0F00) >> 8] /= 2;
          pc += 2;
          break;
      }
      break;
    case 0x9000:
      if(vreg[(opcode & 0x0F00) >> 8] != vreg[(opcode & 0x00F0) >> 4])
        pc += 2;
      pc += 2;
      break;
    case 0xA000:
      ir = opcode & 0x0FFF;
      pc += 2;
      break;
    case 0xB000:
      pc = (opcode & 0x0FFF) + vreg[0];
      break;
    case 0xC000:
      vreg[(opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (opcode & 0x00FF);
      pc += 2;
      break;
    case 0xD000: {
      unsigned short x = vreg[(opcode & 0x0F00) >> 8];
      unsigned short y = vreg[(opcode & 0x00F0) >> 4];
      unsigned short h = opcode & 0x000F;
      unsigned short p;

      vreg[0xF] = 0;
      for(int ly = 0; ly < h; ly++) {
        p = memory[ir + ly];
        for(int lx = 0; lx < 8; lx++) {
          if((p & (0x80 >> lx)) != 0) {
            if(gfx[(x + lx + ((y + ly) *64))] == 1)
              vreg[0xF] = 1;
            gfx[x + lx + ((y + ly) * 64)] ^= 1;
          }
        }
      }
      draw = 1;
      pc += 2;
    } break;
    case 0xE000: // keyboard
      switch(opcode & 0x00FF) {
        case 0x009E:
          if(!key[vreg[(opcode & 0x0F00) >> 8]])
            pc += 2;
          pc += 2;
          break;
        case 0x00A1:
          if(key[vreg[(opcode & 0x0F00) >> 8]])
            pc += 2;
          pc += 2;
          break;
      }
      break;
    case 0xF000:
      switch(opcode & 0x00FF) {
        case 0x0007:
          vreg[(opcode & 0x0F00) >> 8] = delay_timer;
          pc += 2;
          break;
        case 0x000A: {
          bool press = 0;
          for(int i = 0; i <= 16; i++) {
            if(key[i] != 0) {
              vreg[(opcode & 0x0F00) >> 8] = i;
              press = 1;
            }
          }
          if(!press)
            return;
          pc += 2;
        } break;
        case 0x0015:
          delay_timer = vreg[(opcode & 0x0F00) >> 8];
          pc += 2;
          break;
        case 0x0018:
          sound_timer = vreg[(opcode & 0x0F00) >> 8];
          pc += 2;
          break;
        case 0x001E:
          ir += vreg[(opcode & 0x0F00) >> 8];
          pc += 2;
          break;
        case 0x0029:
          ir = vreg[(opcode & 0x0F00) >> 8] * 0x5;
          pc += 2;
          break;
        case 0x0033:
          memory[ir] = vreg[(opcode & 0x0F00) >> 8] / 100;
          memory[ir+1] = (vreg[(opcode & 0x0F00) >> 8] / 10) % 10;
          memory[ir+2] = (vreg[(opcode & 0x0F00) >> 8] % 100) % 10;
          pc += 2;
          break;
        case 0x0055:
          for(int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
            memory[ir + i] = vreg[i];
          ir += ((opcode & 0x0F00) >> 8) + 1;
          pc += 2;
          break;
        case 0x0065:
          for(int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
            vreg[i] = memory[ir + i];
          ir += ((opcode & 0x0F00) >> 8) + 1;
          pc += 2;
          break;
      }
      break;
  }
  // update timers
  if(delay_timer > 0)
    delay_timer--;
  if(sound_timer > 0) {
    if(sound_timer == 1)
      std::cout << '\a';
    sound_timer--;
  }
}

void chip8::initialize() {
  pc = 0x200; // program counter starts at 0x200
  opcode = 0;
  ir = 0; // index register
  sp = 0; // stack pointer

  // clear display
  for(int i = 0; i <= 2048; i++)
    gfx[i] = 0;
  // clear stack
  for(int i = 0; i <= 16; i++)
    stack[i] = 0;
  // clear registers and keys
  for(int i = 0; i <= 16; i++)
    key[i] = vreg[i] = 0;
  // clear memory
  for(int i = 0; i <= 4096; i++)
    memory[i] = 0;

  // load fontset
  unsigned char fset[80] = {
    0xF0,0x90,0x90,0x90,0xF0, //0
    0x20,0x60,0x20,0x20,0x70, //1
		0xF0,0x10,0xF0,0x80,0xF0, //2
		0xF0,0x10,0xF0,0x10,0xF0, //3
		0x90,0x90,0xF0,0x10,0x10, //4
		0xF0,0x80,0xF0,0x10,0xF0, //5
		0xF0,0x80,0xF0,0x90,0xF0, //6
		0xF0,0x10,0x20,0x40,0x40, //7
		0xF0,0x90,0xF0,0x90,0xF0, //8
		0xF0,0x90,0xF0,0x10,0xF0, //9
		0xF0,0x90,0xF0,0x90,0x90, //A
		0xE0,0x90,0xE0,0x90,0xE0, //B
		0xF0,0x80,0x80,0x80,0xF0, //C
		0xE0,0x90,0x90,0x90,0xE0, //D
		0xF0,0x80,0xF0,0x80,0xF0, //E
    0xF0,0x80,0xF0,0x80,0x80 //F
  };
  int c = 80;
  for (char f : fset) {
    memory[c] = fset[c-80];
    c++;
  }

  // reset timers
  delay_timer = 0;
  sound_timer = 0;
  draw = 1;
}
