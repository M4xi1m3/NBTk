#include "NBTLoad.hpp"
#include <iostream>

using namespace nbtpp;

void nbt_load(tag* data, Glib::RefPtr<Gtk::TreeStore>& tree_model, NBTModel& model, const Gtk::TreeNodeChildren* tree) {
    Gtk::TreeModel::Row row;

    if (tree == nullptr) {
        row = *(tree_model->append());
    } else {
        row = *(tree_model->append(*tree));
    }

    std::string name = data->name();
    std::string type = name_for_type(data->type());

    row[model.m_col_name] = name;
    row[model.m_col_tag] = data;

    switch (data->type()) {
        case tag_type::TAG_Byte: {
            const tags::tag_byte *s = static_cast<const tags::tag_byte*>(data);
            row[model.m_col_icon] = Gdk::Pixbuf::create_from_file("imgs/tags/byte.png")->scale_simple(16, 16, Gdk::InterpType::INTERP_BILINEAR);
            row[model.m_col_value] = std::to_string(s->value()) + "B";
            break;
        }
        case tag_type::TAG_Short: {
            const tags::tag_short *s = static_cast<const tags::tag_short*>(data);
            row[model.m_col_icon] = Gdk::Pixbuf::create_from_file("imgs/tags/short.png")->scale_simple(16, 16, Gdk::InterpType::INTERP_BILINEAR);
            row[model.m_col_value] = std::to_string(s->value()) + "S";
            break;
        }
        case tag_type::TAG_Int: {
            const tags::tag_int *s = static_cast<const tags::tag_int*>(data);
            row[model.m_col_icon] = Gdk::Pixbuf::create_from_file("imgs/tags/int.png")->scale_simple(16, 16, Gdk::InterpType::INTERP_BILINEAR);
            row[model.m_col_value] = std::to_string(s->value()) + "";
            break;
        }
        case tag_type::TAG_Long: {
            const tags::tag_long *s = static_cast<const tags::tag_long*>(data);
            row[model.m_col_icon] = Gdk::Pixbuf::create_from_file("imgs/tags/long.png")->scale_simple(16, 16, Gdk::InterpType::INTERP_BILINEAR);
            row[model.m_col_value] = std::to_string(s->value()) + "L";
            break;
        }
        case tag_type::TAG_Float: {
            const tags::tag_float *s = static_cast<const tags::tag_float*>(data);
            row[model.m_col_icon] = Gdk::Pixbuf::create_from_file("imgs/tags/float.png")->scale_simple(16, 16, Gdk::InterpType::INTERP_BILINEAR);
            row[model.m_col_value] = std::to_string(s->value()) + "F";
            break;
        }
        case tag_type::TAG_Double: {
            const tags::tag_double *s = static_cast<const tags::tag_double*>(data);
            row[model.m_col_icon] = Gdk::Pixbuf::create_from_file("imgs/tags/double.png")->scale_simple(16, 16, Gdk::InterpType::INTERP_BILINEAR);
            row[model.m_col_value] = std::to_string(s->value()) + "D";
            break;
        }
        case tag_type::TAG_Byte_Array: {
            const tags::tag_bytearray *s = static_cast<const tags::tag_bytearray*>(data);
            row[model.m_col_icon] = Gdk::Pixbuf::create_from_file("imgs/tags/bytearray.png")->scale_simple(16, 16, Gdk::InterpType::INTERP_BILINEAR);
            row[model.m_col_value] = "[" + std::to_string(s->value().size()) + " bytes]";
            break;
        }
        case tag_type::TAG_String: {
            const tags::tag_string *s = static_cast<const tags::tag_string*>(data);
            row[model.m_col_icon] = Gdk::Pixbuf::create_from_file("imgs/tags/string.png")->scale_simple(16, 16, Gdk::InterpType::INTERP_BILINEAR);
            row[model.m_col_value] = "\"" + s->value() + "\"";
            break;
        }
        case tag_type::TAG_List: {
            const tags::tag_list *l = static_cast<const tags::tag_list*>(data);
            row[model.m_col_icon] = Gdk::Pixbuf::create_from_file("imgs/tags/list.png")->scale_simple(16, 16, Gdk::InterpType::INTERP_BILINEAR);
            row[model.m_col_value] = std::to_string(l->value().size()) + " entries";

            for (tag *i : l->value()) {
                nbt_load(i, tree_model, model, &row.children());
            }
            break;
        }
        case tag_type::TAG_Compound: {
            const tags::tag_compound *c = static_cast<const tags::tag_compound*>(data);
            row[model.m_col_icon] = Gdk::Pixbuf::create_from_file("imgs/tags/compound.png")->scale_simple(16, 16, Gdk::InterpType::INTERP_BILINEAR);
            row[model.m_col_value] = std::to_string(c->value().size()) + " entries";

            for (tag *i : c->value()) {
                nbt_load(i, tree_model, model, &row.children());
            }
            break;
        }
        case tag_type::TAG_Int_Array: {
            const tags::tag_intarray *s = static_cast<const tags::tag_intarray*>(data);
            row[model.m_col_icon] = Gdk::Pixbuf::create_from_file("imgs/tags/intarray.png")->scale_simple(16, 16, Gdk::InterpType::INTERP_BILINEAR);
            row[model.m_col_value] = "[" + std::to_string(s->value().size()) + " ints]";
            break;
        }
        case tag_type::TAG_Long_Array: {
            const tags::tag_longarray *s = static_cast<const tags::tag_longarray*>(data);
            row[model.m_col_icon] = Gdk::Pixbuf::create_from_file("imgs/tags/longarray.png")->scale_simple(16, 16, Gdk::InterpType::INTERP_BILINEAR);
            row[model.m_col_value] = "[" + std::to_string(s->value().size()) + " longs]";
            break;
        }
        default:
            break;
    }
}
