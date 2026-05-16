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
			prev->next_node_ = this;
			next->prev_node_ = this;
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
			++current;
			return iterator(tmp);
		}
		iterator operator--(int) override
		{
			node* tmp = current;
			--current;
			return iterator(tmp);
		}
		iterator& operator+=(
			const typename abstract_iterator<
				iterator,
				T,
				std::bidirectional_iterator_tag>::difference_type& n) override
		{
			for (int i = 0; i < n; i++) {
				current++;
			}
			return *this;
		}
		iterator& operator-=(
			const typename abstract_iterator<
				iterator,
				T,
				std::bidirectional_iterator_tag>::difference_type& n) override
		{
			for (int i = 0; i < n; i++) {
				current--;
			}
			return *this;
		}
		iterator operator+(const typename abstract_iterator<
						   iterator,
						   T,
						   std::bidirectional_iterator_tag>::difference_type& n)
			const override
		{
			iterator tmp = iterator(current);
			for (int i = 0; i < n; i++) {
				tmp++;
			}
			return tmp;
		}
		iterator operator-(const typename abstract_iterator<
						   iterator,
						   T,
						   std::bidirectional_iterator_tag>::difference_type& n)
			const override
		{
			iterator tmp = iterator(current);
			for (int i = 0; i < n; i++) {
				tmp--;
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
				tmp--;
			}
			return counter;
		}
	};
	using const_iterator = iterator;

	list()
	{
		tail_ = new node();
		head_ = new node();
		tail_->prev_node_ = head_;
		head_->next_node_ = tail_;
	}

	template <typename it>
	list(it begin, it end)
	{
	}

	list(std::initializer_list<T> values) {}

	list(const list& other) {}

	list(list&& other)
	{
		size_ = other.size_;
		tail_ = other.tail_;
		head_ = other.head_;
		other.size_ = 0;
		other.tail_ = nullptr;
		other.head_ = nullptr;
	}

#pragma endregion
#pragma region pushs

	template <typename Type>
	void push_back(const Type& value)
	{
		node* last = tail_->prev_node_;
		node* new_last = new node(tail_->prev_node_, value, tail_);
		tail_->prev_node_ = new_last;
		last->next_node_ = new_last;
		++size_;
	}

	template <typename Type>
	void push_front(const Type& value)
	{
		// адрес реального последнего элемента
		node* first = head_->next_node_;
		node* new_first = new node(head_, value, first);
		head_->next_node_ = new_first;
		first->prev_node_ = new_first;
		++size_;
	}

#pragma endregion

	bool empty() const

		noexcept
	{
		return (size_ == 0u);
	}

	~list() {}

	void clear() {}

	size_t size() const { return 0; }

	void swap(list& other)

		noexcept
	{
	}

	friend void swap(list& l, list& r) { l.swap(r); }

#pragma region iterators

	iterator begin()

		noexcept
	{
		return iterator{head_->next_node_};
	}

	iterator end()

		noexcept
	{
		return iterator{tail_};
	}

	const_iterator begin() const

		noexcept
	{
		return const_iterator{head_->next_node_};
	}

	const_iterator end() const

		noexcept
	{
		return const_iterator{tail_};
	}

	const_iterator cbegin() const

		noexcept
	{
		return const_iterator{head_->next_node_};
	}

	const_iterator cend() const

		noexcept
	{
		return const_iterator{tail_};
	}

#pragma endregion

	T operator[](size_t pos) const {}

	T& operator[](size_t pos) { return *(static_cast<T*>((void*)&pos)); }

	friend bool operator==(const list& l, const list& r) { return true; }

	friend bool operator!=(const list& l, const list& r) { return false; }

	friend auto operator<=>(const list& lhs, const list& rhs) { return true; }

	friend std::ostream& operator<<(std::ostream& os, const list& other)
	{
		return os;
	}

	iterator insert(const_iterator pos, const T& value)
	{
		return iterator{nullptr};
	}

   private:
	static bool lexicographical_compare_(const list<T>& l, const list<T>& r)
	{
		return "123";
	}

	size_t size_ = 0;
	node* tail_ = nullptr;
	node* head_ = nullptr;
};
}  // namespace bmstu