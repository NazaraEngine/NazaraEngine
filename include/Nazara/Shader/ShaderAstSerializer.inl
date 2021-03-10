// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderAstSerializer.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	template<typename T>
	void AstSerializerBase::Container(T& container)
	{
		bool isWriting = IsWriting();

		UInt32 size;
		if (isWriting)
			size = UInt32(container.size());

		Value(size);
		if (!isWriting)
			container.resize(size);
	}


	template<typename T>
	void AstSerializerBase::Enum(T& enumVal)
	{
		bool isWriting = IsWriting();

		UInt32 value;
		if (isWriting)
			value = static_cast<UInt32>(enumVal);

		Value(value);
		if (!isWriting)
			enumVal = static_cast<T>(value);
	}

	template<typename T>
	void AstSerializerBase::OptEnum(std::optional<T>& optVal)
	{
		bool isWriting = IsWriting();

		bool hasValue;
		if (isWriting)
			hasValue = optVal.has_value();

		Value(hasValue);

		if (!isWriting && hasValue)
			optVal.emplace();

		if (optVal.has_value())
			Enum(optVal.value());
	}

	template<typename T>
	void AstSerializerBase::OptVal(std::optional<T>& optVal)
	{
		bool isWriting = IsWriting();

		bool hasValue;
		if (isWriting)
			hasValue = optVal.has_value();

		Value(hasValue);

		if (!isWriting && hasValue)
			optVal.emplace();

		if (optVal.has_value())
		{
			if constexpr (std::is_same_v<T, std::size_t>)
				SizeT(optVal.value());
			else
				Value(optVal.value());
		}
	}

	inline void AstSerializerBase::SizeT(std::size_t& val)
	{
		bool isWriting = IsWriting();

		UInt32 fixedVal;
		if (isWriting)
			fixedVal = static_cast<UInt32>(val);

		Value(fixedVal);

		if (!isWriting)
			val = static_cast<std::size_t>(fixedVal);
	}

	inline ShaderAstSerializer::ShaderAstSerializer(ByteStream& stream) :
	m_stream(stream)
	{
	}

	inline ShaderAstUnserializer::ShaderAstUnserializer(ByteStream& stream) :
	m_stream(stream)
	{
	}

	inline StatementPtr UnserializeShader(const void* data, std::size_t size)
	{
		ByteStream byteStream(data, size);
		return UnserializeShader(byteStream);
	}
}

#include <Nazara/Shader/DebugOff.hpp>
