
#ifndef _IFLY_TTS_H_
#define _IFLY_TTS_H_
#include "Chain.h"


class IflyTTS : public Chain
{
public:
    IflyTTS();

    ~IflyTTS();

    int tts(const char *text);
};

#endif
