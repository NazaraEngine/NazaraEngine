// Copyright (C) 2012 Maxime Griot
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/IoServicePool.hpp>
//#include <System/Log.h>
#include <asio.hpp>

using asio::ip::tcp;

//---------------------------------------------------------------------
NzIoServicePool::NzIoServicePool(std::size_t pool_size)
    : m_NextIndex(0)
{
    if (pool_size == 0)
        throw std::runtime_error("IoServicePool size is 0");

    for (std::size_t i = 0; i < pool_size; ++i)
    {
        io_service_ptr io_service(new asio::io_service);
        m_IoServices.push_back(io_service);
        m_Running.push_back(false);
    }
}
//---------------------------------------------------------------------
NzIoServicePool::~NzIoServicePool()
{
    Stop();
}
//---------------------------------------------------------------------
void NzIoServicePool::Run()
{
    // Create a pool of threads to run all of the io_services.
    for (std::size_t i = 0; i < m_IoServices.size(); ++i)
    {
        m_Running[i] = true;

        m_Threads.push_back(std::shared_ptr<NzThread> (new NzThread(
                               [this,i]()
        {

            while(m_Running[i])
            {
                //timer.restart();

                m_IoServices[i]->poll();
                m_IoServices[i]->reset();

                //uint32_t const elapsed = std::uint32_t(timer.elapsed() * 1000);

                /*if(elapsed < 50 && m_Running[i])
                {
                    Sleep(50 - elapsed);
                }*/
            }

        })));
    }
}
//---------------------------------------------------------------------
void NzIoServicePool::Stop()
{
    // Explicitly Stop all io_services.
    for (std::size_t i = 0; i < m_IoServices.size(); ++i)
    {
        m_Running[i] = false;
        m_IoServices[i]->stop();
    }

    for (std::size_t i = 0; i < m_Threads.size(); ++i)
        m_Threads[i]->Join();

    m_Threads.clear();
    m_IoServices.clear();
}
//---------------------------------------------------------------------
asio::io_service& NzIoServicePool::GetIoService()
{
    // Use a round-robin scheme to choose the next io_service to use.
    asio::io_service& io_service = *m_IoServices[m_NextIndex];
    ++m_NextIndex;
    if (m_NextIndex == m_IoServices.size())
        m_NextIndex = 0;
    return io_service;
}
//---------------------------------------------------------------------
