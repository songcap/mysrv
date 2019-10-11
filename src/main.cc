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
mysrv::ConfigVar<float>::ptr g_float_value_config =
        mysrv::Config::LookUp("system.value",(float) 10.2f, "system value");

mysrv::ConfigVar<std::vector<int> >::ptr g_int_vec_config =
        mysrv::Config::LookUp("vector_val",std::vector<int>{1,2,3},"vector_list");

mysrv::ConfigVar<std::list<int > >::ptr g_int_list_config =
        mysrv::Config::LookUp("list_val", std::list<int>{3,4,5}, "list_val");

mysrv::ConfigVar<std::set<int > >::ptr g_int_set_config =
        mysrv::Config::LookUp("set_val", std::set<int>{3,4,5}, "set_val");
mysrv::ConfigVar<std::unordered_set<int > >::ptr g_int_unset_config =
        mysrv::Config::LookUp("unset_val", std::unordered_set<int>{3,4,5}, "unorder_val");



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
                    ss <<  std::string(level * 4 , ' ') << node[i] \
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

void test_config() {
    //MYSER_LOG_INFO(MYSRV_LOG_ROOT, "before :  " + g_int_value_config->toString());
    //MYSER_LOG_INFO(MYSRV_LOG_ROOT , "before :  " + g_float_value_config->toString());

    #define XX(g_var , name , prefix)  \
    { \
        auto i = g_var->getVal() ; \
        for(auto j : i) { \
                std::stringstream ss; \
                ss << #prefix <<  " " << #name << " " << j;\
                MYSER_LOG_INFO(MYSRV_LOG_ROOT , ss.str()); \
        } \
    }

    XX(g_int_vec_config,before , vec_test);
    XX(g_int_list_config, before,list_test);
    XX(g_int_set_config, before , set_test);
    XX(g_int_unset_config, before , unset_test);

    YAML::Node root = YAML::LoadFile("/home/song/mysrv/mysrv/conf/log.yaml");
    mysrv::Config::LoadFromYaml(root);

    XX(g_int_vec_config,after , vec_test);
    XX(g_int_list_config, after,list_test);
    XX(g_int_set_config, after ,set_test);
    XX(g_int_unset_config, after , unset_test);

}

int main()
{

    mysrv::Logger::ptr logger(new mysrv::Logger);
    mysrv::LogAppender::ptr appender(new mysrv::FileLogAppender("./log.txt"));
    appender->setLevel(mysrv::LogLevel::ERROR);
    logger->AddAppender(mysrv::LogAppender::ptr(new mysrv::StdoutLogAppender));
    logger->AddAppender(appender);
    test_config();
    //mysrv::LogEvent::ptr event(new mysrv::LogEvent(logger,mysrv::LogLevel::DEBUG,__FILE__, __LINE__, 0 , mysrv::GetThreadPid(), 2, time(0),"sun"));
    //event->getSS() << "hello sylar world";
    //logger->log(mysrv::LogLevel::DEBUG , event);
    //auto i =   mysrv::LoggerManager::GetInstance()->getLogger("XX");

    //std::string str = "XXXX";
    //mysrv::MYSER_LOG_INFO(i ,g_int_value_config->toString()) ;
     //mysrv::MYSER_LOG_ERROR(i ,str) ;


    sleep(1000);
    return 0;
}
