#pragma once

#include <string>
#include <vector>
#include "pugixml.hpp"

namespace stags {

    template<class T, int N> struct the_counter;

	template<class T, int L, int N = 1/*for similarity*/>
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

	namespace xml {
		struct pugi_backend {};
	}

	template<typename MyT>
	struct myt_base {
		typedef MyT myt_;
	};

	template<int N>
	struct ov_tag {};

	class class_info_type {
	public:
		class_info_type(std::string n) : name(n) {}
		std::string name;
	};

	enum xml_method {
		xml_method_element,
		xml_method_attribute,
		xml_method_text,
	};

	template<typename T>
	class_info_type class_info() {
		return class_info(static_cast<T*>(0));
	}

	template<typename T>
	void read_node(pugi::xml_node &node, std::vector<T> &value) {
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
	void read_node(pugi::xml_node &node, T &value) {
		typedef T *type;
		std::vector<field_info_type<T>> fields;
		fields.push_back(field_info(type(), ov_tag<1>()));
		fields.push_back(field_info(type(), ov_tag<2>()));
		fields.push_back(field_info(type(), ov_tag<3>()));
		fields.push_back(field_info(type(), ov_tag<4>()));
		fields.push_back(field_info(type(), ov_tag<5>()));
		fields.push_back(field_info(type(), ov_tag<6>()));
		fields.push_back(field_info(type(), ov_tag<7>()));
		fields.push_back(field_info(type(), ov_tag<8>()));
		fields.push_back(field_info(type(), ov_tag<9>()));

		for (std::vector<field_info_type<T>>::const_iterator iter = fields.begin(), end = fields.end(); iter != end; ++iter)
			iter->deserialize_member(value, node);
	}

	void read_node(pugi::xml_node &node, std::string &value) {
		value = node.text().as_string();
	}

	void read_node(pugi::xml_node &node, unsigned int &value) {
		value = node.text().as_uint();
	}

	template<typename T>
	void read_attr(pugi::xml_attribute &attr, T &value) {

	}

	void read_attr(pugi::xml_attribute &attr, unsigned int &value) {
		value = attr.as_uint();
	}

	template<typename T>
	void fill_node(pugi::xml_node &node, std::vector<T> const &value) {
		std::string element_name = class_info<T>().name;
		for (std::vector<T>::const_iterator iter = value.begin(), end = value.end(); iter != end; ++iter)
		{
			pugi::xml_node new_node = node.append_child(element_name.c_str());
			fill_node(new_node, *iter);
		}
	}

	template<typename T>
	void fill_node(pugi::xml_node &node, T const &value) {
		typedef T *type;
		std::vector<field_info_type<T>> fields;
		fields.push_back(field_info(type(), ov_tag<1>()));
		fields.push_back(field_info(type(), ov_tag<2>()));
		fields.push_back(field_info(type(), ov_tag<3>()));
		fields.push_back(field_info(type(), ov_tag<4>()));
		fields.push_back(field_info(type(), ov_tag<5>()));
		fields.push_back(field_info(type(), ov_tag<6>()));
		fields.push_back(field_info(type(), ov_tag<7>()));
		fields.push_back(field_info(type(), ov_tag<8>()));
		fields.push_back(field_info(type(), ov_tag<9>()));

		for (std::vector<field_info_type<T>>::const_iterator iter = fields.begin(), end = fields.end(); iter != end; ++iter)
			iter->serialize_member(value, node);
	}

	inline void fill_node(pugi::xml_node &node, std::string const &value) {
		node.text() = value.c_str();
	}

	inline void fill_node(pugi::xml_node &node, unsigned value) {
		node.text() = value;
	}

	inline void fill_node(pugi::xml_node &node, int value) {
		node.text() = value;
	}

	inline void fill_node(pugi::xml_node &node, bool value) {
		node.text() = value;
	}

	inline void fill_node(pugi::xml_node &node, long value) {
		node.text() = value;
	}

	template<typename T>
	void fill_attr(pugi::xml_attribute &attr, T const &value) {
		throw std::runtime_error("Unsupported serialization");
	}

	inline void fill_attr(pugi::xml_attribute &attr, bool value) {
		attr = value;
	}

	inline void fill_attr(pugi::xml_attribute &attr, unsigned int value) {
		attr = value;
	}

	inline void fill_attr(pugi::xml_attribute &attr, std::string const& value) {
		attr = value.c_str();
	}

	template<typename T>
	void serialize_field(pugi::xml_node &node, T const &field, std::string name, xml_method method) {
		if (method == xml_method_element)
		{
			pugi::xml_node new_node = node.append_child(name.c_str());
			fill_node(new_node, field);
		}
		else if (method == xml_method_attribute)
		{
			pugi::xml_attribute new_attr = node.append_attribute(name.c_str());
			fill_attr(new_attr, field);
		}
		else if (method == xml_method_text)
		{
			fill_node(node, field);
		}
	}

	template<typename T>
	void deserialize_field(pugi::xml_node &parent, T &field, std::string name, xml_method method) {
		if (method == xml_method_element)
		{
			pugi::xml_node node = parent.select_node(name.c_str()).node();
			read_node(node, field);
		}
		else if (method == xml_method_attribute)
		{
			pugi::xml_attribute attr = parent.attribute(name.c_str());
			read_attr(attr, field);
		}
		else if (method == xml_method_text)
		{
			read_node(parent, field);
		}
	}

	template<typename C>
	struct member_serializer_iface {
		virtual void serialize_member(C const &object, pugi::xml_node &parent, std::string name, xml_method m) = 0;
		virtual void deserialize_member(C &object, pugi::xml_node &parent, std::string name, xml_method m) = 0;
	};

	template<typename C, typename T>
	struct member_serializer : public member_serializer_iface<C> {
		T C::*field;

		member_serializer(T C::*f) : field(f) {}

		virtual void serialize_member(C const &object, pugi::xml_node &parent, std::string name, xml_method method) {
			serialize_field(parent, object.*field, name, method);
		}

		virtual void deserialize_member(C &object, pugi::xml_node &parent, std::string name, xml_method method) {
			deserialize_field(parent, object.*field, name, method);
		}
	};


	template<typename C>
	class field_info_type {
	public:
		field_info_type() : serializer() {}

		template<typename T>
		field_info_type(std::string n, T C::*field, xml_method m) : name(n), method(m) {
			serializer = new member_serializer < C, T >(field);
		}

		void serialize_member(C const &object, pugi::xml_node &parent) const {
			if (serializer) serializer->serialize_member(object, parent, name, method);
		}

		void deserialize_member(C &object, pugi::xml_node &parent) const {
			if (serializer) serializer->deserialize_member(object, parent, name, method);
		}

		member_serializer_iface<C> *serializer;
		std::string name;
		xml_method method;
	};

	template<typename C, typename T>
	field_info_type<C> make_field_info(T C::*field, std::string n, xml_method method) {
		return field_info_type<C>(n, field, method);
	}

	template<typename NoClass>
	class_info_type class_info(NoClass*) { throw std::runtime_error("Not serializable"); }

	template<typename NoClass, int NoTag>
	field_info_type<NoClass> field_info(NoClass*, stags::ov_tag<NoTag>) { return field_info_type<NoClass>(); }

} // stags

#define XML_SERIALIZABLE_NAME(xn, tt, tn) tt tn; namespace stags { class_info_type class_info(tn*){ return class_info_type(xn); } } tt tn : private ::stags::myt_base<tn>
#define XML_SERIALIZABLE(tt, tn) XML_SERIALIZABLE_NAME(#tn, tt, tn)

#define P_XML_FIELD(xn, ct, cn, st) ct cn; friend ::stags::field_info_type<myt_> field_info(myt_*, ::stags::ov_tag<::stags::encode_counter<myt_, __LINE__>::count>) { return ::stags::make_field_info<myt_, ct>(&myt_::cn, #cn, ::stags::xml_method_##st); }

#define XML_ELEMENT_NAME(xn, ct, cn)	P_XML_FIELD(xn, ct, cn, element)
#define XML_ELEMENT(ct, cn)				P_XML_FIELD(#cn, ct, cn, element)
#define XML_ATTRIBUTE_NAME(xn, ct, cn)	P_XML_FIELD(xn, ct, cn, attribute)
#define XML_ATTRIBUTE(ct, cn)			P_XML_FIELD(#cn, ct, cn, attribute)
#define XML_TEXT(ct, cn)				P_XML_FIELD(#cn, ct, cn, text)
#define XML_ARRAY(ct, cn)				P_XML_FIELD(#cn, ct, cn, element)

namespace stags {
	namespace xml {

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

	}
}

