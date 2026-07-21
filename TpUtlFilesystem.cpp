#include "TpUtlFilesystem.h"

namespace Tp
{
namespace Filesystem
{
    std::string getBasename(const std::string & fileName)
    {
        const size_t dot = fileName.find_last_of('.');
        return dot == std::string::npos ? fileName : fileName.substr(0,dot);
    }
}
}