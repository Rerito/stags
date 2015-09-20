#include <iostream>
#include <vector>
#include <typeinfo>

#include <stags/stags.hpp>
#include <stags/stags_ptree.hpp>
#include <catch.hpp>

PTREE_CLASS(
struct, apt_config) {
	PTREE_MEMBER(std::vector<std::string>, packages);
};

PTREE_CLASS(
struct, addons_config) {
	PTREE_MEMBER(apt_config, apt);
};

PTREE_CLASS(
struct, travis_config) {
	PTREE_MEMBER(std::string, language);
	PTREE_MEMBER(bool, sudo);
	PTREE_MEMBER(std::vector<std::string>, script);
	PTREE_MEMBER(addons_config, addons);
};

TEST_CASE("Fetch a single annotation", "[static-info]") {
	using namespace stags::ptree;

	REQUIRE(typeid(get_type<travis_config>::get_field<0>::member_type).name() == typeid(std::string).name());
	REQUIRE(typeid(get_type<travis_config>::get_field<1>::member_type).name() == typeid(bool).name());
	REQUIRE(get_type<travis_config>::field_count == 4);
}

TEST_CASE("Read into a property tree", "[ptree-rw]") {
	using namespace stags::ptree;

	travis_config my_config;
	my_config.script.push_back("git submodule init");
	my_config.addons.apt.packages.push_back("libboost-dev");
	my_config.sudo = false;
	my_config.language = "cpp";

	property_tree<travis_config> ptree;
	ptree.read(my_config);

	REQUIRE(ptree.values["addons.apt.packages[0]"] == "libboost-dev");
	REQUIRE(ptree.values["sudo"] == "0");
}