#ifndef NBTMODEL_HPP_
#define NBTMODEL_HPP_

#include <gtkmm.h>
#include "nbtpp/nbt.hpp"

class NBTModel: public Gtk::TreeModelColumnRecord {
public:
    NBTModel() {
        add(m_col_tag);
        add(m_col_icon);
        add(m_col_name);
        add(m_col_value);
    }

    virtual ~NBTModel() {

    }

    Gtk::TreeModelColumn<nbtpp::tag*> m_col_tag;
    Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>> m_col_icon;
    Gtk::TreeModelColumn<Glib::ustring> m_col_name;
    Gtk::TreeModelColumn<Glib::ustring> m_col_value;
};

#endif /* NBTMODEL_HPP_ */
