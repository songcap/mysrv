#ifndef   __CONFIG_H__
#define  __CONFIG_H__
#include <memory>
#include <sstream>
#include <boost/lexical_cast.hpp>

#include "log.h"

namespace mysrv{

class ConfigVarBase{
public:
    typedef std::shared_ptr<ConfigVarBase> ptr;
    ConfigVarBase(std::string name , std::string des)
    :m_name(name) ,
    m_description(des){

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

//具体的配置项
template<class T>
class ConfigVar : public ConfigVarBase{
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
                        return boost::lexical_cast<std::string> (m_val);
                 }catch(std::exception &e){
                        std::string str = "configVar::toString exception"
                       + std::string(e.what()) +  " convert : " + typeid(m_val).name()+ " to string";
                         MYSER_LOG_ERROR(MYSRV_LOG_ROOT ,str ) ;
                 }
                return "";
        }


        bool  fromString(const std::string &val) override{
         try{
                    return boost::lexical_cast<T>(val);
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

private:
    static   ConfigVarMap s_datas;
};

}

#endif /**__CONFIG_H__**/
