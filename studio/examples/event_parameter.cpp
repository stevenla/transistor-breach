/*==============================================================================
Event Parameter Example
Copyright (c), Firelight Technologies Pty, Ltd 2012-2014.

This example demonstrates how to control event playback using game parameters.
==============================================================================*/
#include "fmod_studio.hpp"
#include "fmod.hpp"
#include "common.h"

int FMOD_Main()
{
    void *extraDriverData = 0;
    Common_Init(&extraDriverData);

    FMOD::Studio::System system;
    ERRCHECK( FMOD::Studio::System::create(&system) );
    ERRCHECK( system.initialize(32, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, extraDriverData) );

    FMOD::Studio::Bank masterBank;
    ERRCHECK( system.loadBankFile(Common_MediaPath("Master Bank.bank"), &masterBank) );

    FMOD::Studio::Bank stringsBank;
    ERRCHECK( system.loadBankFile(Common_MediaPath("Master Bank.bank.strings"), &stringsBank) );

    FMOD::Studio::Bank ambienceBank;
    ERRCHECK( system.loadBankFile(Common_MediaPath("Character.bank"), &ambienceBank) );
    
    FMOD::Studio::ID eventID = {0};
    ERRCHECK( system.lookupEventID("/Character/Footsteps/Footsteps", &eventID) );

    FMOD::Studio::EventDescription eventDescription;
    ERRCHECK( system.getEvent(&eventID, FMOD_STUDIO_LOAD_BEGIN_NOW, &eventDescription) );

    FMOD::Studio::EventInstance eventInstance;
    ERRCHECK( eventDescription.createInstance(&eventInstance) );

    FMOD::Studio::ParameterInstance surfaceParameter;
    ERRCHECK( eventInstance.getParameter("Surface", &surfaceParameter) );

    // Make the event audible to start with
    ERRCHECK( surfaceParameter.setValue(1.0f) );

    float surfaceParameterValue = 0;
    ERRCHECK( surfaceParameter.getValue(&surfaceParameterValue) );
    
    ERRCHECK( eventInstance.start() );

    do
    {
        Common_Update();

        if (Common_BtnPress(BTN_ACTION1))
        {
            surfaceParameterValue -= 1.0f;
            ERRCHECK( surfaceParameter.setValue(surfaceParameterValue) );
            ERRCHECK( surfaceParameter.getValue(&surfaceParameterValue) );
        }

        if (Common_BtnPress(BTN_ACTION2))
        {
            surfaceParameterValue += 1.0f;
            ERRCHECK( surfaceParameter.setValue(surfaceParameterValue) );
            ERRCHECK( surfaceParameter.getValue(&surfaceParameterValue) );
        }

        ERRCHECK( system.update() );

        Common_Draw("==================================================");
        Common_Draw("Event Parameter Example.");
        Common_Draw("Copyright (c) Firelight Technologies 2014-2014.");
        Common_Draw("==================================================");
        Common_Draw("");
        Common_Draw("Surface Parameter = %1.1f", surfaceParameterValue);
        Common_Draw("");
        Common_Draw("Surface Parameter:");
        Common_Draw("Press %s to decrease value", Common_BtnStr(BTN_ACTION1));
        Common_Draw("Press %s to increase value", Common_BtnStr(BTN_ACTION2));
        Common_Draw("");
        Common_Draw("Press %s to quit", Common_BtnStr(BTN_QUIT));

        Common_Sleep(50);
    } while (!Common_BtnPress(BTN_QUIT));

    ERRCHECK( system.release() );

    Common_Close();

    return 0;
}
