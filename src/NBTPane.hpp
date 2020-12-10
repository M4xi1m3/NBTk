#ifndef NBTPANE_HPP_
#define NBTPANE_HPP_

#include <gtkmm.h>
#include "NBTModel.hpp"
#include "NBTLoad.hpp"
#include "nbtpp/nbt.hpp"

class NBTPane {
public:
    NBTPane(nbtpp::nbt& tags, const std::string& file, const std::string& path) : m_file(file), m_path(path) {

        m_tree = Gtk::TreeView();
        m_tree_model = Gtk::TreeStore::create(m_model);
        m_tree.set_model(m_tree_model);

        m_tree.remove_all_columns();
        m_tree_model->clear();

        nbt_load(tags.getTag(), m_tree_model, m_model, nullptr);
        // tree_model->clear();

        m_str_col = Gtk::CellRendererText();
        m_px_buf = Gtk::CellRendererPixbuf();
        m_px_col = Gtk::TreeViewColumn();

        m_px_col.pack_start(m_px_buf, false);
        m_px_col.pack_start(m_str_col, false);

        m_px_col.set_cell_data_func(m_px_buf, [this](Gtk::CellRenderer* renderer, const Gtk::TreeModel::iterator& iter) {
            if (iter) {
                Gtk::TreeModel::Row row = *iter;
                Glib::RefPtr<Gdk::Pixbuf> icon = row[m_model.m_col_icon];
                m_px_buf.property_pixbuf() = icon;
            }
        });

        m_px_col.set_cell_data_func(m_str_col, [this](Gtk::CellRenderer* renderer, const Gtk::TreeModel::iterator& iter) {
            if (iter) {
                Gtk::TreeModel::Row row = *iter;
                Glib::ustring name = row[m_model.m_col_name];
                Glib::ustring value = row[m_model.m_col_value];
                m_str_col.property_text() = name + ": " + value;
            }
        });

        m_tree.append_column(m_px_col);
        m_tree.property_headers_visible() = false;

        m_scroll = Gtk::ScrolledWindow();
        m_scroll.add(m_tree);
    }

    inline Gtk::Widget& getContent() {
        return m_scroll;
    }

    ~NBTPane() {

    }

    const std::string& getFile() const {
        return m_file;
    }

    const std::string& getPath() const {
        return m_path;
    }

private:
    Gtk::TreeView m_tree;
    Gtk::ScrolledWindow m_scroll;

    NBTModel m_model;
    Gtk::CellRendererText m_str_col;
    Gtk::CellRendererPixbuf m_px_buf;
    Gtk::TreeViewColumn m_px_col;
    Glib::RefPtr<Gtk::TreeStore> m_tree_model;

    std::string m_file;
    std::string m_path;
};



#endif /* NBTPANE_HPP_ */
