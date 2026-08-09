#pragma once

#include <stddef.h>
#include <vector>
#include <algorithm>

namespace Tp
{
    template<class T>
    class Grid2DIndexer;
    
    template<class T>
    class Grid2D
    {
        
        using _TData = std::vector< std::vector<T> >;
        _TData _data;
        size_t _sizeX;
        size_t _sizeY;
        
    public:
        template<typename GRID_T> 
        friend class Grid2DIndexer;
    
        using value_type             = Tup<size_t,size_t,T>;
        using allocator_type         = std::allocator<T>;
        using size_type              = size_t;
        using difference_type        = std::ptrdiff_t;
        using reference              = value_type &;
        using const_reference        = const value_type &;
        
        
        struct iterator
        {
            Grid2D * ptr;
            size_t idx;
            
            using iterator_category = std::input_iterator_tag;
            using value_type        = T;
            using difference_type   = std::ptrdiff_t;
            using pointer           = T*;
            using reference         = std::tuple<size_t,size_t,T &>;;
            
            iterator& operator++()
            {
                idx++;
                return *this;
            }

            iterator operator++(int)
            {
                iterator tmp = *this;
                idx++;
                return tmp;
            }
            
            reference operator*() const 
            {
                auto col = idx / ptr->_sizeY;
                auto row = idx % ptr->_sizeY;
                return reference{row,col,ptr->_data.at(col).at(row)};  
            }
            
            friend bool operator==(const iterator& a, const iterator& b) { return a.idx == b.idx; };
            friend bool operator!=(const iterator& a, const iterator& b) { return a.idx != b.idx; };
        };
        
        struct const_iterator
        {
            const Grid2D * ptr;
            size_t idx;
            
            using iterator_category = std::input_iterator_tag;
            using value_type        = T;
            using difference_type   = std::ptrdiff_t;
            using pointer           = const T*;
            using reference         = std::tuple<size_t,size_t,const T &>;;
            
            const_iterator& operator++()
            {
                idx++;
                return *this;
            }

            const_iterator operator++(int)
            {
                const_iterator tmp = *this;
                idx++;
                return tmp;
            }
            
            reference operator*() const 
            {
                auto col = idx / ptr->_sizeY;
                auto row = idx % ptr->_sizeY;
                return reference{row,col,ptr->_data.at(col).at(row)}; 
            }
            
            friend bool operator==(const const_iterator& a, const const_iterator& b) { return a.idx == b.idx; };
            friend bool operator!=(const const_iterator& a, const const_iterator& b) { return a.idx != b.idx; };
        };
        
        



        Grid2D() = default;
        Grid2D(const Grid2D & o) : _data(o), _sizeX(o._sizeX), _sizeY(o._sizeY) {}
        Grid2D(Grid2D && o) : _data(o._data), _sizeX(o._sizeX), _sizeY(o._sizeY) {}
        Grid2D(size_t sizeX,size_t sizeY)
        {
            _sizeX = sizeX;
            _sizeY = sizeY;
            _data.resize(sizeY);
            for(auto & row : _data)
            {
                row.resize(sizeX);
            }
        }
        Grid2D(const Vec<Vec<T>> & val)
        {
            _data = val;
            _sizeY = val.size();
            if(val.empty())
            {
                _sizeX = 0;
                return;
            }
            
            _sizeX = val.begin()->size();
        }
        
        constexpr Grid2D & operator=(const Grid2D & o)
        { 
            _data.operator=(o._data);
            _sizeX=o._sizeX; 
            _sizeY=o._sizeY;
            return *this;
        }
        
        
        template<typename SELF>
        auto & get(this SELF && self,size_t x,size_t y)
        {
            return self._data.at(y).at(x);
        }
        
        void set(size_t x,size_t y,const T & val)
        {
            _data.at(y).at(x) = val;
        }
        
        constexpr iterator          begin()  noexcept { return iterator{this,0};            }
        constexpr iterator            end()  noexcept { return iterator{this,_sizeX*_sizeY};}
        
        constexpr const_iterator          begin() const noexcept { return const_iterator{this,0};            }
        constexpr const_iterator            end() const noexcept { return const_iterator{this,_sizeX*_sizeY};}



        constexpr size_type                size() const noexcept { return _data.size();     }
        constexpr size_type            max_size() const noexcept { return _data.max_size(); }
        constexpr bool                    empty() const noexcept { return _data.empty();    }
        
        constexpr size_t sizeX() const {return _sizeX;};
        constexpr size_t sizeY() const {return _sizeY;};

        template<typename SELF>
        constexpr auto operator[](this SELF & self,size_type x) noexcept
        {
            return Grid2DIndexer<SELF>(&self,x);
        }
        //const_iterator find(const value_type & v){return std::find(_data.begin(),_data.end(),v);}
        
        void clear(){_data.clear();}
        
        operator const _TData &() const{return _data;}
        operator _TData &(){return _data;}
        
        void resize(size_t x,size_t y)
        {
            _data.resize(y);
            for(auto & row : _data)
            {
                row.resize(x);
            }
        }


        /* 
            Constraint breaking:
                constexpr void push_back(const value_type & v) {_data.push_back(v);}
                void erase(const_iterator v){_data.erase(v);}
                constexpr reference operator[](size_type i) noexcept { return _data.operator[](i); }
        */
    };
    
    template<class T>
    class Grid2DIndexer
    {
        T * _ptr;
        size_t _x;
    public:
        
        Grid2DIndexer(T * ptr,size_t x):_ptr(ptr),_x(x) {}
        
        constexpr auto & operator[](size_t y) noexcept { return _ptr->get(_x,y); }
    };
    
    
    template<typename VALUE_T, typename FNC_T>
    auto transform(const Grid2D<VALUE_T> & container, FNC_T function)
    {
        using RET_VT = std::invoke_result_t<decltype(function), VALUE_T>;
        Grid2D<RET_VT> ret(container.sizeX(),container.sizeY());
        for(const auto & xyv : container)
        {
            const auto & [x,y,v] = xyv;
            try{ ret.set(x,y,function(xyv)); } catch(const Pass & pass){}
        }
        return ret;
    }
}