//
// Created by Administrator on 2020/11/9.
//

#ifndef MXPLAYER_STREAMENGINE_H
#define MXPLAYER_STREAMENGINE_H

#include <aaudio/AAudio.h>
class StreamEngine {
public:
    StreamEngine();
    ~StreamEngine();
    void start();
    void stop();

private:
    AAudioStreamBuilder *inputBuilder;
    AAudioStreamBuilder *outputBuilder;
    AAudioStream *inputStream;
    AAudioStream *outputStream;
    aaudio_result_t result;
};

#endif //MXPLAYER_STREAMENGINE_H
