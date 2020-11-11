
#include <pthread.h>
#include <stdio.h>
#include <iosfwd>
#include <cstring>
#include "include/StreamEngine.h"
#include "include/MLog.h"

#define LOG_TAG "StreamEngine"
const int AAUDIO_NANOS_PER_MILLISECOND = 1000000;
/*note 3. 路径需要在应用包名下，其他路径没有权限*/
const char *filePath = "/data/data/com.example.erfan/";
FILE *filp = nullptr;

FILE *createFile() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char *fileName = new char[128];
    sprintf(fileName, "%s%d%d%d%s", filePath, ltm->tm_hour, ltm->tm_min, ltm->tm_sec, ".pcm");
    filp = fopen(fileName, "abw+");
    delete[] fileName;
    if (filp == nullptr) {
        LOGE("%s open file %s failed", __func__, filePath);
        return nullptr;
    }
    return filp;
}

aaudio_data_callback_result_t inDataCallback(
        AAudioStream *stream,
        void *userData,
        void *audioData,
        int32_t numFrames) {
    //fwrite(audioData, 4, numFrames, filp);   /*note 1. 16bit->2bytes * channel->2,所以这里每帧是4个字节*/
    AAudioStream *loutputStream = static_cast<AAudioStream *>(userData);
    aaudio_result_t result = AAudioStream_write(loutputStream, audioData, numFrames, 0);
    if (result != AAUDIO_OK) {
        LOGE("%s output stream write failed", __func__);
    }
    return AAUDIO_CALLBACK_RESULT_CONTINUE;
}

StreamEngine::StreamEngine() {
    //create input Builder
    result = AAudio_createStreamBuilder(&inputBuilder);
    if (result != AAUDIO_OK) {
        LOGE("%s create Stream inputBuilder failed", __func__);
    }
    AAudioStreamBuilder_setDirection(inputBuilder, AAUDIO_DIRECTION_INPUT); //设置流的方向
    AAudioStreamBuilder_setDeviceId(inputBuilder, AAUDIO_UNSPECIFIED);
    AAudioStreamBuilder_setSampleRate(inputBuilder, 48000);
    AAudioStreamBuilder_setChannelCount(inputBuilder, 2);
    AAudioStreamBuilder_setFormat(inputBuilder, AAUDIO_FORMAT_PCM_I16);
    AAudioStreamBuilder_setPerformanceMode(inputBuilder, AAUDIO_PERFORMANCE_MODE_LOW_LATENCY);

    //create output Builder
    result = AAudio_createStreamBuilder(&outputBuilder);
    if (result != AAUDIO_OK) {
        LOGE("%s create Stream outputBuilder failed", __func__);
    }
    AAudioStreamBuilder_setDirection(outputBuilder, AAUDIO_DIRECTION_OUTPUT);
    AAudioStreamBuilder_setDeviceId(outputBuilder, AAUDIO_UNSPECIFIED);
    AAudioStreamBuilder_setSampleRate(outputBuilder, 48000);
    AAudioStreamBuilder_setFormat(outputBuilder, AAUDIO_FORMAT_PCM_I16);
    AAudioStreamBuilder_setChannelCount(outputBuilder, 2);
    AAudioStreamBuilder_setPerformanceMode(outputBuilder, AAUDIO_PERFORMANCE_MODE_LOW_LATENCY);
}

StreamEngine::~StreamEngine() {
    if (filp != nullptr) {
        fclose(filp);
        filp = nullptr;
    }
}

void StreamEngine::start() {
    //createFile();
    //create output stream
    result = AAudioStreamBuilder_openStream(outputBuilder, &outputStream);
    if (result != AAUDIO_OK) {
        LOGE("%s open outputStream failed", __func__);
    }
    result = AAudioStream_requestStart(outputStream);
    if (result != AAUDIO_OK) {
        LOGE("%s outputStream request Start failed", __func__);
    }

    /*note 2. 设置数据回调函数，在使用AAudioStream_read API读取数据时报错了，原因以后分析*/
    AAudioStreamBuilder_setDataCallback(inputBuilder, inDataCallback, outputStream);

    //create input stream
    result = AAudioStreamBuilder_openStream(inputBuilder, &inputStream);
    if (result != AAUDIO_OK) {
        LOGE("%s open inputStream failed", __func__);
    }
    result = AAudioStream_requestStart(inputStream);
    if (result != AAUDIO_OK) {
        LOGE("%s inputStream request Start failed", __func__);
    }
}

void StreamEngine::stop() {
    aaudio_stream_state_t inputState = AAUDIO_STREAM_STATE_CLOSING;
    int64_t timeoutNanos = 100 * AAUDIO_NANOS_PER_MILLISECOND;
    aaudio_stream_state_t nextState = AAUDIO_STREAM_STATE_UNINITIALIZED;

    if (inputStream != nullptr) {
        result = AAudioStream_requestStop(inputStream);
        if (result != AAUDIO_OK) {
            LOGE("%s inputStream requestStop failed", __func__);
        }

        AAudioStream_waitForStateChange(inputStream, inputState, &nextState, timeoutNanos);
        if (nextState == AAUDIO_STREAM_STATE_STOPPED) {
            result = AAudioStream_close(inputStream);
            if (result == AAUDIO_OK) {
                LOGD("%s inputStream close successful", __func__);
            }
        }
    }

    if (outputStream != nullptr) {
        result = AAudioStream_requestStop(outputStream);
        if (result != AAUDIO_OK) {
            LOGE("%s outputStream requestStop failed", __func__);
        }
        AAudioStream_waitForStateChange(outputStream, inputState, &nextState, timeoutNanos);
        if (nextState == AAUDIO_STREAM_STATE_STOPPED) {
            result = AAudioStream_close(outputStream);
            if (result == AAUDIO_OK) {
                LOGD("%s outputStream close successful", __func__);
            }
        }
    }
}