#ifndef NBTLOAD_HPP_
#define NBTLOAD_HPP_

#include "NBTModel.hpp"
#include "nbtpp/tag.hpp"

void nbt_load(const nbtpp::tag* data, Glib::RefPtr<Gtk::TreeStore>& tree_model, NBTModel& model, const Gtk::TreeNodeChildren* tree);

#endif /* NBTLOAD_HPP_ */
