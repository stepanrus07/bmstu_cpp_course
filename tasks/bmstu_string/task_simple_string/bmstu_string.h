#pragma once

#include <exception>
#include <iostream>

namespace bmstu
{
template <typename T>
class basic_string;

typedef basic_string<char> string;
typedef basic_string<wchar_t> wstring;
typedef basic_string<char16_t> u16string;
typedef basic_string<char32_t> u32string;

template <typename T>
class basic_string;

typedef basic_string<char> string;
typedef basic_string<wchar_t> wstring;
// typedef basic_string<char8_t> u8string;
typedef basic_string<char16_t> u16string;
typedef basic_string<char32_t> u32string;

template <typename T>
#ifdef _MSC_VER
class basic_string
#else
class basic_string
#endif
{
   public:
	/// Конструктор по умолчанию
	basic_string() : ptr_(new T[1]{0}), size_(0) {}

	basic_string(size_t size) : ptr_(new T[size + 1]), size_(size) {
		for (size_t i = 0; i < size_; i++) {
			ptr_[i] = ' ';
		}
		ptr_[size_] = '\0';
	}

	basic_string(std::initializer_list<T> il)
		: ptr_(new T[il.size() + 1]), size_(il.size())
	{
		const T* il_ptr = il.begin();
		for (size_t i = 0; i < size_; i++) {
			ptr_[i] = il_ptr[i];
		}
		ptr_[size_] = '\0';
	}

	/// Конструктор с параметром си-с
	basic_string(const T* c_str) {
		size_ = strlen_(c_str);
		ptr_ = new T[size_ + 1];
		for (size_t i = 0; i < size_; i++) {
			ptr_[i] = c_str[i];
		}
		ptr_[size_] = '\0';
	}

	/// Конструктор копирования
	basic_string(const basic_string& other) {
		size_ = other.size_;
		ptr_ = new T[size_ + 1];
		for (size_t i = 0; i < size_; i++) {
			ptr_[i] = other.ptr_[i];
		}
		ptr_[size_] = '\0';
	}

	/// Перемещающий конструктор
	basic_string(basic_string&& dying) {
		size_ = dying.size_;
		ptr_ = dying.ptr_;
		dying.size_ = 0;
		dying.ptr_ = nullptr;
	}

	/// Деструктор
	~basic_string() {delete[] ptr_;}

	/// Геттер на си-строку
	const T* c_str() const { return ptr_; }

	size_t size() const { return size_; }

	/// Оператор копирующего присваивания
	basic_string& operator=(basic_string&& other) { 
		delete[]ptr_;
		size_ = other.size_;
		ptr_ = other.ptr_;
		other.size_ = 0;
		other.ptr_ = new T[1];
		other.ptr_[0] = '\0';
		return *this; }

	/// Оператор копирующего присваивания си строки
	basic_string& operator=(const T* c_str) { 
		size_ = strlen_(c_str);
		ptr_ = new T[size_ + 1];
		for (size_t i = 0; i < size_; i++) {
			ptr_[i] = c_str[i];
		}
		ptr_[size_] = '\0';
		return *this; }

	/// Оператор копирующего присваивания
	basic_string& operator=(const basic_string& other) { 
		if (this == &other) {return *this;}
		size_ = other.size_;
		ptr_ = new T[size_ + 1];
		for (size_t i = 0; i < size_; i++) {
			ptr_[i] = other.ptr_[i];
		}
		ptr_[size_] = '\0';
		return *this; }

	friend basic_string<T> operator+(const basic_string<T>& left,
									 const basic_string<T>& right)
	{
		basic_string new_str(left.size_ + right.size_);
		for (size_t i = 0; i < left.size_; i++) {
			new_str.ptr_[i] = left.ptr_[i];
		}
		for (size_t i = 0; i < right.size_; i++) {
			new_str.ptr_[left.size_ + i] = right.ptr_[i];
		}
		return new_str;
	}

	template <typename S>
	friend S& operator<<(S& os, const basic_string& obj)
	{
		os<<obj.c_str();
		return os;
	}

	template <typename S>
	friend S& operator>>(S& is, basic_string& obj)
	{
		T ch;
		while (is.get(ch)) {
			obj += ch;
		}
		return is;
	}

	basic_string& operator+=(const basic_string& other) { 
		size_t new_size = size_ + other.size_;
		T* new_ptr = new T[new_size + 1];
		for (size_t i = 0; i < size_; i++) {
			new_ptr[i] = ptr_[i];
		}
		delete[]ptr_;
		for (size_t i = 0; i < other.size_; i++) {
			new_ptr[size_ + i] = other.ptr[i];
		}
		ptr_ = new_ptr;
		size_ = new_size;
		ptr_[size_] = '\0';
		return *this; }

	basic_string& operator+=(T symbol) { 
		T* new_ptr = new T[size_ + 2];
		for (size_t i = 0; i < size_; i++) {
			new_ptr[i] = ptr_[i];
		}
		delete[]ptr_;
		ptr_ = new_ptr;
		ptr_[size_] = symbol;
		size_++;
		ptr_[size_] = '\0';
		return *this; }

	T& operator[](size_t index) { 
		if (index < 0 || index >= size_) {
			throw std::out_of_range("Wrong index");
		}
		return *(ptr_ + index); }

	T& at(size_t index) { 
		if (index < 0 || index >= size_) {
			throw std::out_of_range("Wrong index");
		}
		return ptr_[index]; }

	T* data() { return ptr_; }

   private:
    T* ptr_ = nullptr;
	size_t size_;

	static size_t strlen_(const T* str) { 
		size_t res = 0;
		while (str[res] != '\0') {
			res++;
		}
		return res; }

	void clean_() {
		delete[] ptr_;
		size_ = 0;
		ptr_ = nullptr;
	}
};
}  // namespace bmstu
