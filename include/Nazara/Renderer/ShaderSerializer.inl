// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/ShaderSerializer.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	template<typename T>
	void ShaderSerializerBase::Container(T& container)
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
	void ShaderSerializerBase::Enum(T& enumVal)
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
	void ShaderSerializerBase::OptEnum(std::optional<T>& optVal)
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
	void ShaderSerializerBase::OptVal(std::optional<T>& optVal)
	{
		bool isWriting = IsWriting();

		bool hasValue;
		if (isWriting)
			hasValue = optVal.has_value();

		Value(hasValue);

		if (!isWriting && hasValue)
			optVal.emplace();

		if (optVal.has_value())
			Value(optVal.value());
	}

	template<typename T>
	void ShaderSerializerBase::Node(std::shared_ptr<T>& node)
	{
		bool isWriting = IsWriting();

		ShaderNodes::NodePtr value;
		if (isWriting)
			value = node;

		Node(value);
		if (!isWriting)
			node = std::static_pointer_cast<T>(value);
	}

	template<typename T>
	void ShaderSerializerBase::Variable(std::shared_ptr<T>& var)
	{
		bool isWriting = IsWriting();

		ShaderNodes::VariablePtr value;
		if (isWriting)
			value = var;

		Variable(value);
		if (!isWriting)
			var = std::static_pointer_cast<T>(value);
	}

	inline void ShaderSerializerBase::Value(std::size_t& val)
	{
		bool isWriting = IsWriting();

		UInt32 value;
		if (isWriting)
			value = static_cast<UInt32>(val);

		Value(value);
		if (!isWriting)
			val = static_cast<std::size_t>(value);
	}

	inline ShaderSerializer::ShaderSerializer(ByteStream& stream) :
	m_stream(stream)
	{
	}

	inline ShaderUnserializer::ShaderUnserializer(ByteStream& stream) :
	m_stream(stream)
	{
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
