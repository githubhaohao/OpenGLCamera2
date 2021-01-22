//
// Created by ByteFlow on 2019/1/14.
//

#ifndef NDKFRAMEWORK_LOGUTIL_H
#define NDKFRAMEWORK_LOGUTIL_H

#include<android/log.h>
#include <sys/time.h>
#define  LOGCATE(...)  __android_log_print(ANDROID_LOG_ERROR,"ByteFlow",__VA_ARGS__)
#define  LOGCATV(...)  __android_log_print(ANDROID_LOG_VERBOSE,"ByteFlow",__VA_ARGS__)
#define  LOGCATD(...)  __android_log_print(ANDROID_LOG_DEBUG,"ByteFlow",__VA_ARGS__)
#define  LOGCATI(...)  __android_log_print(ANDROID_LOG_INFO,"ByteFlow",__VA_ARGS__)

#define FUN_BEGIN_TIME(FUN) {\
    LOGCATE("%s:%s start", __FILE__, FUN); \
    long long t0 = GetCurrentTime();

#define FUN_END_TIME(FUN) \
    long long t1 = GetCurrentTime(); \
    LOGCATE("%s:%s cost time %ldms", __FILE__, FUN, (long)(t1-t0));}

static long long GetCurrentTime()
{
#if defined(__linux__)
	struct timeval time;
	gettimeofday(&time, NULL);
	long long curTime = ((long long)(time.tv_sec))*1000+time.tv_usec/1000;
	return curTime;
#elif defined(_WIN32)
	return  (long long)timeGetTime();
#endif
}

#endif //NDKFRAMEWORK_LOGUTIL_H
