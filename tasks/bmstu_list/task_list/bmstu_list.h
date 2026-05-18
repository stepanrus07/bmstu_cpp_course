#include <cstddef>
#include <iterator>
#include <ostream>
#include "abstract_iterator.h"

namespace bmstu
{
template <typename T>
class list
{
	struct node
	{
		node() = default;

		node(node* prev, const T& value, node* next)
			: value_(value), prev_node_(prev), next_node_(next)
		{
		}

		T value_;
		node* next_node_ = nullptr;
		node* prev_node_ = nullptr;
	};

   public:
	struct iterator
		: public abstract_iterator<iterator, T, std::bidirectional_iterator_tag>
	{
		node* current;
		iterator() : current(nullptr) {}
		iterator(node* node) : current(node) {}
		iterator& operator++() override
		{
			current = current->next_node_;
			return *this;
		}
		iterator& operator--() override
		{
			current = current->prev_node_;
			return *this;
		}
		iterator operator++(int) override
		{
			node* tmp = current;
			current = current->next_node_;
			return iterator(tmp);
		}
		iterator operator--(int) override
		{
			node* tmp = current;
			current = current->prev_node_;
			return iterator(tmp);
		}
		iterator& operator+=(
			const typename abstract_iterator<
				iterator,
				T,
				std::bidirectional_iterator_tag>::difference_type& n) override
		{
			for (int i = 0; i < n; i++)
			{
				current = current->next_node_;
			}
			return *this;
		}
		iterator& operator-=(
			const typename abstract_iterator<
				iterator,
				T,
				std::bidirectional_iterator_tag>::difference_type& n) override
		{
			for (int i = 0; i < n; i++)
			{
				current = current->prev_node_;
			}
			return *this;
		}
		iterator operator+(const typename abstract_iterator<
						   iterator,
						   T,
						   std::bidirectional_iterator_tag>::difference_type& n)
			const override
		{
			iterator tmp(current);
			for (int i = 0; i < n; i++)
			{
				++tmp;
			}
			return tmp;
		}
		iterator operator-(const typename abstract_iterator<
						   iterator,
						   T,
						   std::bidirectional_iterator_tag>::difference_type& n)
			const override
		{
			iterator tmp(current);
			for (int i = 0; i < n; i++)
			{
				--tmp;
			}
			return tmp;
		}
		typename abstract_iterator<iterator,
								   T,
								   std::bidirectional_iterator_tag>::reference
		operator*() const override
		{
			return current->value_;
		}
		typename abstract_iterator<iterator,
								   T,
								   std::bidirectional_iterator_tag>::pointer
		operator->() const override
		{
			return &(current->value_);
		}
		bool operator==(const iterator& other) const override
		{
			return current == other.current;
		}
		bool operator!=(const iterator& other) const override
		{
			return current != other.current;
		}
		explicit operator bool() const override { return current != nullptr; }
		typename abstract_iterator<
			iterator,
			T,
			std::bidirectional_iterator_tag>::difference_type
		operator-(const iterator& other) const override
		{
			int counter = 0;
			iterator tmp(current);
			while (tmp != other)
			{
				counter++;
				--tmp;
			}
			return counter;
		}
	};
	using const_iterator = iterator;

	list()
	{
		tail_ = new node();
		head_ = new node();
		size_ = 0;
		tail_->prev_node_ = head_;
		head_->next_node_ = tail_;
	}

	template <typename it>
	list(it begin, it end)
	{
		head_ = new node();
		tail_ = new node();
		size_ = 0;
		node* curr = head_;
		for (it i = begin; i != end; i++)
		{
			node* new_node = new node(curr, *i, nullptr);
			curr->next_node_ = new_node;
			curr = new_node;
			size_++;
		}
		curr->next_node_ = tail_;
		tail_->prev_node_ = curr;
	}

	list(std::initializer_list<T> values)
	{
		head_ = new node();
		tail_ = new node();
		size_ = values.size();
		head_->next_node_ = tail_;
		tail_->prev_node_ = head_;
		node* curr = head_;
		for (const T& el : values)
		{
			node* new_node = new node(curr, el, nullptr);
			curr->next_node_ = new_node;
			curr = new_node;
		}
		curr->next_node_ = tail_;
		tail_->prev_node_ = curr;
	}

	list(const list& other)
	{
		size_ = other.size_;
		head_ = new node();
		tail_ = new node();
		node* curr = head_;
		iterator iterator(other.head_->next_node_);
		for (size_t i = 0; i < size_; i++)
		{
			node* new_node = new node(curr, *(iterator++), nullptr);
			curr->next_node_ = new_node;
			curr = new_node;
		}
		curr->next_node_ = tail_;
		tail_->prev_node_ = curr;
	}

	list(list&& other)
	{
		size_ = other.size_;
		other.size_ = 0;
		tail_ = other.tail_;
		head_ = other.head_;
		other.tail_ = nullptr;
		other.head_ = nullptr;
	}

#pragma endregion
#pragma region pushs

	template <typename Type>
	void push_back(const Type& value)
	{
		node* last = tail_->prev_node_;
		node* new_last = new node(last, value, tail_);
		tail_->prev_node_ = new_last;
		last->next_node_ = new_last;
		++size_;
	}

	template <typename Type>
	void push_front(const Type& value)
	{
		node* first = head_->next_node_;
		node* new_first = new node(head_, value, first);
		head_->next_node_ = new_first;
		first->prev_node_ = new_first;
		++size_;
	}

#pragma endregion

	bool empty() const noexcept { return (size_ == 0u); }

	~list()
	{
		clear();
		delete head_;
		delete tail_;
	}

	void clear()
	{
		node* curr = head_->next_node_;
		for (size_t i = 0; i < size_; i++)
		{
			curr = curr->next_node_;
			delete curr->prev_node_;
		}
		size_ = 0;
		tail_->prev_node_ = head_;
		head_->next_node_ = tail_;
	}

	size_t size() const { return size_; }

	void swap(list& other) noexcept
	{
		std::swap(size_, other.size_);
		std::swap(head_, other.head_);
		std::swap(tail_, other.tail_);
	}

	friend void swap(list& l, list& r) { l.swap(r); }

#pragma region iterators

	iterator begin() noexcept { return iterator(head_->next_node_); }

	iterator end() noexcept { return iterator(tail_); }

	const_iterator begin() const noexcept
	{
		return const_iterator(head_->next_node_);
	}

	const_iterator end() const noexcept { return const_iterator(tail_); }

	const_iterator cbegin() const noexcept
	{
		return const_iterator(head_->next_node_);
	}

	const_iterator cend() const noexcept { return const_iterator(tail_); }

#pragma endregion

	T operator[](size_t pos) const
	{
		if (pos < 0 || pos >= size_)
		{
			throw std::out_of_range("Index out of range");
		}
		return *(begin() + pos);
	}

	T& operator[](size_t pos)
	{
		if (pos < 0 || pos >= size_)
		{
			throw std::out_of_range("Index out of range");
		}
		return *(begin() + pos);
	}

	friend bool operator==(const list& l, const list& r)
	{
		if (l.size() != r.size())
		{
			return false;
		}
		iterator it_l = l.begin();
		iterator it_r = r.begin();
		for (size_t i = 0; i < r.size(); i++)
		{
			if (*(it_l) != *(it_r))
			{
				return false;
			}
			it_l++;
			it_r++;
		}
		return true;
	}

	friend bool operator!=(const list& l, const list& r) { return !(l == r); }

	friend auto operator<=>(const list& lhs, const list& rhs)
	{
		iterator it_l = lhs.begin();
		iterator it_r = rhs.begin();
		for (size_t i = 0; i < std::min(lhs.size(), rhs.size()); i++)
		{
			if (*(it_l) != *(it_r))
			{
				return *(it_l) <=> *(it_r);
			}
			it_l++;
			it_r++;
		}
		return lhs.size() <=> rhs.size();
	}

	friend std::ostream& operator<<(std::ostream& os, const list& other)
	{
		os << "{";
		for (size_t i = 0; i < other.size() - 1; i++)
		{
			os << other[i];
			os << ", ";
		}
		os << other[other.size() - 1];
		os << "}";
		return os;
	}

	iterator insert(const_iterator pos, const T& value)
	{
		node* new_node = new node(pos->prev_node_, T(value), pos->next_node_);
		pos->next_node_ = new_node;
		pos->next_node_->prev_node_ = new_node;
		return pos;
	}

   private:
	static bool lexicographical_compare_(const list<T>& l, const list<T>& r)
	{
		iterator it_l = l.begin();
		iterator it_r = r.begin();
		for (size_t i = 0; i < std::min(l.size(), r.size()); i++)
		{
			if (*(it_l) != *(it_r))
			{
				return *(it_l) < *(it_r);
			}
			it_l++;
			it_r++;
		}
		return l.size() < r.size();
	}

	size_t size_ = 0;
	node* tail_ = nullptr;
	node* head_ = nullptr;
};
}  // namespace bmstu