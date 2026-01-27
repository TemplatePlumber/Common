module;

#include <string>
#include <fstream>
#include <map>
#include <set>
#include <format>
#include <algorithm> // For std::max
#include <sstream>   // For std::stringstream

#include "TpUtlString.h" 


export module TpUtlLogging; // Name your module


export namespace Tp
{
    namespace Filesystem
    {
        bool touch(const char * filePath, const bool clear = false)
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
    }

    struct LogFile
    {
        const char * _fileName = "";
        LogFile(const char * fileName) : _fileName(fileName)
        {
            std::ofstream file(_fileName, std::ios_base::trunc);
        };
        
        template <typename ... Ts>
        void write(const std::format_string<Ts...> fmt, Ts && ... args)
        {
            // Note: Your original code used 'log', but it wasn't defined. 
            // Assuming you meant Filesystem::append or a local helper.
            Filesystem::append(_fileName, fmt, std::forward<Ts>(args)...);
        }
    };

    struct StateFile
    {
        const char * _fileName = "";
        std::set<std::string> _rowLabels;
        std::set<std::string> _colLabels;
        std::map<std::string, std::map<std::string, std::string>> _valueByRowAndColumn;

        StateFile(const char * filePath) : _fileName(filePath) { 
            Tp::Filesystem::touch(filePath); 
        };
        
        template <typename COL_T, typename ROW_T, typename VAL_T>
        void set(const COL_T & col, const ROW_T & row, const VAL_T & value)
        {
            const auto rowStr = Tp::toString(row);
            const auto colStr = Tp::toString(col);
            
            _rowLabels.insert(rowStr);
            _colLabels.insert(colStr);
            _valueByRowAndColumn[rowStr][colStr] = Tp::toString(value);
            flush();
        }
        
        void flush()
        {
            std::stringstream ss;
            std::map<std::string, size_t> maxByColumn;
            size_t maxOfRowLabels = 0;

            for(const auto & r : _rowLabels)
            {
                maxOfRowLabels = std::max(maxOfRowLabels, r.length());
                for(const auto & c : _colLabels)
                {
                    auto & cmax = maxByColumn[c];
                    cmax = std::max(cmax, _valueByRowAndColumn[r][c].length());
                }
            }
            
            ss << std::format("{:>{}}", " ", maxOfRowLabels);
            for(const auto & c : _colLabels)
            {
                ss << std::format(" {:>{}}", c, maxByColumn[c]);
            }
            ss << "\n";
            
            for(const auto & r : _rowLabels)
            {
                ss << std::format("{:>{}}", r, maxOfRowLabels);
                for(const auto & c : _colLabels)
                {
                    if (_valueByRowAndColumn[r].contains(c))
                    {
                        ss << std::format(" {:>{}}", _valueByRowAndColumn[r][c], maxByColumn[c]);
                    }
                    else
                    {
                        ss << std::format(" {:>{}}", "-", maxByColumn[c]);
                    }
                }
                ss << "\n";
            }
            
            Filesystem::overwrite(_fileName, "{}", ss.str());
        }
    };
}