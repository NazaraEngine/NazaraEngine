// Copyright (C) 2012 Maxime Griot
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/HttpClient.hpp>

#include <boost/bind.hpp>

using asio::ip::tcp;

/* Converts a hex character to its integer value */
char from_hex(char ch)
{
    return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

/* Converts an integer value to its hex character*/
char to_hex(char code)
{
    static char hex[] = "0123456789abcdef";
    return hex[code & 15];
}


std::string UrlEncode(const std::string& str)
{
    std::string ret;
    for(auto itor = str.begin(), end = str.end(); itor != end; ++itor)
    {
        if (isalnum(*itor) || *itor == '-' || *itor == '_' || *itor == '.' || *itor == '~')
            ret += std::string(1,*itor);
        else if (*itor == ' ')
            ret += std::string(1,'+');
        else
        {
            ret += std::string("%");
            ret += std::string(1,to_hex(*itor >> 4));
            ret += std::string(1,to_hex(*itor & 15));
        }
    }
    ret += std::string("\0");
    return ret;
}

namespace detail
{
class NAZARA_API NzHttpRequestState
{
public:
    NzHttpRequestState (std::unique_ptr <std::function<void(std::string)>> on_success, std::unique_ptr <std::function<void()>> on_complete, asio::io_service& pIoService)
        : OnSuccess(std::move(on_success)),
          OnComplete(std::move(on_complete)),
          mResolver(pIoService),
          mSocket(pIoService)
    {
    }

    tcp::resolver mResolver;
    tcp::socket mSocket;
    asio::streambuf mRequest;
    asio::streambuf mResponse;
    std::ostringstream mContent;

    std::unique_ptr <std::function<void(std::string)>> OnSuccess;
    std::unique_ptr <std::function<void()>> OnComplete;
};
};

NzHttpClient::NzHttpClient ()
{
}

void NzHttpClient::AsyncRequest(std::unique_ptr <std::function<void(std::string)>> on_success, std::unique_ptr <std::function<void()>> on_complete,
                              asio::io_service& io_service, const std::string& server, const std::string& path, const std::string& pPostData)
{
    std::shared_ptr <detail::NzHttpRequestState> state = std::make_shared <detail::NzHttpRequestState> (std::move(on_success), std::move(on_complete), io_service);

    std::ostream requestStream(&state->mRequest);
    requestStream << "POST " << path << " HTTP/1.1\r\n";
    requestStream << "Host: " << server << "\r\n";
    requestStream << "Accept: */*\r\n";
    requestStream << "Content-Length: " << pPostData.size() << "\r\n";
    requestStream << "Content-Type: application/x-www-form-urlencoded\r\n";
    requestStream << "Connection: close\r\n\r\n";
    requestStream << pPostData;

    tcp::resolver::query query(server, "http");
    state->mResolver.async_resolve(query,
                                   boost::bind(&NzHttpClient::HandleResolve, state,
                                           asio::placeholders::error,
                                           asio::placeholders::iterator));
}

void NzHttpClient::AsyncRequest(std::unique_ptr <std::function<void(std::string)>> on_success, asio::io_service& io_service,
                              const std::string& server, const std::string& path, const std::string& pPostData)
{
    std::unique_ptr <std::function <void()>> signal_on_complete(new std::function <void()> ());

    NzHttpClient::AsyncRequest(std::move(on_success), std::move(signal_on_complete), io_service, server, path, pPostData);
}

void NzHttpClient::AsyncRequest (std::function <void(std::string)> on_success, asio::io_service& io_service,
                               const std::string& server, const std::string& path, const std::string& pPostData)
{
    std::unique_ptr <std::function <void(std::string)>> signal_on_success(new std::function <void(std::string)> (on_success));
    std::unique_ptr <std::function <void()>> signal_on_complete(new std::function <void()> ());

    NzHttpClient::AsyncRequest(std::move(signal_on_success), std::move(signal_on_complete), io_service, server, path, pPostData);
}

void NzHttpClient::AsyncRequest (std::function <void(std::string)> on_success, std::function <void()> on_complete,
                               asio::io_service& io_service, const std::string& server, const std::string& path, const std::string& pPostData)
{
    std::unique_ptr <std::function <void(std::string)>> signal_on_success(new std::function <void(std::string)> (on_success));
    std::unique_ptr <std::function <void()>> signal_on_complete(new std::function <void()> (on_complete));

    NzHttpClient::AsyncRequest(std::move(signal_on_success), std::move(signal_on_complete), io_service, server, path, pPostData);
}

void NzHttpClient::AsyncRequest (asio::io_service& io_service, const std::string& server, const std::string& path, const std::string& pPostData)
{
    std::unique_ptr <std::function <void(std::string)>> signal_on_success(new std::function <void(std::string)> ());
    std::unique_ptr <std::function <void()>> signal_on_complete(new std::function <void()> ());

    NzHttpClient::AsyncRequest(std::move(signal_on_success), std::move(signal_on_complete), io_service, server, path, pPostData);
}

void NzHttpClient::HandleResolve(std::shared_ptr <detail::NzHttpRequestState> state, const asio::error_code& pError,
                               tcp::resolver::iterator pEndpointItor)
{
    if (!pError)
    {
        asio::async_connect(state->mSocket, pEndpointItor,
                            boost::bind(&NzHttpClient::HandleConnect, state,
                                        asio::placeholders::error));
    }
    else
    {
        //Framework::System::Log::Debug("NzHttpClient::HandleResolve - Error: " + pError.message());
        state->OnComplete->operator()();
    }
}

void NzHttpClient::HandleConnect(std::shared_ptr <detail::NzHttpRequestState> state, const asio::error_code& pError)
{
    if (!pError)
    {
        asio::async_write(state->mSocket, state->mRequest,
                          boost::bind(&NzHttpClient::HandleWriteRequest, state,
                                      asio::placeholders::error));
    }
    else
    {
        //Framework::System::Log::Debug("NzHttpClient::HandleConnect - Error: " + pError.message());
        state->OnComplete->operator()();
    }
}

void NzHttpClient::HandleWriteRequest(std::shared_ptr <detail::NzHttpRequestState> state, const asio::error_code& pError)
{
    if (!pError)
    {
        asio::async_read_until(state->mSocket, state->mResponse, "\r\n",
                               boost::bind(&NzHttpClient::HandleStatusLine, state,
                                           asio::placeholders::error));
    }
    else
    {
        //Framework::System::Log::Debug("NzHttpClient::HandleWriteRequest - Error: " + pError.message());
        state->OnComplete->operator()();
    }
}

void NzHttpClient::HandleStatusLine(std::shared_ptr <detail::NzHttpRequestState> state, const asio::error_code& pError)
{
    if (!pError)
    {
        std::istream responseStream(&state->mResponse);
        std::string httpVersion;
        responseStream >> httpVersion;
        unsigned int statusCode;
        responseStream >> statusCode;
        std::string statusMessage;
        std::getline(responseStream, statusMessage);
        if (!responseStream || httpVersion.substr(0, 5) != "HTTP/")
        {
            //Framework::System::Log::Debug("NzHttpClient::HandleStatusLine - Invalid response\n");
            state->OnComplete->operator()();
            return;
        }
        if (statusCode != 200)
        {
            //Framework::System::Log::Debug(str(boost::format("NzHttpClient::HandleStatusLine - Response returned with status code %i\n") % statusCode));
            state->OnComplete->operator()();
            return;
        }

        asio::async_read_until(state->mSocket, state->mResponse, "\r\n\r\n",
                               boost::bind(&NzHttpClient::HandleHeaders, state,
                                           asio::placeholders::error));
    }
    else
    {
        //Framework::System::Log::Debug("NzHttpClient::HandleStatusLine - Error: " + pError.message());
        state->OnComplete->operator()();
    }
}

void NzHttpClient::HandleHeaders(std::shared_ptr <detail::NzHttpRequestState> state, const asio::error_code& pError)
{
    if (!pError)
    {
        std::istream responseStream(&state->mResponse);
        std::string header;
        while (std::getline(responseStream, header) && header != "\r");

        if (state->mResponse.size() > 0)
            state->mContent << &state->mResponse;

        asio::async_read(state->mSocket, state->mResponse,
                         asio::transfer_at_least(1),
                         boost::bind(&NzHttpClient::HandleContent, state,
                                     asio::placeholders::error));
    }
    else
    {
       // Framework::System::Log::Debug("NzHttpClient::HandleHeaders - Error: " + pError.message());
        state->OnComplete->operator()();
    }
}

void NzHttpClient::HandleContent(std::shared_ptr <detail::NzHttpRequestState> state, const asio::error_code& pError)
{
    if (!pError)
    {
        state->mContent << &state->mResponse;

        asio::async_read(state->mSocket, state->mResponse,
                         asio::transfer_at_least(1),
                         boost::bind(&NzHttpClient::HandleContent, state,
                                     asio::placeholders::error));
    }
    else if (pError != asio::error::eof)
    {
        //Framework::System::Log::Debug("NzHttpClient::HandleContent - Error: " + pError.message());
        state->OnComplete->operator()();
    }
    else
    {
        state->OnSuccess->operator()(state->mContent.str());
        state->OnComplete->operator()();
    }
}
