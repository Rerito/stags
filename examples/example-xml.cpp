#include <stags/stags_xml.hpp>
#include <pugixml.hpp>

#include <iostream>
#include <sstream>

#define CATCH_CONFIG_MAIN
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
