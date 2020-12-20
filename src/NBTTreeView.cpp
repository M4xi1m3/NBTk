
#include "NBTTreeView.hpp"
#include "NBTEditor.hpp"

void NBTTreeView::mark_unsaved() {
    m_pane->saved = false;
    m_editor->mark_saved(m_pane);
}

void NBTTreeView::save(std::ofstream& out) {
    m_tag->save(out);

    m_pane->saved = true;
    m_editor->mark_saved(m_pane);
}
