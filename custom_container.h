#pragma once

#include <memory>
#include <stdexcept>
#include <iterator>

template<typename _T, typename _Allocator = std::allocator<_T>> class custom_forward_list;
template <typename _custom_forward_list, typename _pointer, typename _reference> class custom_forward_list_iterator;

template<typename T>
class forward_list_node 
{
	using _custom_forward_list_type = custom_forward_list<T, std::allocator<T>>;
	using _Allocator				= typename _custom_forward_list_type::allocator_type;
	using value_type				= typename _Allocator::value_type;
	using pointer					= typename _Allocator::pointer;
	using const_pointer				= typename _Allocator::const_pointer;
	using reference					= typename _Allocator::reference;
	using const_reference			= typename _Allocator::const_reference;
	using size_type					= typename _Allocator::size_type;
	using difference_type			= typename _Allocator::difference_type;

public:

	forward_list_node(const T& element, forward_list_node<T>* next) : element(std::move(element)), next(next) {}
	
	~forward_list_node() { delete next; }
	
	T element;	
	forward_list_node<T>* next;
}; 

template <typename _custom_forward_list_t, typename _pointer, typename _reference>
class custom_forward_list_iterator : public std::iterator<
	std::forward_iterator_tag,
	typename _custom_forward_list_t::value_type,
	//typename _custom_forward_list_t::difference_type,
	_pointer,
	_reference> 
{

public:
	using value_type		= typename _custom_forward_list_t::value_type;
//	using difference_type	= typename _custom_forward_list_t::difference_type;
	using reference			= typename _custom_forward_list_t::reference;
	using const_reference	= typename _custom_forward_list_t::const_reference;
	using pointer			= typename _custom_forward_list_t::pointer;
	using const_pointer		= typename _custom_forward_list_t::const_pointer;
	using iterator_category	= std::forward_iterator_tag;

	using link_type = forward_list_node<value_type>*;
	using iterator_base = custom_forward_list_iterator<_custom_forward_list_t, _pointer, _reference>;

	friend class custom_forward_list<value_type, typename _custom_forward_list_t::allocator_type>;

	reference operator*() 
	{
		return (*_pointee).element;
	}

	custom_forward_list_iterator& operator++() 
	{
		_pointee = (link_type)((*_pointee).next);
		return *this;
	}

	custom_forward_list_iterator operator++(int) 
	{
		custom_forward_list_iterator _temp = *this;
		return ++ * this, _temp;
	}

	bool operator==(const iterator_base& other) const 
	{
		return _pointee == other._pointee;
	};

	bool operator!=(const iterator_base& other) const 
	{
		return _pointee != other._pointee;
	};

	custom_forward_list_iterator() { }
	~custom_forward_list_iterator() = default;

private:
	link_type _pointee;
	explicit custom_forward_list_iterator(const link_type& pointee) : _pointee(pointee) { }	
}; 

template <typename _T, typename _Allocator>
class custom_forward_list 
{
public:
	using allocator_type	= _Allocator;
	using value_type		= typename _Allocator::value_type;
	//using size_type			= typename _Allocator::size_type;
	using size_type = size_t;
	//using difference_type	= typename _Allocator::difference_type;
	using reference			= typename _Allocator::reference;
	using const_reference	= typename _Allocator::const_reference;
	using pointer			= typename _Allocator::pointer;
	using const_pointer		= typename _Allocator::const_pointer;
	using list_type			= custom_forward_list<_T, _Allocator>;
	using iterator			= custom_forward_list_iterator<list_type, pointer, reference >;
	using const_iterator	= custom_forward_list_iterator<list_type, const_pointer, const_reference>;

	using _custom_forward_list_node = forward_list_node<value_type>;
	using _node_pointer = forward_list_node<value_type>*;

	explicit custom_forward_list(const allocator_type& alloc = allocator_type()) : _Allocatorator(alloc) { init(); };

	~custom_forward_list() { delete _head; }
	bool empty() { return _head == nullptr; }
	size_type size() const { return _size; }
		
	void push_front(const_reference element) 
	{
		auto new_node = create_node(element);
		new_node->next = _head;
		_head = new_node;
		_size++;
	}

	iterator make_iterator(_node_pointer& node) 
	{
		return iterator(node);
	}

	const_iterator make_iterator(const _node_pointer& node) const noexcept 
	{
		return const_iterator(node);
	}

	iterator begin() noexcept { return make_iterator(_head); }
	iterator end() noexcept { return make_iterator(_tail); }

	const_iterator cbegin() const noexcept { return make_iterator(_head); }
	const_iterator cend() const noexcept { return make_iterator(_tail); }

	allocator_type get_Allocatorator() const { _Allocatorator; }

private:

	size_type _size;
	allocator_type _Allocatorator;
	_node_pointer _head;
	_node_pointer _tail;

	void init() 
	{
		_head = create_node(value_type{});
		_tail = _head;
		_size = 0;
	}

	_node_pointer create_node(const_reference element) 
	{
		auto new_element = _Allocatorator.allocate(1);
		_Allocatorator.construct(new_element, element);
		auto new_node = new _custom_forward_list_node(element, nullptr);
		return new_node;
	}
}; 




