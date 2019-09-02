#include <stdio.h>
#include <iostream>
#include <fstream>
#include <yaml-cpp/yaml.h>

#include "log.h"
#include "util.h"
#include "config.h"

using namespace std;

mysrv::ConfigVar<int>::ptr  g_int_value_config =
        mysrv::Config::LookUp("system.port" ,(int ) 8080 , "system port");


void print_yaml (const YAML::Node & node , int level)
{
        stringstream ss;
        if(node.IsScalar()) {
               ss << std::string(level * 4, ' ' ) << node.Scalar() <<  " - " << node.Type() << level;
                std::cout << ss.str() << std::endl;
                MYSER_LOG_INFO(MYSRV_LOG_ROOT,ss.str());
                ss.clear();
        }else if(node.IsNull())  {
                ss << std::string(4 * level , ' ' ) << " NULL - " << node.Type() << " - " << level;
                std::cout << ss.str() << std::endl;
                MYSER_LOG_INFO(MYSRV_LOG_ROOT, ss.str());
                ss.clear();
        }else if(node.IsMap()){
                for(auto it = node.begin() ;
                  it != node.end() ; it ++)
                {
                    ss << std::string(level * 4, ' ' )  \
                    << it->first << " - " \
                     << it -> second.Type() \
                     <<  " - " << level;
                     std::cout << ss.str() << std::endl;
                    MYSER_LOG_INFO(MYSRV_LOG_ROOT,ss.str());
                    print_yaml(it->second , level + 1);
                    ss.clear();
                }
        }
        else if(node.IsSequence()){
                for(size_t i= 0 ; i < node.size() ; i ++){
                    ss <<  std::string(level * 4 , ' ') <<  i \
                  <<  " - " <<  node[i].Type() << " - " << level;
                  std::cout << ss.str() << std::endl;
                    MYSER_LOG_INFO(MYSRV_LOG_ROOT,ss.str());
                    ss.clear();
                    print_yaml(node[i] , level + 1);
                }
        }
}

void testYaml()
{

       YAML::Node root = YAML::LoadFile("/home/song/mysrv/mysrv/conf/log.yaml");
        //stringstream ss;
        //ss << root;
        print_yaml(root , 0);
        //MYSER_LOG_INFO(MYSRV_LOG_ROOT, ss.str());
}

int main()
{
    testYaml();
    mysrv::Logger::ptr logger(new mysrv::Logger);
    mysrv::LogAppender::ptr appender(new mysrv::FileLogAppender("./log.txt"));
    appender->setLevel(mysrv::LogLevel::ERROR);
    logger->AddAppender(mysrv::LogAppender::ptr(new mysrv::StdoutLogAppender));
    logger->AddAppender(appender);
    //mysrv::LogEvent::ptr event(new mysrv::LogEvent(logger,mysrv::LogLevel::DEBUG,__FILE__, __LINE__, 0 , mysrv::GetThreadPid(), 2, time(0),"sun"));
    //event->getSS() << "hello sylar world";
    //logger->log(mysrv::LogLevel::DEBUG , event);
    auto i =   mysrv::LoggerManager::GetInstance()->getLogger("XX");

    std::string str = "XXXX";
    mysrv::MYSER_LOG_INFO(i ,g_int_value_config->toString()) ;
    mysrv::MYSER_LOG_ERROR(i ,str) ;
    return 0;
}
