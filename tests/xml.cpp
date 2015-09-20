#include <stags/stags_xml.hpp>
#include <pugixml.hpp>

#include <iostream>
#include <sstream>

#include <catch.hpp>

XML_SERIALIZABLE(
struct, recipe)
{
	XML_ELEMENT(
	std::string, name);

	XML_ATTRIBUTE(
	unsigned int, difficulty);
};

XML_SERIALIZABLE(
struct, recipe_book)
{
	XML_ELEMENT(
	std::string, author);

	XML_ELEMENT(
	unsigned int, edition);

	XML_ARRAY(
	std::vector<recipe>, recipes);
};


TEST_CASE("Object is XML-serialized", "[stags-xml]") {
	recipe_book my_own_cakes;
	my_own_cakes.author = "Vanellope von Schweetz";
	my_own_cakes.edition = 8;
	my_own_cakes.recipes.resize(2);
	my_own_cakes.recipes[0].name = "Carrot Cake";
	my_own_cakes.recipes[0].difficulty = 4;
	my_own_cakes.recipes[1].name = "Big Ass Cookie";
	my_own_cakes.recipes[1].difficulty = 2;

	pugi::xml_document document;
	stags::xml::xml_serializer<recipe_book, stags::xml::pugi_backend> serializer;
	serializer.serialize(document, my_own_cakes);

	recipe_book new_book = serializer.deserialize(document);

	REQUIRE(new_book.author == my_own_cakes.author);
	REQUIRE(new_book.edition == my_own_cakes.edition);
	REQUIRE(new_book.recipes[0].name == my_own_cakes.recipes[0].name);
}

XML_SERIALIZABLE(
struct, attribute_test_struct) 
{
	XML_ATTRIBUTE(
	std::string, str_attr);
	XML_ATTRIBUTE(
	int, int_attr);
	XML_ATTRIBUTE(
	bool, bool_attr);
	XML_ATTRIBUTE(
	unsigned long long, ull_attr);
};

TEST_CASE("Attributes are serialized", "[stags-xml]") {
	pugi::xml_document document;
	stags::xml::xml_serializer<attribute_test_struct, stags::xml::pugi_backend> serializer;
	attribute_test_struct test_struct;
	test_struct.bool_attr = true;
	test_struct.int_attr = -7;
	test_struct.str_attr = "hello";
	test_struct.ull_attr = 743560972354792;

	serializer.serialize(document, test_struct);
	attribute_test_struct new_test_struct = serializer.deserialize(document);

	REQUIRE(test_struct.str_attr == new_test_struct.str_attr);
	REQUIRE(test_struct.bool_attr == new_test_struct.bool_attr);
	REQUIRE(test_struct.int_attr == new_test_struct.int_attr);
	REQUIRE(test_struct.ull_attr == new_test_struct.ull_attr);
}

XML_SERIALIZABLE(
struct, element_test_struct) 
{
	XML_ELEMENT(
	std::string, str_elem);
	XML_ELEMENT(
	int, int_elem);
	XML_ELEMENT(
	bool, bool_elem);
	XML_ELEMENT(
	unsigned long long, ull_elem);
};

TEST_CASE("Simple elements are serialized", "[stags-xml]") {
	pugi::xml_document document;
	stags::xml::xml_serializer<element_test_struct, stags::xml::pugi_backend> serializer;
	element_test_struct test_struct;
	test_struct.bool_elem = true;
	test_struct.int_elem = -7;
	test_struct.str_elem = "hello";
	test_struct.ull_elem = 743560972354792;

	serializer.serialize(document, test_struct);
	element_test_struct new_test_struct = serializer.deserialize(document);

	REQUIRE(test_struct.str_elem == new_test_struct.str_elem);
	REQUIRE(test_struct.bool_elem == new_test_struct.bool_elem);
	REQUIRE(test_struct.int_elem == new_test_struct.int_elem);
	REQUIRE(test_struct.ull_elem == new_test_struct.ull_elem);
}