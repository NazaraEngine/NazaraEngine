// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/AbstractHash.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ByteArray.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline ByteArray File::ComputeHash(HashType hash, const String& filePath)
	{
		return ComputeHash(AbstractHash::Get(hash).get(), filePath);
	}

	inline ByteArray File::ComputeHash(AbstractHash* hash, const String& filePath)
	{
		return Nz::ComputeHash(hash, File(filePath));
	}

	template<>
	struct Hashable<File>
	{
		bool operator()(const Nz::File& originalFile, AbstractHash* hash) const
		{

			File file(originalFile.GetPath());
			if (!file.Open(OpenMode_ReadOnly))
			{
				NazaraError("Unable to open file");
				return false;
			}

			UInt64 remainingSize = file.GetSize();

			char buffer[NAZARA_CORE_FILE_BUFFERSIZE];
			while (remainingSize > 0)
			{
				unsigned int size = static_cast<unsigned int>(std::min(remainingSize, static_cast<UInt64>(NAZARA_CORE_FILE_BUFFERSIZE)));
				if (file.Read(&buffer[0], sizeof(char), size) != sizeof(char)*size)
				{
					NazaraError("Unable to read file");
					return false;
				}

				remainingSize -= size;
				hash->Append(reinterpret_cast<UInt8*>(&buffer[0]), size);
			}

			return true;
		}
	};
}

#include <Nazara/Core/DebugOff.hpp>
