// Copyright (C) 2012 Maxime Griot
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef PACKET_HPP
#define PACKET_HPP

#include <Nazara/Prerequesites.hpp>

#include <cstdint>
#include <vector>
#include <list>
#include <map>
#include <string>

class NAZARA_API NzPacket
{
public:

    enum
    {
        kHandshake,
        kMessage
    };

    NzPacket();
    /**
     * Construct a NzPacket.
     * @param pOpcode Opcode to send.
     * @param pObjectId The object's id.
     * @param pType The NzPacket's type (ping, handshake or NzPacket).
     */
    NzPacket(uint32_t pOpcode, uint8_t pType = kMessage);
    NzPacket(NzPacket&& pNzPacket);

    NzPacket& operator=(NzPacket&& pNzPacket);

    NzPacket& operator<<(bool pData);
    NzPacket& operator<<(const uint8_t& pData);
    NzPacket& operator<<(const uint16_t& pData);
    NzPacket& operator<<(const uint32_t& pData);
    NzPacket& operator<<(const uint64_t& pData);
    NzPacket& operator<<(const int8_t& pData);
    NzPacket& operator<<(const int16_t& pData);
    NzPacket& operator<<(const int32_t& pData);
    NzPacket& operator<<(const int64_t& pData);
    NzPacket& operator<<(const float& pData);
    NzPacket& operator<<(const double& pData);
    NzPacket& operator<<(const std::string& pData);

    NzPacket& operator>>(bool& pData);
    NzPacket& operator>>(uint8_t& pData);
    NzPacket& operator>>(uint16_t& pData);
    NzPacket& operator>>(uint32_t& pData);
    NzPacket& operator>>(uint64_t& pData);
    NzPacket& operator>>(int8_t& pData);
    NzPacket& operator>>(int16_t& pData);
    NzPacket& operator>>(int32_t& pData);
    NzPacket& operator>>(int64_t& pData);
    NzPacket& operator>>(float& pData);
    NzPacket& operator>>(double& pData);
    NzPacket& operator>>(std::string& pData);

    template <class K, class U>
    NzPacket& operator<<(const std::map<K, U>& pData)
    {
        *this << (uint32_t)pData.size();
        for(auto itor = pData.begin(), end = pData.end(); itor != end; ++itor)
        {
            *this << itor->first << itor->second;
        }

        return *this;
    }

    template <class K, class U>
    NzPacket& operator>>(std::map<K, U>& pData)
    {
        uint32_t size;
        *this >> size;
        if(size > 1000)
            return *this;

        for(uint64_t i = 0; i < size; ++i)
        {
            K key;
            U data;
            *this >> key >> data;
            pData.insert(std::pair<K,U>(key, data));
        }

        return *this;
    }

    template <class U>
    NzPacket& operator<<(const std::vector<U>& pData)
    {
        *this << (uint32_t)pData.size();
        for(auto itor = pData.begin(), end = pData.end(); itor != end; ++itor)
        {
            *this << *itor;
        }

        return *this;
    }

    template <class U>
    NzPacket& operator>>(std::vector<U>& pData)
    {
        uint32_t size;
        *this >> size;
        if(size > 1000)
            return *this;

        for(uint64_t i = 0; i < size; ++i)
        {
            U data;
            *this >> data;
            pData.push_back(data);
        }

        return *this;
    }

    template <class U>
    NzPacket& operator<<(const std::list<U>& pData)
    {
        *this << (uint32_t)pData.size();
        for(auto itor = pData.begin(), end = pData.end(); itor != end; ++itor)
        {
            *this << *itor;
        }

        return *this;
    }

    template <class U>
    NzPacket& operator>>(std::list<U>& pData)
    {
        uint32_t size;
        *this >> size;
        if(size > 1000)
            return *this;

        for(uint64_t i = 0; i < size; ++i)
        {
            U data;
            *this >> data;
            pData.push_back(data);
        }

        return *this;
    }

    void InitializeHeader	(const std::string& pData);
    void Initialize			(const std::vector<char>& pData);
    void Initialize			(const std::string& pData);
    void Write				(uint8_t* pData, size_t pSize, int32_t pPos = -1);

    uint32_t ReadObjectId();
    int32_t GetPosition();

    std::string			GetHeader();
    const std::string&	GetBuffer();

    void Finalize();

    uint8_t  Type;
    uint32_t Size;
    uint32_t Opcode;
    uint32_t ObjectId;

protected:

    std::string					mBuffer;
    uint8_t						mChecksum;
};

#endif // PACKET_HPP
