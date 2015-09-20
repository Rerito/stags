#pragma once

#include <map>
#include <sstream>

#ifdef MSVC_TYPEOF_HACK
#define STAGS_SPLIT_TYPEDEF(name, expr) \
	enum { name ## typeof_id = sizeof(*boost::type_of::encode_start(expr)) }; \
	typedef typename boost::type_of::msvc_typeid_wrapper<name ## typeof_id>::type name;
#else
#define STAGS_SPLIT_TYPEDEF(name, expr) typedef BOOST_TYPEOF_KEYWORD(expr) name;
#endif

namespace stags {
namespace ptree {

template<typename T>
struct get_type{
	template<int N>
	struct get_field {
		typedef T *ptr;

		enum { tag = max_tag - 1 - N };
		STAGS_SPLIT_TYPEDEF(result_type, field_info(ptr(), overload_choice<tag>()));
		typedef typename result_type::member_type member_type;

		result_type operator()() {
			return field_info(ptr(), overload_choice<tag>());
		}
	};

	enum { field_count = max_tag - 1 - STAGS_NEXT_ID(T) };
};

template<typename T>
std::string to_string(T const &obj, stag) {
	std::ostringstream oss;
	oss << obj;
	return oss.str();
}

template<typename T>
T to_value(std::string const &str) {
	std::istringstream iss(str);
	T obj = T();
	iss >> obj;
	return obj;
}

template<typename T, typename P>
struct inspector;

template<typename T, typename P, int N>
struct inspector_rec {
	static void cont(P const &process, T &obj) {
		inspector<T, P>::template read_fields_rec<N - 1>(process, obj);
	}
};

template<typename T, typename P>
struct inspector_rec <T, P, 0> {
	static void cont(P const &process, T &obj) { }
};

template<typename T, typename P>
struct inspector {
	
	template<int N>
	static void read_fields_rec(P const &process, T &obj) {
		typename get_type<T>::template get_field<N>::result_type info = typename get_type<T>::template get_field<N>()();
		dispatch(process.sublevel(info.name), obj.*(info.field));
		inspector_rec<T, P, N>::cont(process, obj);
	}

	template<typename S>
	static void dispatch(P const &process, S & value, typename enable_if<is_tostring<S>::value, void>::type * = 0) {
		process.call(value);
	}

	template<typename S>
	static void dispatch(P const &process, S & value, typename enable_if<has_class_info<S>::value, void>::type * = 0) {
		inspector<S, P>::template read_fields_rec<get_type<T>::field_count>(process, value);
	}

	template<typename U>
	static void dispatch(P const & process, std::vector<U> &value) {
		unsigned int counter = 0;
		for (typename std::vector<U>::iterator it = value.begin(), end = value.end(); it != end; ++it)			
			inspector<U, P>::dispatch(process.at(counter), *it);
	}
};

template<typename AT>
struct navigator {
	std::string path;

	AT clone_append(std::string const & frag) const {
		AT subprocess(*(AT*)this);
		subprocess.path += frag;
		return subprocess;
	}

	AT sublevel(std::string const &component) const {
		return clone_append((path.empty() ? "" : ".") + component);
	}

	AT at(int index) const {
		std::ostringstream subpath_builder;
		subpath_builder << "[" << index << "]";
		return clone_append(subpath_builder.str());
	}
};

struct ptree_writer : public navigator<ptree_writer> {
	std::map<std::string, std::string> &values;
	
	ptree_writer(std::map<std::string, std::string> &v) : values(v) {}

	template<typename T>
	void call(T const &value) const {
		values[path] = to_string(value, stag());
	}
};

struct ptree_reader : public navigator<ptree_reader> {
	std::map<std::string, std::string> &values;

	ptree_reader(std::map<std::string, std::string> &v) : values(v) {}

	template<typename T>
	void call(T &value) const {
		value = to_value<T>(values[path]);
	}
};

template<typename T>
struct property_tree {
	std::map<std::string, std::string> values;

	void read(T const &obj) {
		ptree_writer process(values);
		inspector<T, ptree_writer>::template read_fields_rec<get_type<T>::field_count>(process, const_cast<T &>(obj));
	}

	void write(T &obj) {
		ptree_reader process(values);
		inspector<T, ptree_reader>::template read_fields_rec<get_type<T>::field_count>(process, obj);
	}
};

} // ptree
} // stags

#define PTREE_CLASS(tt, tn) tt tn; namespace stags { class_info_type class_info(tn*){ return class_info_type(#tn); } } tt tn : private ::stags::serializable<tn>
#define PTREE_MEMBER(mt, mn) mt mn; friend ::stags::field_info_type<myt_, mt, STAGS_NEXT_ID(myt_), 0> field_info(myt_*, ::stags::overload_choice<STAGS_NEXT_ID(myt_)>) { return ::stags::field_info_type<myt_, mt, STAGS_NEXT_ID(myt_), 0>(#mn, &myt_::mn); }
