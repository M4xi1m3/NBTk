#include <gtkmm.h>

#include "NBTModel.hpp"
#include "nbtpp/nbt.hpp"
#include "nbtpp/tag.hpp"
#include "NBTEditor.hpp"

#include <iostream>
#include <fstream>

using namespace nbtpp;

int main(int argc, char* argv[]) {
    auto app = Gtk::Application::create("io.github.m4xi1m3.nbtk");
    app->register_application();

    NBTEditor editor(app);
    app->run(editor);

    return 0;
}
