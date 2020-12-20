#ifndef NBTEDITOR_HPP_
#define NBTEDITOR_HPP_

#include <gtkmm.h>
#include <fstream>
#include <list>

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

        // About action
        app->add_action("about", sigc::mem_fun(m_about, &Gtk::AboutDialog::show));

        // Open action
        app->add_action("open", sigc::mem_fun(this, &NBTEditor::open_action));
        app->add_accelerator("<Primary>o", "app.open");

        // Close action
        m_app->add_action("closefile", sigc::mem_fun(this, &NBTEditor::close_file));
        m_app->add_accelerator("<Primary>w", "app.closefile");

        // Initialize window
        set_titlebar(*m_header);
        set_icon_from_file("imgs/logo-128.png");
        add(*m_main);
        set_default_size(800, 600);
        update_topbar();

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
    void close_file() {
        if (m_main_stack->get_visible_child() != nullptr) {
            NBTPane *pane = static_cast<NBTPane*>(m_main_stack->get_visible_child());
            if (!pane->saved) {
                Gtk::MessageDialog unsaved_dialog("Save changes to \"" + pane->getFile() + "\" before closing ?", false, Gtk::MessageType::MESSAGE_WARNING,
                        Gtk::ButtonsType::BUTTONS_NONE, true);
                unsaved_dialog.set_transient_for(*this);
                unsaved_dialog.set_secondary_text("Your changes will be lost if you don't save them.");
                unsaved_dialog.add_button("Close without saving", 1)->get_style_context()->add_class("destructive-action");
                unsaved_dialog.add_button("Cancel", 0);
                this->set_focus(*unsaved_dialog.add_button("Save", 2));

                switch (unsaved_dialog.run()) {
                    case 0:
                        return;
                    case 2:
                        if (pane->getPath() == "") {
                            save_as();
                        } else {
                            save();
                        }
                        // no break
                    case 1:
                        break;
                }
            }

            m_main_stack->remove(*pane);
            m_tabs.remove(pane);
            update_topbar();
        } else {
            m_app->quit();
        }
    }

    void save_as() {
        if (m_main_stack->get_visible_child() != nullptr) {
            NBTPane *pane = static_cast<NBTPane*>(m_main_stack->get_visible_child());

            Gtk::FileChooserDialog file_dialog(*this, "Save NBT File...", Gtk::FileChooserAction::FILE_CHOOSER_ACTION_SAVE,
                    Gtk::DialogFlags::DIALOG_USE_HEADER_BAR);
            file_dialog.add_button("Save", Gtk::ResponseType::RESPONSE_OK);
            file_dialog.add_button("_Cancel", Gtk::ResponseType::RESPONSE_CANCEL);

            const int response = file_dialog.run();
            file_dialog.hide();
            if (response == Gtk::ResponseType::RESPONSE_OK) {
                std::string selected_uri = file_dialog.get_filename();

                try {
                    pane->save_as(selected_uri, file_dialog.get_file()->get_basename(), file_dialog.get_file()->get_parent()->get_parse_name());
                    update_topbar();
                } catch (std::exception &e) {
                    Gtk::MessageDialog error_dialog("Error saving NBT File", false, Gtk::MessageType::MESSAGE_ERROR, Gtk::ButtonsType::BUTTONS_OK, true);
                    error_dialog.set_transient_for(*this);
                    error_dialog.set_secondary_text("An error happened while saving " + file_dialog.get_file()->get_basename());
                    error_dialog.run();
                }
            }
        }
    }

    void save() {
        if (m_main_stack->get_visible_child() != nullptr) {
            NBTPane *pane = static_cast<NBTPane*>(m_main_stack->get_visible_child());

            if (pane->getPath() == "") {
                save_as();
                return;
            }

            try {
                pane->save();
            } catch (std::exception &e) {
                Gtk::MessageDialog error_dialog("Error saving NBT File", false, Gtk::MessageType::MESSAGE_ERROR, Gtk::ButtonsType::BUTTONS_OK, true);
                error_dialog.set_transient_for(*this);
                error_dialog.set_secondary_text("An error happened while saving " + pane->getFile());
                error_dialog.run();
            }
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
            m_app->remove_accelerator("app.save");
            m_app->remove_accelerator("app.saveas");
            m_app->remove_action("save");
            m_app->remove_action("saveas");
        } else if (!m_app->has_action("save") && m_tabs.size() > 0) {
            // About save action
            m_app->add_action("save", sigc::mem_fun(this, &NBTEditor::save));
            m_app->add_action("saveas", sigc::mem_fun(this, &NBTEditor::save_as));
            m_app->add_accelerator("<Primary>s", "app.save");
            m_app->add_accelerator("<Primary><shift>s", "app.saveas");
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
            m_header_title->set_label(m_tabs.front()->getFile());
            m_header_title->show();
            m_header_subtitle->set_label(m_tabs.front()->getDirectory());
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
            std::string selected_uri = file_dialog.get_filename();

            if (load_file(selected_uri, file_dialog.get_file()->get_basename(), file_dialog.get_file()->get_parent()->get_parse_name())) {

                update_topbar();

                m_main_stack->show_all();
            } else {
                Gtk::MessageDialog error_dialog("Error loading NBT File", false, Gtk::MessageType::MESSAGE_ERROR, Gtk::ButtonsType::BUTTONS_OK, true);
                error_dialog.set_transient_for(*this);
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

    std::list<NBTPane*> m_tabs;

    Glib::RefPtr<Gtk::Application> m_app;
};

#endif
