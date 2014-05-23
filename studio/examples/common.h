#include "common_platform.h"
#include "fmod.h"
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define NUM_COLUMNS 50
#define NUM_ROWS 25

enum Common_Button
{
    BTN_ACTION1,
    BTN_ACTION2,
    BTN_ACTION3,
    BTN_ACTION4,
    BTN_LEFT,
    BTN_RIGHT,
    BTN_UP,
    BTN_DOWN,
    BTN_MORE,
    BTN_QUIT
};

/* Cross platform functions (common) */
void Common_Fatal(const char *format, ...);
void Common_Draw(const char *format, ...);
void ERRCHECK(FMOD_RESULT result);

/* Functions with platform specific implementation (common_platform) */
void Common_Init(void **extraDriverData);
void Common_Close();
void Common_Update();
void Common_Sleep(unsigned int ms);
void Common_Exit(int returnCode);
void Common_DrawText(const char *text);
void Common_LoadFileMemory(const char *name, void **buff, int *length);
void Common_UnloadFileMemory(void *buff);
bool Common_BtnPress(Common_Button btn);
bool Common_BtnDown(Common_Button btn);
const char *Common_BtnStr(Common_Button btn);
const char *Common_MediaPath(const char *fileName);
