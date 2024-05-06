#ifndef  LISH_H
#define LIST_H

#include <cassert>
#include <iostream>

struct ListNodeBase {
	ListNodeBase(ListNodeBase* _Lptr = nullptr) : 
		next(_Lptr) 
	{}
	ListNodeBase* next;
	virtual ~ListNodeBase() = default;
};

template<class T>
struct ListNode : public ListNodeBase {
	ListNode(const T& value, ListNodeBase* _Lptr = nullptr) :
		ListNodeBase(_Lptr),
		data(value)
	{}
	T data;
};

template<class T>
class ConstIterator {
public:
	using iterator_category = std::forward_iterator_tag;

	using _Nodeptr = ListNodeBase*;
	using value_type = T;
	using difference_type = ptrdiff_t;
	using pointer = const T*;
	using reference = const T&;

	ConstIterator(_Nodeptr _Pnode = nullptr) : ptr_(_Pnode) {}

	bool operator==(const ConstIterator& right) const {
		return ptr_ == right.ptr_;
	}

	bool operator!=(const ConstIterator& right) const {
		return !(*this == right);
	}

	reference operator*() const {
		assert(ptr_ != nullptr);
		return static_cast<ListNode<value_type>*>(ptr_)->data;
	}

	pointer operator->() const {
		assert(ptr_ != nullptr);
		return &(static_cast<ListNode<value_type>*>(ptr_)->data);
	}

	ConstIterator& operator++() {
		assert(ptr_ != nullptr);
		ptr_ = ptr_->next;
		return *this;
	}

	ConstIterator operator++(int) {
		ConstIterator temp = *this;
		ptr_ = ptr_->next;
		return temp;
	}

	_Nodeptr ptr_;
};


template<class T>
class Iterator : public ConstIterator<T> {
public:
	using _Mybase = ConstIterator<T>;
	using iterator_category = std::forward_iterator_tag;

	using _Nodeptr = ListNodeBase*;
	using value_type = T;
	using difference_type = ptrdiff_t;
	using pointer = T*;
	using reference = T&;

	Iterator(_Nodeptr _Pnode = nullptr) : _Mybase(_Pnode) {}

	reference operator*() const {
		return const_cast<reference>(_Mybase::operator*());
	}

	pointer operator->() const {
		return const_cast<pointer>(_Mybase::operator->());
	}

	Iterator& operator++() {
		_Mybase::operator++();
		return *this;
	}

	Iterator operator++(int) {
		Iterator temp = *this;
		_Mybase::operator++();
		return temp;
	}
};


template <class T>
class ForwardList {
public:
	using value_type = T;
	using size_type = size_t;
	using difference_type = ptrdiff_t;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using iterator = Iterator<T>;
	using const_iterator = ConstIterator<T>;

	ForwardList();
	ForwardList(const ForwardList& copy);
	ForwardList(ForwardList&& move) noexcept;
	~ForwardList();

	ForwardList& operator=(const ForwardList& copy);
	ForwardList& operator=(ForwardList&& move) noexcept;

	iterator before_begin() { return iterator(&head_); }
	const_iterator cbefore_begin() { return const_iterator(&head_); }

	iterator begin() { return iterator(head_.next); }
	iterator end() { return iterator(); }
	const_iterator cbegin() { return const_iterator(head_.next); }
	const_iterator cend() { return const_iterator(); }

	iterator insert_after(iterator pos, const T& value);
	iterator erase_after(const_iterator pos);

	void swap(ForwardList& other) noexcept;
	void clear();
	bool empty() const noexcept;

private:
	ListNodeBase head_;
};

template<class T>
inline ForwardList<T>::ForwardList() :
	head_(nullptr)
{}

template<class T>
inline ForwardList<T>::ForwardList(const ForwardList& copy) :
	head_(nullptr)
{
	const ListNodeBase* from = &copy.head_;
	ListNodeBase* to = &this->head_;
	try {
		while (from->next) {
			const ListNode<T>* nextf = static_cast<ListNode<T>*>(from->next);
			to->next = new ListNode<T>(*nextf);
			from = from->next;
			to = to->next;
		}
	}
	catch (...) {
		this->clear();
	}
}

template<class T>
inline ForwardList<T>::ForwardList(ForwardList&& move) noexcept :
	ForwardList<T>()
{
	this->swap(move);
}

template<class T>
inline ForwardList<T>::~ForwardList() {
	this->clear();
}

template<class T>
inline ForwardList<T>& ForwardList<T>::operator=(const ForwardList& copy) {
	ForwardList<T>* temp = new ForwardList<T>(copy);
	this->swap(temp);
	return *this;
}

template<class T>
inline ForwardList<T>& ForwardList<T>::operator=(ForwardList&& move) noexcept{
	this->swap(move);
	return *this;
}

template<class T>
inline Iterator<T> ForwardList<T>::insert_after(iterator pos, const T& value) {
	ListNode<T>* p = new ListNode<T>(value);
	try {
		p->next = pos.ptr_->next;
		pos.ptr_->next = p;
	}
	catch (...) {
		delete p;
		throw;
	}
	return iterator(p);
}

template<class T>
inline Iterator<T> ForwardList<T>::erase_after(const_iterator pos) {
	ListNode<T>* p = static_cast<ListNode<T>*>(pos.ptr_->next);
	pos.ptr_->next = p->next;
	delete p;
	p = nullptr;
	return iterator(pos.ptr_->next);
}

template<class T>
inline void ForwardList<T>::swap(ForwardList& other) noexcept {
	std::swap(head_, other.head_);
}

template<class T>
inline void ForwardList<T>::clear() {
	ListNodeBase* p = head_.next;
	while (p) {
		ListNode<T>* temp = static_cast<ListNode<T>*>(p);
		p = p->next;
		delete temp;
	}
	head_ = nullptr;
}

template<class T>
inline bool ForwardList<T>::empty() const noexcept {
	return head_.next == nullptr;
}

#endif
