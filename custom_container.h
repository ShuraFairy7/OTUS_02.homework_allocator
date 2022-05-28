#pragma once

#include <memory>
#include <stdexcept>
#include <iterator>

//	Тип для узла списка
template <typename T>
struct list_node {
	list_node* _next;
	T	_val;

	list_node() { _next = nullptr; _val = T(); };
};

//	Константный итератор для обхода списка
template<typename T>
struct onewaylist_const_iterator
{//	iterator_traits
	using _Self = onewaylist_const_iterator<T>;

	//using difference_type = ptrdiff_t;
	using iterator_category = std::forward_iterator_tag;
	using value_type = T;
	using pointer = const T*;
	using reference = const T&;
	//	Значение соответствующего узла в списке
	list_node<T>* _data;
	//	Конструктор по умолчанию
	onewaylist_const_iterator() :_data(nullptr) {

	};
	//	Конструктор с параметром. Для создания внутри контейнера
	explicit onewaylist_const_iterator(list_node<T>* ptrNode) :_data(ptrNode) {};

public:
	//	Получение ссылки на хранимый элемент
	reference operator*() const {
		return _data->_val;
	}
	//	Получение указателя на хранимый элемент
	pointer operator->() const { return &(_data->_val); }

	//Переход к следующему элементу контейнера
	_Self& operator++() {
		if (nullptr == _data)	return *this;

		else _data = _data->_next;

		return *this;
	}

	//Сравнение итераторов для прохода по циклу
	bool operator==(const _Self& rhs) const {
		return _data == rhs._data;
	}

	bool operator!=(const _Self& rhs) const
	{
		return _data != rhs._data;
	}
};

//	Сам контейнер. Односвязный список.
template<typename T, //	хранимый тип
	class Allocator = std::allocator<T>> // Распределитель памяти
	class custom_forward_list {
	//	Типы для использования внутри контейнера
	using pointer = T*;

	//	Элемент списка		
	using ptrNode = list_node<T>*;

	//	Аллокатор для узлов списка
	typedef  typename Allocator::template rebind<list_node <T> >::other node_allocator;

	//Указатель на первый элемент списка
	ptrNode	_first;
	//	Указатель на последний элемент списка
	ptrNode	_last;

	//	Узел для реализации окончания списка
	list_node<T> node_end;

	//	Аллокатор для размещения элементов
	node_allocator 	_allocator;

	//	Выделяем память и размещаем элемент в контейнере
	ptrNode	_createNode(const T& new_val) {
		//	Выделение памяти для узла списка
		auto ptr = _allocator.allocate(1);
		//std::cout << "allocate in: " << ptr << std::endl;
		//	Конструирование узла списка
		_allocator.construct(ptr);

		//	Задаём значения элементу списка
		ptr->_val = new_val;
		ptr->_next = nullptr;
		//std::cout << "next: " << ptr->_next << std::endl;
		return ptr;
	};

	//	уничтожаем элемент списка
	//void _destructElement(ptrElement rem){}

	public:
		custom_forward_list() :_first(nullptr), _last(nullptr), _allocator() {
			//std::cout << "sizeof list_node: " << sizeof(list_node<T>) << std::endl;
			//std::cout << "sizeof T: " << sizeof(T) << std::endl;
		};

		//	Почистить всю выделенную память
		~custom_forward_list() {
			auto curr = _first;
			while (curr != nullptr) {
				//std::cout << "curr: " << curr << " next: " << curr->_next << std::endl;
				auto ptrRemove(curr);
				curr = curr->_next;
				//	По очереди удаляем все элементы списка

				//std::cout << "deallocate: " << ptrRemove << std::endl;
				//std::cout << "next: " << ptrRemove->_next << std::endl;
				_allocator.destroy(ptrRemove);
				_allocator.deallocate(ptrRemove, 1);
			}
		};

		//	Добавляем элемент в конец списка		
		void push_back(const T& value) {
			//	Выделяем память под элемент
			auto newElement = _createNode(value);

			//	Если элементов ещё нет - добавляем первый
			if (nullptr == _last) {
				_first = newElement;
				_last = newElement;
			}
			else {
				//	Если элементы уже есть - редактируем last	
				_last->_next = newElement;
				//std::cout << "prev: " << _last << " next: " << _last->_next << " _last: " << newElement << std::endl;
				_last = newElement;
				_last->_next = nullptr;
			}
		};

		onewaylist_const_iterator<T> cbegin() const noexcept {
			return onewaylist_const_iterator<T>(_first);
		}
		onewaylist_const_iterator<T> cend() noexcept {
			return onewaylist_const_iterator<T>();
		}
};


/*
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
*/



