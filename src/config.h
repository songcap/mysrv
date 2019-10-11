#ifndef   __CONFIG_H__
#define  __CONFIG_H__
#include <memory>
#include <sstream>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <yaml-cpp/yaml.h>
#include <list>
#include <set>
#include <unordered_map>
#include <map>
#include <vector>
#include <unordered_set>

#include "log.h"

namespace mysrv{

class ConfigVarBase{
public:
    typedef std::shared_ptr<ConfigVarBase> ptr;
    ConfigVarBase(std::string name , std::string des)
    :m_name(name) ,
    m_description(des){
        //将所有的ｎａｍｅ的大写字符斗转为小写
       std::transform( m_name.begin()  , m_name.end() ,  m_name.begin() , ::tolower);
    }
    virtual ~ConfigVarBase() {}
    const std::string & getName() const{ return m_name; }
    const std::string & getDescription() const{ return m_description;}

    virtual std::string toString() = 0;
    virtual bool fromString(const std::string &val) = 0;
protected:
    std::string m_name;
    std::string m_description;
};

template<class F , class T>
class LexicalCast{
public:
    T operator() (const F& v) {
        return boost::lexical_cast<T>(v);
    }
};

template<class T>
class LexicalCast<std::vector<T> , std::string> {
public:
    std::string operator() (const std::vector<T> &  v) {
         YAML::Node node;
         for(auto &i : v) {
             node.push_back(LexicalCast<T , std::string>() (i) );
         }
         std::stringstream ss;
         ss << node;
         return ss.str();
    }
};

template<class T>
class LexicalCast<std::string, std::vector<T> > {
public:
        std::vector<T> operator() (const std::string& V) {
            YAML::Node node = YAML::Load(V);
            std::vector<T> vec;
            std::stringstream ss;
            for(size_t i = 0; i < node.size() ; ++i) {
                    ss.str("");
                    ss << node[i];
                    vec.push_back(LexicalCast<std::string , T>() (ss.str())) ;
            }
            return vec;
        }
};

template<class T>
class LexicalCast<std::list<T> , std::string> {
public:
    std::string operator() (const std::list<T> &  v) {
         YAML::Node node;
         for(auto &i : v) {
             node.push_back(LexicalCast<T , std::string>() (i) );
         }
         std::stringstream ss;
         ss << node;
         return ss.str();
    }
};

template<class T>
class LexicalCast<std::string, std::list<T> > {
public:
        std::list<T> operator() (const std::string& V) {
            YAML::Node node = YAML::Load(V);
            std::list<T> vec;
            std::stringstream ss;
            for(size_t i = 0; i < node.size() ; ++i) {
                    ss.str("");
                    ss << node[i];
                    vec.push_back(LexicalCast<std::string , T>() (ss.str())) ;
            }
            return vec;
        }
};

template<class T>
class LexicalCast<std::set<T> , std::string> {
public:
    std::string operator() (const std::set<T> &  v) {
         YAML::Node node;
         for(auto &i : v) {
             node.push_back(LexicalCast<T , std::string>() (i) );
         }
         std::stringstream ss;
         ss << node;
         return ss.str();
    }
};

template<class T>
class LexicalCast<std::string, std::set<T> > {
public:
        std::set<T> operator() (const std::string& V) {
            YAML::Node node = YAML::Load(V);
            std::set<T> sets;
            std::stringstream ss;
            for(size_t i = 0; i < node.size() ; ++i) {
                    ss.str("");
                    ss << node[i];
                    sets.insert(LexicalCast<std::string , T>() (ss.str())) ;
            }
            return sets;
        }
};

template<class T>
class LexicalCast<std::unordered_set<T> , std::string> {
public:
    std::string operator() (const std::unordered_set<T> &  v) {
         YAML::Node node;
         for(auto &i : v) {
             node.push_back(LexicalCast<T , std::string>() (i) );
         }
         std::stringstream ss;
         ss << node;
         return ss.str();
    }
};

template<class T>
class LexicalCast<std::string, std::unordered_set<T> > {
public:
        std::unordered_set<T> operator() (const std::string& V) {
            YAML::Node node = YAML::Load(V);
            std::unordered_set<T> vec;
            std::stringstream ss;
            for(size_t i = 0; i < node.size() ; ++i) {
                    ss.str("");
                    ss << node[i];
                    vec.insert(LexicalCast<std::string , T>() (ss.str())) ;
            }
            return vec;
        }
};

//具体的配置项
/**
1.FromStr需要进行序列化 希望实现一个括号的操作符重载来实现对不同类型的转换
2.ToStr需要进行反序列化
**/
template<class T , class FromStr = LexicalCast<std::string, T >  , class ToStr = LexicalCast<T , std::string>  >
class ConfigVar : public ConfigVarBase  {
public:
        typedef  std::shared_ptr<ConfigVar> ptr;
        ConfigVar(const std::string & name,
             const T& value,
             const std::string &description = "" )
             : ConfigVarBase(name,description),
             m_val(value)  {

        }

        std::string toString()  override  {
           try{
                        return ToStr()(m_val);
                        //return  boost::lexical_cast<std::string> (m_val);
                 }catch(std::exception &e){
                        std::string str = "configVar::toString exception"
                       + std::string(e.what()) +  " convert : " + typeid(m_val).name()+ " to string";
                         MYSER_LOG_ERROR(MYSRV_LOG_ROOT ,str ) ;
                 }
                return "";
        }


        bool  fromString(const std::string &val) override{
         try{
                    //setValue(boost::lexical_cast<T>(val));
                    setValue(FromStr()(val));
             }catch(std::exception &e){
                     MYSER_LOG_ERROR(MYSRV_LOG_ROOT ,  "configVar::toString exception"
                    + std::string(e.what()) + " convert : string to" + typeid(m_val).name() ) ;
             }
        }
        const T & getVal()  const   {     return m_val;}
        void setValue(const T& val)   {   m_val = val;}
private:
    T m_val;
};

class Config{
public:
    //这个容器也是存储的是一些配置项的配置信息
    typedef  std::map<std::string, ConfigVarBase::ptr > ConfigVarMap;

    template<class T>
    static typename ConfigVar<T>::ptr LookUp(const std::string &name,
            const T& default_val, const std::string & description = "")  {

            auto tmp = _Lookup<T>(name);
            if(!tmp) {
                MYSER_LOG_ERROR(MYSRV_LOG_ROOT , "LookUP : " + name + "don't exist ");
            }
            else{
                MYSER_LOG_INFO(MYSRV_LOG_ROOT , "LookUP " + name + "exist");
                return tmp;
            }
           //如果有非法字符
            if(name.find_first_not_of("abcdefghijklmnopqrstuvwxyz0123456789._") != std::string::npos) {
                MYSER_LOG_ERROR(MYSRV_LOG_ROOT , "invalid argument : " + name);
                throw std::invalid_argument(name);
            }

            typename ConfigVar<T>::ptr v(new ConfigVar<T>(name,default_val,description));
            s_datas[name] = v;
            return v;
    }

    template<class T>
    static typename ConfigVar<T>::ptr _Lookup(const std::string &name){
            auto it = s_datas.find(name);
            if(it == s_datas.end()){
                return NULL;
            }
            return std::dynamic_pointer_cast<ConfigVar<T> > (it->second);
    }

    static void LoadFromYaml(const  YAML::Node& root);

    static   ConfigVarBase::ptr LookupBase(const std::string &name );

private:
    static  ConfigVarMap s_datas;
};

}

#endif /**__CONFIG_H__**/
