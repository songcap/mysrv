#include <stdio.h>
#include "log.h"
#include "util.h"
#include <iostream>

using namespace std;

int main()
{
    mysrv::Logger::ptr logger(new mysrv::Logger);
    logger->AddAppender(mysrv::LogAppender::ptr(new mysrv::StdoutLogAppender));

    //mysrv::LogEvent::ptr event(new mysrv::LogEvent(logger,mysrv::LogLevel::DEBUG,__FILE__, __LINE__, 0 , mysrv::GetThreadPid(), 2, time(0),"sun"));
    //event->getSS() << "hello sylar world";
    //logger->log(mysrv::LogLevel::DEBUG , event);
    mysrv::MYSER_LOG_INFO(logger , "test marco") ;
    return 0;
}
