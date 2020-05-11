// Copyright (c) 2017 Kolya Kosenko

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

#define BOOST_UI_SOURCE

#include <boost/ui/native/config.hpp>

#include <boost/ui/frame.hpp>
#include <boost/ui/event_loop.hpp>
#include <boost/ui/native/event.hpp>
#include <boost/ui/native/string.hpp>
#include <boost/ui/native/widget.hpp>
#include <boost/ui/detail/memcheck.hpp>

#include <wx/frame.h>
#include <wx/evtloop.h>
#include <wx/menu.h>

#include "res/boost.xpm"

namespace boost {
namespace ui    {

class frame::detail_impl : public detail::window_detail<wxFrame>
{
public:
    explicit detail_impl(const uistring& title)
    {
        wxFrame* w = new wxFrame(NULL, wxID_ANY, native::from_uistring(title),
               wxDefaultPosition, wxDefaultSize,
               wxDEFAULT_FRAME_STYLE
              );
        w->SetIcon(boost_xpm);
        set_native_handle(w);

        w->Bind(wxEVT_CLOSE_WINDOW, &detail_impl::on_close, this);
    }
    virtual ~detail_impl()
    {
        delete_native();
    }
    void show_modal()
    {
        wxCHECK_RET(m_native, "Widget should be created");
        m_native->Show();

        m_modal_loop.run();
    }
#if wxUSE_MENUS
    void set_menu_bar(wxMenuBar* wxmb)
    {
        wxCHECK_RET(m_native, "Widget should be created");
        m_native->SetMenuBar(wxmb);
    }
#endif
#if wxUSE_STATUSBAR
    void set_status_bar(wxStatusBar* wxsb)
    {
        wxCHECK_RET(m_native, "Widget should be created");
        m_native->SetStatusBar(wxsb);
    }
#endif

private:
    void on_close(wxCloseEvent& e)
    {
        e.Skip();

        m_modal_loop.exit();
    }

    event_loop m_modal_loop;
};

frame::detail_impl* frame::get_impl()
{
    return get_detail_impl<detail_impl>();
}


frame& frame::create(const uistring& title)
{
    detail_set_detail_impl(new detail_impl(title));

    return *this;
}

ui::menu_bar frame::menu_bar()
{
    ui::menu_bar mb;
    mb.create();

    menu_bar::native_handle_type void_mb = mb.native_handle();
    wxCHECK_MSG(void_mb, ui::menu_bar(), "Invalid menu bar handle");

#if wxUSE_MENUS
    wxObject* object_mb = static_cast<wxObject*>(void_mb);
    wxMenuBar* wxmb = dynamic_cast<wxMenuBar*>(object_mb);
    wxCHECK_MSG(wxmb, ui::menu_bar(), "Invalid menu bar type");

    detail_impl* impl = get_impl();
    wxCHECK_MSG(impl, mb, "Widget should be created");

    impl->set_menu_bar(wxmb);
#endif

    return mb;
}

ui::status_bar frame::status_bar()
{
    ui::status_bar sb;
    sb.create(*this);

    status_bar::native_handle_type void_sb = sb.native_handle();
    wxCHECK_MSG(void_sb, ui::status_bar(), "Invalid status bar handle");

#if wxUSE_STATUSBAR
    wxObject* object_sb = static_cast<wxObject*>(void_sb);
    wxStatusBar* wxsb = dynamic_cast<wxStatusBar*>(object_sb);
    wxCHECK_MSG(wxsb, ui::status_bar(), "Invalid status bar type");

    detail_impl* impl = get_impl();
    wxCHECK_MSG(impl, sb, "Widget should be created");

    impl->set_status_bar(wxsb);
#endif

    return sb;
}

} // namespace ui
} // namespace boost
