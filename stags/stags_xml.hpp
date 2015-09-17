#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include "pugixml.hpp"

#include <stags/stags.hpp>

namespace stags {
namespace xml {

struct pugi_backend {};

enum method {
	method_element,
	method_attribute,
	method_text,
};

template<typename C>
class any_field_info_type;

template<typename T>
std::vector<any_field_info_type<T> > get_field_info();

/* PUGIXML Y U DO THIS :'( 
 * No easy way to convert to a given type with pugixml, so
 * we just list all the conversions */
template<typename PugiType>
struct pugi_converter {
	template<typename Target>
	static Target convert_primitive(PugiType const &textish) {
		return convert_primitive(textish, Target());
	}

	static bool convert_primitive(PugiType const &textish, bool) {
		return textish.as_bool();
	}

	static int convert_primitive(PugiType const &textish, int) {
		return textish.as_int();
	}

	static unsigned int convert_primitive(PugiType const &textish, unsigned int) {
		return textish.as_uint();
	}

	static float convert_primitive(PugiType const &textish, float) {
		return textish.as_float();
	}

	static double convert_primitive(PugiType const &textish, double) {
		return textish.as_double();
	}

	static long long convert_primitive(PugiType const &textish, long long) {
		return textish.as_llong();
	}

	static unsigned long long convert_primitive(PugiType const &textish, unsigned long long) {
		return textish.as_ullong();
	}
};

template<typename Target, typename PugiType>
Target pugi_convert_primitive(PugiType const &textish) {
	return pugi_converter<PugiType>::template convert_primitive<Target>(textish);
}

template<typename T>
void read_node(pugi::xml_node const &node, T &value, typename enable_if<has_class_info<T>::value, void>::type * = 0);

template<typename T>
void read_node(pugi::xml_node const &node, std::vector<T> &value) {
	std::string element_name = class_info<T>().name;

	pugi::xpath_node_set nodes = node.select_nodes(element_name.c_str());
	value.reserve(nodes.size());
	for (pugi::xpath_node_set::const_iterator iter = nodes.begin(), end = nodes.end(); iter != end; ++iter)
	{
		value.push_back(T());
		read_node(iter->node(), value.back());
	}
}

template<typename T>
void read_node(pugi::xml_node const &node, T &value, typename enable_if<has_class_info<T>::value, void>::type *) {
	std::vector<any_field_info_type<T> > fields = get_field_info<T>();

	for (typename std::vector<any_field_info_type<T> >::const_iterator iter = fields.begin(), end = fields.end(); iter != end; ++iter)
		iter->deserialize_member(value, node);
}

void read_node(pugi::xml_node const &node, std::string &value) {
	value = node.text().as_string();
}

template<typename T>
void read_node(pugi::xml_node const &node, T &value, typename enable_if<is_primitive<T>::value, void>::type * = 0) {
	value = pugi_convert_primitive<T>(node.text());
}

template<typename T>
void read_attr(pugi::xml_attribute &attr, T &value, overload_choice<0>, typename enable_if<is_primitive<T>::value, void>::type * = 0) {
	value = pugi_convert_primitive<T>(attr);
}

inline
void read_attr(pugi::xml_attribute &attr, std::string &value, overload_choice<0>) {
	value = attr.as_string();
}

template<typename T>
void read_attr(pugi::xml_attribute &attr, T &value, overload_choice<1>) {
	throw;
}

template<typename T>
void read_attr(pugi::xml_attribute &attr, T &value) {
	read_attr(attr, value, select_overload());
}

template<typename T>
void fill_node(pugi::xml_node &node, T const &value, typename enable_if<has_class_info<T>::value, void>::type * = 0);

template<typename T>
void fill_node(pugi::xml_node &node, std::vector<T> const &value) {
	std::string element_name = class_info<T>().name;
	for (typename std::vector<T>::const_iterator iter = value.begin(), end = value.end(); iter != end; ++iter)
	{
		pugi::xml_node new_node = node.append_child(element_name.c_str());
		fill_node(new_node, *iter);
	}
}

template<typename T>
void fill_node(pugi::xml_node &node, T const &value, typename enable_if<has_class_info<T>::value, void>::type *) {
	std::vector<any_field_info_type<T> > fields = get_field_info<T>();

	for (typename std::vector<any_field_info_type<T> >::const_iterator iter = fields.begin(), end = fields.end(); iter != end; ++iter)
		iter->serialize_member(value, node);
}

inline void fill_node(pugi::xml_node &node, std::string const &value) {
	node.text() = value.c_str();
}

template<typename T>
void fill_node(pugi::xml_node &node, T value, typename enable_if<is_primitive<T>::value, void>::type * = 0) {
	node.text() = value;
}

template<typename T>
void fill_attr(pugi::xml_attribute &attr, T const &value, overload_choice<1>) {
	throw std::runtime_error("Unsupported serialization");
}

template<typename T>
void fill_attr(pugi::xml_attribute &attr, T value, overload_choice<0>, typename enable_if<is_primitive<T>::value, void>::type * = 0) {
	attr = value;
}

inline void fill_attr(pugi::xml_attribute &attr, std::string const& value, overload_choice<0>) {
	attr = value.c_str();
}

template<typename T>
void fill_attr(pugi::xml_attribute &attr, T const &value) {
	fill_attr(attr, value, select_overload());
}

template<typename T>
void serialize_field(pugi::xml_node &node, T const &field, std::string name, method method) {
	if (method == method_element)
	{
		pugi::xml_node new_node = node.append_child(name.c_str());
		fill_node(new_node, field);
	}
	else if (method == method_attribute)
	{
		pugi::xml_attribute new_attr = node.append_attribute(name.c_str());
		fill_attr(new_attr, field);
	}
	else if (method == method_text)
	{
		fill_node(node, field);
	}
}

template<typename T>
void deserialize_field(pugi::xml_node const &parent, T &field, std::string name, method method) {
	if (method == method_element)
	{
		pugi::xml_node node = parent.select_node(name.c_str()).node();
		read_node(node, field);
	}
	else if (method == method_attribute)
	{
		pugi::xml_attribute attr = parent.attribute(name.c_str());
		read_attr(attr, field);
	}
	else if (method == method_text)
	{
		read_node(parent, field);
	}
}

template<typename C>
class any_field_info_type {
	struct iserializer {
		virtual void serialize_member(C const &object, pugi::xml_node &parent) const = 0;
		virtual void deserialize_member(C &object, pugi::xml_node const &parent) const = 0;
	};

	template<typename Member, int Tag, int Method>
	struct field_serializer : public iserializer {
		field_info_type<C, Member, Tag, Method> field_info;

		field_serializer(field_info_type<C, Member, Tag, Method> const &fi) : field_info(fi) {}

		void serialize_member(C const &object, pugi::xml_node &parent) const {
			serialize_field(parent, object.*field_info.field, field_info.name, (stags::xml::method)Method);
		}

		void deserialize_member(C &object, pugi::xml_node const &parent) const {
			deserialize_field(parent, object.*field_info.field, field_info.name, (stags::xml::method)Method);
		}
	};

	iserializer *serializer;

public:
	any_field_info_type() : serializer() {}

	template<typename Member, int Tag, int Method>
	any_field_info_type(field_info_type<C, Member, Tag, Method> const &info) {
		serializer = new field_serializer<Member, Tag, Method>(info);
	}

	void serialize_member(C const &object, pugi::xml_node &parent) const {
		serializer->serialize_member(object, parent);
	}

	void deserialize_member(C &object, pugi::xml_node const &parent) const {
		serializer->deserialize_member(object, parent);
	}
};

template<int N, typename T>
void append_field_maybe(std::vector<any_field_info_type<T> > &fields);

template<typename Tag>
struct field_aggregator {
	template<int N, typename T>
	static void append_next_field(std::vector<any_field_info_type<T> > &fields) {
		typedef T *type;
		fields.push_back(field_info(type(), overload_choice<N>()));
		append_field_maybe<N + 1>(fields);
	}
};

template<>
struct field_aggregator < no_field > {
	template<int N, typename T>
	static void append_next_field(std::vector<any_field_info_type<T> > &fields) {}
};

template<int N, typename T>
void append_field_maybe(std::vector<any_field_info_type<T> > &fields) {
	typedef T *type;
	typedef BOOST_TYPEOF(field_info(type(), overload_choice<N>())) result_type;
	field_aggregator<result_type>::template append_next_field<N, T>(fields);
}

template<typename T>
std::vector<any_field_info_type<T> > get_field_info() {
	std::vector<any_field_info_type<T> > info;
	append_field_maybe<STAGS_NEXT_ID(T) - 1>(info);
	return info;
}

template<typename RootElement, typename>
class xml_serializer {
public:
	xml_serializer() {}

	void serialize(pugi::xml_document &doc, RootElement const &object) {
		class_info_type info = class_info<RootElement>();

		pugi::xml_node node = doc.root().append_child(info.name.c_str());
		fill_node(node, object);
	}

	RootElement deserialize(pugi::xml_document &doc) {
		class_info_type info = class_info<RootElement>();

		RootElement object;
		read_node(doc.root().select_node(info.name.c_str()).node(), object);
		return object;
	}
};

} // xml
} // stags

#define XML_SERIALIZABLE_NAME(xn, tt, tn) tt tn; namespace stags { class_info_type class_info(tn*){ return class_info_type(xn); } } tt tn : private ::stags::serializable<tn>
#define XML_SERIALIZABLE(tt, tn) XML_SERIALIZABLE_NAME(#tn, tt, tn)

#define P_XML_FIELD(xn, ct, cn, st) ct cn; friend ::stags::field_info_type<myt_, ct, STAGS_NEXT_ID(myt_), ::stags::xml::method_##st> field_info(myt_*, ::stags::overload_choice<STAGS_NEXT_ID(myt_)>) { return ::stags::field_info_type<myt_, ct, STAGS_NEXT_ID(myt_), ::stags::xml::method_##st>(xn, &myt_::cn); }

#define XML_ELEMENT_NAME(xn, ct, cn)	P_XML_FIELD(xn, ct, cn, element)
#define XML_ELEMENT(ct, cn)				P_XML_FIELD(#cn, ct, cn, element)
#define XML_ATTRIBUTE_NAME(xn, ct, cn)	P_XML_FIELD(xn, ct, cn, attribute)
#define XML_ATTRIBUTE(ct, cn)			P_XML_FIELD(#cn, ct, cn, attribute)
#define XML_TEXT(ct, cn)				P_XML_FIELD(#cn, ct, cn, text)
#define XML_ARRAY(ct, cn)				P_XML_FIELD(#cn, ct, cn, element)
