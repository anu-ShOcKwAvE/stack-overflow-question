/*
                 THE BLAZE C++ LIBRARY
                    <blazeContainer.hp
                ---Written by: Anu Panicker---
                
Firstly, I am taking the time to thank all those people who gave me solutions to many of the seemingly impossible 
challenges I faced during writing this library. 

Secondly, I thank myself for not burning the computer 
even when the compiler said there were errors in the
comments.

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2018 Anu Panicker. 
Email: anupanicker@rocketmail.com
Insta: middle.finger.is.my.peace.sign

DISCLAIMER: When I wrote this code only me and god understood it, at the time you are reading this only god will be able to understand this, so if you find any mistakes you have to deal with it yourself.
*/

#ifndef BLAZE_HPP
#define BLAZE_HPP

#include <algorithm>
#include <cstring>
#include <iterator>
#include <memory>
#include <utility>
#include <stdexcept>


using namespace std;

namespace blaze 
{
	/*
template <bool...> struct bool_sequence {};
template <bool... Bs>
using bool_and = is_same<bool_sequence<Bs...>,
bool_sequence<(Bs || true)...>>;
  
template <typename T>
std::true_type create(T v);
  
template <typename T, typename U>
  decltype(create<U>({std::declval<T>()})) 
  test_nonnarow_conv(int);
template <typename T, typename U>
  std::false_type test_nonnarow_conv(long);
template <typename T, typename U>
  using is_nonarrow_convertible
  = decltype(test_nonnarow_conv<T, U>(0));
template <typename T, typename... Ts>
  using nonarrow_convertible
  = bool_and<is_nonarrow_convertible<Ts, T>::value...>;
 
# define REQUIRES(...)                                      \
  typename std::enable_if<(__VA_ARGS__), bool>::type = true \
  */;
	template<typename Tp, typename Alloc = std::allocator<Tp> >
class uvector : private Alloc
{
	static_assert(std::is_standard_layout<Tp>(), "A uvector can only hold classes with standard layout");
private:
#if __cplusplus > 201402L
	typedef std::allocator_traits<allocator_type>::is_always_equal allocator_is_always_equal;
#else
	typedef std::false_type allocator_is_always_equal;
#endif
public:
	typedef Tp value_type;
	typedef Alloc allocator_type;
	typedef Tp& reference;
	typedef const Tp& const_reference;
	typedef Tp* pointer;
	typedef const Tp* const_pointer;
	typedef Tp* iterator;
	typedef const Tp* const_iterator;
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
	typedef std::ptrdiff_t difference_type;
	typedef std::size_t size_t;
	typedef std::size_t size_type;
	
private:
	pointer _begin, _end, _endOfStorage;
	
public:

	explicit uvector(const allocator_type& allocator = Alloc()) noexcept
	: Alloc(allocator), _begin(nullptr), _end(nullptr), _endOfStorage(nullptr){}
	explicit uvector(size_t n) :
		_begin(allocate(n)),
		_end(_begin + n),
		_endOfStorage(_end){}
	
	uvector(size_t n, const value_type& val, const allocator_type& allocator = Alloc()) :
		Alloc(allocator),
		_begin(allocate(n)),
		_end(_begin + n),
		_endOfStorage(_end)
	{std::uninitialized_fill_n<Tp*,size_t>(_begin, n, val);}
	
	template<class InputIterator>
	uvector(InputIterator first, InputIterator last, const allocator_type& allocator = Alloc()) :
		Alloc(allocator)
	{
		construct_from_range<InputIterator>(first, last, std::is_integral<InputIterator>());
	}
	
	uvector(const uvector<Tp,Alloc>& other) :	Alloc(std::allocator_traits<Alloc>::select_on_container_copy_construction(static_cast<allocator_type>(other))),
		_begin(allocate(other.size())),
		_end(_begin + other.size()),
		_endOfStorage(_end)
	{
		memcpy(_begin, other._begin, other.size() * sizeof(Tp));
	}

	uvector(const uvector<Tp,Alloc>& other, const allocator_type& allocator) :
		Alloc(allocator),
		_begin(allocate(other.size())),
		_end(_begin + other.size()),
		_endOfStorage(_end)
	{
		memcpy(_begin, other._begin, other.size() * sizeof(Tp));
	}

	uvector(uvector<Tp,Alloc>&& other) noexcept :
		Alloc(std::move(other)),
		_begin(other._begin),
		_end(other._end),
		_endOfStorage(other._endOfStorage)
	{
		other._begin = nullptr;
		other._end = nullptr;
		other._endOfStorage = nullptr;
	}
	
	uvector(uvector<Tp,Alloc>&& other, const allocator_type& allocator) noexcept :
		Alloc(allocator),
		_begin(other._begin),
		_end(other._end),
		_endOfStorage(other._endOfStorage)
	{
		other._begin = nullptr;
		other._end = nullptr;
		other._endOfStorage = nullptr;
	}

	uvector(std::initializer_list<Tp> initlist, const allocator_type& allocator = Alloc()) :
		Alloc(allocator),
		_begin(allocate(initlist.size())),
		_end(_begin + initlist.size()),
		_endOfStorage(_end)
	{
		iterator destIter = _begin;
		for(typename std::initializer_list<Tp>::const_iterator i=initlist.begin(); i!=initlist.end(); ++i)
		{
			*destIter = *i;
			++destIter;
		}
	}
	
	~uvector() noexcept {deallocate();}
	
	uvector& operator=(const uvector<Tp,Alloc>& other)
	{
		return assign_copy_from(other, typename std::allocator_traits<Alloc>::propagate_on_container_copy_assignment());
	}
	
	uvector& operator=(uvector<Tp,Alloc>&& other) noexcept(
		std::allocator_traits<Alloc>::propagate_on_container_move_assignment::value||
		allocator_is_always_equal::value)
	{
		return assign_move_from(std::move(other), typename std::allocator_traits<Alloc>::propagate_on_container_move_assignment());
	}
	
	iterator begin() noexcept { return _begin; }
	const_iterator begin() const noexcept { return _begin; }
	iterator end() noexcept { return _end; }
	const_iterator end() const noexcept { return _end; }
	
	reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
	const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
	
	reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
	const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
	
	const_iterator cbegin() const noexcept { return _begin; }
	const_iterator cend() const noexcept { return _end; }
	
	const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }
	const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }
	
	size_t size() const noexcept { return _end - _begin; }
	size_t max_size() const noexcept { return Alloc::max_size(); }
	

	void resize(size_t n)
	{
		if(capacity() < n)
		{
			size_t newSize = enlarge_size(n);
			pointer newStorage = allocate(newSize);
			memcpy(newStorage, _begin, size() * sizeof(Tp));
			deallocate();
			_begin = newStorage;
			_endOfStorage = _begin + newSize;
		}
		_end = _begin + n;
	}
	

	void resize(size_t n, const Tp& val)
	{
		size_t oldSize = size();
		if(capacity() < n)
		{
			pointer newStorage = allocate(n);
			memcpy(newStorage, _begin, size() * sizeof(Tp));
			deallocate();
			_begin = newStorage;
			_endOfStorage = _begin + n;
		}
		_end = _begin + n;
		if(oldSize < n)
			std::uninitialized_fill<Tp*,size_t>(_begin + oldSize, _end, val);
	}
	
	size_t capacity() const noexcept { return _endOfStorage - _begin; }
	
	bool empty() const noexcept { return _begin == _end; }
	
	void reserve(size_t n)
	{
		if(capacity() < n)
		{
			const size_t curSize = size();
			pointer newStorage = allocate(n);
			memcpy(newStorage, _begin, curSize * sizeof(Tp));
			deallocate();
			_begin = newStorage;
			_end = newStorage + curSize;
			_endOfStorage = _begin + n;
		}
	}
	
	void shrink_to_fit()
	{
		const size_t curSize = size();
		if(curSize == 0)
		{
			deallocate();
			_begin = nullptr;
			_end = nullptr;
			_endOfStorage = nullptr;
		}
		else if(curSize < capacity()) {
			pointer newStorage = allocate(curSize);
			memcpy(newStorage, _begin, curSize * sizeof(Tp));
			deallocate();
			_begin = newStorage;
			_end = newStorage + curSize;
			_endOfStorage = _begin + curSize;
		}
	}
	
	Tp& operator[](size_t index) noexcept { return _begin[index]; }
	
	const Tp& operator[](size_t index) const noexcept { return _begin[index]; }
	
	Tp& at(size_t index)
	{
		check_bounds(index);
		return _begin[index];
	}

	const Tp& at(size_t index) const
	{
		check_bounds(index);
		return _begin[index];
	}
	
	Tp& front() noexcept { return *_begin; }
	const Tp& front() const noexcept { return *_begin; }
	
	Tp& back() noexcept { return *(_end - 1); }
	const Tp& back() const noexcept { return *(_end - 1); }
	
	Tp* data() noexcept { return _begin; } 
	const Tp* data() const noexcept { return _begin; }
	
	template<class InputIterator>
  void assign(InputIterator first, InputIterator last)
	{
		assign_from_range<InputIterator>(first, last, std::is_integral<InputIterator>());
	}
	
	void assign(size_t n, const Tp& val)
	{
		if(n > capacity())
		{
			iterator newStorage = allocate(n);
			deallocate();
			_begin = newStorage;
			_endOfStorage = _begin + n;
		}
		_end = _begin + n;
		std::uninitialized_fill_n<Tp*,size_t>(_begin, n, val);
	}
	
	void assign(std::initializer_list<Tp> initlist)
	{
		if(initlist.size() > capacity())
		{
			iterator newStorage = allocate(initlist.size());
			deallocate();
			_begin = newStorage;
			_endOfStorage = _begin + initlist.size();
		}
		_end = _begin + initlist.size();
		iterator destIter = _begin;
		for(typename std::initializer_list<Tp>::const_iterator i=initlist.begin(); i!=initlist.end(); ++i)
		{
			*destIter = *i;
			++destIter;
		}
	}

	void push_back(const Tp& item)
	{
		if(_end == _endOfStorage)
			enlarge(enlarge_size(1));
		*_end = item;
		++_end;
	}
	
	void push_back(Tp&& item)
	{
		if(_end == _endOfStorage)
			enlarge(enlarge_size(1));
		*_end = std::move(item);
		++_end;
	}
	
	void pop_back() {--_end;}
	
	iterator insert(const_iterator position, const Tp& item)
	{
		if(_end == _endOfStorage)
		{
			size_t index = position - _begin;
			enlarge_for_insert(enlarge_size(1), index, 1);
			position = _begin + index;
		}
		else {
			memmove(const_cast<iterator>(position)+1, position, (_end - position) * sizeof(Tp));
			++_end;
		}
		*const_cast<iterator>(position) = item;
		return const_cast<iterator>(position);
	}

	iterator insert(const_iterator position, size_t n, const Tp& val)
	{
		if(capacity() < size() + n)
		{
			size_t index = position - _begin;
			enlarge_for_insert(enlarge_size(n), index, n);
			position = _begin + index;
		}
		else {
			memmove(const_cast<iterator>(position)+n, position, (_end - position) * sizeof(Tp));
			_end += n;
		}
		std::uninitialized_fill_n<Tp*,size_t>(const_cast<iterator>(position), n, val);
		return const_cast<iterator>(position);
	}

	template <class InputIterator>
	iterator insert(const_iterator position, InputIterator first, InputIterator last)
	{
		return insert_from_range<InputIterator>(position, first, last, std::is_integral<InputIterator>());
	}

	iterator insert(const_iterator position, Tp&& item)
	{
		if(_end == _endOfStorage)
		{
			size_t index = position - _begin;
			enlarge_for_insert(enlarge_size(1), index, 1);
			position = _begin + index;
		}
		else {
			memmove(const_cast<iterator>(position)+1, position, (_end - position) * sizeof(Tp));
			++_end;
		}
		*const_cast<iterator>(position) = std::move(item);
		return const_cast<iterator>(position);
	}

	iterator insert(const_iterator position, std::initializer_list<Tp> initlist)
	{
		if(capacity() < size() + initlist.size())
		{
			size_t index = position - _begin;
			enlarge_for_insert(enlarge_size(initlist.size()), index, initlist.size());
			position = _begin + index;
		}
		else {
			memmove(const_cast<iterator>(position)+initlist.size(), position, (_end - position) * sizeof(Tp));
			_end += initlist.size();
		}
		iterator destIter = const_cast<iterator>(position);
		for(typename std::initializer_list<Tp>::const_iterator i=initlist.begin(); i!=initlist.end(); ++i)
		{
			*destIter = *i;
			++destIter;
		}
		return const_cast<iterator>(position);
	}

	iterator erase(const_iterator position)
	{
		--_end;
		memmove(const_cast<iterator>(position), position+1, (_end-position)*sizeof(Tp));
		return const_cast<iterator>(position);
	}

	iterator erase(const_iterator first, const_iterator last)
	{
		size_t n = last - first;
		_end -= n;
		memmove(const_cast<iterator>(first), first+n, (_end-first)*sizeof(Tp));
		return const_cast<iterator>(first);
	}

	void swap(uvector<Tp, Alloc>& other) noexcept
	{
		swap(other, typename std::allocator_traits<Alloc>::propagate_on_container_swap());
	}
	
	void clear() {_end = _begin;}

	template<typename... Args>
	iterator emplace(const_iterator position, Args&&... args)
	{
		if(_end == _endOfStorage)
		{
			size_t index = position - _begin;
			enlarge_for_insert(enlarge_size(1), index, 1);
			position = _begin + index;
		}
		else {
			memmove(const_cast<iterator>(position)+1, position, (_end - position) * sizeof(Tp));
			++_end;
		}
		*const_cast<iterator>(position) = Tp(std::forward<Args>(args)...);
		return const_cast<iterator>(position);
	}
	
	template<typename... Args>
	void emplace_back(Args&&... args)
	{
		if(_end == _endOfStorage)
			enlarge(enlarge_size(1));
		*_end = Tp(std::forward<Args>(args)...);
		++_end;
	}
	
	allocator_type get_allocator() const noexcept
	{return *this;}

	iterator insert_uninitialized(const_iterator position, size_t n)
	{
		if(capacity() < size() + n)
		{
			size_t index = position - _begin;
			enlarge_for_insert(enlarge_size(n), index, n);
			position = _begin + index;
		}
		else {
			memmove(const_cast<iterator>(position)+n, position, (_end - position) * sizeof(Tp));
			_end += n;
		}
		return const_cast<iterator>(position);
	}

	template <class InputIterator>
	void push_back(InputIterator first, InputIterator last)
	{
		push_back_range<InputIterator>(first, last, std::is_integral<InputIterator>());
	}

	void push_back(size_t n, const Tp& val)
	{
		if(capacity() - size() < n)
		{
			enlarge(enlarge_size(n));
		}
		std::uninitialized_fill_n<Tp*,size_t>(_end, n, val);
		_end += n;
	}

	void push_back(std::initializer_list<Tp> initlist)
	{
		if(capacity() - size() < initlist.size())
		{
			enlarge(enlarge_size(initlist.size()));
		}
		for(typename std::initializer_list<Tp>::iterator i = initlist.begin(); i != initlist.end(); ++i)
		{
			*_end = *i;
			++_end;
		}
	}

	void push_back_uninitialized(size_t n)
	{
		resize(size() + n);
	}
	
private:
	
	pointer allocate(size_t n)
	{
		return Alloc::allocate(n);
	}
	
	void deallocate() noexcept
	{
		deallocate(_begin, capacity());
	}
	
	void deallocate(pointer begin, size_t n) noexcept
	{
		if(begin != nullptr)
			Alloc::deallocate(begin, n);
	}
	
	template<typename InputIterator>
	void construct_from_range(InputIterator first, InputIterator last, std::false_type)
	{
		construct_from_range<InputIterator>(first, last, typename std::iterator_traits<InputIterator>::iterator_category());
	}
	
	template<typename Integral>
	void construct_from_range(Integral n, Integral val, std::true_type)
	{
		_begin = allocate(n);
		_end = _begin + n;
		_endOfStorage = _end;
		std::uninitialized_fill_n<Tp*,size_t>(_begin, n, val);
	}
	
	template<typename InputIterator>
	void construct_from_range(InputIterator first, InputIterator last, std::forward_iterator_tag)
	{
		size_t n = std::distance(first, last);
		_begin = allocate(n);
		_end = _begin + n;
		_endOfStorage = _begin + n;
		Tp* destIter = _begin;
		while(first != last)
		{
			*destIter = *first;
			++destIter; ++first;
		}
	}
	
	template<typename InputIterator>
	void assign_from_range(InputIterator first, InputIterator last, std::false_type)
	{
		assign_from_range<InputIterator>(first, last, typename std::iterator_traits<InputIterator>::iterator_category());
	}

	template<typename Integral>
	void assign_from_range(Integral n, Integral val, std::true_type)
	{
		if(size_t(n) > capacity())
		{
			iterator newStorage = allocate(n);
			deallocate();
			_begin = newStorage;
			_endOfStorage = _begin + n;
		}
		_end = _begin + n;
		std::uninitialized_fill_n<Tp*,size_t>(_begin, n, val);
	}
	
	template<typename InputIterator>
	void assign_from_range(InputIterator first, InputIterator last, std::forward_iterator_tag)
	{
		size_t n = std::distance(first, last);
		if(n > capacity())
		{
			iterator newStorage = allocate(n);
			deallocate();
			_begin = newStorage;
			_endOfStorage = _begin + n;
		}
		_end = _begin + n;
		Tp* destIter = _begin;
		while(first != last)
		{
			*destIter = *first;
			++destIter; ++first;
		}
	}
	
	template<typename InputIterator>
	iterator insert_from_range(const_iterator position, InputIterator first, InputIterator last, std::false_type)
	{
		return insert_from_range<InputIterator>(position, first, last,
			typename std::iterator_traits<InputIterator>::iterator_category());
	}
	
	template<typename Integral>
	iterator insert_from_range(const_iterator position, Integral n, Integral val, std::true_type)
	{
		if(capacity() < size() + n)
		{
			size_t index = position - _begin;
			enlarge_for_insert(enlarge_size(n), index, n);
			position = _begin + index;
		}
		else {
			memmove(const_cast<iterator>(position)+n, position, (_end - position) * sizeof(Tp));
			_end += n;
		}
		std::uninitialized_fill_n<Tp*,size_t>(const_cast<iterator>(position), n, val);
		return const_cast<iterator>(position);
	}
	
	template<typename InputIterator>
	iterator insert_from_range(const_iterator position, InputIterator first, InputIterator last, std::forward_iterator_tag)
	{
		size_t n = std::distance(first, last);
		if(capacity() < size() + n)
		{
			size_t index = position - _begin;
			enlarge_for_insert(enlarge_size(n), index, n);
			position = _begin + index;
		}
		else {
			memmove(const_cast<iterator>(position)+n, position, (_end - position) * sizeof(Tp));
			_end += n;
		}
		Tp* destIter = const_cast<iterator>(position);
		while(first != last)
		{
			*destIter = *first;
			++destIter; ++first;
		}
		return const_cast<iterator>(position);
	}
	
	void check_bounds(size_t index) const
	{
		if(index >= size())
			throw std::out_of_range("Access to element in uvector past end");
	}
	
	size_t enlarge_size(size_t extra_space_needed) const noexcept
	{
		return size() + std::max(size(), extra_space_needed);
	}
	
	void enlarge(size_t newSize)
	{
		pointer newStorage = allocate(newSize);
		memcpy(newStorage, _begin, size() * sizeof(Tp));
		deallocate();
		_end = newStorage + size();
		_begin = newStorage;
		_endOfStorage = _begin + newSize;
	}
	
	void enlarge_for_insert(size_t newSize, size_t insert_position, size_t insert_count)
	{
		pointer newStorage = allocate(newSize);
		memcpy(newStorage, _begin, insert_position * sizeof(Tp));
		memcpy(newStorage + insert_position + insert_count, _begin + insert_position, (size() - insert_position) * sizeof(Tp));
		deallocate();
		_end = newStorage + size() + insert_count;
		_begin = newStorage;
		_endOfStorage = _begin + newSize;
	}
	
	uvector& assign_copy_from(const uvector<Tp,Alloc>& other, std::false_type)
	{
		const size_t n = other.size();
		if(n > capacity()) {
			iterator newStorage = allocate(n);
			deallocate();
			_begin = newStorage;
			_end = _begin + n;
			_endOfStorage = _end;
		}
		memcpy(_begin, other._begin, n * sizeof(Tp));
		return *this;
	}
	
	uvector& assign_copy_from(const uvector<Tp,Alloc>& other, std::true_type)
	{
		if(allocator_is_always_equal() || static_cast<Alloc&>(other) == static_cast<Alloc&>(*this))
		{
			assign_copy_from(other, std::false_type());
		}
		else {
			const size_t n = other.size();
			iterator newStorage = static_cast<Alloc&>(other).allocate(n);
			deallocate();
			_begin = newStorage;
			_end = _begin + n;
			_endOfStorage = _end;
			memcpy(_begin, other._begin, n * sizeof(Tp));
			Alloc::operator=(static_cast<Alloc&>(other));
		}
		return *this;
	}
	
	uvector& assign_move_from(uvector<Tp,Alloc>&& other, std::false_type) noexcept(allocator_is_always_equal::value)
	{
		if(allocator_is_always_equal::value || static_cast<Alloc&>(other) == static_cast<Alloc&>(*this))
		{
			deallocate();
			_begin = other._begin;
			_end = other._end;
			_endOfStorage = other._endOfStorage;
			other._begin = nullptr;
			other._end = nullptr;
			other._endOfStorage = nullptr;
		}
		else {
			assign_copy_from(other, std::false_type());
		}
		return *this;
	}
	
	uvector& assign_move_from(uvector<Tp,Alloc>&& other, std::true_type) noexcept
	{
		deallocate();
		Alloc::operator=(std::move(static_cast<Alloc&>(other)));
		_begin = other._begin;
		_end = other._end;
		_endOfStorage = other._endOfStorage;
		other._begin = nullptr;
		other._end = nullptr;
		other._endOfStorage = nullptr;
		return *this;
	}

	void swap(uvector<Tp,Alloc>& other, std::true_type) noexcept
	{
		std::swap(_begin, other._begin);
		std::swap(_end, other._end);
		std::swap(_endOfStorage, other._endOfStorage);
		std::swap(static_cast<Alloc&>(other), static_cast<Alloc&>(*this));
	}
	
	// implementation of swap without propagate_on_container_swap
	void swap(uvector<Tp,Alloc>& other, std::false_type) noexcept
	{
		std::swap(_begin, other._begin);
		std::swap(_end, other._end);
		std::swap(_endOfStorage, other._endOfStorage);
	}
	
	template<typename InputIterator>
	void push_back_range(InputIterator first, InputIterator last, std::false_type)
	{
		push_back_range<InputIterator>(first, last, typename std::iterator_traits<InputIterator>::iterator_category());
	}

	template<typename Integral>
	void push_back_range(Integral n, Integral val, std::true_type)
	{
		if(capacity() - size() < size_t(n))
		{
			enlarge(enlarge_size(n));
		}
		std::uninitialized_fill_n<Tp*,size_t>(_end, n, val);
		_end += n;
	}
	
	template<typename InputIterator>
	void push_back_range(InputIterator first, InputIterator last, std::forward_iterator_tag)
	{
		size_t n = std::distance(first, last);
		if(n > capacity() - size())
		{
			enlarge(enlarge_size(n));
		}
		while(first != last)
		{
			*_end = *first;
			++_end;
			++first;
		}
	}
	
};

template<class Tp, class Alloc>
inline bool operator==(const uvector<Tp,Alloc>& lhs, const uvector<Tp,Alloc>& rhs) noexcept
{
	return lhs.size()==rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<class Tp, class Alloc>
inline bool operator!=(const uvector<Tp,Alloc>& lhs, const uvector<Tp,Alloc>& rhs) noexcept
{
	return !(lhs == rhs);
}

template <class Tp, class Alloc>
inline bool operator<(const uvector<Tp,Alloc>& lhs, const uvector<Tp,Alloc>& rhs) noexcept
{
	const size_t minSize = std::min(lhs.size(), rhs.size());
	for(size_t i=0; i!=minSize; ++i)
	{
		if(lhs[i] < rhs[i])
			return true;
		else if(lhs[i] > rhs[i])
			return false;
	}
	return lhs.size() < rhs.size();
}

template <class Tp, class Alloc>
inline bool operator<=(const uvector<Tp,Alloc>& lhs, const uvector<Tp,Alloc>& rhs) noexcept
{
	const size_t minSize = std::min(lhs.size(), rhs.size());
	for(size_t i=0; i!=minSize; ++i)
	{
		if(lhs[i] < rhs[i])
			return true;
		else if(lhs[i] > rhs[i])
			return false;
	}
	return lhs.size() <= rhs.size();
}

template <class Tp, class Alloc>
inline bool operator>(const uvector<Tp,Alloc>& lhs, const uvector<Tp,Alloc>& rhs) noexcept
{
	return rhs < lhs;
}

template <class Tp, class Alloc>
inline bool operator>=(const uvector<Tp,Alloc>& lhs, const uvector<Tp,Alloc>& rhs) noexcept
{
	return rhs <= lhs;
}

template <class Tp, class Alloc>
inline void swap(uvector<Tp,Alloc>& x, uvector<Tp,Alloc>& y)
{
	x.swap(y);
}
	#include<vector>
template <typename T>
class uvector2d 
{
public:
    uvector2d(size_t xAxis=0, size_t yAxis=0, T const & t=T()) : xAxis(xAxis), yAxis(yAxis), data(xAxis*yAxis, t) {}
   
uvector2d(std::initializer_list<std::initializer_list<T>> list)
    : xAxis(list.size()), yAxis(list.begin().size())  {
  data.reserve(xAxis * yAxis);
  for (const auto& inner_list : list)
   for (const auto& element : inner_list)
     data.push_back(element);
}
    T & operator()(size_t xNum, size_t yNum) 
   {return data[xNum*yAxis+yNum];}
   T const & operator()(size_t xNum, size_t yNum) const      {return data[xNum*yAxis+yNum];}
private:
    size_t xAxis,yAxis;
    vector<T> data; 
};
    
template <typename T>
class uvector3d {
public:
    uvector3d(size_t xAxis=0, size_t yAxis=0, size_t zAxis=0, T const & t=T()) : xAxis(xAxis), yAxis(yAxis), zAxis(zAxis), data(xAxis*yAxis*zAxis, t) {} 
    T & operator()(size_t xNum, size_t yNum, size_t zNum) 
    {return data[xNum*yAxis*zAxis + yNum*zAxis + zNum];}
    T const & operator()(size_t xNum, size_t yNum, size_t zNum) const
    {return data[xNum*yAxis*zAxis + yNum*zAxis + zNum];}
private:
    size_t xAxis,yAxis,zAxis;
    uvector<T> data; };}
    
#endif
