Transistor BREACH()
-------------------

*   By: Steven La (www.stevenla.com)

WIP tool to extract audio from FMOD .bank v2 files. Works for OS X, but the
`.cpp` files should be portable.

Currently only works for Transistor by Supergiant Games. Move all .bank files
(including the .strings) to the media folder. Extracting music is hard-coded:
change the string parameter in `lookupEventID` to the name of the audio you
want to extract. Uncomment `lowLevel->setOutput()` to extract audio to a wav
file stored in the same folder as the application. The program does not quit
when the song is finished, so you'll have to estimate when the song is over
to manually quit the application.

You can alter the volumes on certain audio channels (the humming or the
singing, for example) by un-commenting the code in the do-while loop.

It's REALLY messy right now. I'm planning on making a proper command line
tool to record the music, with options to change the volume on different
channels.

