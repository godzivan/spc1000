#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <SDL.h>
//#include <SDL_mixer.h>
#include "cpu.h"
#include "mc6847.h"
#include "ay8910.h"
#include "keyboard.h"
#include "cassette.h"
#include "spcall.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "ugui.h"
#ifdef __cplusplus
}
#endif
#define LOW 0
#define HIGH 1
#define OUTPUT 1
#define INPUT 0

typedef struct _reg {
    char IPLK;
    char motor;
    char pulse;
    char button;
    _reg() { 
        IPLK = 1;
    }
} Registers;

Registers reg;

void pinMode(int pin, int mode)
{
    
}

// SDL_Palette *create_palette()
// {
//     SDL_Palette *p = SDL_AllocPalette(2);
//     p->colors[0].r = 0xff;
//     p->colors[0].g = 0x00;
//     p->colors[0].b = 0x00;
//     p->colors[1].r = 0xff;
//     p->colors[1].g = 0xff;
//     p->colors[1].b = 0xff;
//     return p;
// }

CMC6847 mc6847;
CKeyboard kbd;
CPU cpu;
AY8910 ay8910;
Cassette cassette;

uint8_t memory[0x10000];

static uint8_t rb(void *userdata, uint16_t addr) {
    uint8_t data;
    data = (reg.IPLK ? ROM[addr & 0x7fff] : memory[addr&0xffff]);
    return data;
}

static void wb(void *userdata, uint16_t addr, uint8_t val) {
    memory[addr&0xffff] = val;
}

static uint8_t in(z80* const z, uint16_t port) {
    uint8_t retval = 0xff;
	if (port >= 0x8000 && port <= 0x8009) // Keyboard Matrix
	{
		return kbd.matrix(port);
	}
	else if ((port & 0xE000) == 0xA000) // IPLK
	{
		reg.IPLK = !reg.IPLK;
	} else if ((port & 0xE000) == 0x2000) // GMODE
	{
		return mc6847.GMODE;
	} else if ((port & 0xE000) == 0x0000) // VRAM reading
	{
		return mc6847.VRAM[port];
	}	
    else if ((port & 0xFFFE) == 0x4000) // PSG
	{
		if (port & 0x01) // Data
		{
			if (ay8910.reg == 14)
			{
				// 0x80 - cassette data input
				// 0x40 - motor status
				// 0x20 - print status
//				if (spcsys.prt.poweron)
//                {
//                    printf("Print Ready Check.\n");
//                    retval &= 0xcf;
//                }
//                else
//                {
//                    retval |= 0x20;
//                }
				if (cassette.motor) //(spcsys.cas.button == CAS_PLAY && spcsys.cas.motor)
				{
					retval &= (~(0x40)); // 0 indicates Motor On
					if (cassette.read(cpu.getCycles(), rb(0, 0x3c5)) == 1)
							retval |= 0x80; // high
						else
							retval &= 0x7f; // low
                    // if (cpu.r->pc == 0x2ba && retval & 0x80)
                    // {
                    //     printf("%d(%d)", retval&0x80 ? 1:0, cpu.r->h);
                    // }
                    // else
                    // {
                    //     printf("0x%04x:%d\n", cpu.r->pc, retval&0x80 ? 1 : 0);
                    // }
                    // if (cassette.pos < 10)
                    // {
                    //     printf("pc:0x%04x\n", cpu.r->pc);
                    //     // cpu.debug();                        
                    // }
				}
				else
					retval |= 0x40;

			}
			else 
			{
				int data = ay8910.read();
				//printf("r(%d,%d)\n", spcsys.psgRegNum, data);
				return data;
			}
		} else if (port & 0x02)
		{
            retval = (cassette.read1() == 1 ? retval | 0x80 : retval & 0x7f);
		}
	}
    // printf("pc:%04x, port:%04x, val:%02x\n", cpu.r->pc, port, retval);
    return retval;
}

static void out(z80* const z, uint16_t port, uint8_t val) {
	if ((port & 0xE000) == 0x0000) // VRAM area
	{
		mc6847.VRAM[port&0x1fff] = val;
	}
	else if ((port & 0xE000) == 0xA000) // IPLK area
	{
		reg.IPLK = !reg.IPLK;	// flip IPLK switch
	}
	else if ((port & 0xE000) == 0x2000)	// GMODE setting
	{
		mc6847.GMODE = val;
	}
	else if ((port & 0xE000) == 0x6000) // SMODE
	{
		// if (reg.button != CASSETTE_STOP)
		{

			if ((val & 0x02)) // Motor
			{
				if (reg.pulse == 0)
				{
					reg.pulse = 1;
				}
			}
			else
			{
				if (reg.pulse)
				{
					reg.pulse = 0;
                    cassette.motor = !cassette.motor;
                    cpu.set_turbo(cassette.motor);
                    wb(NULL, 0x3c5, cassette.motor ? 1 : 90);
                    // printf("montor:%d(%d)\n", cassette.motor, rb(0, 0x3c5));
				}
			}
		}
        if (cassette.motor)
        {
            cassette.write(val&1);            
        }
//		if (reg.button == CAS_REC && reg.motor)
//		{
//			CasWrite(&reg, Value & 0x01);
//		}
	}
	else if ((port & 0xFFFE) == 0x4000) // PSG
	{

		if (port & 0x01) // Data
		{
		    if (ay8910.reg == 15) // Line Printer
			{
				if (val != 0)
				{
			    //spcsys.prt.bufs[spcsys.prt.length++] = Value;
				//                    printf("PRT <- %c (%d)\n", Value, Value);
				//                    printf("%s(%d)\n", spcsys.prt.bufs, spcsys.prt.length);
				}
			}
			ay8910.write(val);
            // printf("reg:%d, val:%d\n", ay8910.reg, val);
		}
		else // Reg Num
		{
			ay8910.latch(val);
            // printf("reg:%d,", val);
		}
	}
}

#define CPU_FREQ 4000000
#define PSG_CLOCK PSG_CLOCK_RATE
#define SPC1000_AUDIO_FREQ PSG_CLOCK_RATE
#define SPC1000_AUDIO_BUFFER_SIZE 1024

static unsigned ptime;
static unsigned etime;
SDL_AudioSpec audioSpec;
int audid;

void callback(
  void* userdata,
  Uint8* stream,
  int    len)
{
    int samples = len / (sizeof(int16_t) * audioSpec.channels);
    ay8910.pushbuf((int16_t*)stream, samples);
    // for(int i = 0; i < samples; i++)
    // {
    //     printf("%02x", stream[i]);
    // }
    // SDL_memset(stream, 0, len);
    // printf("samples:%d\n", len);
    // renderAudioDevice(hbc56Device(i), str, samples);
}

unsigned int execute(Uint32 interval, void* name)
{
    static int frame = 0;
    etime = SDL_GetTicks();
    // int steps = (etime - ptime) * CPU_FREQ/1000;
    cpu.pulse_irq(0);
    int steps = cpu.exec(etime);
    cpu.clr_irq();
    if (frame++%2)
        mc6847.Update();
    ay8910.update(etime);
    ptime = etime;
    return 0;
}


#include <iostream>

SDL_Renderer *renderer;
SDL_Surface *surface;
SDL_Window *screen;
SDL_Texture *texture;
SDL_Texture *texture_title;
UG_COLOR *pixels;
SDL_Event event;
UG_GUI ug;
int w, h;
uint32_t textout_time;
char text[256];

void SetPixel(UG_S16 x, UG_S16 y, UG_COLOR color)
{
    if (pixels) {
        pixels[x + y * w * 2] = 0xff000000 | color;
    }
}

void setText(const char *s, int keep_time = 2000)
{
    textout_time = SDL_GetTicks() + keep_time;
    UG_SetForecolor(C_WHITE);
    UG_FillFrame(10, 10, 639, 50, C_BLACK);
    // printf("%s\n", s);
    UG_PutString(10, 10, (char *)s);
}

void clearText()
{
    UG_FillFrame(10, 10, 640, 40, C_BLACK);
}

void reset()
{
    cpu.init();
    cpu.set_read_write(rb, wb);
    cpu.set_in_out(in, out);
    mc6847.Initialize();
    ay8910.reset();
    memset(memory, 0, 0x10000);
    reg.IPLK = true;
}

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

SDL_Rect dstrect;

void ToggleFullscreen(SDL_Window* window) {
    Uint32 flag = SDL_WINDOW_FULLSCREEN_DESKTOP;
    static int lastWindowX, lastWindowY;
    bool isFullscreen = SDL_GetWindowFlags(window) & flag;
    if(!isFullscreen){
        SDL_GetWindowPosition(window, &lastWindowX, &lastWindowY);
    }

    if(isFullscreen){
        int w, h;
        SDL_Rect viewport;
        SDL_RenderGetViewport(renderer, &viewport);
        SDL_GetWindowSize(window, &w, &h);
        if (w * 3 > h * 4) {
            viewport.w = h * 4 / 3;
            viewport.h = h;
        } else {
            viewport.w = w;
            viewport.h = w * 3 / 4;
        }
        viewport.x = (w - viewport.w) / 2;
        viewport.y = (h - viewport.h) / 2;
        SDL_RenderSetViewport(renderer, &viewport);
        // cout << "set window to: " << lastWindowX << " " << lastWindowY << endl;
        SDL_SetWindowPosition(window, lastWindowX, lastWindowY);
        dstrect.x = 0;
        dstrect.y = 0;
        dstrect.w = SCREEN_WIDTH;
        dstrect.h = SCREEN_HEIGHT;         
    }
    else
    {
        dstrect.x = (1920-1080*4/3)/2;
        dstrect.y = 0;
        dstrect.w = 1080*4/3;
        dstrect.h = 1080; 
    }
    SDL_SetWindowFullscreen(window, isFullscreen ? 0 : flag);
}

void ProcessSpecialKey(SDL_Keysym ksym)
{
	int index = ksym.sym % 256;
    if (ksym.mod & KMOD_ALT)
    {
        switch (ksym.sym)
        {
            case SDLK_LEFT: 
                cassette.prev();
                cassette.get_title(text);
                setText(text);
                break;
            case SDLK_RIGHT:
                cassette.next();
                cassette.get_title(text);
                setText(text);
                break;
            case SDLK_RETURN:
                ToggleFullscreen(screen);
                break;
        }
    }
    else {
        switch (ksym.sym)
        {
            case SDLK_F10:
                reset();
                break;
        }
    }
}

void  main_loop()
{
    static int i = 1000;
    SDL_Delay(16);
    execute(16, NULL);
    if (SDL_PollEvent(&event)) {
        if (event.type == SDL_KEYDOWN)
        {
            ProcessSpecialKey(event.key.keysym);
        }
        kbd.handle_event(event);
    }
    int ret = SDL_UpdateTexture(texture, NULL, mc6847.GetBuffer(), w*2);
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
    // SDL_LockSurface(surface);
    if (SDL_GetTicks() < textout_time)
    {
        SDL_UpdateTexture(texture_title, NULL, surface->pixels, w*2*4);
        SDL_RenderCopy(renderer, texture_title, NULL, &dstrect);
    }
    SDL_RenderPresent(renderer);
}

#include <sys/stat.h>
int main(int argc, char *argv[]) {

    char *tapdir = (char *)"../tape/taps";
    if (argc > 1)
    {
        struct stat sb;
        if (!stat(argv[1], &sb))
            tapdir = argv[1];
    }
    // struct timer_wait tw;
    int led_status = LOW;
    reset();
    w = 320; h = 240;
    UG_Init(&ug, SetPixel, w * 2, h * 2);
    UG_FontSelect(&FONT_12X20);
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_TIMER);
    // int modeCount = SDL_GetNumDisplayModes(0);
	// SDL_Log("Mode count: %d", modeCount);
	// SDL_DisplayMode mode;
	// A list of the display sizes available on your device, 
	// these can often be determined by your OS and Window Manager instead of
	// your actual hardware
	// for(int i = 0; i < modeCount; i ++)
	// {
	// 	SDL_GetDisplayMode(0, i, &mode);
	// 	SDL_Log("Mode %d: (w, h) = (%d, %d)", i, mode.w, mode.h);
	// }
	// // Fetching the closest mode to your requested size
	// mode.w = SCREEN_WIDTH;
	// mode.h = SCREEN_HEIGHT;
	// SDL_DisplayMode modeRecieved;
    dstrect.w = SCREEN_WIDTH;
    dstrect.h = SCREEN_HEIGHT;
    dstrect.x = dstrect.y = 0;
	// SDL_GetClosestDisplayMode(0, &mode, &modeRecieved);
    // printf("wxh:%dx%d\n", modeRecieved.w, modeRecieved.h);
	// SCREEN_WIDTH = modeRecieved.w;
	// SCREEN_HEIGHT = modeRecieved.h;
    // Default screen resolution (set in config.txt or auto-detected)
    // SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP, &screen, &renderer);
    SDL_CreateWindowAndRenderer(w * 2, h * 2, SDL_WINDOW_BORDERLESS, &screen, &renderer);
    surface = SDL_CreateRGBSurface(SDL_SWSURFACE, w * 2, h * 2, 32, 0, 0, 0, 0);
    SDL_SetColorKey(surface, SDL_TRUE, 0x0);
    texture_title = SDL_CreateTextureFromSurface(renderer, surface);
    // surface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 16, SDL_PIXELFORMAT_RGB565);
    // SDL_LockSurface(surface);
    pixels = (UG_COLOR *)surface->pixels;
    UG_SetBackcolor(C_BLACK);
    UG_SetForecolor(C_RED);
    // UG_PutString(6, 10, "Hello World!");
    // SDL_UnlockSurface(surface);
    // palette = create_palette();
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, w, h);
    if (!texture) {
        printf("%s\n", SDL_GetError());
    }
    //Initialize SDL2 Audio
    SDL_AudioSpec specs = {};
    specs.freq = SPC1000_AUDIO_FREQ;
    specs.format = AUDIO_S16SYS;
    specs.channels = 1;
    specs.samples = SPC1000_AUDIO_BUFFER_SIZE;
    specs.callback = callback;
    constexpr int PLAYBACK_DEV = 0;
    audid  = SDL_OpenAudioDevice( nullptr, PLAYBACK_DEV, &specs, &audioSpec, SDL_AUDIO_ALLOW_CHANNELS_CHANGE );
    if( audid == 0 )
    {
        std::cerr << "Error opening audio device: " << SDL_GetError() << std::endl;
        return 1;
    }
    SDL_PauseAudioDevice(audid, 0);
    // pinMode(16, OUTPUT);
    // register_timer(&tw, 250000);
    ptime = SDL_GetTicks();
    ay8910.initTick(ptime);
    cpu.initTick(ptime);
    cassette.loaddir(tapdir);
#ifdef EMSCRIPTEN    
    emscripten_set_main_loop(main_loop, -1, 1);
#else
    do {
        main_loop();
    }
    while(event.type != SDL_QUIT);
#endif
}
