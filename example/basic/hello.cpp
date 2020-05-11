// Copyright (c) 2017 Kolya Kosenko

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// "Hello, World" example
// This example creates dialog with one button. When user press button
// informational dialog is shown.
// Code is written in C++03 style.

#include <boost/ui.hpp>

namespace ui = boost::ui;

void handle_button_press()
{
    ui::info_dialog("Hello, C++ World!");
}

int ui_main()
{
    ui::dialog dlg("Hello, Boost.UI!");

    ui::button(dlg, "Say &hello")
        .on_press(&handle_button_press);

    dlg.show_modal();

    return 0;
}

int main(int argc, char* argv[])
{
    return ui::entry(&ui_main, argc, argv);
}
