#include <ostream>
#include <stdexcept>
#include <utility>
#include "array_ptr.h"

namespace bmstu
{
template <typename T>
class simple_vector
{
   public:
	class iterator
	{
	   public:
		using iterator_category = std::contiguous_iterator_tag;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using difference_type = std::ptrdiff_t;

		iterator() = default;

		iterator(const iterator& other) : ptr_(other.ptr_) {}

		iterator(std::nullptr_t) noexcept : ptr_(nullptr) {}

		iterator(iterator&& other) noexcept
		{
			ptr_ = other.ptr_;
			other.ptr_ = nullptr;
		}

		explicit iterator(pointer ptr) : ptr_(ptr) {}

		reference operator*() const { return *ptr_; }

		pointer operator->() const { return ptr_; }

		friend pointer to_address(const iterator& it) noexcept
		{
			return it.ptr_;
		}

		iterator& operator=(const iterator& other)
		{
			ptr_ = other.ptr_;
			return *this;
		}

		iterator& operator=(iterator&& other) noexcept
		{
			ptr_ = other.ptr_;
			other.ptr_ = nullptr;
			return *this;
		}

#pragma region Operators
		iterator& operator++()
		{
			ptr_++;
			return *this;
		}

		iterator& operator--()
		{
			ptr_--;
			return *this;
		}

		iterator operator++(int)
		{
			iterator tmp = iterator(ptr_);
			ptr_++;
			return tmp;
		}

		iterator operator--(int)
		{
			iterator tmp = iterator(ptr_);
			ptr_--;
			return tmp;
		}

		explicit operator bool() const { return ptr_ != nullptr; }

		friend bool operator==(const iterator& lhs, const iterator& rhs)
		{
			return lhs.ptr_ == rhs.ptr_;
		}

		friend bool operator==(const iterator& lhs, std::nullptr_t)
		{
			return lhs.ptr_ == nullptr;
		}

		iterator& operator=(std::nullptr_t) noexcept
		{
			ptr_ = nullptr;
			return *this;
		}

		friend bool operator==(std::nullptr_t, const iterator& rhs)
		{
			return rhs.ptr_ == nullptr;
		}

		friend bool operator!=(const iterator& lhs, const iterator& rhs)
		{
			return lhs.ptr_ != rhs.ptr_;
		}

		iterator operator+(const difference_type& n) const noexcept
		{
			return iterator(ptr_ + n);
		}

		iterator operator+=(const difference_type& n) noexcept
		{
			ptr_ += n;
			return *this;
		}

		iterator operator-(const difference_type& n) const noexcept
		{
			return iterator(ptr_ - n);
		}

		iterator operator-=(const difference_type& n) noexcept
		{
			ptr_ += n;
			return *this;
		}

		friend difference_type operator-(const iterator& end,
										 const iterator& begin) noexcept
		{
			return end.ptr_ - begin.ptr_;
		}

#pragma endregion
	   private:
		pointer ptr_ = nullptr;
	};

	simple_vector() noexcept = default;

	~simple_vector() { clear(); };

	simple_vector(std::initializer_list<T> init) noexcept
	{
		size_ = init.size();
		capacity_ = size_;
		data_ = array_ptr<T>(size_);
		for (size_t i = 0; i < size_; i++)
		{
			new (&data_[i]) T(init.begin()[i]);
		}
	}

	simple_vector(const simple_vector& other)
	{
		size_ = other.size();
		capacity_ = other.capacity();
		data_ = array_ptr<T>(capacity_);
		for (size_t i = 0; i < size_; i++)
		{
			new (&data_[i]) T(other[i]);
		}
	}

	simple_vector(simple_vector&& other) noexcept
	{
		swap(other);
		
	}

	simple_vector& operator=(const simple_vector& other)
	{
		clear();
		size_ = other.size();
		capacity_ = other.capacity();
		data_ = array_ptr<T>(capacity_);
		for (size_t i = 0; i < size_; i++)
		{
			new (&data_[i]) T(other[i]);
		}
		return *this;
	}

	simple_vector(size_t size, const T& value = T{})
	{
		data_ = array_ptr<T>(size);
		size_ = size;
		capacity_ = size;
		for (size_t i = 0; i < size; i++)
		{
			new (&data_[i]) T(value);
		}
	}

	iterator begin() noexcept { return iterator(data_.get()); }

	iterator end() noexcept { return iterator(data_.get() + size_); }

	using const_iterator = iterator;

	const_iterator begin() const noexcept { return iterator(data_.get()); }

	const_iterator end() const noexcept
	{
		return iterator(data_.get() + size_);
	}

	typename iterator::reference operator[](size_t index) noexcept
	{
		return data_.get()[index];
	}

	typename const_iterator::reference operator[](size_t index) const noexcept
	{
		return data_.get()[index];
	}

	typename iterator::reference at(size_t index) { return data_.get()[index]; }

	typename const_iterator::reference at(size_t index) const
	{
		return data_.get()[index];
	}

	size_t size() const noexcept { return size_; }

	size_t capacity() const noexcept { return capacity_; }

	void swap(simple_vector& other) noexcept
	{
		std::swap(data_, other.data_);
		std::swap(size_, other.size_);
		std::swap(capacity_, other.capacity_);
	}

	friend void swap(simple_vector& lhs, simple_vector& rhs) noexcept
	{
		std::swap(lhs.data_, rhs.data_);
		std::swap(lhs.size_, rhs.size_);
		std::swap(lhs.capacity_, rhs.capacity_);
	}

	void reserve(size_t new_cap)
	{
		if (new_cap <= capacity_)
		{
			return;
		}
		array_ptr<T> new_data(new_cap);
		for (size_t i = 0; i < size_; i++)
		{
			new (&new_data[i]) T(data_[i]);
			data_[i].~T();
		}
		data_ = std::move(new_data);
		capacity_ = new_cap;
	}

	void resize(size_t new_size)
	{
		if (new_size < 0)
		{
			return;
		}
		if (new_size <= size_)
		{
			for (size_t i = new_size; i < size_; i++)
			{
				data_[i].~T();
			}
			size_ = new_size;
			return;
		}
		if (new_size <= capacity_)
		{
			for (size_t i = size_; i < new_size; i++)
			{
				new (&data_[i]) T();
			}
			size_ = new_size;
			return;
		}
		if (new_size > capacity_)
		{
			array_ptr<T> new_data(new_size);
			for (size_t i = 0; i < size_; i++)
			{
				new (&new_data[i]) T(data_[i]);
				data_[i].~T();
			}
			data_ = std::move(new_data);
			size_ = new_size;
			capacity_ = new_size;
			return;
		}
	}

	iterator insert(const_iterator where, T&& value)
	{
		size_t index = where - begin();
		reserve(std::max(index + 1, size_ + 1));
		if (index >= size_)
		{
			for (size_t i = size_; i < index; i++)
			{
				new (&data_[i]) T();
			}
			new (&data_[index]) T(std::move(value));
			size_ = index + 1;
		}
		else if (index < size_)
		{
			for (size_t i = size_; i > index; i--)
			{
				new (&data_[i]) T(data_[i - 1]);
				data_[i - 1].~T();
			}
			new (&data_[index]) T(std::move(value));
			size_++;
		}
		return iterator(data_.get() + index);
	}

	iterator insert(const_iterator where, const T& value) { 
		size_t index = where - begin();
		reserve(std::max(index + 1, size_ + 1));
		if (index >= size_)
		{
			for (size_t i = size_; i < index; i++)
			{
				new (&data_[i]) T();
			}
			new (&data_[index]) T(value);
			size_ = index + 1;
		}
		else if (index < size_)
		{
			for (size_t i = size_; i > index; i--)
			{
				new (&data_[i]) T(data_[i - 1]);
				data_[i - 1].~T();
			}
			new (&data_[index]) T(value);
			size_++;
		}
		return iterator(data_.get() + index);
	 }

	iterator erase(iterator where)
	{
		size_t index = where - begin();
		if (index < 0 || index >= size_)
		{
			throw std::out_of_range("Index out of range");
		}
		data_[index].~T();
		for (size_t i = index; i < size_ - 1; i++)
		{
			new (&data_[i]) T(data_[i + 1]);
			data_[i].~T();
		}
		size_--;
		data_[size_].~T();
		return iterator(data_.get() + index);
	}

	void push_back(T&& value)
	{
		if (size_ + 1 > capacity_)
		{
			if (size_ == 0)
			{
				array_ptr<T> new_data(1);
				data_ = std::move(new_data);
				size_ = 1;
				capacity_ = 1;
				return;
			}
			array_ptr<T> new_data(capacity_ * 2);
			for (size_t i = 0; i < size_; i++)
			{
				new (&new_data[i]) T(data_[i]);
				data_[i].~T();
			}
			data_ = std::move(new_data);
			capacity_ *= 2;
		}
		new (&data_[size_]) T(std::move(value));
		size_++;
	}

	void push_back(const T& value)
	{
		if (size_ + 1 > capacity_)
		{
			if (size_ == 0)
			{
				array_ptr<T> new_data(1);
				data_ = std::move(new_data);
				size_ = 1;
				capacity_ = 1;
				return;
			}
			array_ptr<T> new_data(capacity_ * 2);
			for (size_t i = 0; i < size_; i++)
			{
				new (&new_data[i]) T(data_[i]);
				data_[i].~T();
			}
			data_ = std::move(new_data);
			capacity_ *= 2;
		}
		new (&data_[size_]) T(value);
		size_++;
	}

	void clear() noexcept
	{
		for (size_t i = 0; i < size_; i++)
		{
			data_[i].~T();
		}
		size_ = 0;
	}

	bool empty() const noexcept { return size_ == 0; }

	void pop_back()
	{
		if (empty())
		{
			return;
		}
		size_--;
		data_[size_].~T();
		return;
	}

	friend bool operator==(const simple_vector& lhs, const simple_vector& rhs)
	{
		if (lhs.size() != rhs.size())
		{
			return false;
		}
		for (size_t i = 0; i < rhs.size(); i++)
		{
			if (lhs.data_[i] != rhs.data_[i])
			{
				return lhs.data_[i] < rhs.data_[i];
			}
		}
		return true;
	}

	friend bool operator!=(const simple_vector& lhs, const simple_vector& rhs)
	{
		if (lhs.size() != rhs.size())
		{
			return true;
		}
		for (size_t i = 0; i < rhs.size(); i++)
		{
			if (lhs.data_[i] != rhs.data_[i])
			{
				return true;
			}
		}
		return false;
	}

	friend auto operator<=>(const simple_vector& lhs, const simple_vector& rhs)
	{
		for (size_t i = 0; i < std::min(lhs.size(), rhs.size()); i++)
		{
			if (lhs.data_[i] != rhs.data_[i])
			{
				return lhs.data_[i] <=> rhs.data_[i];
			}
		}
		return lhs.size() <=> rhs.size();
	}

	friend std::ostream& operator<<(std::ostream& os, const simple_vector& vec)
	{
		os << "{";
		for (size_t i = 0; i < vec.size(); i++)
		{
			os << vec[i];
			if (i < (vec.size() - 1))
			{
				os << ", ";
			}
		}
		os << "}";
		return os;
	}

   private:
	static bool alphabet_compare(const simple_vector<T>& lhs,
								 const simple_vector<T>& rhs)
	{
		for (size_t i = 0; i < std::min(lhs.size(), rhs.size()); i++)
		{
			if (lhs.data_[i] != rhs.data_[i])
			{
				return lhs.data_[i] < rhs.data_[i];
			}
		}
		return lhs.size() < rhs.size();
	}
	array_ptr<T> data_;
	size_t size_ = 0;
	size_t capacity_ = 0;
};
}  // namespace bmstu
