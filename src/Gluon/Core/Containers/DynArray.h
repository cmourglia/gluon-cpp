#pragma once

#include <Gluon/Core/Defines.h>

#include <vector>

// For now this is just a wrapper around std::vector with convenience methods
// added along the road. At some point, the STL will just get dropped.

template <typename T>
class DynArray
{
public:
	using Iterator      = typename std::vector<T>::iterator;
	using ConstIterator = typename std::vector<T>::const_iterator;

	DynArray() noexcept = default;

	explicit DynArray(const i32 size, const T& defaultValue = T{}) noexcept
	    : m_dynArray(size, defaultValue)
	{
	}

	~DynArray() noexcept = default;

	DEFAULT_CTORS(DynArray);

	void Reserve(const i32 size) { m_dynArray.reserve(size); }

	void Resize(const i32 size) { m_dynArray.resize(size); }

	void Clear() { m_dynArray.clear(); }

	i32 NumElements() const { return static_cast<i32>(m_dynArray.size()); }

	bool IsEmpty() const { return m_dynArray.empty(); }

	const T& Get(const i32 index) const { return m_dynArray.at(index); }

	T& Get(const i32 index) { return m_dynArray.at(index); }

	const T& operator[](const i32 index) const { return m_dynArray[index]; }

	T& operator[](const i32 index) { return m_dynArray[index]; }

	const T* Data() const { return m_dynArray.data(); }

	T* Data() { return m_dynArray.data(); }

	Iterator begin() { return m_dynArray.begin(); }

	ConstIterator begin() const { return m_dynArray.begin(); }

	ConstIterator cbegin() const { return m_dynArray.cbegin(); }

	Iterator end() { return m_dynArray.end(); }

	ConstIterator end() const { return m_dynArray.end(); }

	ConstIterator cend() const { return m_dynArray.cend(); }

	Iterator BeginIterator() { return begin(); }

	ConstIterator BeginIterator() const { return begin(); }

	ConstIterator ConstBeginIterator() const { return cbegin(); }

	Iterator EndIterator() { return end(); }

	ConstIterator EndIterator() const { return end(); }

	ConstIterator ConstEndIterator() const { return cend(); }

	void Add(const T& value) { m_dynArray.push_back(value); }

	void Add(T&& value) { m_dynArray.push_back(std::move(value)); }

	template <typename... Args>
	void Emplace(Args... args)
	{
		m_dynArray.emplace_back(std::forward<Args>(args)...);
	}

	void Insert(const T& elem, i32 index)
	{
		m_dynArray.insert(begin() + index, elem);
	}

	void Insert(T&& elem, i32 index)
	{
		m_dynArray.insert(begin() + index, elem);
	}

	void Insert(Iterator where, Iterator rangeStart, Iterator rangeEnd)
	{
		m_dynArray.insert(where, rangeStart, rangeEnd);
	}

	void Append(const DynArray<T>& other)
	{
		Reserve(NumElements() + other.NumElements());
		m_dynArray.insert(m_dynArray.end(),
		                  other.m_dynArray.begin(),
		                  other.m_dynArray.end());
	}

	T& Last() { return m_dynArray.back(); }

	const T& Last() const { return m_dynArray.back(); }

	void Pop() { m_dynArray.pop_back(); }

	// ArrayView<T> GetView()
	// {
	// 	return ArrayView<T>(Data(), Count());
	// }

private:
	std::vector<T> m_dynArray;
};