/*==============================================================================
Effects Example
Copyright (c), Firelight Technologies Pty, Ltd 2004-2014.

This example shows how to apply some of the built in software effects to sounds
by applying them to the master channel group. All software sounds played here
would be filtered in the same way. To filter per channel, and not have other
channels affected, simply apply the same functions to the FMOD::Channel instead
of the FMOD::ChannelGroup.

Note in this example you don't have to add and remove units each time, you could
simply add them all at the start then use DSP::setActive to toggle them on and
off.
==============================================================================*/
#include "fmod.hpp"
#include "common.h"

int FMOD_Main()
{
    FMOD::System       *system        = 0;
    FMOD::Sound        *sound         = 0;
    FMOD::Channel      *channel       = 0;
    FMOD::ChannelGroup *mastergroup   = 0; 
    FMOD::DSP          *dsplowpass    = 0;
    FMOD::DSP          *dsphighpass   = 0;
    FMOD::DSP          *dspecho       = 0;
    FMOD::DSP          *dspflange     = 0;
    FMOD_RESULT         result;
    unsigned int        version;
    void               *extradriverdata = 0;

    Common_Init(&extradriverdata);

    /*
        Create a System object and initialize
    */
    result = FMOD::System_Create(&system);
    ERRCHECK(result);

    result = system->getVersion(&version);
    ERRCHECK(result);

    if (version < FMOD_VERSION)
    {
        Common_Fatal("FMOD lib version %08x doesn't match header version %08x", version, FMOD_VERSION);
    }

    result = system->init(32, FMOD_INIT_NORMAL | FMOD_INIT_PROFILE_ENABLE, extradriverdata);
    ERRCHECK(result);

    result = system->getMasterChannelGroup(&mastergroup);
    ERRCHECK(result);

    result = system->createSound(Common_MediaPath("drumloop.wav"), FMOD_SOFTWARE, 0, &sound);
    ERRCHECK(result);

    result = system->playSound(sound, 0, false, &channel);
    ERRCHECK(result);

    /*
        Create some effects to play with
    */
    result = system->createDSPByType(FMOD_DSP_TYPE_LOWPASS, &dsplowpass);
    ERRCHECK(result);
    result = system->createDSPByType(FMOD_DSP_TYPE_HIGHPASS, &dsphighpass);
    ERRCHECK(result);
    result = system->createDSPByType(FMOD_DSP_TYPE_ECHO, &dspecho);
    ERRCHECK(result);
    result = system->createDSPByType(FMOD_DSP_TYPE_FLANGE, &dspflange);
    ERRCHECK(result);

    /*
        Main loop
    */
    do
    {
        Common_Update();

        if (Common_BtnPress(BTN_MORE))
        {
            bool paused;

            result = channel->getPaused(&paused);
            ERRCHECK(result);

            paused = !paused;

            result = channel->setPaused(paused);
            ERRCHECK(result);
        }

        if (Common_BtnPress(BTN_ACTION1))
        {
            bool active;

            result = dsplowpass->getActive(&active);
            ERRCHECK(result);

            if (active)
            {
                result = mastergroup->removeDSP(dsplowpass);
                ERRCHECK(result);
            }
            else
            {
                result = mastergroup->addDSP(0, dsplowpass, 0);
                ERRCHECK(result);
            }
        }

        if (Common_BtnPress(BTN_ACTION2))
        {
            bool active;

            result = dsphighpass->getActive(&active);
            ERRCHECK(result);

            if (active)
            {
                result = mastergroup->removeDSP(dsphighpass);
                ERRCHECK(result);
            }
            else
            {
                result = mastergroup->addDSP(0, dsphighpass, 0);
                ERRCHECK(result);
            }
        }

        if (Common_BtnPress(BTN_ACTION3))
        {
            bool active;

            result = dspecho->getActive(&active);
            ERRCHECK(result);

            if (active)
            {
                result = mastergroup->removeDSP(dspecho);
                ERRCHECK(result);
            }
            else
            {
                result = mastergroup->addDSP(0, dspecho, 0);
                ERRCHECK(result);

                result = dspecho->setParameterFloat(FMOD_DSP_ECHO_DELAY, 50.0f);
                ERRCHECK(result);
            }
        }

        if (Common_BtnPress(BTN_ACTION4))
        {
            bool active;

            result = dspflange->getActive(&active);
            ERRCHECK(result);

            if (active)
            {
                result = mastergroup->removeDSP(dspflange);
                ERRCHECK(result);
            }
            else
            {
                result = mastergroup->addDSP(0, dspflange, 0);
                ERRCHECK(result);
            }
        }

        result = system->update();
        ERRCHECK(result);

        {
            bool paused = 0;
            bool dsplowpass_active;
            bool dsphighpass_active;
            bool dspecho_active;
            bool dspflange_active;

            dsplowpass   ->getActive(&dsplowpass_active);
            dsphighpass  ->getActive(&dsphighpass_active);
            dspecho      ->getActive(&dspecho_active);
            dspflange    ->getActive(&dspflange_active);

            if (channel)
            {
                result = channel->getPaused(&paused);
                if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
                {
                    ERRCHECK(result);
                }
            }

            Common_Draw("==================================================");
            Common_Draw("Effects Example.");
            Common_Draw("Copyright (c) Firelight Technologies 2004-2014.");
            Common_Draw("==================================================");
            Common_Draw("");
            Common_Draw("Press %s to pause/unpause sound", Common_BtnStr(BTN_MORE));
            Common_Draw("Press %s to toggle dsplowpass effect", Common_BtnStr(BTN_ACTION1));
            Common_Draw("Press %s to toggle dsphighpass effect", Common_BtnStr(BTN_ACTION2));
            Common_Draw("Press %s to toggle dspecho effect", Common_BtnStr(BTN_ACTION3));
            Common_Draw("Press %s to toggle dspflange effect", Common_BtnStr(BTN_ACTION4));
            Common_Draw("Press %s to quit", Common_BtnStr(BTN_QUIT));
            Common_Draw("");
            Common_Draw("%s : lowpass[%c] highpass[%c] echo[%c] flange[%c]", 
                    paused              ? "Paused " : "Playing",
                    dsplowpass_active   ? 'x' : ' ',
                    dsphighpass_active  ? 'x' : ' ',
                    dspecho_active      ? 'x' : ' ',
                    dspflange_active    ? 'x' : ' ');
        }

        Common_Sleep(50);
    } while (!Common_BtnPress(BTN_QUIT));

    /*
        Shut down
    */
    result = sound->release();
    ERRCHECK(result);
    result = system->close();
    ERRCHECK(result);
    result = system->release();
    ERRCHECK(result);

    Common_Close();

    return 0;
}
