#pragma once

#ifndef __PRETTY_FUNCTION__
#include "pretty.h"
#endif

#include <cstddef>
#include <cstdlib>
#include <utility>
#include <array>
#include <cassert>

//#include <cstddef>
#include <memory>
#include <stdexcept>
#include <algorithm>

//#define USE_PRETTY 1

template <typename T, std::size_t BLOCK_SIZE>
class custom_allocator 
{
public:
    //	Allocator traits
    using 	size_type = size_t;
    using difference_type	= std::ptrdiff_t; //int;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using value_type = T;

    custom_allocator() :_allocated(nullptr), _counter(0) {
        std::cout << "map allocator constructor" << std::endl;
        _allocated = std::malloc(BLOCK_SIZE * sizeof(T));

        //	Åñëè âûäåëèòü íå óäàëîñü
        if (nullptr == _allocated)
            //	Òîãäà ó íàñ ïðîáëåìû
            throw std::bad_alloc();

        std::cout << "allocated: " << _allocated << std::endl;
    };

    ~custom_allocator() {
        std::cout << "map allocator destructor" << std::endl;
        free(_allocated);
    }

    T* allocate(std::size_t n) {
        std::cout << "allocate " << n << "*" << sizeof(T) << std::endl;
        T* res(nullptr);

        //	Âû÷èñëèì àäðåñ 
        res = reinterpret_cast <T*> (_allocated);
        res += _counter;
        ++_counter;

        std::cout << "return value: " << res << std::endl;
        return res;
    };

    template<class U, class... Args>
    void construct(U* p, Args&&... args) {
        std::cout << "construct " << p << std::endl;
        new(static_cast<void*>(p)) U(std::forward<Args>(args)...);
    }

    template<class U>
    void destroy(U* p) {
        std::cout << "destruct " << p << std::endl;
        p->~U();
    }

    template<class U> struct rebind { typedef custom_allocator<U, BLOCK_SIZE> other; };

    void deallocate(T* p, std::size_t n) {
        p = p;
        std::cout << "deallocate " << n << std::endl;
        --_counter;
        std::cout << "counter: " << _counter << std::endl;
    };
private:
    void* _allocated;
    size_t _counter;
};

/*
template <typename T, size_t MAX_SIZE>
class custom_allocator
{
private:
    constexpr static const bool FREE    = false;
    constexpr static const bool IN_USE  = true;

public: 
    using value_type        = T;
    using pointer           = T*;
    using const_pointer     = const T*;
    using reference         = T&;
    using const_reference   = const T&;
    using size_type         = size_t;
    using difference_type   = std::ptrdiff_t;   
        
    template<typename U>
    struct rebind
    {
        using other = custom_allocator<U, MAX_SIZE>;
    };
    
    custom_allocator() : _pointer_first_free(_in_use)
    {
        initialise();
    }
    
    custom_allocator([[maybe_unused]] const custom_allocator& rhs) : _pointer_first_free(_in_use)
    {
        initialise();
    }
    
    template<typename U>
    custom_allocator(const custom_allocator<U, MAX_SIZE>& rhs) : _pointer_first_free(_in_use)
    {
        initialise();
    }
    
    ~custom_allocator() { }
    
    pointer address(reference x) const { return &x; }
    
    const_pointer address(const_reference x) const { return x; }
    
    pointer allocate(size_type n, [[maybe_unused]] const_pointer cp = 0)
    {
//#ifndef USE_PRETTY
//        std::cout << "allocate: [n = " << n << "]" << std::endl;
//#else
//        std::cout << __PRETTY_FUNCTION__ << "[n = " << n << "]" << std::endl;
//#endif        
        bool* _pointer_first = _pointer_first_free;
        bool* const _pointer_end = &_in_use[_size_block];
        
        if (n == 1)
        {            
            if (_pointer_first == _pointer_end)
                throw std::bad_alloc();
            
            *_pointer_first = IN_USE;
        }
        else
        {            
            _pointer_first = std::search_n(_pointer_first, _pointer_end, static_cast<long>(n), FREE);
                        
            if (_pointer_first == _pointer_end)
            {
                throw std::bad_alloc();
            }
            
            std::fill(_pointer_first, _pointer_first + n, IN_USE);
        }
        
        if (_pointer_first == _pointer_first_free)                    
            _pointer_first_free = std::find(_pointer_first + n, _pointer_end, FREE);  // èùåì ñëåäóþùþþ ñâîáîäíóþ ÿ÷åéêó
                
        const size_t offset = std::distance(_in_use, _pointer_first) * sizeof(value_type);

        return (reinterpret_cast<pointer>(&_pointer_buffer[offset]));
    }
    
    void deallocate(pointer p, size_type n)
    {
//#ifndef USE_PRETTY
//        std::cout << "deallocate: [n  = " << n << "] " << std::endl;
//#else
//        std::cout << __PRETTY_FUNCTION__ << "[n = " << n << "]" << std::endl;
//#endif
        size_t index = std::distance(_pointer_buffer, reinterpret_cast<char*>(p)) / sizeof(value_type);    // Find the start of the range.        

        bool* _pointer_first = &_in_use[index];
        
        if (n == 1)
        {
            *_pointer_first = FREE;
        }
        else
        {
            std::fill(_pointer_first, _pointer_first + n, FREE);
        }
        
        if (_pointer_first < _pointer_first_free)        
            _pointer_first_free = _pointer_first;        
    }
    
    size_type max_size() const
    {
        return MAX_SIZE;
    }
    
    void construct(pointer p, const value_type& x)
    {
//#ifndef USE_PRETTY
//        std::cout << "construct" << std::endl;
//#else
//        std::cout << __PRETTY_FUNCTION__ << std::endl;
//#endif        
        new (p)value_type(x); // Placement 'new'
    }

    template<typename U, typename ...Args>
    void construct(U* p, Args &&...args) {
//#ifndef USE_PRETTY
//        std::cout << "construct template" << std::endl;
//#else
//        std::cout << __PRETTY_FUNCTION__ << std::endl;
//#endif
        new(p) U(std::forward<Args>(args)...);  // Placement 'new'
    }
        
    void destroy(pointer p)
    {
//#ifndef USE_PRETTY
//        std::cout << "destroy" << std::endl;
//#else
//        std::cout << __PRETTY_FUNCTION__ << std::endl;
//#endif        
        p->~value_type();   
    }

private:
    constexpr static const size_t ALIGNMENT = std::alignment_of<T>::value - 1;    
        
    void initialise()
    {        
        _pointer_buffer = reinterpret_cast<char*>((reinterpret_cast<size_t>(&_buffer[0]) + ALIGNMENT) & ~ALIGNMENT);
        
        std::fill(_in_use, _in_use + _size_block, FREE); // 
    }
    constexpr static const size_t _size_block = MAX_SIZE + 1;    
    
    char _buffer[(_size_block * sizeof(value_type)) + ALIGNMENT + 1];   // The allocation buffer. Ensure enough space for correct alignment.
    
    char* _pointer_buffer = nullptr; // Pointer to the first valid location in the buffer after alignment.
    
    bool _in_use[_size_block];  // ìàññèâ ôëàãîâ äëÿ ÿ÷ååê ïàìÿòè, êîòîðûå èñïîëüçóþòñÿ
    
    bool* _pointer_first_free = nullptr; // óêàçàòåëü íà ïåðâóþ ñâîáîäíóþ ÿ÷åéêó ïàìÿòè    
};
*/

