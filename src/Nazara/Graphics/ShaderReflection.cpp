// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ShaderReflection.hpp>
#include <NZSL/Ast/ExpressionType.hpp>
#include <optional>
#include <stdexcept>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	void ShaderReflection::Reflect(nzsl::Ast::Module& module)
	{
		module.rootNode->Visit(*this);
	}

	void ShaderReflection::Visit(nzsl::Ast::DeclareExternalStatement& node)
	{
		ExternalBlockData* externalBlock = nullptr;
		if (!node.tag.empty())
		{
			if (m_externalBlocks.find(node.tag) != m_externalBlocks.end())
				throw std::runtime_error("duplicate tag " + node.tag);

			externalBlock = &m_externalBlocks[node.tag];
		}

		for (auto& externalVar : node.externalVars)
		{
			UInt32 bindingIndex = externalVar.bindingIndex.GetResultingValue();
			UInt32 bindingSet = externalVar.bindingSet.GetResultingValue();

			ShaderBindingType bindingType;
			const auto& varType = externalVar.type.GetResultingValue();
			if (IsStorageType(varType))
				bindingType = ShaderBindingType::StorageBuffer;
			else if (IsSamplerType(varType))
				bindingType = ShaderBindingType::Texture;
			else if (IsUniformType(varType))
				bindingType = ShaderBindingType::UniformBuffer;
			else
				throw std::runtime_error("unexpected type " + ToString(varType));

			// TODO: Get correct shader stage type

			m_pipelineLayoutInfo.bindings.push_back({
				bindingSet,               // setIndex
				bindingIndex,             // bindingIndex
				bindingType,              // type
				nzsl::ShaderStageType_All // shaderStageFlags
			});

			if (!externalVar.tag.empty() && externalBlock)
			{
				switch (bindingType)
				{
					case ShaderBindingType::StorageBuffer:
					{
						if (externalBlock->storageBlocks.find(externalVar.tag) != externalBlock->storageBlocks.end())
							throw std::runtime_error("duplicate storage buffer tag " + externalVar.tag + " in external block " + node.tag);

						ExternalStorageBlock& storageBuffer = externalBlock->storageBlocks[externalVar.tag];
						storageBuffer.bindingIndex = bindingIndex;
						storageBuffer.bindingSet   = bindingSet;
						storageBuffer.structIndex  = std::get<nzsl::Ast::StorageType>(varType).containedType.structIndex;
						break;
					}

					case ShaderBindingType::Texture:
					{
						if (externalBlock->samplers.find(externalVar.tag) != externalBlock->samplers.end())
							throw std::runtime_error("duplicate textures tag " + externalVar.tag + " in external block " + node.tag);

						const auto& samplerType = std::get<nzsl::Ast::SamplerType>(varType);

						ExternalTexture& texture = externalBlock->samplers[externalVar.tag];
						texture.bindingIndex = bindingIndex;
						texture.bindingSet   = bindingSet;
						texture.imageType    = samplerType.dim;
						texture.sampledType  = samplerType.sampledType;
						break;
					}

					case ShaderBindingType::UniformBuffer:
					{
						if (externalBlock->uniformBlocks.find(externalVar.tag) != externalBlock->uniformBlocks.end())
							throw std::runtime_error("duplicate storage buffer tag " + externalVar.tag + " in external block " + node.tag);

						ExternalUniformBlock& uniformBuffer = externalBlock->uniformBlocks[externalVar.tag];
						uniformBuffer.bindingIndex = bindingIndex;
						uniformBuffer.bindingSet = bindingSet;
						uniformBuffer.structIndex = std::get<nzsl::Ast::UniformType>(varType).containedType.structIndex;
						break;
					}
				}
			}
		}
	}

	void ShaderReflection::Visit(nzsl::Ast::DeclareStructStatement& node)
	{
		if (!node.description.layout.HasValue())
			return;

		if (node.description.layout.GetResultingValue() != nzsl::Ast::MemoryLayout::Std140)
			throw std::runtime_error("unexpected layout for struct " + node.description.name);

		if (node.description.isConditional)
			throw std::runtime_error("struct " + node.description.name + " condition must be resolved");

		StructData structData(nzsl::StructLayout::Std140);

		for (auto& member : node.description.members)
		{
			if (member.cond.HasValue() && !member.cond.IsResultingValue())
				throw std::runtime_error("unresolved member " + member.name + " in struct " + node.description.name);

			std::size_t offset = nzsl::Ast::RegisterStructField(structData.fieldOffsets, member.type.GetResultingValue(), [&](std::size_t structIndex) -> const nzsl::FieldOffsets&
			{
				auto it = m_structs.find(structIndex);
				if (it == m_structs.end())
					throw std::runtime_error("struct #" + std::to_string(structIndex) + " not found");

				return it->second.fieldOffsets;
			});
			
			if (member.tag.empty())
				continue;

			if (structData.members.find(member.tag) != structData.members.end())
				throw std::runtime_error("duplicate struct member tag " + member.tag + " in struct " + node.description.name);

			auto& structMember = structData.members[member.tag];
			structMember.offset = offset;
			structMember.type = member.type.GetResultingValue();
		}

		m_structs.emplace(node.structIndex.value(), std::move(structData));
	}
}
