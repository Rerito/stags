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
struct inspector;

template<typename T, int N>
struct inspector_rec {
	static void cont(std::map<std::string, std::string> &values, std::string const &path, T const &obj) {
		inspector<T>::template read_fields_rec<N - 1>(values, path, obj);
	}
};

template<typename T>
struct inspector_rec <T, 0> {
	static void cont(std::map<std::string, std::string> &values, std::string const &path, T const &obj) { }
};

template<typename T>
struct inspector {
	
	template<int N>
	static void read_fields_rec(std::map<std::string, std::string> &values, std::string const &path, T const &obj) {
		typename get_type<T>::template get_field<N>::result_type info = typename get_type<T>::template get_field<N>()();
		dispatch(values, path + info.name, obj.*(info.field));
		inspector_rec<T, N>::cont(values, path, obj);
	}

	template<typename S>
	static void dispatch(std::map<std::string, std::string> &values, std::string const &path, S const & value, typename enable_if<is_tostring<S>::value, void>::type * = 0) {
		values[path] = to_string(value, stag());
	}

	template<typename S>
	static void dispatch(std::map<std::string, std::string> &values, std::string const &path, S const & value, typename enable_if<has_class_info<S>::value, void>::type * = 0) {
		std::string new_path = path + ".";
		inspector<S>::template read_fields_rec<get_type<T>::field_count>(values, new_path, value);
	}

	template<typename U>
	static void dispatch(std::map<std::string, std::string> &values, std::string const &path, std::vector<U> const &value) {
		unsigned int counter = 0;
		for (typename std::vector<U>::const_iterator it = value.begin(), end = value.end(); it != end; ++it)
		{
			std::ostringstream subpath_builder;
			subpath_builder << path << "[" << counter++ << "]";
			inspector<U>::dispatch(values, subpath_builder.str(), *it);
		}
	}
};

template<typename T>
struct property_tree {
	std::map<std::string, std::string> values;

	void read(T const &obj) {
		inspector<T>::template read_fields_rec<get_type<T>::field_count>(values, "", obj);
	}
};

} // ptree
} // stags

#define PTREE_CLASS(tt, tn) tt tn; namespace stags { class_info_type class_info(tn*){ return class_info_type(#tn); } } tt tn : private ::stags::serializable<tn>
#define PTREE_MEMBER(mt, mn) mt mn; friend ::stags::field_info_type<myt_, mt, STAGS_NEXT_ID(myt_), 0> field_info(myt_*, ::stags::overload_choice<STAGS_NEXT_ID(myt_)>) { return ::stags::field_info_type<myt_, mt, STAGS_NEXT_ID(myt_), 0>(#mn, &myt_::mn); }
