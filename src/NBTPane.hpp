#ifndef NBTPANE_HPP_
#define NBTPANE_HPP_

#include <gtkmm.h>
#include "NBTModel.hpp"
#include "NBTLoad.hpp"
#include "NBTTreeView.hpp"
#include "nbtpp/nbt.hpp"

class NBTPane {
public:
    NBTPane(nbtpp::nbt* tags, const std::string& file, const std::string& path) : m_file(file), m_path(path) {
        m_tree.loadTag(tags);

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
    NBTTreeView m_tree;
    Gtk::ScrolledWindow m_scroll;

    std::string m_file;
    std::string m_path;
};



#endif /* NBTPANE_HPP_ */
