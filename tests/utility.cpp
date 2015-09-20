#include <iostream>
#include <vector>

#include <stags/stags.hpp>
#include <catch.hpp>

template<typename T>
bool is_primitive(const T &, stags::overload_choice<0>, typename stags::enable_if<stags::is_primitive<T>::value, void>::type * = 0) {
	return true;
}

template<typename T>
bool is_primitive(const T &, stags::overload_choice<1>) {
	return false;
}

template<typename T>
bool is_primitive(const T & t) {
	return is_primitive(t, stags::select_overload());
}

TEST_CASE("Check for primitives", "[primitive]") {
	REQUIRE(is_primitive(5U));
	REQUIRE(is_primitive(-5));
	REQUIRE(is_primitive(5UL));
	REQUIRE(is_primitive(-5L));
	REQUIRE(is_primitive(5ULL));
	REQUIRE(is_primitive(-5LL));
	REQUIRE(is_primitive(static_cast<signed char>('a')));
	REQUIRE(is_primitive(static_cast<unsigned char>('a')));
	REQUIRE(is_primitive(-5.0));
	REQUIRE(is_primitive(-5.0f));

	REQUIRE(!is_primitive("hello"));
	REQUIRE(!is_primitive(std::cout));
	REQUIRE(!is_primitive(std::vector<int>()));
}

struct has {};
struct has_not {};

stags::class_info_type class_info(has*) { throw; }

TEST_CASE("Check for class info", "[classinfo]") {
	REQUIRE(stags::has_class_info<has>::value);
	REQUIRE(!stags::has_class_info<has_not>::value);
}
