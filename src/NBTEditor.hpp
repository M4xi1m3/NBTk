#ifndef NBTEDITOR_HPP_
#define NBTEDITOR_HPP_

#include <gtkmm.h>
#include <fstream>
#include <vector>

class NBTEditor;

#include "NBTLoad.hpp"
#include "NBTPane.hpp"
#include "NBTTreeView.hpp"

class NBTEditor: public Gtk::ApplicationWindow {
public:
    NBTEditor(Glib::RefPtr<Gtk::Application> app) : Gtk::ApplicationWindow(), m_app(app) {
        // Load header
        Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_file("ui/window.ui");
        builder->get_widget("header", m_header);
        builder->get_widget("header_menu_btn", m_header_menu_btn);
        builder->get_widget("header_save_btn", m_header_save_btn);

        // Load header title display / controls
        builder->get_widget("header_title_box", m_header_title_box);
        builder->get_widget("header_title", m_header_title);
        builder->get_widget("header_subtitle", m_header_subtitle);
        builder->get_widget("header_switcher", m_header_switcher);

        // Load about
        builder->get_widget("about", m_about);

        // Load main content
        builder->get_widget("main", m_main);
        builder->get_widget("main_label", m_main_label);
        builder->get_widget("main_stack", m_main_stack);

        // Load menues
        Glib::RefPtr<Gtk::Builder> menu_builder = Gtk::Builder::create_from_file("ui/menu.ui");
        Glib::RefPtr<Glib::Object> obj = menu_builder->get_object("menu_more");
        Glib::RefPtr<Gio::Menu> gmenu = Glib::RefPtr<Gio::Menu>::cast_dynamic(obj);
        m_header_menu = new Gtk::Popover(*m_header_menu_btn, gmenu);
        m_header_menu_btn->set_popover(*m_header_menu);

        // About about action
        app->add_action("about", sigc::mem_fun(m_about, &Gtk::AboutDialog::show));

        // Open click action
        app->add_action("open", sigc::mem_fun(this, &NBTEditor::open_action));

        // Initialize window
        set_titlebar(*m_header);
        set_icon_from_file("imgs/logo-128.png");
        add(*m_main);
        set_default_size(800, 600);
        update_topbar();

        app->add_accelerator("<Primary>o", "app.open");
        app->add_accelerator("<Primary>s", "app.save");

    }

    void mark_saved(NBTPane* pane) {
        if (m_tabs.size() == 1) {
            m_header_title->set_label((pane->saved ? "" : "*") + pane->getFile());
        }

        m_main_stack->child_property_title(*pane) = (pane->saved ? "" : "*") + pane->getFile();
    }

    virtual ~NBTEditor() {

    }

private:
    void save() {
        if (m_main_stack->get_visible_child() != nullptr) {
            NBTPane* pane = static_cast<NBTPane*>(m_main_stack->get_visible_child());
            pane->save();
        }
    }

    bool load_file(std::string file_name, std::string basename, std::string dirname) {
        std::ifstream file(file_name);
        nbtpp::nbt *tags = new nbtpp::nbt();

        try {
            tags->load_file(file);
        } catch (std::exception &e) {
            std::cout << e.what() << std::endl;
            delete tags;
            return false;
        }

        NBTPane *pane = new NBTPane(tags, file_name, basename, dirname, this);
        m_main_stack->add(*pane, file_name, basename);
        pane->show();
        m_main_stack->set_visible_child(*pane);
        m_tabs.push_back(pane);

        return true;
    }

    void update_topbar() {
        if (m_app->has_action("save") && m_tabs.size() <= 0) {
            m_app->remove_action("save");
        } else if (!m_app->has_action("save") && m_tabs.size() > 0) {
            // About save action
            m_app->add_action("save", sigc::mem_fun(this, &NBTEditor::save));
        }

        if (m_tabs.size() <= 0) {
            m_main_label->show();
            m_header_title->set_label("NBTk");
            m_header_title->show();
            m_header_subtitle->hide();
            m_header_title_box->show();
            m_header_switcher->hide();
            m_header_save_btn->set_sensitive(false);
        } else if (m_tabs.size() == 1) {
            m_main_label->hide();
            m_header_title->set_label(m_tabs[0]->getFile());
            m_header_title->show();
            m_header_subtitle->set_label(m_tabs[0]->getDirectory());
            m_header_subtitle->show();
            m_header_title_box->show();
            m_header_switcher->hide();
            m_header_save_btn->set_sensitive(true);
        } else {
            m_main_label->hide();
            m_header_title->hide();
            m_header_subtitle->hide();
            m_header_title_box->hide();
            m_header_switcher->show();
            m_header_save_btn->set_sensitive(true);
        }
    }

    void open_action() {
        Gtk::FileChooserDialog file_dialog(*this, "Open NBT File...", Gtk::FileChooserAction::FILE_CHOOSER_ACTION_OPEN,
                Gtk::DialogFlags::DIALOG_USE_HEADER_BAR);
        file_dialog.add_button("Select File", Gtk::ResponseType::RESPONSE_OK);
        file_dialog.add_button("_Cancel", Gtk::ResponseType::RESPONSE_CANCEL);

        const int response = file_dialog.run();
        file_dialog.hide();
        if (response == Gtk::ResponseType::RESPONSE_OK) {
            auto selected_uri = file_dialog.get_filename();

            std::cout << "URI selected = " << selected_uri << std::endl;

            if (load_file(selected_uri, file_dialog.get_file()->get_basename(), file_dialog.get_file()->get_parent()->get_parse_name())) {
                std::cout << "Loaded!" << std::endl;

                update_topbar();

                m_main_stack->show_all();
            } else {
                Gtk::MessageDialog error_dialog("Error loading NBT File", false, Gtk::MessageType::MESSAGE_ERROR, Gtk::ButtonsType::BUTTONS_OK);
                error_dialog.set_secondary_text("An error happened while loading " + file_dialog.get_file()->get_basename());
                error_dialog.run();
            }
        }
    }

    Gtk::HeaderBar *m_header;
    Gtk::Popover *m_header_menu;
    Gtk::MenuButton *m_header_menu_btn;
    Gtk::Button *m_header_save_btn;

    Gtk::Box *m_header_title_box;
    Gtk::Label *m_header_title;
    Gtk::Label *m_header_subtitle;
    Gtk::StackSwitcher *m_header_switcher;

    Gtk::AboutDialog *m_about;

    Gtk::Box *m_main;
    Gtk::Label *m_main_label;
    Gtk::Stack *m_main_stack;

    std::vector<NBTPane*> m_tabs;

    Glib::RefPtr<Gtk::Application> m_app;
};

#endif
