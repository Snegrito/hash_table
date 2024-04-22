#ifndef DICTIONARY_MAP
#define DICTIONARY_MAP

#include "hash_table.h"

template<class Key>
class DictionaryMap {
public:
	using key_type = Key;
	using mapped_type = size_t;
	using value_type = std::pair<Key, mapped_type>;
	using _Nodeptr = HashNode<value_type>;
	using iterator = Iterator<_Nodeptr>;
	using const_iterator = ConstIterator<_Nodeptr>;

	DictionaryMap(size_t count = 1);
	DictionaryMap(const DictionaryMap<Key>& copy) = default;
	DictionaryMap(DictionaryMap<Key>&& move) = default;
	DictionaryMap<Key>& operator=(const DictionaryMap<Key>& copy) = default;
	DictionaryMap<Key>& operator=(DictionaryMap<Key>&& move) noexcept = default;
	~DictionaryMap() = default;

	iterator begin() { return table.begin(); }
	iterator end() { return table.end(); }

	void insert(const key_type& key);
	bool erase(const key_type& key);
	std::size_t find(const key_type& key);
	
	size_t size() noexcept;
	bool empty() noexcept;
	void clear();
	
	void popularWords(value_type* arr);
	void print(std::ostream& out);

private:
	HashTable<Key, size_t> table;

	void sortArr(value_type* arr);
};


template<class Key>
inline DictionaryMap<Key>::DictionaryMap(size_t count) :
	table{}
{
	table.rehash(count);
}

template<class Key>
inline void DictionaryMap<Key>::insert(const key_type& key) {
	auto elem = table.find(key);
	if (elem.ptr_) {
		++elem->data.second;
		return;
	}
	table.insert(std::pair<Key, size_t>(key, 1));
}

template<class Key>
inline bool DictionaryMap<Key>::erase(const key_type& key) {
	if (table.erase(key)) {
		return true;
	}
	return false;
}

template<class Key>
inline std::size_t DictionaryMap<Key>::find(const key_type& key) {
	auto node = table.find(key);
	if (!node.ptr_) {
		return 0;
	}
	return node->data.second;
}

template<class Key>
inline size_t DictionaryMap<Key>::size() noexcept {
	return table.size();
}

template<class Key>
inline bool DictionaryMap<Key>::empty() noexcept {
	return table.size() == 0;
}

template<class Key>
inline void DictionaryMap<Key>::clear() {
	table.clear();
}

template<class Key>
inline void DictionaryMap<Key>::popularWords(value_type* arr) {
	if (table.size() == 0) {
		return;
	}
	if (table.size() < 4) {
		auto iter = table.begin();
		for (size_t i = 0; i < table.size(); ++i) {
			arr[i] = iter->data;
			++iter;
		}
		return;
	}
	auto iter = table.begin();
	for (size_t i = 0; i < 3; ++i) {
		arr[i] = iter->data;
		++iter;
	}
	sortArr(arr);
	while (iter.ptr_) {
		if (iter->data.second > arr[0].second) {
			arr[0] = iter->data;
			sortArr(arr);
		}
		++iter;
	}
}

template<class Key>
inline void DictionaryMap<Key>::print(std::ostream& out) {
	auto iter = table.begin();
	while (iter.ptr_) {
		out << '(' << iter->data.first << " : " << iter->data.second << ") ";
		++iter;
	}
	std::cout << '\n';
}

template<class Key>
inline void DictionaryMap<Key>::sortArr(value_type* arr) {
	for (size_t i = 0; i < 2; i++) {
		bool isSwap = false;
		for (int j = 0; j < 2 - i; j++) {
			if (arr[j].second > arr[j + 1].second) {
				std::swap(arr[j], arr[j + 1]);
				isSwap = true;
			}
		}
		if (!isSwap) {
			break;
		}
	}
}

#endif