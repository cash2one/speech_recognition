
#ifndef _IFLY_STT_H_
#define _IFLY_STT_H_
#include "Chain.h"


class IflySTT : public Chain
{
public:
    IflySTT();

    ~IflySTT();

    int open();

    int close();

    int stt(void *data, int len);

    int next(unsigned int cmd, void *data, int len);

    int getText();

    bool getRun() { return run_; }

protected:
    bool run_;

    int audio_stat_;

    const char* session_id_;
};

#endif
