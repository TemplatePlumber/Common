#include "TpUtlString.h"

namespace Tp
{
namespace String
{
    bool endsWith(const std::string & str,const std::string & suffix)
    {
        return str.size() >= suffix.size() && str.compare(str.size()-suffix.size(),suffix.size(),suffix) == 0;
    }

    std::vector<std::string> splitFields(const std::string & str,char delim)
    {
        std::vector<std::string> fields;
        std::string current;
        for(const char c : str)
        {
            if(c == delim)
            {
                fields.push_back(current);
                current.clear();
            }
            else
            {
                current += c;
            }
        }
        fields.push_back(current);
        return fields;
    }

    std::string joinFields(const std::vector<std::string> & fields,char delim)
    {
        std::string ret;
        for(size_t i=0;i<fields.size();i++)
        {
            if(i > 0)
            {
                ret += delim;
            }
            ret += fields[i];
        }
        return ret;
    }
}
}