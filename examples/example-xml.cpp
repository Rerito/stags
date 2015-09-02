#include <stags/stags_xml.hpp>
#include <pugixml.hpp>

#include <iostream>
#include <sstream>

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

int main(int argc, char* argv[])
{
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

	std::ostringstream oss;
	document.print(oss);

	std::cout << oss.str() << std::endl;

	recipe_book new_book = serializer.deserialize(document);

	return 0;
}

