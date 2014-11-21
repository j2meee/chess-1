#include "utils.h"

std::string utils::getTimestamp()
{
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%X", &tstruct);
    return buf;
}

long utils::getTime()
{
    #ifdef WIN32
    SYSTEMTIME time;
    GetSystemTime(&time);
    return time.wMilliseconds;
    #else
    return std::chrono::system_clock::now().time_since_epoch()/std::chrono::milliseconds(1);
    #endif
}

bool utils::inputWaiting()
{
    fd_set fds;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 1;
    FD_ZERO(&fds);
    FD_SET(fileno(stdin), &fds);
    return select(sizeof(fds)*8, &fds, NULL, NULL, &tv);
}
