// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Copyright (c) 2017 Kolya Kosenko

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// GUI implementation of Boost.ASIO chat client example.
// Start chat server from Boost.ASIO examples.

#define BOOST_ASIO_NO_DEPRECATED

#include <cstdlib>
#include <deque>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include "libs/asio/example/cpp03/chat/chat_message.hpp"

#include <boost/ui.hpp>
namespace ui = boost::ui;

class chat_client
{
    typedef chat_client this_type;

public:
    chat_client(boost::function<void(const std::string&)> output_fn)
        : m_socket(m_io_context), m_output_fn(output_fn)
    {
    }

    ~chat_client()
    {
        close();
    }

    void connect(const std::string& host, const std::string& service)
    {
        boost::asio::ip::tcp::resolver resolver(m_io_context);
        boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, service);

        boost::asio::async_connect(m_socket, endpoints,
            boost::bind(&this_type::handle_connect, this,
                boost::asio::placeholders::error));

        m_thread = boost::thread(&this_type::run, this);
    }

    void write(const std::string& str)
    {
        const std::string actual_str = str.substr(0, chat_message::max_body_length);
        chat_message msg;
        msg.body_length(actual_str.size());
        std::memcpy(msg.body(), actual_str.c_str(), msg.body_length());
        msg.encode_header();
        boost::asio::post(m_io_context, boost::bind(&this_type::do_write, this, msg));
    }

private:
    void run()
    {
        try
        {
            m_io_context.run();
        }
        catch ( std::exception& e )
        {
            ui::log::error() << e.what();
        }
    }

    void close()
    {
        boost::asio::post(m_io_context, boost::bind(&this_type::do_close, this));
        m_thread.join();
    }

    void handle_connect(const boost::system::error_code& error)
    {
        if ( !error )
        {
            boost::asio::async_read(m_socket,
                boost::asio::buffer(m_read_msg.data(), chat_message::header_length),
                boost::bind(&this_type::handle_read_header, this,
                    boost::asio::placeholders::error));
        }
        else on_error(error);
    }

    void handle_read_header(const boost::system::error_code& error)
    {
        if ( !error && m_read_msg.decode_header() )
        {
            boost::asio::async_read(m_socket,
                boost::asio::buffer(m_read_msg.body(), m_read_msg.body_length()),
                boost::bind(&this_type::handle_read_body, this,
                    boost::asio::placeholders::error));
        }
        else do_close(error);
    }

    void handle_read_body(const boost::system::error_code& error)
    {
        if ( !error )
        {
            const std::string str(m_read_msg.body(),
                                  m_read_msg.body() + m_read_msg.body_length());
            m_output_fn(str);

            boost::asio::async_read(m_socket,
                boost::asio::buffer(m_read_msg.data(), chat_message::header_length),
                boost::bind(&this_type::handle_read_header, this,
                    boost::asio::placeholders::error));
        }
        else do_close(error);
    }

    void do_write(chat_message msg)
    {
        const bool write_in_progress = !m_write_msgs.empty();
        m_write_msgs.push_back(msg);

        if ( !write_in_progress )
        {
            boost::asio::async_write(m_socket,
                boost::asio::buffer(m_write_msgs.front().data(),
                    m_write_msgs.front().length()),
                boost::bind(&this_type::handle_write, this,
                    boost::asio::placeholders::error));
        }
    }

    void handle_write(const boost::system::error_code& error)
    {
        if ( !error )
        {
            m_write_msgs.pop_front();

            if ( !m_write_msgs.empty() )
            {
                boost::asio::async_write(m_socket,
                    boost::asio::buffer(m_write_msgs.front().data(),
                    m_write_msgs.front().length()),
                    boost::bind(&this_type::handle_write, this,
                        boost::asio::placeholders::error));
            }
        }
        else do_close(error);
    }

    void do_close()
    {
        m_socket.close();
    }

    void do_close(const boost::system::error_code& error)
    {
        do_close();
        on_error(error);
    }

    void on_error(const boost::system::error_code& error)
    {
        std::ostringstream ss;
        ss << error.message();
        ss << "\n\nError code: " << error.value();
        ss << ", category: " << error.category().name();
        ui::log::error() << ss.str();
    }

private:
    boost::asio::io_context m_io_context;
    boost::asio::ip::tcp::socket m_socket;

    chat_message m_read_msg;
    std::deque<chat_message> m_write_msgs;
    boost::thread m_thread;
    boost::function<void(const std::string&)> m_output_fn;
};

class asio_dialog : public ui::dialog
{
    typedef asio_dialog this_type;

public:
    asio_dialog();

private:
    void on_connect();
    void on_send();
    void on_send_date();
    void on_clear_output();
    void on_output(const std::string& str);
    void on_output_thread_safe(const std::string& str);

    chat_client m_client;

    ui::string_box m_host;
    ui::string_box m_service;
    ui::string_box m_message;
    ui::list_box m_messages;
};

asio_dialog::asio_dialog() : ui::dialog(
    "Boost.ASIO Chat Client + Boost.UI example"),
    m_client(boost::bind(&this_type::on_output, this, _1))
{
    ui::vbox(*this)
        << ( ui::hbox()
            << m_host.create(*this, "localhost")
                .tooltip("Host name")
                .layout().stretch()
            << m_service.create(*this, "3000")
                .tooltip("Port number")
            << ui::button(*this, "&Connect")
                .on_press(&this_type::on_connect, this)
                .tooltip("Connects to the server")
           ).layout().justify()
        << ( ui::hbox()
            << m_message.create(*this, "Test message")
                .tooltip("Message text")
                .layout().stretch()
            << ui::button(*this, "&Send")
                .on_press(&this_type::on_send, this)
                .tooltip("Sends message to the server")
           ).layout().justify()
        << ui::button(*this, "Send current &date")
            .on_press(&this_type::on_send_date, this)
            .tooltip("Sends current date and time to the server")
            .layout().justify()
        << m_messages.create(*this)
            .tooltip("Server output")
            .layout().justify().stretch()
        << ui::button(*this, "C&lear output")
            .on_press(&this_type::on_clear_output, this)
            .tooltip("Clears output text")
        ;

    resize(500, 400);
}

void asio_dialog::on_connect()
{
    m_client.connect(m_host.text().u8string(), m_service.text().u8string());
}

void asio_dialog::on_send()
{
    m_client.write(m_message.text().u8string());
    m_message.clear();
}

void asio_dialog::on_send_date()
{
    time_t rawtime;
    std::time(&rawtime);
    char buf[100];
    std::strftime(buf, sizeof buf / sizeof buf[0], "%A %c", std::localtime(&rawtime));
    m_client.write(buf);
}

void asio_dialog::on_clear_output()
{
    m_messages.clear();
}

void asio_dialog::on_output(const std::string& str)
{
    ui::call_async(boost::bind(&this_type::on_output_thread_safe, this, str));
}

void asio_dialog::on_output_thread_safe(const std::string& str)
{
    m_messages.push_back(ui::utf8(str));
}

int ui_main(int argc, char* argv[])
{
    asio_dialog().show_modal();

    return 0;
}

int main(int argc, char* argv[])
{
    return ui::entry(&ui_main, argc, argv);
}
