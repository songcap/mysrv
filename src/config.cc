#include "config.h"

namespace mysrv{

Config::ConfigVarMap  Config::s_datas;

static   ConfigVarBase LookupBase(const std::string &name ){
     auto it = s_datas.find(name);
     return it == s_datas.end() ? nullptr : it->second;
}

static void  ListAllMember(const std::string& prefix ,
                                                    const YAML::Node& node,
                                                  std::list<std::pair<std::string, const YAML::Node> > & output){
                        if(prefix.find_first_not_of("abcdefghijklmnopqrstuvwxyz._1234567890")
                                        != std::string::npos)
                        {
                            MYSER_LOG_ERROR(MYSRV_LOG_ROOT() , "Config invalid name :  " + prefix + " : " + node);
                            return;
                        }
                        output.push_back(std::make_pair(prefix,node));
                        if(node.IsMap())
                        {
                            for(auto it = node.begin();
                                it != node.end() ; ++it)
                                {
                                    ListAllMember(prefix.empty() ? it->first.Scalar()
                                      : prefix + "." + it->first.Scalar() , it->second,output );
                                }
                        }
 }

void Config::LoadFromYaml(const  YAML::Node& root){
            std::list<std::string& ,const YAML::Node> > &output;
            ListAllMember("",root,output);

            for(auto & i : output)
            {
                std::string key = output.first;
                if(key.empty()){
                    continue;
                }

                std::transform(key.begin(), key.end(),::tolower);
                ConfigVarBase::ptr var = LookupBase(key);

                 if(var) {
                     if(i.second.IsScalar())  {
                         var->fromString(i.second.Scalar());
                     }
                     else{
                         stringstream ss;
                         ss << i.second;
                         var->fromString(ss.str());
                     }
                 }
            }
}

}
