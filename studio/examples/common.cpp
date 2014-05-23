#include "common.h"
#include "fmod_errors.h"
#include <stdarg.h>
#include <stdio.h>

void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        Common_Fatal("FMOD error %d - %s", result, FMOD_ErrorString(result));
    }
}

void Common_Fatal(const char *format, ...)
{
    char error[1024] = {0};

    va_list args;
    va_start(args, format);
    vsprintf(error, format, args);
    va_end(args);

    do
    {
        Common_Draw("A fatal error has occurred...");
        Common_Draw("");
        Common_Draw("%s", error);
        Common_Draw("");
        Common_Draw("Press %s to quit", Common_BtnStr(BTN_QUIT));

        Common_Update();
        Common_Sleep(50);
    } while (!Common_BtnPress(BTN_QUIT));

    Common_Exit(-1);
}

void Common_Draw(const char *format, ...)
{
    char string[1024] = {0};
    char *stringPtr = string;

    va_list args;
    va_start(args, format);
    vsprintf(string, format, args);
    va_end(args);

    unsigned int length = (unsigned int)strlen(string);

    do
    {
        bool consumeNewLine = false;
        unsigned int copyLength = length;

        // Search for new line characters
        char *newLinePtr = strchr(stringPtr, '\n');
        if (newLinePtr)
        {
            consumeNewLine = true;
            copyLength = (unsigned int)(newLinePtr - stringPtr);
        }

        if (copyLength > NUM_COLUMNS)
        {
            // Hard wrap by default
            copyLength = NUM_COLUMNS;

            // Loop for a soft wrap
            for (int i = NUM_COLUMNS - 1; i >= 0; i--)
            {
                if (stringPtr[i] == ' ')
                {
                    copyLength = i + 1;
                    break;
                }
            }
        }

        // Null terminate the sub string temporarily by swapping out a char
        char tempChar = stringPtr[copyLength];
        stringPtr[copyLength] = 0;
        Common_DrawText(stringPtr);
        stringPtr[copyLength] = tempChar;

        copyLength += (consumeNewLine ? 1 : 0);
        length -= copyLength;
        stringPtr += copyLength;
    } while (length > 0);
}
