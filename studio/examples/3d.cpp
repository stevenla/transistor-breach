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
    void *extraDriverData = 0;
    Common_Init(&extraDriverData);

    FMOD::Studio::System system;
    FMOD_RESULT result = FMOD::Studio::System::create(&system);
    ERRCHECK(result);
    
    FMOD::System* lowLevel;
    system.getLowLevelSystem(&lowLevel);
    
    lowLevel->setOutput(FMOD_OUTPUTTYPE_WAVWRITER);


    result = system.initialize(32, FMOD_STUDIO_INIT_ALLOW_MISSING_PLUGINS, FMOD_INIT_NORMAL, extraDriverData);
    ERRCHECK(result);

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
    
    FMOD::Studio::ID eventID = {0};
    ERRCHECK( system.lookupEventID("/Music/MoodyArpeggioVIRUS_MC", &eventID) );
    
    FMOD::Studio::EventDescription eventDescription;
    ERRCHECK( system.getEvent(&eventID, FMOD_STUDIO_LOAD_BEGIN_NOW, &eventDescription) );

    FMOD::Studio::EventInstance eventInstance;
    ERRCHECK( eventDescription.createInstance(&eventInstance) );

    ERRCHECK( eventInstance.start() );
    
    // Position the listener at the origin
    FMOD_3D_ATTRIBUTES attributes = { { 0 } };
    attributes.forward.z = 1.0f;
    attributes.up.y = 1.0f;
    ERRCHECK( system.setListenerAttributes(&attributes) );

    // Position the event 2 units in front of the listener
    attributes.position.z = 0.0f;
    ERRCHECK( eventInstance.set3DAttributes(&attributes) );
    
    FMOD_STUDIO_PLAYBACK_STATE state;

    do
    {
        Common_Update();
        

        result = system.update();
        ERRCHECK(result);
        
        Common_Draw("==================================================");
        Common_Draw("TRANSISTOR BREACH.");
        Common_Draw("Copyright (c) Firelight Technologies 2014-2014.");
        Common_Draw("==================================================");
        Common_Draw("Enjoy!",
            Common_BtnStr(BTN_LEFT), Common_BtnStr(BTN_RIGHT), Common_BtnStr(BTN_UP), Common_BtnStr(BTN_DOWN));
        Common_Draw("Press %s to quit", Common_BtnStr(BTN_QUIT));
        
        
        
        Common_Sleep(50);
        
        /*
        FMOD::ChannelGroup *topGroup;
        int topNumChannels;
        eventInstance.getChannelGroup(&topGroup);
        topGroup->getNumGroups(&topNumChannels);
        
        FMOD::ChannelGroup *sub1;
        int numChannels;
        int numGroups;
        topGroup->getGroup(3, &sub1);
        
        sub1->getNumChannels(&numChannels);
        sub1->getNumGroups(&numGroups);
        
        if (numChannels > 0) {
            FMOD::Channel *channel;
            sub1->getChannel(0, &channel);
            channel->setVolume(0.0);
        }*/
        
    } while (!Common_BtnPress(BTN_QUIT));

    result = system.release();
    ERRCHECK(result);

    Common_Close();

    return 0;
}
