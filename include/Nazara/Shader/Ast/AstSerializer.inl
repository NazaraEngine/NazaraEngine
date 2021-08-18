// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/AstSerializer.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	template<typename T>
	void AstSerializerBase::Attribute(AttributeValue<T>& attribute)
	{
		UInt32 valueType;
		if (IsWriting())
		{
			if (!attribute.HasValue())
				valueType = 0;
			else if (attribute.IsExpression())
				valueType = 1;
			else if (attribute.IsResultingValue())
				valueType = 2;
			else
				throw std::runtime_error("unexpected attribute");
		}

		Value(valueType);

		switch (valueType)
		{
			case 0:
				if (!IsWriting())
					attribute = {};

				break;

			case 1:
			{
				if (!IsWriting())
				{
					ExpressionPtr expr;
					Node(expr);

					attribute = std::move(expr);
				}
				else
					Node(const_cast<ExpressionPtr&>(attribute.GetExpression())); //< not used for writing

				break;
			}

			case 2:
			{
				if (!IsWriting())
				{
					T value;
					if constexpr (std::is_enum_v<T>)
						Enum(value);
					else
						Value(value);

					attribute = std::move(value);
				}
				else
				{
					T& value = const_cast<T&>(attribute.GetResultingValue()); //< not used for writing
					if constexpr (std::is_enum_v<T>)
						Enum(value);
					else
						Value(value);
				}

				break;
			}
		}
	}

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
