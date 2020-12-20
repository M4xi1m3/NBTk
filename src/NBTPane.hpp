#ifndef NBTPANE_HPP_
#define NBTPANE_HPP_

#include <gtkmm.h>

class NBTPane;

#include "NBTTreeView.hpp"
#include "NBTEditor.hpp"
#include "NBTModel.hpp"
#include "NBTLoad.hpp"
#include "nbtpp/nbt.hpp"

class NBTPane : public Gtk::ScrolledWindow {
public:
    NBTPane(nbtpp::nbt* tags, const std::string& path, const std::string& file, const std::string& directory, NBTEditor* editor) : m_path(path), m_file(file), m_directory(directory), m_editor(editor) {
        m_tree.setEditor(editor, this);
        m_tree.loadTag(tags);
        add(m_tree);
    }

    ~NBTPane() {

    }

    const std::string& getFile() const {
        return m_file;
    }

    const std::string& getDirectory() const {
        return m_directory;
    }

    const std::string& getPath() const {
        return m_path;
    }

    void save() {
        std::ofstream out(m_path);
        m_tree.save(out);
    }

    void save_as(const std::string& path, const std::string& file, const std::string& directory) {
        std::ofstream out(path);
        m_tree.save(out);

        m_path = path;
        m_file = file;
        m_directory = directory;

    }

    bool saved = true;

private:
    NBTTreeView m_tree;
    NBTEditor* m_editor;

    std::string m_file;
    std::string m_directory;
    std::string m_path;
};



#endif /* NBTPANE_HPP_ */
