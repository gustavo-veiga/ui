// Copyright (c) 2017 Kolya Kosenko

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

#define BOOST_UI_SOURCE

#include <boost/ui/listbox.hpp>
#include <boost/ui/native/event.hpp>
#include <boost/ui/native/string.hpp>
#include <boost/ui/native/widget.hpp>
#include <boost/ui/detail/memcheck.hpp>

#include <wx/listbox.h>

namespace boost {
namespace ui    {

#if wxUSE_LISTBOX

class listbox::detail_impl : public detail::widget_detail<wxListBox>
{
public:
    explicit detail_impl(widget& parent)
    {
        set_native_handle(new wxListBox(native::from_widget(parent), wxID_ANY));
    }

    explicit detail_impl(widget& parent, const std::vector<uistring>& options)
    {

        set_native_handle(new wxListBox(native::from_widget(parent),
            wxID_ANY, wxDefaultPosition, wxDefaultSize,
            native::from_vector_uistring(options)));
    }
    size_type selected_index() const
    {
        wxCHECK(m_native, npos);
        return m_native->GetSelection();
    }
    uistring selected_string() const
    {
        wxCHECK(m_native, uistring());
        return native::to_uistring(m_native->GetStringSelection());
    }
};

#endif

listbox::detail_impl* listbox::get_impl()
{
#if wxUSE_LISTBOX
    return get_detail_impl<detail_impl>();
#else
    return NULL;
#endif
}

const listbox::detail_impl* listbox::get_impl() const
{
#if wxUSE_LISTBOX
    return get_detail_impl<detail_impl>();
#else
    return NULL;
#endif
}

listbox& listbox::create(widget& parent)
{
#if wxUSE_LISTBOX
    detail_set_detail_impl(new detail_impl(parent));
#endif

    return *this;
}

listbox& listbox::create(widget& parent, const std::vector<uistring>& options)
{
#if wxUSE_LISTBOX
    detail_set_detail_impl(new detail_impl(parent, options));
#endif

    return *this;
}

listbox::size_type listbox::selected_index() const
{
#if wxUSE_LISTBOX
    const detail_impl* impl = get_impl();
    wxCHECK(impl, npos);

    return impl->selected_index();
#else
    return npos;
#endif
}

uistring listbox::selected_string() const
{
#if wxUSE_LISTBOX
    const detail_impl* impl = get_impl();
    wxCHECK(impl, uistring());
    wxASSERT(has_selection());

    return impl->selected_string();
#else
    return uistring();
#endif
}

listbox& listbox::on_select(const boost::function<void()>& handler)
{
#if wxUSE_LISTBOX
    native::bind_helper(*this, wxEVT_LISTBOX, handler);
#endif

    return *this;
}

listbox& listbox::on_select_event(const boost::function<void(index_event&)>& handler)
{
#if wxUSE_LISTBOX
    native::bind_event_helper(*this, wxEVT_LISTBOX, handler);
#endif

    return *this;
}

listbox& listbox::on_activate(const boost::function<void()>& handler)
{
#if wxUSE_LISTBOX
    native::bind_helper(*this, wxEVT_LISTBOX_DCLICK, handler);
#endif

    return *this;
}

listbox& listbox::on_activate_event(const boost::function<void(index_event&)>& handler)
{
#if wxUSE_LISTBOX
    native::bind_event_helper(*this, wxEVT_LISTBOX_DCLICK, handler);
#endif

    return *this;
}

} // namespace ui
} // namespace boost
