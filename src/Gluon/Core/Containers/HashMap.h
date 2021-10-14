#pragma once

#include <Gluon/Core/Defines.h>

#include <unordered_map>

// For now this is just a wrapper around std::unorederd_map, with convenience
// methods added along the road. At some point, the STL will just get dropped.
template <typename Key, typename Value>
class HashMap
{
public:
	DEFAULT_CTORS(HashMap);

	HashMap()  = default;
	~HashMap() = default;

	using Iterator      = typename std::unordered_map<Key, Value>::iterator;
	using ConstIterator = typename std::unordered_map<Key, Value>::const_iterator;

	Iterator      begin() { return m_hash_map.begin(); }
	ConstIterator begin() const { return m_hash_map.begin(); }
	ConstIterator cbegin() const { return m_hash_map.cbegin(); }
	Iterator      end() { return m_hash_map.end(); }
	ConstIterator end() const { return m_hash_map.end(); }
	ConstIterator cend() const { return m_hash_map.cend(); }

	bool is_empty() const { return m_hash_map.empty(); }

	i32 num_elements() const { return (i32)m_hash_map.size(); }

	void clear() { m_hash_map.clear(); }

	void add(const Key& key, const Value& value) { m_hash_map[key] = value; }

	void add(Key&& key, Value&& value) { m_hash_map[std::move(key)] = std::move(value); }

	Iterator remove(const Key& key) { return m_hash_map.erase(key); }

	Iterator remove(Key&& key) { return m_hash_map.erase(key); }

	const Value& get_value_or(const Key& key, const Value& other) const
	{
		if (auto found = m_hash_map.find(key); found != m_hash_map.end())
		{
			return found->second;
		}

		return other;
	};

	Value& get_value_or(const Key& key, Value& other)
	{
		if (auto found = m_hash_map.find(key); found != m_hash_map.end())
		{
			return found->second;
		}

		return other;
	}

	Value& operator[](const Key& key) { return m_hash_map[key]; }

	Value& operator[](Key&& key) { return m_hash_map[key]; }

	Iterator      find(const Key& key) { return m_hash_map.find(key); }
	Iterator      find(Key&& key) { return m_hash_map.find(key); }
	ConstIterator find(const Key& key) const { return m_hash_map.find(key); }
	ConstIterator find(Key&& key) const { return m_hash_map.find(key); }

private:
	std::unordered_map<Key, Value> m_hash_map;
};

template <typename Value>
class StringHashMap : public HashMap<std::string, Value>
{
};