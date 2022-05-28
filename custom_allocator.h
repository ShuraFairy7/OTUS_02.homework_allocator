#pragma once

#ifndef __PRETTY_FUNCTION__
#include "pretty.h"
#endif

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <algorithm>

//#define USE_PRETTY 1

template <typename _T, std::size_t MEMORY_BLOCK_SIZE>
class custom_allocator 
{
public:    
    using size_type         = size_t;
    using difference_type   = int;
    using pointer           = _T*;
    using const_pointer     = const _T*;
    using reference         = _T&;
    using const_reference   = const _T&;
    using value_type        = _T;

    custom_allocator() :_allocated_block(nullptr), _counter_slot(0)
    {        
#ifndef USE_PRETTY
        std::cout << "allocator constructor" << std::endl;
#else
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        _allocated_block = std::malloc(MEMORY_BLOCK_SIZE * sizeof(value_type));
        
        if (nullptr == _allocated_block)
            throw std::bad_alloc();

        std::cout << "allocated: " << _allocated_block << std::endl;
    };

    ~custom_allocator() 
    {        
#ifndef USE_PRETTY
        std::cout << "allocator destructor" << std::endl;
#else
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif       
        free(_allocated_block);
    }

    pointer allocate(std::size_t n)
    {
#ifndef USE_PRETTY
        std::cout << "allocate: [n = " << n << "]" << std::endl;
#else
        std::cout << __PRETTY_FUNCTION__ << "[n = " << n << "]" << std::endl;
#endif        
        pointer result_value(nullptr);            
        result_value = reinterpret_cast <pointer> (_allocated_block);
        result_value += _counter_slot;
        ++_counter_slot;

        std::cout << "return value: " << result_value << std::endl;
        return result_value;
    };

    template<class U, class... Args>
    void construct(U* p, Args&&... args) 
    {        
#ifndef USE_PRETTY
        std::cout << "construct " << p << std::endl;
#else
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif        
        new(static_cast<void*>(p)) U(std::forward<Args>(args)...);  // Placement 'new'
    }

    template<class U>
    void destroy(U* p) 
    {        
#ifndef USE_PRETTY
        std::cout << "destroy " << p << std::endl;
#else
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        p->~U();
    }

    template<class U> struct rebind { using other = custom_allocator<U, MEMORY_BLOCK_SIZE>; };

    void deallocate([[maybe_unused]] pointer p, std::size_t n)
    {           
#ifndef USE_PRETTY
        std::cout << "deallocate: [n  = " << n << "] " << std::endl;
#else
        std::cout << __PRETTY_FUNCTION__ << "[n = " << n << "]" << std::endl;
#endif
        --_counter_slot;
        std::cout << "counter: " << _counter_slot << std::endl;
    };
private:
    void* _allocated_block;
    size_t _counter_slot;
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

