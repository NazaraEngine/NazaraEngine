// Copyright (C) 2012 Maxime Griot
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/Packet.hpp>
#include <sstream>
#include <stdexcept>
#include <memory>
#include <cstring>

//---------------------------------------------------------------------
NzPacket::NzPacket()
    :Opcode(0), ObjectId(0), Type(0)
{
}
//---------------------------------------------------------------------
NzPacket::NzPacket(uint32_t pOpcode, uint8_t pType)
    :Opcode(pOpcode), ObjectId(0), Type(pType)
{
}
//---------------------------------------------------------------------
NzPacket::NzPacket(NzPacket&& pNzPacket)
    :Opcode(std::move(pNzPacket.Opcode)),
     ObjectId(std::move(pNzPacket.ObjectId)),
     Type(std::move(pNzPacket.Type)),
     Size(std::move(pNzPacket.Size)),
     mBuffer(std::move(pNzPacket.mBuffer))
{
}
//---------------------------------------------------------------------
NzPacket& NzPacket::operator=(NzPacket&& pNzPacket)
{
    Opcode = std::move(pNzPacket.Opcode);
    ObjectId = std::move(pNzPacket.ObjectId);
    Type = std::move(pNzPacket.Type);
    Size = std::move(pNzPacket.Size);
    mBuffer = std::move(pNzPacket.mBuffer);

    return *this;
}
//---------------------------------------------------------------------
NzPacket& NzPacket::operator<<(bool pData)
{
    *this << uint8_t(pData ? 1 : 0);
    return *this;
}
//---------------------------------------------------------------------
NzPacket& NzPacket::operator<<(const uint8_t& pData)
{
    mBuffer.append((char*)&pData, 1);
    return *this;
}
//---------------------------------------------------------------------
NzPacket& NzPacket::operator<<(const uint16_t& pData)
{
    mBuffer.append((char*)&pData, 2);
    return *this;
}
//---------------------------------------------------------------------
NzPacket& NzPacket::operator<<(const uint32_t& pData)
{
    mBuffer.append((char*)&pData, 4);
    return *this;
}
//---------------------------------------------------------------------
NzPacket& NzPacket::operator<<(const uint64_t& pData)
{
    mBuffer.append((char*)&pData, 8);
    return *this;
}
//---------------------------------------------------------------------
NzPacket& NzPacket::operator<<(const int8_t& pData)
{
    mBuffer.append((char*)&pData, 1);
    return *this;
}
//---------------------------------------------------------------------
NzPacket& NzPacket::operator<<(const int16_t& pData)
{
    mBuffer.append((char*)&pData, 2);
    return *this;
}
//---------------------------------------------------------------------
NzPacket& NzPacket::operator<<(const int32_t& pData)
{
    mBuffer.append((char*)&pData, 4);
    return *this;
}
//---------------------------------------------------------------------
NzPacket& NzPacket::operator<<(const int64_t& pData)
{
    mBuffer.append((char*)&pData, 8);
    return *this;
}
//---------------------------------------------------------------------
NzPacket& NzPacket::operator<<(const float& pData)
{
    mBuffer.append((char*)&pData, 4);
    return *this;
}
//---------------------------------------------------------------------
NzPacket& NzPacket::operator<<(const double& pData)
{
    mBuffer.append((char*)&pData, 8);
    return *this;
}
//---------------------------------------------------------------------
NzPacket& NzPacket::operator<<(const std::string& pData)
{
    *this << (uint32_t)pData.size();
    mBuffer.append(pData.c_str(), pData.size());

    return *this;
}
//---------------------------------------------------------------------
NzPacket& NzPacket::operator>>(bool& pData)
{
    uint8_t i;
    *this >> i;
    pData = i ? true : false;
    return *this;
}
//---------------------------------------------------------------------
NzPacket& NzPacket::operator>>(uint8_t& pData)
{
    if(mBuffer.size() < 1)
    {
        pData = 0;
        return *this;
    }
    pData = *(uint8_t*)&mBuffer[0];
    mBuffer.erase(0,1);
    return *this;
}
//---------------------------------------------------------------------
NzPacket& NzPacket::operator>>(uint16_t& pData)
{
    if(mBuffer.size() < 2)
    {
        pData = 0;
        return *this;
    }
    pData = *(uint16_t*)&mBuffer[0];
    mBuffer.erase(0,2);
    return *this;
}
//---------------------------------------------------------------------
NzPacket& NzPacket::operator>>(uint32_t& pData)
{
    if(mBuffer.size() < 4)
    {
        pData = 0;
        return *this;
    }
    pData = *(uint32_t*)&mBuffer[0];
    mBuffer.erase(0,4);
    return *this;
}
//---------------------------------------------------------------------
NzPacket& NzPacket::operator>>(uint64_t& pData)
{
    if(mBuffer.size() < 8)
    {
        pData = 0;
        return *this;
    }
    pData = *(uint64_t*)&mBuffer[0];
    mBuffer.erase(0,8);
    return *this;
}
//---------------------------------------------------------------------
NzPacket& NzPacket::operator>>(int8_t& pData)
{
    if(mBuffer.size() < 1)
    {
        pData = 0;
        return *this;
    }
    pData = *(int8_t*)&mBuffer[0];
    mBuffer.erase(0,1);
    return *this;
}
//---------------------------------------------------------------------
NzPacket& NzPacket::operator>>(int16_t& pData)
{
    if(mBuffer.size() < 2)
    {
        pData = 0;
        return *this;
    }
    pData = *(int16_t*)&mBuffer[0];
    mBuffer.erase(0,2);
    return *this;
}
//---------------------------------------------------------------------
NzPacket& NzPacket::operator>>(int32_t& pData)
{
    if(mBuffer.size() < 4)
    {
        pData = 0;
        return *this;
    }
    pData = *(int32_t*)&mBuffer[0];
    mBuffer.erase(0,4);
    return *this;
}
//---------------------------------------------------------------------
NzPacket& NzPacket::operator>>(int64_t& pData)
{
    if(mBuffer.size() < 8)
    {
        pData = 0;
        return *this;
    }
    pData = *(int64_t*)&mBuffer[0];
    mBuffer.erase(0,8);
    return *this;
}
//---------------------------------------------------------------------
NzPacket& NzPacket::operator>>(float& pData)
{
    if(mBuffer.size() < 4)
    {
        pData = 0.f;
        return *this;
    }
    pData = *(float*)&mBuffer[0];
    mBuffer.erase(0,4);
    return *this;
}
//---------------------------------------------------------------------
NzPacket& NzPacket::operator>>(double& pData)
{
    if(mBuffer.size() < 8)
    {
        pData = 0;
        return *this;
    }
    pData = *(double*)&mBuffer[0];
    mBuffer.erase(0,8);
    return *this;
}
//---------------------------------------------------------------------
NzPacket& NzPacket::operator>>(std::string& pData)
{
    uint32_t size = 0;
    *this >> size;

    // Check for fake string size to prevent memory hacks
    if(size > mBuffer.size() || size == 0)
    {
        std::ostringstream os;
        os << "String size (" << size << ") > NzPacket size (" << mBuffer.size() << ")";
        throw std::out_of_range(os.str());
    }

    pData = mBuffer.substr(0,size);

    pData.shrink_to_fit();

    mBuffer.erase(0,size);
    return *this;
}
//---------------------------------------------------------------------
void NzPacket::Write(uint8_t* pData, size_t pSize, int32_t pPos)
{
    if(pPos < 0)
        mBuffer.append((char*)pData, pSize);
    else if(pPos + pSize <= mBuffer.size())
    {
        std::memcpy(&mBuffer[pPos], pData, pSize);
    }
}
//---------------------------------------------------------------------
void NzPacket::Initialize(const std::vector<char>& pData)
{
    mBuffer.clear();
    mBuffer.append(&pData[0], pData.size());
}
//---------------------------------------------------------------------
void NzPacket::Initialize(const std::string& pData)
{
    mBuffer.clear();
    mBuffer.append(&pData[0], pData.size());
}
//---------------------------------------------------------------------
uint32_t NzPacket::ReadObjectId()
{
    *this >> ObjectId;
    return ObjectId;
}
//---------------------------------------------------------------------
void NzPacket::InitializeHeader(const std::string& pData)
{
    Type = *(uint8_t*)&pData[0];
    Size = *(uint32_t*)&pData[1];
    mChecksum = *(uint8_t*)&pData[5];
}
//---------------------------------------------------------------------
std::string NzPacket::GetHeader()
{
    Finalize();

    std::string data;

    data.append((char*)&Type, 1);
    data.append((char*)&Size, 4);
    data.append((char*)&mChecksum, 1);
    data.append((char*)&Opcode, 4);
    data.append((char*)&ObjectId, 4);

    return data;
}
//---------------------------------------------------------------------
int32_t NzPacket::GetPosition()
{
    return (int32_t)mBuffer.size();
}
//---------------------------------------------------------------------
const std::string& NzPacket::GetBuffer()
{
    return mBuffer;
}
//---------------------------------------------------------------------
void NzPacket::Finalize()
{
    mChecksum = Type ^ ((char*)&Size)[0] ^ ((char*)&Size)[1] ^ ((char*)&Size)[2] ^ ((char*)&Size)[3];
}
//---------------------------------------------------------------------
