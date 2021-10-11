#pragma once

#include <Gluon/Core/Defines.h>

#include <vector>

// For now this is just a wrapper around std::vector with convenience methods
// added along the road. At some point, the STL will just get dropped.

template <typename T>
class Array
{
public:
	using Iterator      = typename std::vector<T>::iterator;
	using ConstIterator = typename std::vector<T>::const_iterator;

	Array() noexcept = default;

	explicit Array(const i32 size, const T& defaultValue = T{}) noexcept
	    : m_dyn_array(size, defaultValue)
	{
	}

	Array(std::initializer_list<T> list)
	    : m_dyn_array(std::move(list))
	{
	}

	~Array() noexcept = default;

	DEFAULT_CTORS(Array);

	void reserve(const i32 size) { m_dyn_array.reserve(size); }

	void resize(const i32 size) { m_dyn_array.resize(size); }

	void clear() { m_dyn_array.clear(); }

	i32 num_elements() const { return static_cast<i32>(m_dyn_array.size()); }

	bool is_empty() const { return m_dyn_array.empty(); }

	const T& element(const i32 index) const { return m_dyn_array.at(index); }

	T& element(const i32 index) { return m_dyn_array.at(index); }

	const T& operator[](const i32 index) const { return m_dyn_array[index]; }

	T& operator[](const i32 index) { return m_dyn_array[index]; }

	const T* data() const { return m_dyn_array.data(); }

	T* data() { return m_dyn_array.data(); }

	Iterator begin() { return m_dyn_array.begin(); }

	ConstIterator begin() const { return m_dyn_array.begin(); }

	ConstIterator cbegin() const { return m_dyn_array.cbegin(); }

	Iterator end() { return m_dyn_array.end(); }

	ConstIterator end() const { return m_dyn_array.end(); }

	ConstIterator cend() const { return m_dyn_array.cend(); }

	void add(const T& value) { m_dyn_array.push_back(value); }

	void add(T&& value) { m_dyn_array.push_back(std::move(value)); }

	template <typename... Args>
	void emplace(Args... args)
	{
		m_dyn_array.emplace_back(std::forward<Args>(args)...);
	}

	void insert(const T& elem, i32 index)
	{
		m_dyn_array.insert(begin() + index, elem);
	}

	void insert(T&& elem, i32 index)
	{
		m_dyn_array.insert(begin() + index, elem);
	}

	void insert(Iterator where, Iterator rangeStart, Iterator rangeEnd)
	{
		m_dyn_array.insert(where, rangeStart, rangeEnd);
	}

	void append(const Array<T>& other)
	{
		Reserve(num_elements() + other.num_elements());
		m_dyn_array.insert(m_dyn_array.end(),
		                   other.m_dyn_array.begin(),
		                   other.m_dyn_array.end());
	}

	T&       first() { return m_dyn_array.front(); }
	const T& first() const { return m_dyn_array.front(); }

	T&       last() { return m_dyn_array.back(); }
	const T& last() const { return m_dyn_array.back(); }

	void pop() { m_dyn_array.pop_back(); }

	// ArrayView<T> GetView()
	// {
	// 	return ArrayView<T>(Data(), Count());
	// }

private:
	std::vector<T> m_dyn_array;
};