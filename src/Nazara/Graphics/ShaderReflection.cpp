// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/ShaderReflection.hpp>
#include <NZSL/Ast/ExpressionType.hpp>
#include <optional>
#include <stdexcept>

namespace Nz
{
	void ShaderReflection::Reflect(nzsl::Ast::Module& module)
	{
		m_isConditional = false;

		for (auto& importedModule : module.importedModules)
			importedModule.module->rootNode->Visit(*this);

		module.rootNode->Visit(*this);
	}

	void ShaderReflection::Visit(nzsl::Ast::ConditionalStatement& node)
	{
		bool previousConditional = m_isConditional;
		m_isConditional = true;

		RecursiveVisitor::Visit(node);

		m_isConditional = previousConditional;
	}

	void ShaderReflection::Visit(nzsl::Ast::DeclareExternalStatement& node)
	{
		ExternalBlockData* externalBlock = nullptr;
		if (!node.tag.empty())
		{
			if (m_externalBlocks.contains(node.tag))
				throw std::runtime_error("duplicate tag " + node.tag);

			externalBlock = &m_externalBlocks[node.tag];
		}

		if (m_isConditional)
			throw std::runtime_error("external block " + node.tag + " condition must be resolved");

		for (auto& externalVar : node.externalVars)
		{
			UInt32 bindingIndex = externalVar.bindingIndex.GetResultingValue();
			UInt32 bindingSet = externalVar.bindingSet.GetResultingValue();

			ShaderBindingType bindingType;
			UInt32 arraySize = 1;
			const auto* varType = &externalVar.type.GetResultingValue();

			if (IsStorageType(*varType))
				bindingType = ShaderBindingType::StorageBuffer;
			else if (IsSamplerType(*varType))
				bindingType = ShaderBindingType::Sampler;
			else if (IsTextureType(*varType))
				bindingType = ShaderBindingType::Texture;
			else if (IsUniformType(*varType))
				bindingType = ShaderBindingType::UniformBuffer;
			else if (IsArrayType(*varType))
			{
				const auto& arrayType = std::get<nzsl::Ast::ArrayType>(*varType);
				const auto& innerType = arrayType.containedType->type;
				if (!IsSamplerType(innerType))
					throw std::runtime_error("unexpected type " + nzsl::Ast::ToString(innerType) + " in array " + nzsl::Ast::ToString(arrayType));

				arraySize = arrayType.length;
				bindingType = ShaderBindingType::Sampler;
				varType = &innerType;
			}
			else
				throw std::runtime_error("unexpected type " + nzsl::Ast::ToString(varType));

			// TODO: Get more precise shader stage type
			m_pipelineLayoutInfo.bindings.push_back({
				bindingSet,               // setIndex
				bindingIndex,             // bindingIndex
				arraySize,                // arraySize
				bindingType,              // type
				nzsl::ShaderStageType_All // shaderStageFlags
			});

			if (!externalVar.tag.empty() && externalBlock)
			{
				switch (bindingType)
				{
					case ShaderBindingType::Sampler:
					{
						if (externalBlock->samplers.contains(externalVar.tag))
							throw std::runtime_error("duplicate sampler tag " + externalVar.tag + " in external block " + node.tag);

						const auto& samplerType = std::get<nzsl::Ast::SamplerType>(*varType);

						ExternalSampler& texture = externalBlock->samplers[externalVar.tag];
						texture.bindingIndex = bindingIndex;
						texture.bindingSet   = bindingSet;
						texture.imageType    = samplerType.dim;
						texture.sampledType  = samplerType.sampledType;
						break;
					}

					case ShaderBindingType::StorageBuffer:
					{
						if (externalBlock->storageBlocks.contains(externalVar.tag))
							throw std::runtime_error("duplicate storage buffer tag " + externalVar.tag + " in external block " + node.tag);

						ExternalStorageBlock& storageBuffer = externalBlock->storageBlocks[externalVar.tag];
						storageBuffer.bindingIndex = bindingIndex;
						storageBuffer.bindingSet   = bindingSet;
						storageBuffer.structIndex  = std::get<nzsl::Ast::StorageType>(*varType).containedType.structIndex;
						break;
					}

					case ShaderBindingType::Texture:
					{
						if (externalBlock->textures.contains(externalVar.tag))
							throw std::runtime_error("duplicate textures tag " + externalVar.tag + " in external block " + node.tag);

						const auto& textureType = std::get<nzsl::Ast::TextureType>(*varType);

						ExternalTexture& texture = externalBlock->textures[externalVar.tag];
						texture.bindingIndex = bindingIndex;
						texture.bindingSet = bindingSet;
						texture.accessPolicy = textureType.accessPolicy;
						texture.baseType = textureType.baseType;
						texture.imageFormat = textureType.format;
						texture.imageType = textureType.dim;
						break;
					}

					case ShaderBindingType::UniformBuffer:
					{
						if (externalBlock->uniformBlocks.contains(externalVar.tag))
							throw std::runtime_error("duplicate storage buffer tag " + externalVar.tag + " in external block " + node.tag);

						ExternalUniformBlock& uniformBuffer = externalBlock->uniformBlocks[externalVar.tag];
						uniformBuffer.bindingIndex = bindingIndex;
						uniformBuffer.bindingSet = bindingSet;
						uniformBuffer.structIndex = std::get<nzsl::Ast::UniformType>(*varType).containedType.structIndex;
						break;
					}
				}
			}
		}
	}

	void ShaderReflection::Visit(nzsl::Ast::DeclareOptionStatement& node)
	{
		if (!node.optType.IsResultingValue())
			throw std::runtime_error("option " + node.optName + " has unresolved type " + node.optName);

		if (m_isConditional)
			throw std::runtime_error("option " + node.optName + " condition must be resolved");

		OptionData& optionData = m_options[node.optName];
		optionData.hash = nzsl::Ast::HashOption(node.optName);
		optionData.type = node.optType.GetResultingValue();
	}

	void ShaderReflection::Visit(nzsl::Ast::DeclareStructStatement& node)
	{
		if (!node.description.layout.HasValue())
			return;

		if (node.description.conditionIndex != 0 || m_isConditional)
			throw std::runtime_error("struct " + node.description.name + " condition must be resolved");

		nzsl::Ast::MemoryLayout structLayout = node.description.layout.GetResultingValue();
		if (structLayout != nzsl::Ast::MemoryLayout::Std140 && structLayout != nzsl::Ast::MemoryLayout::Std430)
			throw std::runtime_error("unexpected layout for struct " + node.description.name);

		StructData structData((structLayout == nzsl::Ast::MemoryLayout::Std140) ? nzsl::StructLayout::Std140 : nzsl::StructLayout::Std430);

		for (auto& member : node.description.members)
		{
			if (member.cond.HasValue() && !member.cond.IsResultingValue())
				throw std::runtime_error("unresolved member " + member.name + " condition in struct " + node.description.name);

			if (!member.type.IsResultingValue())
				throw std::runtime_error("unresolved member " + member.name + " type in struct " + node.description.name);

			std::size_t offset = nzsl::Ast::RegisterStructField(structData.fieldOffsets, member.type.GetResultingValue(), [&](std::size_t structIndex) -> const nzsl::FieldOffsets&
			{
				auto it = m_structs.find(structIndex);
				if (it == m_structs.end())
					throw std::runtime_error("struct #" + std::to_string(structIndex) + " not found");

				return it->second.fieldOffsets;
			});

			std::size_t size = structData.fieldOffsets.GetSize() - offset;

			if (member.tag.empty())
				continue;

			if (structData.members.find(member.tag) != structData.members.end())
				throw std::runtime_error("duplicate struct member tag " + member.tag + " in struct " + node.description.name);

			auto& structMember = structData.members[member.tag];
			structMember.offset = offset;
			structMember.size = size;
			structMember.type = member.type.GetResultingValue();
		}

		m_structs.emplace(node.structIndex.value(), std::move(structData));
	}
}
