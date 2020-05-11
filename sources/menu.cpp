// Copyright (c) 2017, 2018 Kolya Kosenko

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

#define BOOST_UI_SOURCE

#include <boost/ui/native/config.hpp>

#include <boost/ui/menu.hpp>
#include <boost/ui/native/string.hpp>
#include <boost/ui/native/widget.hpp>
#include <boost/ui/detail/memcheck.hpp>

#include <wx/menu.h>

namespace boost {
namespace ui    {

#if wxUSE_MENUS

class menu_bar::native_impl : public wxMenuBar, private detail::memcheck
{
public:
    void append(const menu& i);
};

class menu::native_impl : public wxMenu, private detail::memcheck
{
    typedef native_impl this_type;

public:
    native_impl()
    {
        init();
    }

    native_impl(const uistring& text)
        : m_text(text)
    {
        init();
    }

    void append(const menu& i)
    {
        AppendSubMenu(i.m_impl, native::from_uistring(i.m_impl->m_text));
        i.m_shared_count.detach();
    }

    uistring text() { return m_text; }

private:
    void init()
    {
        Bind(wxEVT_MENU, &this_type::on_menu, this);
    }

    void on_menu(wxCommandEvent& event);

    uistring m_text;
};

class menu::item::native_impl : public wxMenuItem
{
public:
    native_impl(const uistring& text)
        : wxMenuItem(NULL, wxID_ANY, native::from_uistring(text)) {}

    void on_menu();

    typedef std::vector< boost::function<void()> > menu_handlers_type;
    menu_handlers_type m_menu_handlers;
};

void menu::native_impl::on_menu(wxCommandEvent& event)
{
    wxMenuItem* rawitem = FindItem(event.GetId());
    wxCHECK_RET(rawitem, "Unable to find menu item by id");

    item::native_impl* item = dynamic_cast<item::native_impl*>(rawitem);
    wxCHECK_RET(item, "Menu item has invalid type");

    item->on_menu();
}

void menu::item::native_impl::on_menu()
{
    for ( menu_handlers_type::const_reverse_iterator iter = m_menu_handlers.rbegin();
            iter != m_menu_handlers.rend(); ++iter )
    {
        (*iter)();
    }
}

void menu_bar::native_impl::append(const menu& i)
{
    Append(i.m_impl, native::from_uistring( i.m_impl->text() ));
    i.m_shared_count.detach();
}

#endif // wxUSE_MENUS

menu_bar::menu_bar() : m_impl(NULL)
{
}

menu_bar::menu_bar(const menu_bar& other)
{
    m_impl = other.m_impl;
}

void menu_bar::create()
{
#if wxUSE_MENUS
    m_impl = new native_impl;
#else
    m_impl = NULL;
#endif
}

menu_bar::~menu_bar()
{
    // It should be deleted by frame
    //delete m_impl;
}

menu_bar& menu_bar::append(const menu& i)
{
    wxCHECK_MSG(m_impl, *this, "Widget should be created");

#if wxUSE_MENUS
    m_impl->append(i);
#endif

    return *this;
}

menu::menu()
{
#if wxUSE_MENUS
    m_impl = new native_impl;
#else
    m_impl = NULL;
#endif
}

menu::menu(const uistring& text)
{
#if wxUSE_MENUS
    m_impl = new native_impl(text);
#else
    m_impl = NULL;
#endif
}

menu::~menu()
{
#if wxUSE_MENUS
    if ( m_shared_count.may_delete() )
    {
        delete m_impl;
    }
#endif
}

menu& menu::append(const item& i)
{
#if wxUSE_MENUS
    m_impl->Append(i.m_impl);
#endif
    return *this;
}

menu& menu::append(const menu& i)
{
#if wxUSE_MENUS
    m_impl->append(i);
#endif
    return *this;
}

menu& menu::append(const separator&)
{
#if wxUSE_MENUS
    m_impl->AppendSeparator();
#endif
    return *this;
}

void menu::popup(widget& w)
{
    wxWindow *window = native::from_widget(w);
    wxCHECK_RET(window, "Widget should be valid");

#if wxUSE_MENUS
    window->PopupMenu(m_impl);
#endif
}

menu::item::item(const uistring& text)
{
#if wxUSE_MENUS
    m_impl = new native_impl(text);
#else
    m_impl = NULL;
#endif
}

void menu::item::on_press_raw(const boost::function<void()>& handler)
{
    wxCHECK_RET(m_impl, "Widget should be created");

    m_impl->m_menu_handlers.push_back(handler);
}

} // namespace ui
} // namespace boost
