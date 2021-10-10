#pragma once

#include <Gluon/Core/Defines.h>

#include <unordered_map>

// For now this is just a wrapper around std::unorederd_map, with convenience methods added along the road.
// At some point, the STL will just get dropped.
template <typename Key, typename Value>
class HashMap
{
public:
	DEFAULT_CTORS(HashMap);

	HashMap()  = default;
	~HashMap() = default;

	using Iterator      = typename std::unordered_map<Key, Value>::iterator;
	using ConstIterator = typename std::unordered_map<Key, Value>::const_iterator;

	Iterator begin()
	{
		return m_hashMap.begin();
	}
	ConstIterator begin() const
	{
		return m_hashMap.begin();
	}
	ConstIterator cbegin() const
	{
		return m_hashMap.cbegin();
	}
	Iterator end()
	{
		return m_hashMap.end();
	}
	ConstIterator end() const
	{
		return m_hashMap.end();
	}
	ConstIterator cend() const
	{
		return m_hashMap.cend();
	}

	bool IsEmpty() const
	{
		return m_hashMap.empty();
	}

	i32 GetNumElements() const
	{
		return (i32)m_hashMap.size();
	}

	void Clear()
	{
		m_hashMap.clear();
	}

	void Add(const Key& key, const Value& value)
	{
		m_hashMap[key] = value;
	}

	void Add(Key&& key, Value&& value)
	{
		m_hashMap[std::move(key)] = std::move(value);
	}

	Iterator Remove(const Key& key)
	{
		return m_hashMap.erase(key);
	}

	Iterator Remove(Key&& key)
	{
		return m_hashMap.erase(key);
	}

	const Value& GetValueOr(const Key& key, const Value& other) const
	{
		if (auto found = m_hashMap.find(key); found != m_hashMap.end())
		{
			return found->second;
		}

		return other;
	};

	Value& GetValueOr(const Key& key, Value& other)
	{
		if (auto found = m_hashMap.find(key); found != m_hashMap.end())
		{
			return found->second;
		}

		return other;
	}

	Value& operator[](const Key& key)
	{
		return m_hashMap[key];
	}

	Value& operator[](const Key&& key)
	{
		return m_hashMap[key];
	}

private:
	std::unordered_map<Key, Value> m_hashMap;
};

template <typename Value>
class StringHashMap : public HashMap<std::string, Value>
{
};