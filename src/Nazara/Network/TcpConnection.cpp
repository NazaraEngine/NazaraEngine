// Copyright (C) 2012 Maxime Griot
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/TcpConnection.hpp>
#include <boost/bind.hpp>
#include <fstream>

//---------------------------------------------------------------------
NzTcpConnection::NzTcpConnection(asio::io_service& pIoService)
    : mSocket(pIoService), mTimeout(pIoService), mResolver(pIoService), mStrategy(nullptr)
{
}
//---------------------------------------------------------------------
NzTcpConnection::~NzTcpConnection()
{
}
//---------------------------------------------------------------------
void NzTcpConnection::Close()
{
    if(mSocket.is_open())
    {
        asio::error_code e;
        mSocket.shutdown(asio::socket_base::shutdown_both, e);
        mSocket.close(e);
    }
}
//---------------------------------------------------------------------
void NzTcpConnection::OnTimeout(const asio::error_code& e)
{
    if(e != asio::error::operation_aborted)
    {
        Close();
    }
}
//---------------------------------------------------------------------
void NzTcpConnection::Start()
{
    mTimeout.expires_from_now(boost::posix_time::minutes(5));
    mTimeout.async_wait(boost::bind(&NzTcpConnection::OnTimeout, shared_from_this(), _1));
    async_read();
}
//---------------------------------------------------------------------
void NzTcpConnection::Write(const std::string& pData)
{
    mSocket.get_io_service().post(boost::bind(&NzTcpConnection::DoWrite, shared_from_this(), pData));
}
//---------------------------------------------------------------------
void NzTcpConnection::DoWrite(std::string pData)
{
    bool write_in_progress = !mToSend.empty();
    mToSend.push_back(pData);
    if (!write_in_progress)
    {
        async_write(mToSend.front());
    }
}
//---------------------------------------------------------------------
void NzTcpConnection::HandleRead(const asio::error_code& pError)
{
    if(!pError)
    {
        async_read();
    }
    else
    {
        Close();
    }
}
//---------------------------------------------------------------------
void NzTcpConnection::HandleWrite(const asio::error_code& pError)
{
    if(!pError)
    {
        mTimeout.expires_from_now(boost::posix_time::minutes(5));
        mTimeout.async_wait(boost::bind(&NzTcpConnection::OnTimeout, shared_from_this(), _1));

        mToSend.pop_front();
        if (!mToSend.empty())
        {
            async_write(mToSend.front());
        }
    }
    else
    {
        //System::Log::Debug("handle write failed.");
        Close();
    }
}
//---------------------------------------------------------------------
asio::ip::tcp::socket& NzTcpConnection::GetSocket()
{
    return mSocket;
}
//---------------------------------------------------------------------
void NzTcpConnection::async_write(const std::string& pData)
{
    mOutboundData = pData;

    asio::async_write(mSocket, asio::buffer(mOutboundData), boost::bind(&NzTcpConnection::HandleWrite,shared_from_this(), asio::placeholders::error));
}
//---------------------------------------------------------------------
void NzTcpConnection::async_read()
{
    // Read the header
    mInboundHeader.resize(mStrategy->GetHeaderSize());
    asio::async_read(mSocket, asio::buffer(mInboundHeader), asio::transfer_exactly(mStrategy->GetHeaderSize()),
                     boost::bind(&NzTcpConnection::handle_read_header,shared_from_this(), asio::placeholders::error, asio::placeholders::bytes_transferred));
}
//---------------------------------------------------------------------
void NzTcpConnection::handle_read_header(const asio::error_code& e, size_t transfered)
{
    if (e)
    {
        this->HandleRead(e);
    }
    else
    {
        mTimeout.expires_from_now(boost::posix_time::minutes(5));
        mTimeout.async_wait(boost::bind(&NzTcpConnection::OnTimeout, shared_from_this(), _1));

        std::string data(mInboundHeader.data(), mInboundHeader.size());

        uint32_t size = mStrategy->HandleHeader(data);

        if(size > 1<<16)
        {
            //System::Log::Debug("handle read header failed.");
            asio::error_code error(asio::error::invalid_argument);
            this->HandleRead(error);
            return;
        }
        else if(size == 0)
        {
            async_read();
            return;
        }

        // Ready up the buffer
        mInboundData.resize(size);

        // Read the buffer
        asio::async_read(mSocket, asio::buffer(mInboundData), asio::transfer_exactly(mInboundData.size()),
                         boost::bind(&NzTcpConnection::handle_read_data, shared_from_this(),
                                     asio::placeholders::error, asio::placeholders::bytes_transferred));
    }
}
//---------------------------------------------------------------------
void NzTcpConnection::handle_read_data(const asio::error_code& e, size_t pBytes)
{
    // If error tell the caller.
    if (e)
    {
        this->HandleRead(e);
    }
    else
    {
        mTimeout.expires_from_now(boost::posix_time::minutes(5));
        mTimeout.async_wait(boost::bind(&NzTcpConnection::OnTimeout, shared_from_this(), _1));
        // Try to handle the payload
        try
        {
            mStrategy->HandleContent(std::string(mInboundData.data(), pBytes));
        }
        catch (...)
        {
            // Unable to decode data.
            asio::error_code error(asio::error::invalid_argument);
            this->HandleRead(error);
            return;
        }

        // Inform caller that data has been received ok.
        this->HandleRead(e);
    }
}
//---------------------------------------------------------------------
bool NzTcpConnection::IsOffline()
{
    return !mSocket.is_open();
}
//---------------------------------------------------------------------
void NzTcpConnection::SetStrategy(INzStrategy* pStrategy)
{
    mStrategy = pStrategy;
}
//---------------------------------------------------------------------
void NzTcpConnection::Connect(const std::string& pAddress, const std::string& pPort)
{
    asio::ip::tcp::resolver::query query(pAddress, pPort);
    mResolver.async_resolve(query, boost::bind(&NzTcpConnection::handle_resolve, this,
                            asio::placeholders::error,
                            asio::placeholders::iterator));
}
//---------------------------------------------------------------------
void NzTcpConnection::handle_resolve(const asio::error_code& ec, asio::ip::tcp::resolver::iterator pEndpointItor)
{
    if(!ec)
    {
        asio::async_connect(mSocket, pEndpointItor, boost::bind(&NzTcpConnection::handle_connect, this, asio::placeholders::error));
    }
    else
    {
        OnError(ec.message());
    }
}
//---------------------------------------------------------------------
void NzTcpConnection::handle_connect(const asio::error_code& ec)
{
    if(!ec)
    {
        OnConnect(true);
        //this->Start();
    }
    else
    {
        OnConnect(false);
        OnError(ec.message());
    }
}
//---------------------------------------------------------------------
