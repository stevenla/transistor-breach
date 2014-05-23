/*
    fmod_studio.hpp - FMOD Studio API
    Copyright (c), Firelight Technologies Pty, Ltd. 2014.

    This header defines the C++ API. If you are programming in C use fmod_studio.h.
*/

#ifndef FMOD_STUDIO_HPP
#define FMOD_STUDIO_HPP

#include "fmod_studio_common.h"
#include "fmod_studio.h"

#include "fmod.hpp"

namespace FMOD
{

namespace Studio
{
    typedef FMOD_GUID ID;

    class System;

    class Bank;

    class MixerStrip;
    class EventDescription;

    class EventInstance;
    class ParameterInstance;
    class CueInstance;

    template <class T_Element>
    class Iterator
    {
    public:
        class Impl;
        F_API Iterator();
        F_API Iterator(Impl *);
        F_API ~Iterator();

        // Element access
        bool F_API isValid() const;                                   // True if the iterator is pointing to a valid element.
        FMOD_RESULT F_API currentElement(T_Element *element) const;   // The element the iterator is currently pointing to.

        // Traversal
        FMOD_RESULT F_API advance();                                  // Advance to the next element.

    private:
        Iterator(const Iterator&) { } // disabled
        void operator==(const Iterator&) { } // disabled

        Impl *mImpl;
    };

    typedef Iterator<EventInstance> EventInstanceIterator;

    template <class T_Derived> class HandleFactory;
    template <class T_Derived> class ImplBase;

    template <class T_Derived>
    class Handle
    {
    public:
        // Handle validity
        bool F_API isValid() const;

        class Impl;

    protected:
        F_API Handle();
        F_API Handle(const Handle& other);
        void F_API operator=(const Handle& other);
        ~Handle() { if (mImpl) setImpl(0); }

        void F_API setImpl(Impl *impl);
        Impl *impl() const { return mImpl; }

    private:
        friend class HandleFactory<T_Derived>;
        friend class ImplBase<T_Derived>;
        Impl *mImpl;
        void *mData[4];
    };

    FMOD_RESULT F_API parseID(const char *idString, ID *id);

#ifdef __MACH__
  #pragma GCC visibility push(default)
#endif

    class System : public Handle<System>
    {
    public:
        static FMOD_RESULT F_API create(System *system, unsigned int headerVersion = FMOD_VERSION);
        FMOD_RESULT F_API initialize(int maxchannels, FMOD_STUDIO_INITFLAGS studioFlags, FMOD_INITFLAGS flags, void *extradriverdata);
        FMOD_RESULT F_API release();

        // Playback processing
        FMOD_RESULT F_API update();

        // Low-level API access
        FMOD_RESULT F_API getLowLevelSystem(FMOD::System **system) const;

        // Asset retrieval
        // These functions load data from registered banks if necessary, unless mode is set to FMOD_STUDIO_LOAD_PROHIBITED.
        FMOD_RESULT F_API getEvent(const ID *id, FMOD_STUDIO_LOADING_MODE mode, EventDescription *event) const;
        FMOD_RESULT F_API getMixerStrip(const ID *id, FMOD_STUDIO_LOADING_MODE mode, MixerStrip *mixerStrip) const;
        FMOD_RESULT F_API getBank(const ID *id, Bank *bank) const;

        // Path lookup
        FMOD_RESULT F_API lookupEventID(const char *path, ID *id) const;
        FMOD_RESULT F_API lookupBusID(const char *path, ID *id) const;

        // Listener control
        FMOD_RESULT F_API getListenerAttributes(FMOD_3D_ATTRIBUTES *attributes) const;
        FMOD_RESULT F_API setListenerAttributes(const FMOD_3D_ATTRIBUTES *attributes);

        FMOD_RESULT F_API loadBankFile(const char *filename, Bank *bank);
        FMOD_RESULT F_API loadBankMemory(const char *buffer, int length, FMOD_STUDIO_LOAD_MEMORY_MODE mode, Bank *bank);
        FMOD_RESULT F_API loadBankCustom(const FMOD_STUDIO_BANK_INFO *info, Bank *bank);

        FMOD_RESULT F_API registerPlugin(const FMOD_DSP_DESCRIPTION *description);
        FMOD_RESULT F_API unregisterPlugin(const char *name);

        FMOD_RESULT F_API unloadAll();
    };

    class EventDescription : public Handle<EventDescription>
    {
    public:
        // Property access
        FMOD_RESULT F_API getID(ID *id) const;
        FMOD_RESULT F_API getParameterCount(int *count) const;
        FMOD_RESULT F_API getParameterByIndex(int index, FMOD_STUDIO_PARAMETER_DESCRIPTION *parameter) const;
        FMOD_RESULT F_API getParameter(const char *name, FMOD_STUDIO_PARAMETER_DESCRIPTION *parameter) const;
        FMOD_RESULT F_API getUserPropertyCount(int *count) const;
        FMOD_RESULT F_API getUserPropertyByIndex(int index, FMOD_STUDIO_USER_PROPERTY *property) const;
        FMOD_RESULT F_API getUserProperty(const char *name, FMOD_STUDIO_USER_PROPERTY *property) const;
        FMOD_RESULT F_API getLength(int *length) const;
        FMOD_RESULT F_API getMinimumDistance(float *distance) const;
        FMOD_RESULT F_API getMaximumDistance(float *distance) const;

        FMOD_RESULT F_API isOneshot(bool *oneshot) const;
        FMOD_RESULT F_API isStream(bool *isStream) const;
        FMOD_RESULT F_API is3D(bool *is3D) const;

        // Playback control
        FMOD_RESULT F_API createInstance(EventInstance *instance) const;
        FMOD_RESULT F_API enumeratePersistentInstances(EventInstanceIterator *iterator) const;

        // Convenience functions
        FMOD_RESULT F_API releaseAllInstances();

        // Callbacks
        FMOD_RESULT F_API setCallback(FMOD_STUDIO_EVENT_CALLBACK callback);
        FMOD_RESULT F_API getUserData(void **userData) const;
        FMOD_RESULT F_API setUserData(void *userData);
    };

    class EventInstance : public Handle<EventInstance>
    {
    public:
        // Property access
        FMOD_RESULT F_API getDescription(EventDescription *description) const;

        // Playback control
        FMOD_RESULT F_API getVolume(float *volume) const;
        FMOD_RESULT F_API setVolume(float volume);

        FMOD_RESULT F_API getPitch(float *pitch) const;
        FMOD_RESULT F_API setPitch(float pitch);

        FMOD_RESULT F_API get3DAttributes(FMOD_3D_ATTRIBUTES *attributes) const;
        FMOD_RESULT F_API set3DAttributes(const FMOD_3D_ATTRIBUTES *attributes);

        FMOD_RESULT F_API getPaused(bool *paused) const;
        FMOD_RESULT F_API setPaused(bool paused);

        FMOD_RESULT F_API start();
        FMOD_RESULT F_API stop(FMOD_STUDIO_STOP_MODE mode);

        FMOD_RESULT F_API getTimelinePosition(int *position) const;
        FMOD_RESULT F_API setTimelinePosition(int position);

        FMOD_RESULT F_API getPlaybackState(FMOD_STUDIO_PLAYBACK_STATE *state) const;

        FMOD_RESULT F_API getChannelGroup(ChannelGroup **group) const;

        FMOD_RESULT F_API release();

        FMOD_RESULT F_API isVirtual(bool *virtualState) const;

        FMOD_RESULT F_API getParameter(const char *name, ParameterInstance *parameter) const;
        FMOD_RESULT F_API getParameterByIndex(int index, ParameterInstance *parameter) const;
        FMOD_RESULT F_API getParameterCount(int *count) const;

        FMOD_RESULT F_API setParameterValue(const char *name, float value);
        FMOD_RESULT F_API setParameterValueByIndex(int index, float value);

        FMOD_RESULT F_API getCue(const char *name, CueInstance *cue) const;
        FMOD_RESULT F_API getCueByIndex(int index, CueInstance *cue) const;
        FMOD_RESULT F_API getCueCount(int *count) const;

        FMOD_RESULT F_API createSubEvent(const char *name, EventInstance *instance) const;

        // Loading control
        FMOD_RESULT F_API getLoadingState(FMOD_STUDIO_LOADING_STATE *state) const;

        // Callbacks
        FMOD_RESULT F_API setCallback(FMOD_STUDIO_EVENT_CALLBACK callback);

        FMOD_RESULT F_API getUserData(void **userData) const;
        FMOD_RESULT F_API setUserData(void *userData);
    };

    class CueInstance : public Handle<CueInstance>
    {
    public:
        FMOD_RESULT F_API trigger();
    };

    class ParameterInstance : public Handle<ParameterInstance>
    {
    public:
        // Property access
        FMOD_RESULT F_API getDescription(FMOD_STUDIO_PARAMETER_DESCRIPTION *description) const;

        // Playback control
        FMOD_RESULT F_API getValue(float *value) const;
        FMOD_RESULT F_API setValue(float value);
    };

    class MixerStrip : public Handle<MixerStrip>
    {
    public:
        // Playback control
        FMOD_RESULT F_API getFaderLevel(float *level) const;
        FMOD_RESULT F_API setFaderLevel(float level);

        FMOD_RESULT F_API getPaused(bool *paused) const;
        FMOD_RESULT F_API setPaused(bool paused);

        FMOD_RESULT F_API stopAllEvents(FMOD_STUDIO_STOP_MODE mode);
        
        // Low-level API access
        FMOD_RESULT F_API getChannelGroup(FMOD::ChannelGroup **channelgroup) const;

        // Loading control
        FMOD_RESULT F_API getLoadingState(FMOD_STUDIO_LOADING_STATE *state) const;
        FMOD_RESULT F_API release();
    };

    class Bank : public Handle<Bank>
    {
    public:
        // Property access
        FMOD_RESULT F_API getID(ID *id) const;

        // Loading control
        FMOD_RESULT F_API unload();
        FMOD_RESULT F_API loadSampleData();
        FMOD_RESULT F_API unloadSampleData();
        FMOD_RESULT F_API getSampleLoadingState(FMOD_STUDIO_LOADING_STATE *state) const;
    };

#ifdef __MACH__
  #pragma GCC visibility pop
#endif

} // namespace Studio

} // namespace FMOD

#endif //FMOD_STUDIO_HPP
