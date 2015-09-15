#pragma once

#include <string>
#include <boost/typeof/typeof.hpp>

namespace stags {

struct true_type { enum { value = true }; };
struct false_type { enum { value = false }; };

typedef char yes_sized[1];
typedef char no_sized[2];

template<int I> struct size_to_bool;
template<> struct size_to_bool < 1 > : public true_type {};
template<> struct size_to_bool < 2 > : public false_type {};

template<bool B, class T>
struct enable_if {};

template<class T>
struct enable_if<true, T> { typedef T type; };

template<typename MyT>
struct serializable {
	typedef MyT myt_;
};

template<typename Parent, typename Member, int Tag, int Method>
struct field_info_type {
	std::string name;
	Member Parent::*field;

	typedef char tag_type[Tag];
	tag_type *tag;

	field_info_type(std::string const &n, Member Parent::*f) : name(n), field(f) {}
};

struct no_class_info { static no_sized *has_info(); };
template<typename NoClass>
no_class_info class_info(NoClass*) { throw; }

class class_info_type {
public:
	static yes_sized *has_info();

	class_info_type(std::string n) : name(n) {}
	class_info_type(no_class_info) {}

	std::string name;
};

template<typename T>
class_info_type class_info() {
	return class_info(static_cast<T*>(0));
}

template<typename T>
struct has_class_info : public size_to_bool<sizeof *class_info(static_cast<T*>(0)).has_info()> {};

enum { max_tag = 100 };

struct no_field {
	typedef char tag_type[max_tag];
	tag_type *tag;
};

template<typename T>
struct is_primitive : public false_type {};

template<> struct is_primitive<bool> : public true_type{};
template<> struct is_primitive<unsigned char> : public true_type{};
template<> struct is_primitive<signed char> : public true_type{};
template<> struct is_primitive<unsigned int> : public true_type{};
template<> struct is_primitive<signed int> : public true_type{};
template<> struct is_primitive<unsigned long> : public true_type{};
template<> struct is_primitive<signed long> : public true_type{};
template<> struct is_primitive<unsigned long long> : public true_type{};
template<> struct is_primitive<signed long long> : public true_type{};
template<> struct is_primitive<float> : public true_type{};
template<> struct is_primitive<double > : public true_type{};

template<unsigned I>
struct overload_choice : overload_choice<I + 1>{};

template<> struct overload_choice<max_tag>{};

struct select_overload : overload_choice<0>{};

template<typename NoClass>
no_field field_info(NoClass*, overload_choice<max_tag>) { throw; }

template<typename T, unsigned L>
struct next_id {
	enum { value = sizeof(*field_info((T*)0, select_overload()).tag) - 1 };
};

#define STAGS_NEXT_ID(myt_) ::stags::next_id<myt_, __LINE__>::value
} // stags
