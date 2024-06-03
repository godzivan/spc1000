/**
 * @file common.c
 * @brief SPC-1000 emulator config structure
 * @author Kue-Hwan Sihn (ionique)
 * @date 2005~2007.1
 */

#ifndef __SPC_COMMON_H__
#define __SPC_COMMON_H__

#define MAX_PATH_LENGTH 2048

#if WIN32
#include <Windows.h>
#endif

#include "z80_zazu.h"
#include "AY8910.h"

#include <stdio.h>

typedef unsigned int UINT32;
//enum tapmodes {TAP_GUIDE, TAP_DATA, TAP_PAUSE, TAP_TRASH, TAP_STOP, TAP_STOP2, TAP_PAUSE2, TZX_PURE_TONE,
//	TZX_SEQ_PULSES, TAP_FINAL_BIT};
enum taptypes {TAP_TAP, TAP_SPC};

typedef unsigned short word;

typedef struct
{
    // video variables
	unsigned char colorMode;
	unsigned char scanLine;
	unsigned char progressive; //interlace or progressive 576
	unsigned char dblscan;
	unsigned char frameRate;
	unsigned char zaurus_mini;
	unsigned char text_mini;
	unsigned char bw;
	unsigned char bpp;
	unsigned int  resx, resy;
	unsigned int  dispx, dispy;
	unsigned int  dx, dy;
	unsigned int  rw, rh;
	unsigned char fullscreen;

    // cassette variables
	FILE *wfp;
	FILE *rfp;
	FILE *tap_file;
	unsigned char casTurbo;
	unsigned char tape_fast_load; // 0 normal load; 1 fast load
	unsigned char rewind_on_reset;
	unsigned char tape_write; // 0 can't write; 1 can write
	unsigned char current_tap[MAX_PATH_LENGTH];
	char path_snaps[MAX_PATH_LENGTH];
	unsigned char last_selected_file[MAX_PATH_LENGTH];
	enum taptypes tape_file_type;
    // enum block_type next_block;
	unsigned char tape_stop; // 1=tape stop
	unsigned char tape_stop_fast; // 1=tape stop
    // unsigned char stop_tape_start_countdown; // 1=tape stop start countdown

    // sound variables
	unsigned char ay_emul;
	unsigned char snd_vol;

    // keyboard configuration
	unsigned char pcKeyboard;

	// model configuration
	unsigned char mode;
	unsigned char debug;
	unsigned char turbo;

	//Port variables
	unsigned char port; //SD, USB, SMB or FTP
	unsigned char smb_enable;
	unsigned char SmbUser[32];
	unsigned char SmbPwd[32];
	unsigned char SmbShare[32];
	unsigned char SmbIp[32];
	unsigned char ftp_enable;
	unsigned char FTPUser[32];
	unsigned char FTPPwd[32];
//	unsigned char FTPPath[MAX_PATH_LENGTH];
	unsigned char FTPIp[64];
	unsigned char FTPPassive;
	unsigned short FTPPort;
	char font_name[MAX_PATH_LENGTH];
	int font_size;

} SPCConfig;

/**
 * Cassette structure for tape processing, included in the SPCIO
 */
typedef struct
{
	int motor;			// Motor Status
	int pulse;			// Motor Pulse (0->1->0) causes motor state to flip
	int button;
	int rdVal;
	UINT32 lastTime;
	UINT32 cnt0, cnt1;

	int wrVal;
	UINT32 wrRisingT;	// rising time
} CassetteTape;

typedef union PC
{
    struct {
        unsigned char rDAV : 1;
        unsigned char rRFD : 1;
        unsigned char rDAC : 1;
        unsigned char rNON : 1;
        unsigned char wDAV : 1;
        unsigned char wRFD : 1;
        unsigned char wDAC : 1;
        unsigned char wATN : 1;
    } bits;
    unsigned char b;
} pc_t;

typedef struct
{
    byte rdVal;
    byte wrVal;
    byte rSize;
    byte seq;
    byte isCmd;
    byte cmd;
    byte data[6];
    pc_t PC;
    char diskfile[1024];
    char diskfile2[1024];
    char diskdata[80*16*256]; // 80 tracks 16 sectors 256 byte
    char diskdata2[80*16*256];
    byte modified;
    byte modified2;
    byte write;
    byte write2;
    byte *buffer;
    byte idx;
    int datasize;
    int dataidx;
} FloppyDisk;

typedef struct
{
    unsigned int length;
    byte bufs[1024*1024];
    byte poweron;
} Printer;

/**
 * SPC system structure, Z-80, RAM, ROM, IO(VRAM), etc
 */
typedef struct
{
	byte RAM[0x10000];	// RAM area
	byte VRAM[0x2000];	// Video Memory (6KB)
	int IPLK;				// IPLK switch for memory selection
	byte GMODE;				// GMODE (for video)
	byte psgRegNum;			// Keep PSG (AY-3-8910) register number for next turn of I/O
	z80 Z80R;		// Z-80 register
	AY8910 ay8910;
	byte ROM[0x8000];	// ROM area
	UINT32 tick;
	UINT32 cycles;
	int refrTimer;	// timer for screen refresh
	int refrSet;	// init data for screen refresh timer
	double intrTime;	// variable for interrupt timing
	int IPL_SW;

	//int intrState;

	/* SPC-1000 keyboard matrix. Initially turned off. (high) */
	byte keyMatrix[10];
	CassetteTape cas;
    FloppyDisk fdd;
	Printer prt;
} SPCSystem;

/**
 * SPC simulation structure, realted to the timing
 */
typedef struct
{
	UINT32 baseTick, curTick, prevTick, pauseTick, menuTick;
} SPCSimul;

// typedef struct
// {
// 	SDL_Surface *emul;
// 	SDL_Surface *disp;
//     SDL_Surface *mscr;
//     SDL_Rect rect;
//     int w, h;
//     SDL_Color colores[16];
//     TTF_Font *font;
//    	unsigned int  flag;
// } SDLInfo;

extern char path_snaps[MAX_PATH_LENGTH];
extern char path_taps[MAX_PATH_LENGTH];
extern char path_fdd1[MAX_PATH_LENGTH];
extern char path_fdd2[MAX_PATH_LENGTH];
extern char path_scr1[MAX_PATH_LENGTH];
extern char path_scr2[MAX_PATH_LENGTH];
extern char path_confs[MAX_PATH_LENGTH];
extern char path_tmp[MAX_PATH_LENGTH];
extern char path_disks[MAX_PATH_LENGTH];
extern char load_path_snaps[MAX_PATH_LENGTH];
extern char load_path_taps[MAX_PATH_LENGTH];
extern char load_path_scr1[MAX_PATH_LENGTH];
extern char load_path_poke[MAX_PATH_LENGTH];

extern byte DebugZ80(z80 *R);
extern void help_menu();
extern void taps_menu();
extern void init_menu();
extern void snapshots_menu();
extern void settings_menu();

extern SPCConfig spconf;
extern SPCSystem spcsys;
extern SPCSimul  spcsim;
// extern SDLInfo   spcsdl;
typedef unsigned char PIXEL;
extern int TURBO;

#define SPCCOL_OLDREV		0
#define SPCCOL_NEW1			1
#define SPCCOL_NEW2			2
#define SPCCOL_GREEN		3

#define SCANLINE_ALL		0
#define SCANLINE_NONE		1
#define SCANLINE_045_ONLY	2

#define SCREEN_WIDTH

#define I_PERIOD 400
#define TURBO (spconf.turbo) 
#define I_PERIOD_TURBO (I_PERIOD * (TURBO + 1))
#define INTR_PERIOD 16.6666
#endif

extern long timeGetTime();
extern void ToggleFullScreen();
