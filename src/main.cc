#include <stdio.h>
#include "log.h"
#include "util.h"

int main()
{
    mysrv::Logger::ptr logger(new mysrv::Logger);
    logger->AddAppender(mysrv::LogAppender::ptr(new mysrv::StdoutLogAppender));

    mysrv::LogEvent::ptr event(new mysrv::LogEvent(logger,__FILE__, __LINE__, 0 , 1 , 2, time(0),"sun"));

    logger->log(mysrv::LogLevel::DEBUG , event);
    return 0;
}


