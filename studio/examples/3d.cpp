/*==============================================================================
Event 3D Example
Copyright (c), Firelight Technologies Pty, Ltd 2012-2014.

This example demonstrates how to position events in 3D for spatialization.
==============================================================================*/
#include "fmod_studio.hpp"
#include "fmod.hpp"
#include "fmod_errors.h"
#include "common.h"

const int SCREEN_WIDTH = NUM_COLUMNS;
const int SCREEN_HEIGHT = 16;

int currentScreenPosition = -1;

int FMOD_Main()
{
    // Basic init stuff -- I think this was here when I started
    void *extraDriverData = 0;
    Common_Init(&extraDriverData);

    // Create the FMOD Studio System. This is the brains of the API!!
    FMOD::Studio::System system;
    FMOD_RESULT result = FMOD::Studio::System::create(&system);
    ERRCHECK(result);
    
    // The low-level system is the backbone of the API!!
    FMOD::System* lowLevel;
    system.getLowLevelSystem(&lowLevel);
    
    // Tell the low-level system to write audio-out to a WAV file, instead of the default speakers.
    lowLevel->setOutput(FMOD_OUTPUTTYPE_WAVWRITER);


    // Initialize the system. Missing plugins are okay for what we're doing
    result = system.initialize(32, FMOD_STUDIO_INIT_ALLOW_MISSING_PLUGINS, FMOD_INIT_NORMAL, extraDriverData);
    ERRCHECK(result);

    // Load each of the audio banks in to the system. Nothing worked unless I loaded all of the audio banks.
    
    FMOD::Studio::Bank masterBank;
    ERRCHECK( system.loadBankFile(Common_MediaPath("MasterBank.bank"), &masterBank) );

    FMOD::Studio::Bank stringsBank;
    ERRCHECK( system.loadBankFile(Common_MediaPath("MasterBank.bank.strings"), &stringsBank) );
    
    FMOD::Studio::Bank ambienceBank;
    ERRCHECK( system.loadBankFile(Common_MediaPath("AudenFMOD_Ambience.bank"), &ambienceBank) );
    
    FMOD::Studio::Bank musicBank;
    ERRCHECK( system.loadBankFile(Common_MediaPath("AudenFMOD_Music.bank"), &musicBank) );
    
    FMOD::Studio::Bank oldSoundsBank;
    ERRCHECK( system.loadBankFile(Common_MediaPath("AudenFMOD_OldSounds.bank"), &oldSoundsBank) );
    
    FMOD::Studio::Bank soundsBank;
    ERRCHECK( system.loadBankFile(Common_MediaPath("AudenFMOD_Sounds.bank"), &soundsBank) );
    
    // Look up the event ID by its name. These should be given in a file called `GUIDs.txt`.
    FMOD::Studio::ID eventID = {0};
    ERRCHECK( system.lookupEventID("/Music/SoftJazzy_MC", &eventID) );  // The string is hardcoded; each track has its own string
    
    // From the event ID, we'll load actually load the event audio
    FMOD::Studio::EventDescription eventDescription;
    ERRCHECK( system.getEvent(&eventID, FMOD_STUDIO_LOAD_BEGIN_NOW, &eventDescription) );

    // Create an instance of the event to listen to
    FMOD::Studio::EventInstance eventInstance;
    ERRCHECK( eventDescription.createInstance(&eventInstance) );

    // Start listening!
    ERRCHECK( eventInstance.start() );
    
    // Remember audio is 3D.
    // Position the listener at the origin
    FMOD_3D_ATTRIBUTES attributes = { { 0 } };
    attributes.forward.z = 1.0f;
    attributes.up.y = 1.0f;
    ERRCHECK( system.setListenerAttributes(&attributes) );

    // Position the event x units in front of the listener
    attributes.position.z = 0.0f;
    ERRCHECK( eventInstance.set3DAttributes(&attributes) );
    
    // I have no idea what this is -- it was probably here when I started.
    FMOD_STUDIO_PLAYBACK_STATE state;

    // Keep looping while the song is playing.
    do
    {
        Common_Update();

        result = system.update();
        ERRCHECK(result);
        
        Common_Draw("==================================================");
        Common_Draw("TRANSISTOR BREACH.");
        Common_Draw("==================================================");
        Common_Draw("Enjoy!",
            Common_BtnStr(BTN_LEFT), Common_BtnStr(BTN_RIGHT), Common_BtnStr(BTN_UP), Common_BtnStr(BTN_DOWN));
        Common_Draw("Press %s to quit", Common_BtnStr(BTN_QUIT));
        
        
        
        Common_Sleep(50);
        
        // This section is where the guesswork comes in! We can mute audio channels by number by doing this.
        
        // Each track has a ChannelGroup. Inside a ChannelGroup are a number of other ChannelGroups..
        FMOD::ChannelGroup *topGroup;
        int topNumChannels;  // Only good for debug purposes, e.g. printing the number of channels.
        eventInstance.getChannelGroup(&topGroup);
        topGroup->getNumGroups(&topNumChannels);
        
        // In Transistor's audio, the track's sub-ChannelGroups are the ones that have the actual audio channels.
        FMOD::ChannelGroup *sub1;
        int numChannels;
        int numGroups;
        
        // Get the ChannelGroup that we want to manipulate (e.g. mute)
        topGroup->getGroup(3, &sub1);  // 3 is hardcoded, and may vary for each track.
        
        // Get some info about the Sub-ChannelGroup
        sub1->getNumChannels(&numChannels);
        sub1->getNumGroups(&numGroups);
        
        // The audio channels don't load in instantly, so we want to wait until they exist before doing stuff to them.
        if (numChannels > 0) {
            // Get the audio channel that we want to manuplate.
            FMOD::Channel *channel;
            sub1->getChannel(0, &channel);  // 0 is hardcoded, and may vary for each track.
            channel->setVolume(0.0);  // MUTE!
        }
        
    } while (!Common_BtnPress(BTN_QUIT));

    result = system.release();
    ERRCHECK(result);

    Common_Close();

    return 0;
}
