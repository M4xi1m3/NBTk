#ifndef NBTEDITOR_HPP_
#define NBTEDITOR_HPP_

#include <gtkmm.h>
#include <fstream>

#include "NBTLoad.hpp"

class NBTEditor: public Gtk::ApplicationWindow {
public:
    NBTEditor(Glib::RefPtr<Gtk::Application> app) : Gtk::ApplicationWindow() {
        // Load header
        Glib::RefPtr<Gtk::Builder> header_builder = Gtk::Builder::create_from_file("ui/header.ui");
        header_builder->get_widget("header", m_header);
        header_builder->get_widget("header_menu_btn", m_header_menu_btn);

        // Load menues
        Glib::RefPtr<Gtk::Builder> menu_builder = Gtk::Builder::create_from_file("ui/menu.ui");
        Glib::RefPtr<Glib::Object> obj = menu_builder->get_object("menu_more");
        Glib::RefPtr<Gio::Menu> gmenu = Glib::RefPtr<Gio::Menu>::cast_dynamic(obj);
        m_header_menu = new Gtk::Popover(*m_header_menu_btn, gmenu);
        m_header_menu_btn->set_popover(*m_header_menu);
//        new Gtk::Menu(gmenu);
        // m_header_menu->set_mo

        // Load about menu
        Glib::RefPtr<Gtk::Builder> about_builder = Gtk::Builder::create_from_file("ui/about.ui");
        about_builder->get_widget("about", m_about);

        // Load main content
        Glib::RefPtr<Gtk::Builder> main_builder = Gtk::Builder::create_from_file("ui/main.ui");
        main_builder->get_widget("main", m_main);
        main_builder->get_widget("main_label", m_main_label);
        main_builder->get_widget("main_scroll", m_main_scroll);
        main_builder->get_widget("main_tree", m_main_tree);

        // About click action
        app->add_action("about", [this]() {
            m_about->show();
        });
        // Open click action
        app->add_action("open", sigc::mem_fun(this, &NBTEditor::open_action));

        // Initialize window
        set_titlebar(*m_header);
        set_icon_from_file("imgs/logo-128.png");
        add(*m_main);
        set_default_size(800, 600);

        m_tree_model = Gtk::TreeStore::create(m_model);
        m_main_tree->set_model(m_tree_model);
    }

    bool load_file(std::string file_name) {
        m_main_tree->remove_all_columns();
        m_tree_model->clear();

        std::ifstream file(file_name);
        nbtpp::nbt tags;

        try {
            tags.load_file(file);
        } catch (std::exception &e) {
            std::cout << e.what() << std::endl;
            return false;
        }

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

        m_main_tree->append_column(m_px_col);

        return true;
    }

    void open_action() {
        Gtk::FileChooserDialog file_dialog(*this, "Open NBT File...", Gtk::FileChooserAction::FILE_CHOOSER_ACTION_OPEN, Gtk::DialogFlags::DIALOG_USE_HEADER_BAR);
        file_dialog.add_button("Select File", Gtk::ResponseType::RESPONSE_OK);
        file_dialog.add_button("_Cancel", Gtk::ResponseType::RESPONSE_CANCEL);

        const int response = file_dialog.run();
        file_dialog.hide();
        if (response == Gtk::ResponseType::RESPONSE_OK) {
            auto selected_uri = file_dialog.get_filename();

            std::cout << "URI selected = " << selected_uri << std::endl;

            if (load_file(selected_uri)) {
                std::cout << "Loaded!" << std::endl;

                m_main_scroll->show_all();
                m_main_label->hide();

                m_header->set_title(file_dialog.get_file()->get_basename());
                m_header->set_subtitle(file_dialog.get_file()->get_parent()->get_parse_name());
            } else {
                Gtk::MessageDialog error_dialog("Error loading NBT File", false, Gtk::MessageType::MESSAGE_ERROR, Gtk::ButtonsType::BUTTONS_OK);
                error_dialog.set_secondary_text("An unexpected error happened while loading " + file_dialog.get_file()->get_basename());
                error_dialog.run();
            }
        }
    }

    virtual ~NBTEditor() {
        delete m_header;
        delete m_header_menu;
        delete m_header_menu_btn;

        delete m_about;

        delete m_main;
        delete m_main_label;
        delete m_main_scroll;
        delete m_main_tree;
    }
private:
    Gtk::HeaderBar *m_header;
    Gtk::Popover *m_header_menu;
    Gtk::MenuButton *m_header_menu_btn;

    Gtk::AboutDialog *m_about;

    Gtk::Box *m_main;
    Gtk::Label *m_main_label;
    Gtk::ScrolledWindow *m_main_scroll;
    Gtk::TreeView *m_main_tree;

    NBTModel m_model;
    Gtk::CellRendererText m_str_col;
    Gtk::CellRendererPixbuf m_px_buf;
    Gtk::TreeViewColumn m_px_col;
    Glib::RefPtr<Gtk::TreeStore> m_tree_model;
};

#endif
