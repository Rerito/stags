#pragma once

#include <boost/typeof/typeof.hpp>

namespace stags {

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

struct no_class_info {};
template<typename NoClass>
no_class_info class_info(NoClass*) { throw; }

class class_info_type {
public:
	class_info_type(std::string n) : name(n) {}
	class_info_type(no_class_info) {}

	std::string name;
};

template<typename T>
class_info_type class_info() {
	return class_info(static_cast<T*>(0));
}

enum { max_tag = 100 };

struct no_field {
	typedef char tag_type[max_tag];
	tag_type *tag;
};


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
