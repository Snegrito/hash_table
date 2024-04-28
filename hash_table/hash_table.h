#ifndef HASH_TABLE
#define HASH_TABLE

#include "forward_list.h"
#include <iostream>

template <class T>
struct HashNode {
	size_t cache;
	T data;
};

template <class Key,
	class T,
	class Hash = std::hash<Key>>
	class HashTable {
public:
	using value_type = std::pair<const Key, T>;
	using _Nodeptr = HashNode<value_type>;
	using key_type = Key;
	using mapped_type = T;
	using hasher = Hash;
	using size_type = size_t;
	using difference_type = ptrdiff_t;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using iterator = Iterator<_Nodeptr>;
	using const_iterator = ConstIterator<_Nodeptr>;

	explicit HashTable(size_type count = 1);
	HashTable(const HashTable& copy);
	HashTable(HashTable&& move);
	~HashTable();

	HashTable& operator=(const HashTable& copy);
	HashTable& operator=(HashTable&& move) noexcept;

	iterator begin() { return elems->begin(); }
	iterator end() { return elems->end(); }

	const_iterator cbegin() const { return elems->cbegin(); }
	const_iterator cend() const { return elems->cend(); }

	void rehash(size_type n);

	std::pair<iterator, bool> insert(const value_type& value);

	iterator erase(const_iterator position);
	size_type erase(const key_type& k);

	iterator find(const key_type& key);
	const_iterator find(const key_type& key) const;

	void swap(HashTable& ump) noexcept;
	void clear();

	size_type size() const noexcept;
	size_type bucket_count() const noexcept;

	float load_factor() const noexcept;
	float max_load_factor() const noexcept;
	void max_load_factor(float ml);

private:
	ForwardList<_Nodeptr>* elems;
	Iterator<_Nodeptr>* arr;
	size_type size_;
	size_type bucket_count_;
	float max_load_factor_;

	std::pair<iterator, bool> insert(_Nodeptr node);
};

template<class Key, class T, class Hash>
inline HashTable<Key, T, Hash>::HashTable(size_type count) try :
	elems(new ForwardList<_Nodeptr>),
	arr(new Iterator<_Nodeptr>[count]),
	size_(0),
	bucket_count_(count),
	max_load_factor_(1.0)
{} 
catch (const std::bad_alloc&) {
	delete elems;
	throw;
}

template<class Key, class T, class Hash>
inline HashTable<Key, T, Hash>::HashTable(const HashTable& copy) :
	HashTable(copy.bucket_count_)
{
	try {
		auto listNullIter = copy.elems->end();
		for (auto i = copy.elems->begin(); i != listNullIter; ++i) {
			size_type hashCode = i->cache;
			this->insert(*i);
		}
	}
	catch (...) {
		this->clear();
	}
}

template<class Key, class T, class Hash>
inline HashTable<Key, T, Hash>::HashTable(HashTable&& move) :
	HashTable()
{
	this->swap(move);
}

template<class Key, class T, class Hash>
inline HashTable<Key, T, Hash>::~HashTable() {
	delete elems;
	delete[] arr;
}

template<class Key, class T, class Hash>
inline HashTable<Key, T, Hash>& HashTable<Key, T, Hash>::operator=(const HashTable& copy) {
	HashTable temp(copy);
	this->swap(temp);
	return *this;
}

template<class Key, class T, class Hash>
inline HashTable<Key, T, Hash>& HashTable<Key, T, Hash>::operator=(HashTable&& move) noexcept {
	this->swap(move);
	return *this;
}

template<class Key, class T, class Hash>
inline void HashTable<Key, T, Hash>::rehash(size_type n) {
	if (n < bucket_count_) {
		return;
	}
	HashTable tempHash(n);
	auto listNullIter = elems->end();
	for (auto i = elems->begin(); i != listNullIter; ++i) {
		size_type hashCode = i->cache;
		tempHash.insert(*i);
	}
	this->swap(tempHash);
}

template<class Key, class T, class Hash>
inline std::pair<Iterator<HashNode<std::pair<const Key, T>>>, bool> HashTable<Key, T, Hash>::insert(const value_type& value) {
	try {
		size_type hashCode = hasher{} (value.first);
		if (!arr[hashCode % bucket_count_].ptr_) {
			iterator bktIter = elems->insert_after(elems->before_begin(), _Nodeptr{ hashCode, value });
			arr[hashCode % bucket_count_] = elems->before_begin();
			if (size_ > 0) {
				auto buff = bktIter++;
				size_t cacheMod = bktIter->cache % bucket_count_;
				arr[cacheMod] = buff;
			}
		}
		else {
			elems->insert_after(arr[hashCode % bucket_count_], _Nodeptr{ hashCode, value });
		}
		try {
			++size_;
			if (load_factor() > max_load_factor_) {
				this->rehash(bucket_count_ * 2);
			}
		}
		catch (...) {
			--size_;
			erase(++arr[hashCode % bucket_count_]);
			return std::pair<iterator, bool>(elems->end(), false);
		}
		return std::pair<iterator, bool>(arr[hashCode % bucket_count_], true);
	}
	catch (...) {
		return std::pair<iterator, bool>(elems->end(), false);
	}
}

template<class Key, class T, class Hash>
inline Iterator<HashNode<std::pair<const Key, T>>> HashTable<Key, T, Hash>::erase(const_iterator position) {
	if (!position.ptr_) {
		return elems->end();
	}
	auto deleter = position++;
	size_t hashCode = deleter->cache % bucket_count_;
	auto checkPrev = arr[hashCode];
	if (!position.ptr_ && checkPrev.ptr_->next == deleter.ptr_) {
		delete deleter.ptr_;
		checkPrev.ptr_->next = nullptr;
		arr[hashCode] = elems->end();
		--size_;
		return checkPrev;
	}
	else if (position.ptr_ && (position->cache % bucket_count_ != hashCode)) {
		if (checkPrev.ptr_->next == deleter.ptr_) {
			arr[hashCode] = elems->end();
			checkPrev.ptr_->next = deleter.ptr_->next;
			arr[position->cache % bucket_count_] = checkPrev;
		}
		else {
			arr[hashCode] = checkPrev;
			checkPrev.ptr_->next = deleter.ptr_->next;
		}
	}
	else {
		while (checkPrev.ptr_->next != deleter.ptr_) {
			++checkPrev;
		}
		checkPrev.ptr_->next = deleter.ptr_->next;
	}
	delete deleter.ptr_;
	--size_;
	return checkPrev;
}

template<class Key, class T, class Hash>
inline size_t HashTable<Key, T, Hash>::erase(const key_type& k) {
	auto buff = this->find(k);
	if (!buff.ptr_ || this->erase(buff) == elems->end()) {
		return 0;
	}
	return 1;
}

template<class Key, class T, class Hash>
inline Iterator<HashNode<std::pair<const Key, T>>> HashTable<Key, T, Hash>::find(const key_type& key) {
	size_type modHashCode = hasher{} (key) % bucket_count_;
	iterator iter = arr[modHashCode];
	if (iter.ptr_) {
		++iter;
		while (iter.ptr_ && (iter->cache % bucket_count_ == modHashCode)) {
			if (iter->data.first == key) {
				return iter;
			}
			++iter;
		}
	}
	return elems->end();
}

template<class Key, class T, class Hash>
inline ConstIterator<HashNode<std::pair<const Key, T>>> HashTable<Key, T, Hash>::find(const key_type& key) const {
	size_type modHashCode = hasher{} (key) % bucket_count_;
	const_iterator iter = arr[modHashCode];
	if (iter.ptr_) {
		++iter;
		while (iter.ptr_ && (iter->cache % bucket_count_ == modHashCode)) {
			if (iter->data.first == key) {
				return iter;
			}
			++iter;
		}
	}
	return elems->cend();
}

template<class Key, class T, class Hash>
inline void HashTable<Key, T, Hash>::clear() {
	while (size_ != 0) {
		erase(begin());
	}
}

template<class Key, class T, class Hash>
inline void HashTable<Key, T, Hash>::swap(HashTable& ump) noexcept {
	std::swap(elems, ump.elems);
	std::swap(arr, ump.arr);
	std::swap(size_, ump.size_);
	std::swap(bucket_count_, ump.bucket_count_);
	std::swap(max_load_factor_, ump.max_load_factor_);
}

template<class Key, class T, class Hash>
inline size_t HashTable<Key, T, Hash>::size() const noexcept {
	return size_;
}

template<class Key, class T, class Hash>
inline size_t HashTable<Key, T, Hash>::bucket_count() const noexcept {
	return bucket_count_;
}

template<class Key, class T, class Hash>
inline float HashTable<Key, T, Hash>::load_factor() const noexcept {
	return static_cast<float>(size_) / static_cast<float>(bucket_count_);
}

template<class Key, class T, class Hash>
inline float HashTable<Key, T, Hash>::max_load_factor() const noexcept {
	return max_load_factor_;
}

template<class Key, class T, class Hash>
inline void HashTable<Key, T, Hash>::max_load_factor(float ml) {
	max_load_factor_ = ml;
}

template<class Key, class T, class Hash>
inline  std::pair<Iterator<HashNode<std::pair<const Key, T>>>, bool> HashTable<Key, T, Hash>::insert(_Nodeptr node) {
	size_type hashCode = node.cache;
	if (!arr[hashCode % bucket_count_].ptr_) {
		arr[hashCode % bucket_count_] = elems->before_begin();
		iterator bktIter = elems->insert_after(elems->before_begin(), _Nodeptr{ hashCode, node.data });
		if (size_ > 0) {
			auto value = bktIter++;
			size_t cacheMod = bktIter->cache % bucket_count_;
			arr[cacheMod] = value;
		}
	}
	else {
		elems->insert_after(arr[hashCode % bucket_count_], _Nodeptr{ hashCode, node.data });
	}
	++size_;
	return std::pair<iterator, bool>(arr[hashCode % bucket_count_], true);
}


#endif