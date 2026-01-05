#include "TpUtlCommon.h"


namespace Tp
{
    namespace Filesystem
    {
        bool touch(const char * filePath, const bool clear)
        {
            std::filesystem::path fsFilePath = filePath;
            std::filesystem::path directoryPath = fsFilePath.parent_path();
            try
            {
                std::filesystem::create_directories(directoryPath);
            }
            catch (const std::filesystem::filesystem_error & e)
            {
                std::cerr << "Error creating directory: " << e.what() << std::endl;
            }
            std::ofstream file(filePath, clear ? std::ios_base::trunc : std::ios_base::app);
            return true;
        }
    }
}