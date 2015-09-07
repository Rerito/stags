#pragma once

#include <boost/typeof/typeof.hpp>

namespace stags {

template<class T, int N> struct the_counter;

template<class T, int L, int N = 1>
struct encode_counter
{
	__if_exists(the_counter<T, N + 256>)
	{
		static const unsigned count = (encode_counter<T, L, N + 257>::count);
	}
	__if_not_exists(the_counter<T, N + 256>)
	{
		__if_exists(the_counter<T, N + 64>)
		{
			static const unsigned count = (encode_counter<T, L, N + 65>::count);
		}
		__if_not_exists(the_counter<T, N + 64>)
		{
			__if_exists(the_counter<T, N + 16>)
			{
				static const unsigned count = (encode_counter<T, L, N + 17>::count);
			}
			__if_not_exists(the_counter<T, N + 16>)
			{
				__if_exists(the_counter<T, N + 4>)
				{
					static const unsigned count = (encode_counter<T, L, N + 5>::count);
				}
				__if_not_exists(the_counter<T, N + 4>)
				{
					__if_exists(the_counter<T, N>)
					{
						static const unsigned count = (encode_counter<T, L, N + 1>::count);
					}
					__if_not_exists(the_counter<T, N>)
					{
						static const unsigned count = N;
						typedef the_counter<T, N> type;
					}
				}
			}
		}
	}
};

template<typename MyT>
struct serializable {
	typedef MyT myt_;
};

template<int N>
struct ov_tag {};

template<typename Parent, typename Member, int Method>
struct field_info_type {
	std::string name;
	Member Parent::*field;

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

struct no_field {};
template<typename NoClass, int NoTag>
no_field field_info(NoClass*, stags::ov_tag<NoTag>) { throw; }

template<typename T>
struct is_class_info {
	enum { value = 0 };
};

template<>
struct is_class_info < class_info_type > {
	enum { value = 1 };
};

template<typename T, int N>
struct is_id_active {
	enum { value = is_class_info<BOOST_TYPEOF(field_info((T*)0, ov_tag<N>()))>::value	};
};

template<int N>
struct int_type {
	enum { value = N };
};

template<int N, typename True, typename False>
struct mpl_if {};

template<typename True, typename False>
struct mpl_if < 1, True, False > {
	typedef True type;
};

template<typename True, typename False>
struct mpl_if < 0, True, False > {
	typedef False type;
};

template<typename T, int N>
struct find_id {
	typedef mpl_if < is_id_active<T, N>::value, typename find_id<T, N + 1>::type, typename int_type<N>::type> type;
};

template<typename T>
struct next_available_id {
	enum { value = find_id<T, 0>::type::value };
};

#define STAGS_NEXT_ID(type) stags::next_available_id<type>::value

} // stags
