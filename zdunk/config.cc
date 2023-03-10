#include "config.h"

namespace zdunk
{
    // Config::ConfigVarMap Config::s_datas;

    ConfigVarBase::ptr Config::LookupBase(const std::string &name)
    {
        auto it = GetDatas().find(name);
        return it == GetDatas().end() ? nullptr : it->second;
    }

    static void ListAllMember(const std::string &prefix, const YAML::Node &node, std::list<std::pair<std::string, YAML::Node>> &output)
    {
        if (prefix.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0123456789") != std::string::npos)
        {
            LOG_ERROR(LOG_ROOT()) << "Config invalid name" << prefix << " : " << node;
        }
        output.push_back(std::make_pair(prefix, node));
        if (node.IsMap())
        {
            for (auto it = node.begin(); it != node.end(); it++)
            {
                ListAllMember(prefix.empty() ? it->first.Scalar() : prefix + "." + it->first.Scalar(), it->second, output);
            }
        }
    }

    void Config::LoadFromYaml(const YAML::Node &root)
    {
        std::list<std::pair<std::string, YAML::Node>> all_node;
        ListAllMember("", root, all_node);

        for (auto &i : all_node)
        {
            std::string key = i.first;
            if (key.empty())
            {
                continue;
            }

            std::transform(key.begin(), key.end(), key.begin(), ::tolower);
            ConfigVarBase::ptr var = LookupBase(key);

            if (var)
            {
                if (i.second.IsScalar())
                {
                    var->fromString(i.second.Scalar());
                }
                else
                {
                    std::stringstream ss;
                    ss << i.second;
                    var->fromString(ss.str());
                }
            }
        }

        // 测试代码
        // for (auto &i : all_node)
        //{
        //    std::stringstream ss;
        //    ss << i.second;
        //    std::cout << "*****************************************" << std::endl;
        //    std::cout << "[first]:" << i.first << std::endl
        //              << "[second]:" << ss.str() << std::endl;
        //}
    }
}