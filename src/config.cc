#include "config.h"

namespace mysrv{

Config::ConfigVarMap  Config::s_datas;

ConfigVarBase::ptr Config::LookupBase(const std::string &name ){
     auto it =   s_datas.find(name);
     return it ==  s_datas.end() ? nullptr : it->second;
}

void  ListAllMember(const std::string& prefix ,
                                                    const YAML::Node& node,
                                                  std::list<std::pair<std::string, const YAML::Node> > & output){
                        if(prefix.find_first_not_of("abcdefghijklmnopqrstuvwxyz._1234567890")
                                        != std::string::npos)
                        {
                                std::stringstream ss;
                                ss << node ;
                                std::string msg("Config invalid name: ");
                                msg = msg + prefix + " : " + ss.str();
                                MYSER_LOG_ERROR(MYSRV_LOG_ROOT, msg);
                                return;
                        }
                        output.push_back(std::make_pair(prefix,node));
                        if(node.IsMap())
                        {
                                for(auto it = node.begin();
                                    it != node.end() ; ++it)
                                    {
                                        ListAllMember(prefix.empty() ? it->first.Scalar() : ( prefix + "." + it->first.Scalar()) , it->second , output);
                                    }
                        }
 }


void Config::LoadFromYaml(const  YAML::Node& root){
            std::list<std::pair<std::string ,const YAML::Node > >  output;

            ListAllMember("",root,output);

            for(auto & i : output)
            {
                std::string key = i.first;
                if(key.empty()){
                    continue;
                }

                std::transform( key.begin() , key.end() , key.begin() , ::tolower);
                ConfigVarBase::ptr var = LookupBase(key);

                 if(var) {
                     if(i.second.IsScalar())  {
                         var->fromString(i.second.Scalar());
                     } else{
                         std::stringstream ss;
                         ss << i.second;

                         var->fromString( ss.str() );
                     }
                 }

            }
}

}
