#ifndef NBTTREEVIEW_HPP_
#define NBTTREEVIEW_HPP_

#include <gtkmm.h>

class NBTEditor;
class NBTPane;

#include "NBTModel.hpp"
#include "NBTLoad.hpp"

#include "nbtpp/nbt.hpp"
#include "nbtpp/tag.hpp"

class NBTTreeView: public Gtk::TreeView {
public:
    NBTTreeView() : Gtk::TreeView(), m_saved(true) {

        // Create tree model
        m_tree_model = Gtk::TreeStore::create(m_model_columns);
        set_model(m_tree_model);

        m_px_col.pack_start(m_px_buf, false);
        m_px_col.pack_start(m_str_col, false);

        m_px_col.set_cell_data_func(m_px_buf, [this](Gtk::CellRenderer* renderer, const Gtk::TreeModel::iterator& iter) {
            if (iter) {
                Gtk::TreeModel::Row row = *iter;
                Glib::RefPtr<Gdk::Pixbuf> icon = row[m_model_columns.m_col_icon];
                m_px_buf.property_pixbuf() = icon;
            }
        });

        m_px_col.set_cell_data_func(m_str_col, [this](Gtk::CellRenderer* renderer, const Gtk::TreeModel::iterator& iter) {
            if (iter) {
                Gtk::TreeModel::Row row = *iter;
                Glib::ustring name = row[m_model_columns.m_col_name];
                Glib::ustring value = row[m_model_columns.m_col_value];
                m_str_col.property_text() = name + ": " + value;
            }
        });

        append_column(m_px_col);

        property_headers_visible() = false;

        // Construct menu
        auto item = Gtk::make_managed<Gtk::MenuItem>("_Remove", true);
        item->signal_activate().connect(sigc::mem_fun(*this, &NBTTreeView::on_menu_remove));
        m_menu_popup.append(*item);

        m_menu_popup.accelerate(*this);
        m_menu_popup.show_all();
    }

    virtual ~NBTTreeView() {
        delete m_tag;
    }

    void loadTag(nbtpp::nbt* tag) {
        m_tag = tag;
        m_tag->debug(std::cout);

        nbt_load(m_tag->content(), m_tree_model, m_model_columns, nullptr);

        m_saved = true;
    }

    const nbtpp::nbt* getTag() const {
        return m_tag;
    }

    void setEditor(NBTEditor* editor, NBTPane* pane) {
        m_editor = editor;
        m_pane = pane;
    }

    void save(std::ofstream& out);

protected:
    // Override Signal handler:
    // Alternatively, use signal_button_press_event().connect_notify()
    bool on_button_press_event(GdkEventButton* button_event) {
        bool return_value = false;

        //Call base class, to allow normal handling,
        //such as allowing the row to be selected by the right-click:
        return_value = TreeView::on_button_press_event(button_event);

        //Then do our custom stuff:
        if ((button_event->type == GDK_BUTTON_PRESS) && (button_event->button == 3)) {
            m_menu_popup.popup_at_pointer((GdkEvent*) button_event);

            // Menu::popup_at_pointer() is new in gtkmm 3.22.
            // If you have an older revision, try this:
            //m_Menu_Popup.popup(button_event->button, button_event->time);
        }

        return return_value;
    }

    // Signal handler for popup menu items:
    void on_menu_remove() {
        std::cout << "A popup menu item was selected." << std::endl;

        auto refSelection = get_selection();
        if (refSelection) {

            Gtk::TreeModel::iterator iter = refSelection->get_selected();
            if (iter) {
                nbtpp::tag* child = (*iter)[m_model_columns.m_col_tag];

                if ((*iter)->parent()) {
                    nbtpp::tag* t = (*(*iter)->parent())[m_model_columns.m_col_tag];

                    switch(t->type()) {
                        case nbtpp::TAG_Compound: {
                            nbtpp::tags::tag_compound* tag = dynamic_cast<nbtpp::tags::tag_compound*>(t);
                            tag->remove(child);
                            break;
                        }
                        case nbtpp::TAG_List: {
                            nbtpp::tags::tag_list* tag = dynamic_cast<nbtpp::tags::tag_list*>(t);
                            tag->remove(child);
                            break;
                        }
                        default:
                            // Something went horribly wrong right there...
                            break;
                    }
                } else {
                    m_tag->content(nullptr);
                }

                m_tree_model->erase(iter);
                mark_unsaved();
            }
        }
    }

    void mark_unsaved();

    bool m_saved;

    NBTEditor* m_editor;
    NBTPane* m_pane;

    // Tree model columns
    NBTModel m_model_columns;

    // The Tree model:
    Glib::RefPtr<Gtk::TreeStore> m_tree_model;

    Gtk::Menu m_menu_popup;

    nbtpp::nbt* m_tag;

    Gtk::CellRendererText m_str_col;
    Gtk::CellRendererPixbuf m_px_buf;
    Gtk::TreeViewColumn m_px_col;
};

#endif /* NBTTREEVIEW_HPP_ */
