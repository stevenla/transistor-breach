#import "common.h"
#import <Cocoa/Cocoa.h>
#include <libkern/OSAtomic.h>

const Common_Button BTN_IDS[] = {BTN_ACTION1, BTN_ACTION2, BTN_ACTION3, BTN_ACTION4, BTN_LEFT, BTN_RIGHT, BTN_UP, BTN_DOWN, BTN_MORE};
const unsigned int BTN_COUNT = sizeof(BTN_IDS) / sizeof(BTN_IDS[0]);

NSMutableString *gOutputBuffer;
NSTextField *gOutputWindow;
uint32_t gButtons;
uint32_t gButtonsRead;

void Common_Init(void **extraDriverData)
{
    gOutputBuffer = [NSMutableString stringWithCapacity:(NUM_COLUMNS * NUM_ROWS)];
}

void Common_Close()
{

}

void Common_Update()
{
    [gOutputWindow setStringValue:gOutputBuffer];
    [gOutputBuffer setString:@""];

    do
    {
        gButtonsRead = gButtons;
    } while (!OSAtomicCompareAndSwap32(gButtonsRead, 0, (int32_t*)&gButtons));
}

void Common_Sleep(unsigned int ms)
{
    [NSThread sleepForTimeInterval:(ms / 1000.0f)];
}

void Common_Exit(int returnCode)
{
    exit(-1);
}

void Common_DrawText(const char *text)
{   
    [gOutputBuffer appendFormat:@"%s\n", text];
}

bool Common_BtnPress(Common_Button btn)
{
    return ((gButtonsRead & (1 << btn)) != 0);
}

bool Common_BtnDown(Common_Button btn)
{
    return Common_BtnPress(btn);
}

const char *Common_BtnStr(Common_Button btn)
{
    switch (btn)
    {
        case BTN_ACTION1: return "1";
        case BTN_ACTION2: return "2";
        case BTN_ACTION3: return "3";
        case BTN_ACTION4: return "4";
        case BTN_UP:      return "Up";
        case BTN_DOWN:    return "Down";
        case BTN_LEFT:    return "Left";
        case BTN_RIGHT:   return "Right";
        case BTN_MORE:    return "More";
        case BTN_QUIT:    return "X";
    }
}

const char *Common_MediaPath(const char *fileName)
{
    return [[NSString stringWithFormat:@"%@/media/%s", [[NSBundle mainBundle] resourcePath], fileName] UTF8String];
}

void Common_LoadFileMemory(const char *name, void **buff, int *length)
{
    FILE *file = fopen(name, "rb");

    fseek(file, 0, SEEK_END);
    long len = ftell(file);
    fseek(file, 0, SEEK_SET);

    void *mem = malloc(len);
    fread(mem, 1, len, file);

    fclose(file);

    *buff = mem;
    *length = len;
}

void Common_UnloadFileMemory(void *buff)
{
    free(buff);
}

@interface ExampleApplicationDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
{
    NSWindow *mWindow;
    NSButton *mButtons[BTN_COUNT];
}
@end

@implementation ExampleApplicationDelegate : NSObject

- (id)init
{
    if (self = [super init])
    {
        const unsigned int BUTTON_WIDTH  = 50;
        const unsigned int BUTTON_HEIGHT = 20;
        const unsigned int WINDOW_WIDTH  = BUTTON_WIDTH * BTN_COUNT;
        const unsigned int WINDOW_HEIGHT = 500;
        
        gOutputWindow = [[NSTextField alloc] init];
        [gOutputWindow setFont:[NSFont userFixedPitchFontOfSize:12]];
        [gOutputWindow setEditable:NO];
        [gOutputWindow setBezeled:NO];
        [gOutputWindow setBackgroundColor:[NSColor blackColor]];

        for (unsigned int i = 0; i < BTN_COUNT; i++)
        {
            mButtons[i] = [[NSButton alloc] initWithFrame:NSMakeRect(i * BUTTON_WIDTH, WINDOW_HEIGHT - BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT)];
            [mButtons[i] setAction:@selector(buttonClick:)];
            [mButtons[i] setTag:BTN_IDS[i]];
            [mButtons[i] setTitle:[NSString stringWithUTF8String:Common_BtnStr(BTN_IDS[i])]];
            [mButtons[i] setContinuous:true];
            [gOutputWindow addSubview:mButtons[i]];
        }

        mWindow = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT) styleMask:(NSTitledWindowMask | NSClosableWindowMask) backing:NSBackingStoreBuffered defer:NO];
        [mWindow setTitle:@"FMOD Example"];
        [mWindow setContentView:gOutputWindow];
        [mWindow setDelegate:self];
        
        [NSThread detachNewThreadSelector:@selector(threadMain:) toTarget:self withObject:nil];
    }
    
    return self;
}

- (void)dealloc
{
    for (unsigned int i = 0; i < BTN_COUNT; i++)
    {
        [mButtons[i] release];
    }

    [gOutputWindow release];
    [mWindow release];

    [super dealloc];
}

- (void)applicationWillFinishLaunching:(NSNotification *)notification
{
    [mWindow center];
    [mWindow makeKeyAndOrderFront:self];
}

- (BOOL)windowShouldClose:(id)sender
{
    OSAtomicOr32(1 << BTN_QUIT, &gButtons);
    return NO;
}

- (void)buttonClick:(id)sender
{
    OSAtomicOr32(1 << [sender tag], &gButtons);
}

int FMOD_Main();
- (void)threadMain:(id)arg
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    FMOD_Main();
    [NSApp terminate:nil];
    [pool drain];
}

@end

int main(int argc, char *argv[])
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSApplication *application = [NSApplication sharedApplication];
    ExampleApplicationDelegate *applicationDelegate = [[[ExampleApplicationDelegate alloc] init] autorelease];
    
    [application setDelegate:applicationDelegate];
    [application activateIgnoringOtherApps:YES];
    [application run];
    [pool drain];
    
    return EXIT_SUCCESS;
}
