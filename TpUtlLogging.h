#pragma once

#include <string>
#include <fstream>
#include <map>
#include <set>
#include <format>
#include <algorithm> // For std::max
#include <sstream>   // For std::stringstream
#include <iostream>
#include <filesystem>
#include <stack>

#include "TpUtlString.h" 
#include "TpUtlScopeGuard.h"
#include "TpUtlFilesystem.h"


namespace Tp
{
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
        std::map<std::string, std::map<std::string, std::stack<std::string>>> _stackByRowAndColumn;
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
        
        template <typename COL_T, typename ROW_T, typename VAL_T>
        void push(const COL_T & col, const ROW_T & row, const VAL_T & value)
        {
            const auto rowStr = Tp::toString(row);
            const auto colStr = Tp::toString(col);
            const auto valStr = Tp::toString(value);
            try {
                auto & stack = _stackByRowAndColumn.at(rowStr).at(colStr);
                stack.push(valStr);
            }
            catch(...){}
            set(colStr,rowStr,valStr);
        }
        
        template <typename COL_T, typename ROW_T>
        void pop(const COL_T & col, const ROW_T & row)
        {
            const auto rowStr = Tp::toString(row);
            const auto colStr = Tp::toString(col);
            std::string valStr = "";
            try {
                auto & stack = _stackByRowAndColumn.at(rowStr).at(colStr);
                if(stack.size())
                {
                    valStr = stack.top();
                    stack.pop();
                }
            }
            catch(...){}
            set(colStr,rowStr,valStr);
        }
        
        template <typename COL_T, typename ROW_T>
        Opt<std::string> get(const COL_T & col, const ROW_T & row)
        {
            const auto rowStr = Tp::toString(row);
            const auto colStr = Tp::toString(col);
            
            try
            {
                return _valueByRowAndColumn.at(rowStr).at(colStr);
            }
            catch(std::out_of_range & e){ return Tp::Undefined; }
        }

        template <typename COL_T, typename ROW_T>
        void inc(const COL_T & col, const ROW_T & row)
        {
            try
            {
                auto tblVal = this->get(col,row);
                if(tblVal == Tp::Undefined)
                {
                    throw Tp::Escape{};
                }
                
                i64 nv = Tp::fromString<i64>(tblVal.value())+1;
                this->set(col,row,nv);
            }
            catch(...)
            {
                /*
                    Act as if the initial value was zero if the 
                    prior value was undefined or non-numerical.
                */
                this->set(col,row,"1");
            }
        }
        
        void flush()
        {
            std::stringstream ss;
            std::map<std::string, size_t> maxByColumn;
            size_t maxOfRowLabels = 0;
            
            for(const auto & c : _colLabels)
            {
                auto & cmax = maxByColumn[c];
                cmax = std::max(cmax, c.length());
            }
            
            for(const auto & r : _rowLabels)
            {
                maxOfRowLabels = std::max(maxOfRowLabels, r.length());
                for(const auto & c : _colLabels)
                {
                    try
                    {
                        auto & cmax = maxByColumn[c];
                        cmax = std::max(cmax, _valueByRowAndColumn.at(r).at(c).length());
                    }
                    catch(...){}
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
//                    if (_valueByRowAndColumn.at(r).contains(c))
//                    {
//                        ss << std::format(" {:>{}}", _valueByRowAndColumn[r][c], maxByColumn[c]);
//                    }
//                    else
//                    {
//                        ss << std::format(" {:>{}}", "-", maxByColumn[c]);
//                    }
                    
                    try
                    {
                        ss << std::format(" {:>{}}", _valueByRowAndColumn.at(r).at(c), maxByColumn[c]);
                    }
                    catch(...)
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