#pragma once

#include <iostream>
#include <filesystem>
#include <fstream>

namespace Tp
{
    namespace Filesystem
    {
        inline bool touch(const char * filePath, const bool clear = false)
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
        
        template <typename ... Ts>
        void append(const char * filePath, const std::format_string<Ts...> fmt, Ts && ... args)
        {
            const std::string str = std::format(fmt, std::forward<Ts>(args) ...);
            std::ofstream file(filePath, std::ios_base::app);
            if(file.is_open())
            {
                file << str.c_str() << std::endl;
            }
        }
        
        template <typename ... Ts>
        void overwrite(const char * filePath, const std::format_string<Ts...> fmt, Ts && ... args)
        {
            const std::string str = std::format(fmt, std::forward<Ts>(args) ...);
            std::ofstream file(filePath, std::ios_base::trunc);
            if(file.is_open())
            {
                file << str.c_str() << std::endl;
            }
        }
        
        std::string getBasename(const std::string & fileName);
    }
}