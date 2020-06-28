#ifndef _LOGGING_HPP_
#define _LOGGING_HPP_

#ifdef DEBUG_LOGGING

#ifndef DEBUG_STREAM
#define DEBUG_STREAM Serial
#endif

#define BEGIN_LOG(baud) DEBUG_STREAM.begin(baud)
#define LOG(...) DEBUG_STREAM.print(__VA_ARGS__)
#define LOGF(s) DEBUG_STREAM.print(F(s))


#else
#define BEGIN_LOG(baud)
#define LOG(...)
#define LOGF(s)
#endif

#endif
