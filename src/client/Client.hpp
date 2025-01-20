#pragma once 
#include <iostream>
#include <boost/asio.hpp>
class Client {
public:
    Client(boost::asio::io_context& io_context, const std::string& host, const std::string& port)
        : resolver_(io_context), socket_(io_context) {
        do_connect(host, port);
    }

private:
    void do_connect(const std::string& host, const std::string& port) {
        auto endpoints = resolver_.resolve(host, port);
        boost::asio::async_connect(socket_, endpoints,
            [this](boost::system::error_code ec, tcp::endpoint) {
                if (!ec) {
                    do_write();
                }
            });
    }

    void do_write() {
        std::string msg = "Hello from client";
        boost::asio::async_write(socket_, boost::asio::buffer(msg),
            [this](boost::system::error_code ec, std::size_t /*length*/) {
                if (!ec) {
                    do_read();
                }
            });
    }

    void do_read() {
        boost::asio::async_read(socket_, boost::asio::buffer(reply_, max_length),
            [this](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    std::cout << "Reply from server: ";
                    std::cout.write(reply_, length);
                    std::cout << "\n";
                }
            });
    }

    tcp::resolver resolver_;
    tcp::socket socket_;
    enum { max_length = 1024 };
    char reply_[max_length];
};