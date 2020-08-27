// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderAstSerializer.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	template<typename T>
	void ShaderAstSerializerBase::Container(T& container)
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
	void ShaderAstSerializerBase::Enum(T& enumVal)
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
	void ShaderAstSerializerBase::OptEnum(std::optional<T>& optVal)
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
	void ShaderAstSerializerBase::OptVal(std::optional<T>& optVal)
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
	void ShaderAstSerializerBase::Node(std::shared_ptr<T>& node)
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
	void ShaderAstSerializerBase::Variable(std::shared_ptr<T>& var)
	{
		bool isWriting = IsWriting();

		ShaderNodes::VariablePtr value;
		if (isWriting)
			value = var;

		Variable(value);
		if (!isWriting)
			var = std::static_pointer_cast<T>(value);
	}

	inline ShaderAstSerializer::ShaderAstSerializer(ByteStream& stream) :
	m_stream(stream)
	{
	}

	inline ShaderAstUnserializer::ShaderAstUnserializer(ByteStream& stream) :
	m_stream(stream)
	{
	}
}

#include <Nazara/Shader/DebugOff.hpp>
