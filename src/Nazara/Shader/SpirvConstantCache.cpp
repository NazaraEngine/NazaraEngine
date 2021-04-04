// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/SpirvConstantCache.hpp>
#include <Nazara/Shader/SpirvSection.hpp>
#include <Nazara/Utility/FieldOffsets.hpp>
#include <tsl/ordered_map.h>
#include <stdexcept>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	namespace
	{
		template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

		template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;
	}

	struct SpirvConstantCache::Eq
	{
		bool Compare(const ConstantBool& lhs, const ConstantBool& rhs) const
		{
			return lhs.value == rhs.value;
		}

		bool Compare(const ConstantComposite& lhs, const ConstantComposite& rhs) const
		{
			return Compare(lhs.type, rhs.type) && Compare(lhs.values, rhs.values);
		}

		bool Compare(const ConstantScalar& lhs, const ConstantScalar& rhs) const
		{
			return lhs.value == rhs.value;
		}

		bool Compare(const Bool& /*lhs*/, const Bool& /*rhs*/) const
		{
			return true;
		}

		bool Compare(const Float& lhs, const Float& rhs) const
		{
			return lhs.width == rhs.width;
		}

		bool Compare(const Function& lhs, const Function& rhs) const
		{
			return Compare(lhs.parameters, rhs.parameters) && Compare(lhs.returnType, rhs.returnType);
		}

		bool Compare(const Identifier& lhs, const Identifier& rhs) const
		{
			return lhs.name == rhs.name;
		}

		bool Compare(const Image& lhs, const Image& rhs) const
		{
			return lhs.arrayed == rhs.arrayed
				&& lhs.dim == rhs.dim
				&& lhs.format == rhs.format
				&& lhs.multisampled == rhs.multisampled
				&& lhs.qualifier == rhs.qualifier
				&& Compare(lhs.sampledType, rhs.sampledType)
				&& lhs.depth == rhs.depth
				&& lhs.sampled == rhs.sampled;
		}

		bool Compare(const Integer& lhs, const Integer& rhs) const
		{
			return lhs.width == rhs.width && lhs.signedness == rhs.signedness;
		}

		bool Compare(const Matrix& lhs, const Matrix& rhs) const
		{
			return lhs.columnCount == rhs.columnCount && Compare(lhs.columnType, rhs.columnType);
		}

		bool Compare(const Pointer& lhs, const Pointer& rhs) const
		{
			return lhs.storageClass == rhs.storageClass && Compare(lhs.type, rhs.type);
		}

		bool Compare(const SampledImage& lhs, const SampledImage& rhs) const
		{
			return Compare(lhs.image, rhs.image);
		}

		bool Compare(const Structure& lhs, const Structure& rhs) const
		{
			if (lhs.name != rhs.name)
				return false;

			if (!Compare(lhs.members, rhs.members))
				return false;

			return true;
		}

		bool Compare(const Structure::Member& lhs, const Structure::Member& rhs) const
		{
			if (!Compare(lhs.type, rhs.type))
				return false;

			if (lhs.name != rhs.name)
				return false;

			return true;
		}

		bool Compare(const Variable& lhs, const Variable& rhs) const
		{
			if (lhs.debugName != rhs.debugName)
				return false;

			if (!Compare(lhs.initializer, rhs.initializer))
				return false;

			if (lhs.storageClass != rhs.storageClass)
				return false;

			if (!Compare(lhs.type, rhs.type))
				return false;

			return true;
		}

		bool Compare(const Vector& lhs, const Vector& rhs) const
		{
			return Compare(lhs.componentType, rhs.componentType) && lhs.componentCount == rhs.componentCount;
		}

		bool Compare(const Void& /*lhs*/, const Void& /*rhs*/) const
		{
			return true;
		}


		bool Compare(const Constant& lhs, const Constant& rhs) const
		{
			return Compare(lhs.constant, rhs.constant);
		}

		bool Compare(const Type& lhs, const Type& rhs) const
		{
			return Compare(lhs.type, rhs.type);
		}


		template<typename T>
		bool Compare(const std::optional<T>& lhs, const std::optional<T>& rhs) const
		{
			if (lhs.has_value() != rhs.has_value())
				return false;

			if (!lhs.has_value())
				return true;

			return Compare(*lhs, *rhs);
		}

		template<typename T>
		bool Compare(const std::shared_ptr<T>& lhs, const std::shared_ptr<T>& rhs) const
		{
			if (bool(lhs) != bool(rhs))
				return false;

			if (!lhs)
				return true;

			return Compare(*lhs, *rhs);
		}

		template<typename... T>
		bool Compare(const std::variant<T...>& lhs, const std::variant<T...>& rhs) const
		{
			if (lhs.index() != rhs.index())
				return false;

			return std::visit([&](auto&& arg)
			{
				using U = std::decay_t<decltype(arg)>;
				return Compare(arg, std::get<U>(rhs));
			}, lhs);
		}

		template<typename T>
		bool Compare(const std::vector<T>& lhs, const std::vector<T>& rhs) const
		{
			if (lhs.size() != rhs.size())
				return false;

			for (std::size_t i = 0; i < lhs.size(); ++i)
			{
				if (!Compare(lhs[i], rhs[i]))
					return false;
			}

			return true;
		}

		template<typename T>
		bool Compare(const std::unique_ptr<T>& lhs, const std::unique_ptr<T>& rhs) const
		{
			if (bool(lhs) != bool(rhs))
				return false;

			if (!lhs)
				return true;

			return Compare(*lhs, *rhs);
		}

		template<typename T>
		bool operator()(const T& lhs, const T& rhs) const
		{
			return Compare(lhs, rhs);
		}
	};

	struct SpirvConstantCache::DepRegisterer
	{
		DepRegisterer(SpirvConstantCache& c) :
		cache(c)
		{
		}

		void Register(const Bool&) {}
		void Register(const Float&) {}
		void Register(const Integer&) {}
		void Register(const Void&) {}

		void Register(const Identifier& identifier)
		{
			Register(identifier);
		}

		void Register(const Image& image)
		{
			Register(image.sampledType);
		}

		void Register(const Function& func)
		{
			cache.Register(*func.returnType);
			Register(func.parameters);
		}

		void Register(const Matrix& vec)
		{
			assert(vec.columnType);
			cache.Register(*vec.columnType);
		}

		void Register(const Pointer& ptr)
		{
			assert(ptr.type);
			cache.Register(*ptr.type);
		}

		void Register(const SampledImage& sampledImage)
		{
			assert(sampledImage.image);
			cache.Register(*sampledImage.image);
		}

		void Register(const Structure& s)
		{
			Register(s.members);
		}

		void Register(const SpirvConstantCache::Structure::Member& m)
		{
			cache.Register(*m.type);
		}

		void Register(const Variable& variable)
		{
			assert(variable.type);
			cache.Register(*variable.type);
		}

		void Register(const Vector& vec)
		{
			assert(vec.componentType);
			cache.Register(*vec.componentType);
		}

		void Register(const ConstantBool&)
		{
			cache.Register({ Bool{} });
		}

		void Register(const ConstantScalar& scalar)
		{
			std::visit([&](auto&& arg)
			{
				using T = std::decay_t<decltype(arg)>;

				if constexpr (std::is_same_v<T, double>)
					cache.Register({ Float{ 64 } });
				else if constexpr (std::is_same_v<T, float>)
					cache.Register({ Float{ 32 } });
				else if constexpr (std::is_same_v<T, Int32>)
					cache.Register({ Integer{ 32, true } });
				else if constexpr (std::is_same_v<T, Int64>)
					cache.Register({ Integer{ 64, true } });
				else if constexpr (std::is_same_v<T, UInt32>)
					cache.Register({ Integer{ 32, false } });
				else if constexpr (std::is_same_v<T, UInt64>)
					cache.Register({ Integer{ 64, false } });
				else
					static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");

			}, scalar.value);
		}

		void Register(const ConstantComposite& composite)
		{
			assert(composite.type);
			cache.Register(*composite.type);

			for (auto&& value : composite.values)
			{
				assert(value);
				cache.Register(*value);
			}
		}


		void Register(const Constant& c)
		{
			return Register(c.constant);
		}

		void Register(const Type& t)
		{
			return Register(t.type);
		}


		template<typename T>
		void Register(const std::shared_ptr<T>& ptr)
		{
			assert(ptr);
			return Register(*ptr);
		}

		template<typename T>
		void Register(const std::optional<T>& opt)
		{
			if (opt)
				Register(*opt);
		}

		template<typename... T>
		void Register(const std::variant<T...>& v)
		{
			return std::visit([&](auto&& arg)
			{
				return Register(arg);
			}, v);
		}

		void Register(const std::vector<TypePtr>& lhs)
		{
			for (std::size_t i = 0; i < lhs.size(); ++i)
				cache.Register(*lhs[i]);
		}

		template<typename T>
		void Register(const std::vector<T>& lhs)
		{
			for (std::size_t i = 0; i < lhs.size(); ++i)
				Register(lhs[i]);
		}

		template<typename T>
		void Register(const std::unique_ptr<T>& lhs)
		{
			assert(lhs);
			return Register(*lhs);
		}

		SpirvConstantCache& cache;
	};

	//< FIXME PLZ
	struct AnyHasher
	{
		template<typename U>
		std::size_t operator()(const U&) const
		{
			return 42;
		}
	};

	struct SpirvConstantCache::Internal
	{
		Internal(UInt32& resultId) :
		nextResultId(resultId)
		{
		}

		tsl::ordered_map<std::variant<AnyConstant, AnyType>, UInt32 /*id*/, AnyHasher, Eq> ids;
		tsl::ordered_map<Variable, UInt32 /*id*/, AnyHasher, Eq> variableIds;
		tsl::ordered_map<Structure, FieldOffsets /*fieldOffsets*/, AnyHasher, Eq> structureSizes;
		UInt32& nextResultId;
	};

	SpirvConstantCache::SpirvConstantCache(UInt32& resultId)
	{
		m_internal = std::make_unique<Internal>(resultId);
	}

	SpirvConstantCache::SpirvConstantCache(SpirvConstantCache&& cache) noexcept = default;

	SpirvConstantCache::~SpirvConstantCache() = default;

	UInt32 SpirvConstantCache::GetId(const Constant& c)
	{
		auto it = m_internal->ids.find(c.constant);
		if (it == m_internal->ids.end())
			throw std::runtime_error("constant is not registered");

		return it->second;
	}

	UInt32 SpirvConstantCache::GetId(const Type& t)
	{
		auto it = m_internal->ids.find(t.type);
		if (it == m_internal->ids.end())
			throw std::runtime_error("constant is not registered");

		return it->second;
	}

	UInt32 SpirvConstantCache::GetId(const Variable& v)
	{
		auto it = m_internal->variableIds.find(v);
		if (it == m_internal->variableIds.end())
			throw std::runtime_error("variable is not registered");

		return it->second;
	}

	UInt32 SpirvConstantCache::Register(Constant c)
	{
		AnyConstant& constant = c.constant;

		DepRegisterer registerer(*this);
		registerer.Register(constant);

		std::size_t h = m_internal->ids.hash_function()(constant);
		auto it = m_internal->ids.find(constant, h);
		if (it == m_internal->ids.end())
		{
			UInt32 resultId = m_internal->nextResultId++;
			it = m_internal->ids.emplace(std::move(constant), resultId).first;
		}

		return it.value();
	}

	UInt32 SpirvConstantCache::Register(Type t)
	{
		AnyType& type = t.type;
		if (std::holds_alternative<Identifier>(type))
		{
			assert(m_identifierCallback);
			return Register(*m_identifierCallback(std::get<Identifier>(type).name));
		}

		DepRegisterer registerer(*this);
		registerer.Register(type);

		std::size_t h = m_internal->ids.hash_function()(type);
		auto it = m_internal->ids.find(type, h);
		if (it == m_internal->ids.end())
		{
			UInt32 resultId = m_internal->nextResultId++;
			it = m_internal->ids.emplace(std::move(type), resultId).first;
		}

		return it.value();
	}

	UInt32 SpirvConstantCache::Register(Variable v)
	{
		DepRegisterer registerer(*this);
		registerer.Register(v);

		std::size_t h = m_internal->variableIds.hash_function()(v);
		auto it = m_internal->variableIds.find(v, h);
		if (it == m_internal->variableIds.end())
		{
			UInt32 resultId = m_internal->nextResultId++;
			it = m_internal->variableIds.emplace(std::move(v), resultId).first;
		}

		return it.value();
	}

	void SpirvConstantCache::SetIdentifierCallback(IdentifierCallback callback)
	{
		m_identifierCallback = std::move(callback);
	}

	void SpirvConstantCache::Write(SpirvSection& annotations, SpirvSection& constants, SpirvSection& debugInfos)
	{
		for (auto&& [object, id] : m_internal->ids)
		{
			UInt32 resultId = id;

			std::visit(overloaded
			{
				[&](const AnyConstant& constant) { Write(constant, resultId, constants); },
				[&](const AnyType& type) { Write(type, resultId, annotations, constants, debugInfos); },
			}, object);
		}

		for (auto&& [variable, id] : m_internal->variableIds)
		{
			const auto& var = variable;
			UInt32 resultId = id;

			if (!variable.debugName.empty())
				debugInfos.Append(SpirvOp::OpName, resultId, variable.debugName);

			constants.AppendVariadic(SpirvOp::OpVariable, [&](const auto& appender)
			{
				appender(GetId(*var.type));
				appender(resultId);
				appender(var.storageClass);

				if (var.initializer)
					appender(GetId((*var.initializer)->constant));
			});
		}
	}

	SpirvConstantCache& SpirvConstantCache::operator=(SpirvConstantCache&& cache) noexcept = default;

	auto SpirvConstantCache::BuildConstant(const ShaderConstantValue& value) -> ConstantPtr
	{
		return std::make_shared<Constant>(std::visit([&](auto&& arg) -> SpirvConstantCache::AnyConstant
		{
			using T = std::decay_t<decltype(arg)>;

			if constexpr (std::is_same_v<T, bool>)
				return ConstantBool{ arg };
			else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, Int32> || std::is_same_v<T, UInt32>)
				return ConstantScalar{ arg };
			else if constexpr (std::is_same_v<T, Vector2f> || std::is_same_v<T, Vector2i>)
			{
				return ConstantComposite{
					BuildType(ShaderAst::VectorType{ 2, (std::is_same_v<T, Vector2f>) ? ShaderAst::PrimitiveType::Float32 : ShaderAst::PrimitiveType::Int32 }),
					{
						BuildConstant(arg.x),
						BuildConstant(arg.y)
					}
				};
			}
			else if constexpr (std::is_same_v<T, Vector3f> || std::is_same_v<T, Vector3i>)
			{
				return ConstantComposite{
					BuildType(ShaderAst::VectorType{ 3, (std::is_same_v<T, Vector3f>) ? ShaderAst::PrimitiveType::Float32 : ShaderAst::PrimitiveType::Int32 }),
					{
						BuildConstant(arg.x),
						BuildConstant(arg.y),
						BuildConstant(arg.z)
					}
				};
			}
			else if constexpr (std::is_same_v<T, Vector4f> || std::is_same_v<T, Vector4i>)
			{
				return ConstantComposite{
					BuildType(ShaderAst::VectorType{ 4, (std::is_same_v<T, Vector4f>) ? ShaderAst::PrimitiveType::Float32 : ShaderAst::PrimitiveType::Int32 }),
					{
						BuildConstant(arg.x),
						BuildConstant(arg.y),
						BuildConstant(arg.z),
						BuildConstant(arg.w)
					}
				};
			}
			else
				static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");
		}, value));
	}

	auto SpirvConstantCache::BuildFunctionType(const ShaderAst::ExpressionType& retType, const std::vector<ShaderAst::ExpressionType>& parameters) -> TypePtr
	{
		std::vector<SpirvConstantCache::TypePtr> parameterTypes;
		parameterTypes.reserve(parameters.size());

		for (const auto& parameterType : parameters)
			parameterTypes.push_back(BuildPointerType(parameterType, SpirvStorageClass::Function));

		return std::make_shared<Type>(Function{
			BuildType(retType),
			std::move(parameterTypes)
		});
	}

	auto SpirvConstantCache::BuildPointerType(const ShaderAst::ExpressionType& type, SpirvStorageClass storageClass) -> TypePtr
	{
		return std::make_shared<Type>(Pointer{
			BuildType(type),
			storageClass
		});
	}

	auto SpirvConstantCache::BuildPointerType(const ShaderAst::PrimitiveType& type, SpirvStorageClass storageClass) -> TypePtr
	{
		return std::make_shared<Type>(Pointer{
			BuildType(type),
			storageClass
		});
	}

	auto SpirvConstantCache::BuildType(const ShaderAst::ExpressionType& type) -> TypePtr
	{
		return std::visit([&](auto&& arg) -> TypePtr
		{
			return BuildType(arg);
		}, type);
	}

	auto SpirvConstantCache::BuildType(const ShaderAst::IdentifierType& type) -> TypePtr
	{
		return std::make_shared<Type>(
			Identifier{
				type.name
			}
		);
	}

	auto SpirvConstantCache::BuildType(const ShaderAst::PrimitiveType& type) -> TypePtr
	{
		return std::make_shared<Type>([&]() -> AnyType
		{
			switch (type)
			{
				case ShaderAst::PrimitiveType::Boolean:
					return Bool{};

				case ShaderAst::PrimitiveType::Float32:
					return Float{ 32 };

				case ShaderAst::PrimitiveType::Int32:
					return Integer{ 32, true };
			}

			throw std::runtime_error("unexpected type");
		}());
	}

	auto SpirvConstantCache::BuildType(const ShaderAst::MatrixType& type) -> TypePtr
	{
		return std::make_shared<Type>(
			Matrix{
				BuildType(ShaderAst::VectorType {
					UInt32(type.rowCount), type.type
				}),
				UInt32(type.columnCount)
			});
	}

	auto SpirvConstantCache::BuildType(const ShaderAst::NoType& type) -> TypePtr
	{
		return std::make_shared<Type>(Void{});
	}

	auto SpirvConstantCache::BuildType(const ShaderAst::SamplerType& type) -> TypePtr
	{
		//TODO
		auto imageType = Image{
			{}, //< qualifier
			{}, //< depth
			{}, //< sampled
			SpirvDim::Dim2D, //< dim
			SpirvImageFormat::Unknown, //< format
			BuildType(ShaderAst::PrimitiveType::Float32), //< sampledType
			false, //< arrayed,
			false  //< multisampled
		};

		return std::make_shared<Type>(SampledImage{ std::make_shared<Type>(imageType) });
	}

	auto SpirvConstantCache::BuildType(const ShaderAst::StructDescription& structDesc) -> TypePtr
	{
		Structure sType;
		sType.name = structDesc.name;

		for (const auto& member : structDesc.members)
		{
			auto& sMembers = sType.members.emplace_back();
			sMembers.name = member.name;
			sMembers.type = BuildType(member.type);
		}

		return std::make_shared<Type>(std::move(sType));
	}

	auto SpirvConstantCache::BuildType(const ShaderAst::VectorType& type) -> TypePtr
	{
		return std::make_shared<Type>(Vector{ BuildType(type.type), UInt32(type.componentCount) });
	}

	void SpirvConstantCache::Write(const AnyConstant& constant, UInt32 resultId, SpirvSection& constants)
	{
		std::visit([&](auto&& arg)
		{
			using ConstantType = std::decay_t<decltype(arg)>;

			if constexpr (std::is_same_v<ConstantType, ConstantBool>)
				constants.Append((arg.value) ? SpirvOp::OpConstantTrue : SpirvOp::OpConstantFalse, GetId({ Bool{} }), resultId);
			else if constexpr (std::is_same_v<ConstantType, ConstantComposite>)
			{
				constants.AppendVariadic(SpirvOp::OpConstantComposite, [&](const auto& appender)
				{
					appender(GetId(arg.type->type));
					appender(resultId);

					for (const auto& value : arg.values)
						appender(GetId(value->constant));
				});
			}
			else if constexpr (std::is_same_v<ConstantType, ConstantScalar>)
			{
				std::visit([&](auto&& value)
				{
					using ValueType = std::decay_t<decltype(value)>;

					UInt32 typeId;
					if constexpr (std::is_same_v<ValueType, double>)
						typeId = GetId({ Float{ 64 } });
					else if constexpr (std::is_same_v<ValueType, float>)
						typeId = GetId({ Float{ 32 } });
					else if constexpr (std::is_same_v<ValueType, Int32>)
						typeId = GetId({ Integer{ 32, true } });
					else if constexpr (std::is_same_v<ValueType, Int64>)
						typeId = GetId({ Integer{ 64, true } });
					else if constexpr (std::is_same_v<ValueType, UInt32>)
						typeId = GetId({ Integer{ 32, false } });
					else if constexpr (std::is_same_v<ValueType, UInt64>)
						typeId = GetId({ Integer{ 64, false } });
					else
						static_assert(AlwaysFalse<ValueType>::value, "non-exhaustive visitor");

					constants.Append(SpirvOp::OpConstant, typeId, resultId, SpirvSection::Raw{ &value, sizeof(value) });

				}, arg.value);
			}
			else
				static_assert(AlwaysFalse<ConstantType>::value, "non-exhaustive visitor");
		}, constant);
	}

	void SpirvConstantCache::Write(const AnyType& type, UInt32 resultId, SpirvSection& annotations, SpirvSection& constants, SpirvSection& debugInfos)
	{
		std::visit([&](auto&& arg)
		{
			using T = std::decay_t<decltype(arg)>;

			if constexpr (std::is_same_v<T, Bool>)
				constants.Append(SpirvOp::OpTypeBool, resultId);
			else if constexpr (std::is_same_v<T, Float>)
				constants.Append(SpirvOp::OpTypeFloat, resultId, arg.width);
			else if constexpr (std::is_same_v<T, Function>)
			{
				constants.AppendVariadic(SpirvOp::OpTypeFunction, [&](const auto& appender)
				{
					appender(resultId);
					appender(GetId(*arg.returnType));

					for (const auto& param : arg.parameters)
						appender(GetId(*param));
				});
			}
			else if constexpr (std::is_same_v<T, Identifier>)
				throw std::runtime_error("unexpected identifier");
			else if constexpr (std::is_same_v<T, Image>)
			{
				UInt32 depth;
				if (arg.depth.has_value())
					depth = (*arg.depth) ? 1 : 0;
				else
					depth = 2;

				UInt32 sampled;
				if (arg.sampled.has_value())
					sampled = (*arg.sampled) ? 1 : 0;
				else
					sampled = 2;

				constants.AppendVariadic(SpirvOp::OpTypeImage, [&](const auto& appender)
				{
					appender(resultId);
					appender(GetId(*arg.sampledType));
					appender(arg.dim);
					appender(depth);
					appender(arg.arrayed);
					appender(arg.multisampled);
					appender(sampled);
					appender(arg.format);

					if (arg.qualifier)
						appender(*arg.qualifier);
				});
			}
			else if constexpr (std::is_same_v<T, Integer>)
				constants.Append(SpirvOp::OpTypeInt, resultId, arg.width, arg.signedness);
			else if constexpr (std::is_same_v<T, Matrix>)
				constants.Append(SpirvOp::OpTypeMatrix, resultId, GetId(*arg.columnType), arg.columnCount);
			else if constexpr (std::is_same_v<T, Pointer>)
				constants.Append(SpirvOp::OpTypePointer, resultId, arg.storageClass, GetId(*arg.type));
			else if constexpr (std::is_same_v<T, SampledImage>)
				constants.Append(SpirvOp::OpTypeSampledImage, resultId, GetId(*arg.image));
			else if constexpr (std::is_same_v<T, Structure>)
				WriteStruct(arg, resultId, annotations, constants, debugInfos);
			else if constexpr (std::is_same_v<T, Vector>)
				constants.Append(SpirvOp::OpTypeVector, resultId, GetId(*arg.componentType), arg.componentCount);
			else if constexpr (std::is_same_v<T, Void>)
				constants.Append(SpirvOp::OpTypeVoid, resultId);
			else
				static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");
		}, type);
	}

	void SpirvConstantCache::WriteStruct(const Structure& structData, UInt32 resultId, SpirvSection& annotations, SpirvSection& constants, SpirvSection& debugInfos)
	{
		constants.AppendVariadic(SpirvOp::OpTypeStruct, [&](const auto& appender)
		{
			appender(resultId);

			for (const auto& member : structData.members)
				appender(GetId(*member.type));
		});

		debugInfos.Append(SpirvOp::OpName, resultId, structData.name);

		annotations.Append(SpirvOp::OpDecorate, resultId, SpirvDecoration::Block);

		FieldOffsets structOffsets(StructLayout_Std140);

		for (std::size_t memberIndex = 0; memberIndex < structData.members.size(); ++memberIndex)
		{
			const auto& member = structData.members[memberIndex];
			debugInfos.Append(SpirvOp::OpMemberName, resultId, memberIndex, member.name);

			std::size_t offset = std::visit([&](auto&& arg) -> std::size_t
			{
				using T = std::decay_t<decltype(arg)>;

				if constexpr (std::is_same_v<T, Bool>)
					return structOffsets.AddField(StructFieldType_Bool1);
				else if constexpr (std::is_same_v<T, Float>)
				{
					switch (arg.width)
					{
						case 32: return structOffsets.AddField(StructFieldType_Float1);
						case 64: return structOffsets.AddField(StructFieldType_Double1);
						default: throw std::runtime_error("unexpected float width " + std::to_string(arg.width));
					}
				}
				else if constexpr (std::is_same_v<T, Integer>)
					return structOffsets.AddField((arg.signedness) ? StructFieldType_Int1 : StructFieldType_UInt1);
				else if constexpr (std::is_same_v<T, Matrix>)
				{
					assert(std::holds_alternative<Vector>(arg.columnType->type));
					Vector& columnVec = std::get<Vector>(arg.columnType->type);

					if (!std::holds_alternative<Float>(columnVec.componentType->type))
						throw std::runtime_error("unexpected vector type");

					Float& vecType = std::get<Float>(columnVec.componentType->type);

					StructFieldType columnType;
					switch (vecType.width)
					{
						case 32: columnType = StructFieldType_Float1;  break;
						case 64: columnType = StructFieldType_Double1; break;
						default: throw std::runtime_error("unexpected float width " + std::to_string(vecType.width));
					}

					annotations.Append(SpirvOp::OpMemberDecorate, resultId, memberIndex, SpirvDecoration::ColMajor);
					annotations.Append(SpirvOp::OpMemberDecorate, resultId, memberIndex, SpirvDecoration::MatrixStride, 16);

					return structOffsets.AddMatrix(columnType, arg.columnCount, columnVec.componentCount, true);
				}
				else if constexpr (std::is_same_v<T, Pointer>)
					throw std::runtime_error("unhandled pointer in struct");
				else if constexpr (std::is_same_v<T, Structure>)
				{
					auto it = m_internal->structureSizes.find(arg);
					assert(it != m_internal->structureSizes.end());

					return structOffsets.AddStruct(it->second);
				}
				else if constexpr (std::is_same_v<T, Vector>)
				{
					if (std::holds_alternative<Bool>(arg.componentType->type))
						return structOffsets.AddField(static_cast<StructFieldType>(StructFieldType_Bool1 + arg.componentCount - 1));
					else if (std::holds_alternative<Float>(arg.componentType->type))
					{
						Float& floatData = std::get<Float>(arg.componentType->type);
						switch (floatData.width)
						{
							case 32: return structOffsets.AddField(static_cast<StructFieldType>(StructFieldType_Float1 + arg.componentCount - 1));
							case 64: return structOffsets.AddField(static_cast<StructFieldType>(StructFieldType_Double1 + arg.componentCount - 1));
							default: throw std::runtime_error("unexpected float width " + std::to_string(floatData.width));
						}
					}
					else if (std::holds_alternative<Integer>(arg.componentType->type))
					{
						Integer& intData = std::get<Integer>(arg.componentType->type);
						if (intData.width != 32)
							throw std::runtime_error("unexpected integer width " + std::to_string(intData.width));

						if (intData.signedness)
							return structOffsets.AddField(static_cast<StructFieldType>(StructFieldType_Int1 + arg.componentCount - 1));
						else
							return structOffsets.AddField(static_cast<StructFieldType>(StructFieldType_UInt1 + arg.componentCount - 1));
					}
					else
						throw std::runtime_error("unexpected type for vector");
				}
				else if constexpr (std::is_same_v<T, Function>)
					throw std::runtime_error("unexpected function as struct member");
				else if constexpr (std::is_same_v<T, Identifier>)
					throw std::runtime_error("unexpected identifier");
				else if constexpr (std::is_same_v<T, Image> || std::is_same_v<T, SampledImage>)
					throw std::runtime_error("unexpected opaque type as struct member");
				else if constexpr (std::is_same_v<T, Void>)
					throw std::runtime_error("unexpected void as struct member");
				else
					static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");
			}, member.type->type);

			annotations.Append(SpirvOp::OpMemberDecorate, resultId, memberIndex, SpirvDecoration::Offset, offset);
		}

		m_internal->structureSizes.emplace(structData, std::move(structOffsets));
	}
}
