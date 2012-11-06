// Copyright (C) 2012 Maxime Griot
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef HTTP_CLIENT_HPP
#define HTTP_CLIENT_HPP

#include <Nazara/Prerequesites.hpp>

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <asio.hpp>

using asio::ip::tcp;

namespace detail
{
    class NzHttpRequestState;
};

char from_hex(char ch);
char to_hex(char code);
std::string UrlEncode(const std::string& str);

class NAZARA_API NzHttpClient
{
    public:
        static void AsyncRequest (std::unique_ptr <std::function<void(std::string)>> on_success, std::unique_ptr <std::function<void()>> on_complete,
            asio::io_service& io_service, const std::string& server, const std::string& path, const std::string& pPostData = "");

        static void AsyncRequest (std::unique_ptr <std::function<void(std::string)>> on_success, asio::io_service& io_service,
            const std::string& server, const std::string& path, const std::string& pPostData = "");

        static void AsyncRequest (std::function <void(std::string)> on_success, std::function <void()> on_complete,
            asio::io_service& io_service, const std::string& server, const std::string& path, const std::string& pPostData = "");

        static void AsyncRequest (std::function <void(std::string)> on_success, asio::io_service& io_service,
            const std::string& server, const std::string& path, const std::string& pPostData = "");

        static void AsyncRequest (asio::io_service& io_service, const std::string& server, const std::string& path, const std::string& pPostData = "");

    private:
        NzHttpClient();

        static void HandleResolve(std::shared_ptr <detail::NzHttpRequestState> state, const asio::error_code& err, tcp::resolver::iterator endpoint_iterator);
        static void HandleConnect(std::shared_ptr <detail::NzHttpRequestState> state, const asio::error_code& err);

        static void HandleWriteRequest(std::shared_ptr <detail::NzHttpRequestState> state, const asio::error_code& err);
        static void HandleStatusLine(std::shared_ptr <detail::NzHttpRequestState> state, const asio::error_code& err);
        static void HandleHeaders(std::shared_ptr <detail::NzHttpRequestState> state, const asio::error_code& err);
        static void HandleContent(std::shared_ptr <detail::NzHttpRequestState> state, const asio::error_code& err);
};

#endif // HTTP_CLIENT_HPP
