class chip8 {
public:
  chip8();
  ~chip8();

  bool draw;

  bool loadApp(const char * filename);

  void emulateCycle();

  bool gfx[64*32];
  bool key[16];

private:
  unsigned short pc;
  unsigned short opcode;
  unsigned short ir;
  unsigned short sp;

  unsigned char vreg[16];       // v-regs (v0-vf)
  unsigned short stack[16];
  unsigned char memory[4096];

  unsigned char delay_timer;
  unsigned char sound_timer;

  void initialize();
};
