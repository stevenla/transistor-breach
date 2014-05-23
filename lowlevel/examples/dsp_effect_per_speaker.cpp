/*==============================================================================
DSP Effect Per Speaker Example
Copyright (c), Firelight Technologies Pty, Ltd 2004-2014.

This example shows how to manipulate a DSP network and as an example, creates 2
DSP effects, splitting a single sound into 2 audio paths, which it then filters
seperately.

To only have each audio path come out of one speaker each,
DSPConnection::setLevels is used just before the 2 branches merge back together
again.

For more speakers:
1. Use System::setSoftwareFormat
2. Create more effects, currently 2 for stereo (lowpass and chorus), create one
   per speaker.
3. Under the 'Now connect the 2 effects to channeldsp head.' section, connect
   the extra effects by duplicating the code more times.
4. Filter each effect to each speaker by calling DSP::setInputLevels.  Expand
   the existing code by extending the level arrays from 2, to the number of
   speakers you require, and change the numlevels parameter in 
   DSP::setInputLevels from 2 to the correct number accordingly.
==============================================================================*/
#include "fmod.hpp"
#include "common.h"

int FMOD_Main()
{
    FMOD::System        *system;
    FMOD::Sound         *sound;
    FMOD::Channel       *channel;
    FMOD::ChannelGroup  *mastergroup;
    FMOD::DSP           *dsplowpass, *dspchorus, *dsphead, *dspchannelmixer;
    FMOD::DSPConnection *dsplowpassconnection, *dspchorusconnection;
    FMOD_RESULT          result;
    unsigned int         version;
    float                pan = 0.0f;
    void                *extradriverdata = 0;

    Common_Init(&extradriverdata);

    /*
        Create a System object and initialize.
    */
    result = FMOD::System_Create(&system);
    ERRCHECK(result);

    result = system->getVersion(&version);
    ERRCHECK(result);

    if (version < FMOD_VERSION)
    {
        Common_Fatal("FMOD lib version %08x doesn't match header version %08x", version, FMOD_VERSION);
    }

    /*
        In this special case we want to use stereo output and not worry about varying matrix sizes depending on user speaker mode.
    */
    system->setSoftwareFormat(48000, FMOD_SPEAKERMODE_STEREO, 0);
    ERRCHECK(result);

    /*
        Initialize FMOD
    */
    result = system->init(32, FMOD_INIT_NORMAL, extradriverdata);
    ERRCHECK(result);

    result = system->createSound(Common_MediaPath("drumloop.wav"), FMOD_SOFTWARE | FMOD_LOOP_NORMAL, 0, &sound);
    ERRCHECK(result);

    result = system->playSound(sound, 0, false, &channel);
    ERRCHECK(result);

    /*
        Create the DSP effects.
    */  
    result = system->createDSPByType(FMOD_DSP_TYPE_LOWPASS, &dsplowpass);
    ERRCHECK(result);

    result = dsplowpass->setParameterFloat(FMOD_DSP_LOWPASS_CUTOFF, 1000.0f);
    ERRCHECK(result);
    result = dsplowpass->setParameterFloat(FMOD_DSP_LOWPASS_RESONANCE, 4.0f);
    ERRCHECK(result);

    result = system->createDSPByType(FMOD_DSP_TYPE_CHORUS, &dspchorus);
    ERRCHECK(result);

    result = dspchorus->setParameterFloat(FMOD_DSP_CHORUS_DEPTH, 60.0f);
    ERRCHECK(result);

    /*
        Connect up the DSP network
    */

    /*
        When a sound is played, a subnetwork is set up in the DSP network which looks like this.
        Wavetable is the drumloop sound, and it feeds its data from right to left.

        [DSPHEAD]<------------[DSPCHANNELMIXER]
    */  
    result = system->getMasterChannelGroup(&mastergroup);
    ERRCHECK(result);

    result = mastergroup->getDSP(FMOD_CHANNELCONTROL_DSP_HEAD, &dsphead);
    ERRCHECK(result);

    result = dsphead->getInput(0, &dspchannelmixer, 0);
    ERRCHECK(result);

    /*
        Now disconnect channeldsp head from wavetable to look like this.

        [DSPHEAD]             [DSPCHANNELMIXER]
    */
    result = dsphead->disconnectFrom(dspchannelmixer);
    ERRCHECK(result);

    /*
        Now connect the 2 effects to channeldsp head.  
        Store the 2 connections this makes so we can set their speakerlevels later.

                  [DSPLOWPASS]
                 /x           
        [DSPHEAD]             [DSPCHANNELMIXER]
                 \y           
                  [DSPCHORUS]
    */
    result = dsphead->addInput(dsplowpass, &dsplowpassconnection);  /* x = dsplowpassconnection */
    ERRCHECK(result);
    result = dsphead->addInput(dspchorus, &dspchorusconnection);    /* y = dspchorusconnection */
    ERRCHECK(result);
    
    /*
        Now connect the wavetable to the 2 effects

                  [DSPLOWPASS]
                 /x          \
        [DSPHEAD]             [DSPCHANNELMIXER]
                 \y          /
                  [DSPCHORUS]
    */
    result = dsplowpass->addInput(dspchannelmixer);  /* Ignore connection - we dont care about it. */
    ERRCHECK(result);

    result = dspchorus->addInput(dspchannelmixer);   /* Ignore connection - we dont care about it. */
    ERRCHECK(result);

    /*
        Now the drumloop will be twice as loud, because it is being split into 2, then recombined at the end.
        What we really want is to only feed the dspchannelmixer->dsplowpass through the left speaker, and 
        dspchannelmixer->dspchorus to the right speaker.
        We can do that simply by setting the pan, or speaker levels of the connections.

                  [DSPLOWPASS]
                 /x=1,0      \
        [DSPHEAD]             [DSPCHANNELMIXER]
                 \y=0,1      /
                  [DSPCHORUS]
    */    
    {
        float lowpassmatrix[4] = { 1.0f, 0.0f, 0.0f, 0.0f };
        float chorusmatrix[4]  = { 0.0f, 0.0f, 0.0f, 1.0f };

        /* This is important to make sure our matrix below fits the format of the dspchannelmixer node.. */
        /* ..If we didn't do this the incoming signal might be mono so a stereo matrix doesnt work.      */
        result = dspchannelmixer->setChannelFormat(0, 0, FMOD_SPEAKERMODE_STEREO);
        ERRCHECK(result);

        result = dsplowpassconnection->setMixMatrix(lowpassmatrix, 2, 2);
        ERRCHECK(result);
        result = dspchorusconnection->setMixMatrix(chorusmatrix, 2, 2);
        ERRCHECK(result);
    }

    result = dsplowpass->setBypass(true);
    ERRCHECK(result);
    result = dspchorus->setBypass(true);
    ERRCHECK(result);

    result = dsplowpass->setActive(true);
    ERRCHECK(result);
    result = dspchorus->setActive(true);
    ERRCHECK(result);

    /*
        Main loop.
    */
    do
    {
        bool lowpassbypass, chorusbypass;

        Common_Update();

        result = dsplowpass->getBypass(&lowpassbypass);
        ERRCHECK(result);
        result = dspchorus->getBypass(&chorusbypass);
        ERRCHECK(result);

        if (Common_BtnPress(BTN_ACTION1))
        {
            lowpassbypass = !lowpassbypass;

            result = dsplowpass->setBypass(lowpassbypass);
            ERRCHECK(result);
        }

        if (Common_BtnPress(BTN_ACTION2))
        {
            chorusbypass = !chorusbypass;

            result = dspchorus->setBypass(chorusbypass);
            ERRCHECK(result);
        }

        if (Common_BtnDown(BTN_LEFT))
        {
            pan = (pan <= -0.9f) ? -1.0f : pan - 0.1f; 
            
            result = channel->setPan(pan);
            ERRCHECK(result);
        }

        if (Common_BtnDown(BTN_RIGHT))
        {
            pan = (pan >= 0.9f) ? 1.0f : pan + 0.1f; 
            
            result = channel->setPan(pan);
            ERRCHECK(result);
        }

        result = system->update();
        ERRCHECK(result);

        Common_Draw("==================================================");
        Common_Draw("DSP Effect Per Speaker Example.");
        Common_Draw("Copyright (c) Firelight Technologies 2004-2014.");
        Common_Draw("==================================================");
        Common_Draw("");
        Common_Draw("Press %s to toggle lowpass (left speaker)", Common_BtnStr(BTN_ACTION1));
        Common_Draw("Press %s to toggle chorus (right speaker)", Common_BtnStr(BTN_ACTION2));
        Common_Draw("Press %s or %s to pan sound", Common_BtnStr(BTN_LEFT), Common_BtnStr(BTN_RIGHT));
        Common_Draw("Press %s to quit", Common_BtnStr(BTN_QUIT));
        Common_Draw("");
        Common_Draw("Lowpass (left) is %s", lowpassbypass ? "inactive" : "active");
        Common_Draw("Chorus (right) is %s", chorusbypass ? "inactive" : "active");
        Common_Draw("Pan is %0.2f", pan);

        Common_Sleep(50);
    } while (!Common_BtnPress(BTN_QUIT));

    /*
        Shut down
    */
    result = sound->release();
    ERRCHECK(result);

    result = dsplowpass->release();
    ERRCHECK(result);
    result = dspchorus->release();
    ERRCHECK(result);

    result = system->close();
    ERRCHECK(result);
    result = system->release();
    ERRCHECK(result);

    Common_Close();

    return 0;
}
