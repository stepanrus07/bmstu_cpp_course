#pragma once

#include <exception>
#include <iostream>

namespace bmstu
{
template <typename T>
class basic_string;

using string = basic_string<char>;
using wstring = basic_string<wchar_t>;
using u16string = basic_string<char16_t>;
using u32string = basic_string<char32_t>;

template <typename T>
class basic_string
{
   private:
	static constexpr size_t SSO_CAPACITY =
		(sizeof(T*) + sizeof(size_t) + sizeof(size_t)) / sizeof(T) - 1;

	struct LongString
	{
		T* ptr;
		size_t size;
		size_t capacity;
	};

	struct ShortString
	{
		T buffer[SSO_CAPACITY + 1];
		unsigned char size;
	};

	union Data
	{
		LongString long_str;
		ShortString short_str;
	};

	Data data_;
	bool is_long_;

	bool is_long() const { return is_long_; }

	T* get_ptr() { 
		if (is_long_) {
			return data_.long_str.ptr;
		}
		return data_.short_str.buffer; }

	const T* get_ptr() const { 
		if (is_long_) {
			return data_.long_str.ptr;
		}
		return data_.short_str.buffer; }

	size_t get_size() const { 
		if (is_long_) {
			return data_.long_str.size;
		}
		return data_.short_str.size; }

	size_t get_capacity() const { 
		if (is_long_) {
			return data_.long_str.capacity;
		}
		return SSO_CAPACITY; }

   public:
	basic_string() {
		is_long_ = false;
		data_.short_str.buffer[0] = '\0';
		data_.short_str.size = 0;
	}

	basic_string(size_t size) {
		if (size > SSO_CAPACITY) {
			is_long_ = true;
			data_.long_str.ptr = new T[size + 1];
			data_.long_str.capacity = size + 1;
			data_.long_str.size = size;
			for (size_t i = 0; i < size; i++) {
				data_.long_str.ptr[i] = ' ';
			}
			data_.long_str.ptr[size] = '\0';
		}
		else {
			is_long_ = false;
			for (size_t i = 0; i < size; i++) {
				data_.short_str.buffer[i] = ' ';
			}
			data_.short_str.buffer[size] = '\0';
			data_.short_str.size = size;
		}
	}

	basic_string(std::initializer_list<T> il) {
		size_t size = il.size();
		const T* ptr = il.begin();
		if (size > SSO_CAPACITY) {
			is_long_ = true;
			data_.long_str.ptr = new T[size + 1];
			data_.long_str.capacity = size + 1;
			data_.long_str.size = size;
			for (size_t i = 0; i < size; i++) {
				data_.long_str.ptr[i] = ptr[i];
			}
			data_.long_str.ptr[size] = '\0';
		}
		else {
			is_long_ = false;
			for (size_t i = 0; i < size; i++) {
				data_.short_str.buffer[i] = ptr[i];
			}
			data_.short_str.buffer[size] = '\0';
			data_.short_str.size = size;
		}
	}

	basic_string(const T* c_str) {
		size_t size = strlen_(c_str);
		if (size > SSO_CAPACITY) {
			is_long_ = true;
			data_.long_str.ptr = new T[size + 1];
			data_.long_str.capacity = size + 1;
			data_.long_str.size = size;
			for (size_t i = 0; i < size; i++) {
				data_.long_str.ptr[i] = c_str[i];
			}
			data_.long_str.ptr[size] = '\0';
		}
		else {
			is_long_ = false;
			for (size_t i = 0; i < size; i++) {
				data_.short_str.buffer[i] = c_str[i];
			}
			data_.short_str.buffer[size] = '\0';
			data_.short_str.size = size;
		}
	}

	basic_string(const basic_string& other) {
		is_long_ = other.is_long_;
		if (is_long_) {
			size_t size = other.data_.long_str.size;
			data_.long_str.size = size;
			data_.long_str.capacity = size + 1;
			data_.long_str.ptr = new T[size + 1];
			for (size_t i = 0; i < size; i++) {
				data_.long_str.ptr[i] = other.data_.long_str.ptr[i];
			}
			data_.long_str.ptr[size] = '\0';
		}
		else {
			size_t size = other.data_.short_str.size;
			for (size_t i = 0; i < size; i++) {
				data_.short_str.buffer[i] = other.data_.short_str.buffer[i];
			}
			data_.short_str.size = size;
			data_.short_str.buffer[size] = '\0';
		}
	}

	basic_string(basic_string&& dying) noexcept {
		is_long_ = dying.is_long_;
		if (is_long_) {
			data_.long_str.ptr = dying.data_.long_str.ptr;
			data_.long_str.size = dying.data_.long_str.size;
			data_.long_str.capacity = dying.data_.long_str.capacity;
			dying.data_.long_str.ptr = nullptr;
			dying.data_.long_str.size = 0;
			dying.data_.long_str.capacity = 0;
		}
		else {
			data_.short_str = dying.data_.short_str;
			dying.data_.short_str.buffer[0] = '\0';
		}
		dying.is_long_ = false;
		dying.data_.short_str.size = 0;
	}

	~basic_string() {
		clean_();
	}

	const T* c_str() const { 
		if (is_long_) {
			return data_.long_str.ptr;
		}
		return data_.short_str.buffer; }

	size_t size() const { return get_size(); }

	bool is_using_sso() const { 
		return !is_long_; }

	size_t capacity() const { 
		if (is_long_) {
			return data_.long_str.capacity;
		}
		else {
			return SSO_CAPACITY;
		}
	}

	basic_string& operator=(basic_string&& dying) noexcept { 
		if (this == &dying) {
			return *this;
		}
		is_long_ = dying.is_long_;
		if (is_long_) {
			data_.long_str.ptr = dying.data_.long_str.ptr;
			data_.long_str.size = dying.data_.long_str.size;
			data_.long_str.capacity = dying.data_.long_str.capacity;
			dying.data_.long_str.ptr = nullptr;
			dying.data_.long_str.size = 0;
			dying.data_.long_str.capacity = 0;
		}
		else {
			data_.short_str = dying.data_.short_str;
			dying.data_.short_str.buffer[0] = '\0';
		}
		dying.is_long_ = false;
		dying.data_.short_str.size = 0;
		return *this; }

	basic_string& operator=(const T* c_str) { 
		size_t size = strlen_(c_str);
		if (size > SSO_CAPACITY) {
			is_long_ = true;
			data_.long_str.ptr = new T[size + 1];
			data_.long_str.capacity = size + 1;
			data_.long_str.size = size;
			for (size_t i = 0; i < size; i++) {
				data_.long_str.ptr[i] = c_str[i];
			}
			data_.long_str.ptr[size] = '\0';
		}
		else {
			is_long_ = false;
			for (size_t i = 0; i < size; i++) {
				data_.short_str.buffer[i] = c_str[i];
			}
			data_.short_str.buffer[size] = '\0';
			data_.short_str.size = size;
		}
		return *this; }

	basic_string& operator=(const basic_string& other) { 
		is_long_ = other.is_long_;
		if (is_long_) {
			size_t size = other.data_.long_str.size;
			data_.long_str.size = size;
			data_.long_str.capacity = size + 1;
			data_.long_str.ptr = new T[size + 1];
			for (size_t i = 0; i < size; i++) {
				data_.long_str.ptr[i] = other.data_.long_str.ptr[i];
			}
			data_.long_str.ptr[size] = '\0';
		}
		else {
			size_t size = other.data_.short_str.size;
			for (size_t i = 0; i < size; i++) {
				data_.short_str.buffer[i] = other.data_.short_str.buffer[i];
			}
			data_.short_str.size = size;
			data_.short_str.buffer[size] = '\0';
		}
		return *this; }

	friend basic_string<T> operator+(const basic_string<T>& left,
									 const basic_string<T>& right)
	{
		size_t new_size = left.size() + right.size();
		basic_string str(new_size);
		const T* l_ptr = left.get_ptr();
		const T* r_ptr = right.get_ptr();
		if (str.is_long()) {
			for (size_t i = 0; i < left.size(); i++) {
				str.data_.long_str.ptr[i] = l_ptr[i];
			}
			for (size_t i = 0; i < right.size(); i++) {
				str.data_.long_str.ptr[i + left.size()] = r_ptr[i];
			}
		}
		else {
			for (size_t i = 0; i < left.size(); i++) {
				str.data_.short_str.buffer[i] = l_ptr[i];
			}
			for (size_t i = 0; i < right.size(); i++) {
				str.data_.short_str.buffer[i + left.size()] = r_ptr[i];
			}
		}
		return str;
	}

	template <typename S>
	friend S& operator<<(S& os, const basic_string& obj)
	{
		os << obj.c_str();
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
		size_t new_size = size() + other.size();
		if (new_size > SSO_CAPACITY) {
			T* new_ptr = new T[new_size + 1];
			const T* old_ptr1 = get_ptr();
			const T* old_ptr2 = other.get_ptr();
			for (size_t i = 0; i < size(); i++) {
				new_ptr[i] = old_ptr1[i];
			}
			if (is_long_) {
				delete[] data_.long_str.ptr;
			}
			for (size_t i = 0; i < other.size(); i++) {
				new_ptr[i + size()] = old_ptr2[i];
			}
			new_ptr[new_size] = '\0';
			data_.long_str.ptr = new_ptr;
			data_.long_str.size = new_size;
			data_.long_str.capacity = new_size + 1;
			is_long_ = true;
		}
		else {
			for (size_t i = 0; i < other.size(); i++) {
				data_.short_str.buffer[i + data_.short_str.size] = other.data_.short_str.buffer[i];
			}
			data_.short_str.size = new_size;
			is_long_ = false;
		}
		return *this; }

	basic_string& operator+=(T symbol) { 
		size_t new_size = size() + 1;
		if (new_size > SSO_CAPACITY) {
			T* new_ptr = new T[new_size + 1];
			for (size_t i = 0; i < size(); i++) {
				new_ptr[i] = get_ptr()[i];
			}
			clean_();
			data_.long_str.ptr = new_ptr;
			new_ptr = nullptr;
			data_.long_str.capacity = new_size + 1;
			is_long_ = true;
		}
		
		get_ptr()[new_size - 1] = symbol;
		get_ptr()[new_size] = '\0';
		
		if (is_long()){
			data_.long_str.size = new_size;
		}
		else{
			data_.short_str.size = new_size;
		}
		
		return *this; 
	}

	T& operator[](size_t index) noexcept
	{
		static T dummy;
		if (is_long_) {
			dummy = data_.long_str.ptr[index];
		}
		else {
			dummy = data_.short_str.buffer[index];
		}
		return dummy;
	}

	T& at(size_t index) { 
		T symbol;
		if (is_long_) {
			if (index < 0 || index >= data_.long_str.size) {
				throw std::out_of_range("Wrong index");
			}
			else {
				symbol = data_.long_str.ptr[index];
			}
		}
		else {
			if (index < 0 || index >= data_.short_str.size) {
				throw std::out_of_range("Wrong index");
			}
			else {
				symbol = data_.short_str.buffer[index];
			}
		}
		return symbol; }

	T* data() { 
		if (is_long_) {
			return data_.long_str.ptr;
		}
		return data_.short_str.buffer; }

   private:
	static size_t strlen_(const T* str) { 
		size_t i = 0;
		while (str[i] != '\0') {
			i++;
		}
		return i; }

	void clean_() {
		if (is_long_) {
			delete[] data_.long_str.ptr;
			data_.long_str.ptr = nullptr;
		}
	}
};
}  // namespace bmstu
