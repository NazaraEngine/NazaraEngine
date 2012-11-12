// Copyright (C) 2012 Maxime Griot
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/TcpServer.hpp>
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>

using asio::ip::tcp;

//---------------------------------------------------------------------
NzTcpServer::NzTcpServer(unsigned short pPort)
    :mPort(pPort),mStarted(false)
{
}
//---------------------------------------------------------------------
void NzTcpServer::Scale()
{
    unsigned int logicThreads = 1;
    mIoPool.reset(new NzIoServicePool(logicThreads));
    mAcceptor.reset(new asio::ip::tcp::acceptor(mIoPool->GetIoService(), tcp::endpoint(tcp::v4(), mPort)));

    /*System::Log::Print("NzTcpServer started : 0.0.0.0:" + std::to_string((unsigned long long)mPort));
    System::Log::Print("Running with : " + std::to_string((unsigned long long)logicThreads) + " Network thread(s)");
    System::Log::Print("");*/
}
//---------------------------------------------------------------------
NzTcpServer::~NzTcpServer()
{
    mAcceptor->cancel();
    mAcceptor.reset(nullptr);
}
//---------------------------------------------------------------------
void NzTcpServer::Start()
{
    // note : Order matters !
    Scale();
    Accept();

    mStarted = true;

    mIoPool->Run();
}
//---------------------------------------------------------------------
void NzTcpServer::Run()
{
    mTotalElapsed = 0;

    while(mStarted)
    {
        RunOnce();

        if(mTotalElapsed < 50)
        {
            // This thread doesn't need to be very responsive
           // boost::this_thread::sleep(boost::posix_time::milliseconds(50 - mTotalElapsed));
        }
    }
}
//---------------------------------------------------------------------
void NzTcpServer::RunOnce()
{
    mTotalElapsed += 0;

    OnUpdate(mTotalElapsed);

    mTotalElapsed = 0;
}
//---------------------------------------------------------------------
void NzTcpServer::Stop()
{
    mStarted = false;
}
//---------------------------------------------------------------------
void NzTcpServer::Accept()
{
    NzTcpConnection::pointer session = boost::make_shared<NzTcpConnection>(mIoPool->GetIoService());
    mAcceptor->async_accept(session->GetSocket(),
                            boost::bind(&NzTcpServer::HandleAccept, this,
                                        session, asio::placeholders::error));
}
//---------------------------------------------------------------------
void NzTcpServer::HandleAccept(NzTcpConnection::pointer pSession, const asio::error_code& pError)
{
    if(!pError)
    {
        OnConnection(pSession);
        pSession->Start();

        Accept();
    }
    else
    {
    }
}
//---------------------------------------------------------------------
