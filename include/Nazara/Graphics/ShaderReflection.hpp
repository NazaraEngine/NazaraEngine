// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_SHADERREFLECTION_HPP
#define NAZARA_GRAPHICS_SHADERREFLECTION_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Renderer/RenderPipelineLayout.hpp>
#include <NazaraUtils/StringHash.hpp>
#include <NZSL/Ast/Module.hpp>
#include <NZSL/Ast/Option.hpp>
#include <NZSL/Ast/RecursiveVisitor.hpp>
#include <NZSL/Math/FieldOffsets.hpp>
#include <unordered_map>

namespace Nz
{
	class NAZARA_GRAPHICS_API ShaderReflection : nzsl::Ast::RecursiveVisitor
	{
		public:
			struct ExternalBlockData;
			struct OptionData;
			struct StructData;

			ShaderReflection() = default;
			ShaderReflection(const ShaderReflection&) = delete;
			ShaderReflection(ShaderReflection&&) = delete;
			~ShaderReflection() = default;

			inline const RenderPipelineLayoutInfo& GetPipelineLayoutInfo() const&;
			inline RenderPipelineLayoutInfo&& GetPipelineLayoutInfo() &&;
			inline const ExternalBlockData* GetExternalBlockByTag(std::string_view tag) const;
			inline const OptionData* GetOptionByName(std::string_view optionName) const;
			inline const StructData* GetStructByIndex(std::size_t structIndex) const;

			void Reflect(nzsl::Ast::Module& module);

			ShaderReflection& operator=(const ShaderReflection&) = delete;
			ShaderReflection& operator=(ShaderReflection&&) = delete;

			struct ExternalData
			{
				UInt32 bindingSet;
				UInt32 bindingIndex;
			};

			struct ExternalSampler : ExternalData
			{
				UInt32 arraySize;
				nzsl::ImageType imageType;
				nzsl::Ast::PrimitiveType sampledType;
			};

			struct ExternalStorageBlock : ExternalData
			{
				std::size_t structIndex;
				bool dynamic = false;
			};

			struct ExternalTexture : ExternalData
			{
				UInt32 arraySize;
				nzsl::AccessPolicy accessPolicy;
				nzsl::ImageFormat imageFormat;
				nzsl::ImageType imageType;
				nzsl::Ast::PrimitiveType baseType;
			};

			struct ExternalUniformBlock : ExternalData
			{
				std::size_t structIndex;
				bool dynamic = false;
			};

			struct ExternalBlockData
			{
				std::unordered_map<std::string /*tag*/, ExternalSampler, StringHash<>, std::equal_to<>> samplers;
				std::unordered_map<std::string /*tag*/, ExternalStorageBlock, StringHash<>, std::equal_to<>> storageBlocks;
				std::unordered_map<std::string /*tag*/, ExternalTexture, StringHash<>, std::equal_to<>> textures;
				std::unordered_map<std::string /*tag*/, ExternalUniformBlock, StringHash<>, std::equal_to<>> uniformBlocks;
			};

			struct OptionData
			{
				nzsl::Ast::ExpressionType type;
				nzsl::Ast::OptionHash hash;
			};

			struct StructMemberData
			{
				std::size_t offset;
				std::size_t size;
				nzsl::Ast::ExpressionType type;
			};

			struct StructData
			{
				StructData(nzsl::StructLayout layout) : fieldOffsets(layout) {}

				std::unordered_map<std::string /*tag*/, StructMemberData, StringHash<>, std::equal_to<>> members;
				nzsl::FieldOffsets fieldOffsets;
			};

		private:
			void Visit(nzsl::Ast::ConditionalStatement& node) override;
			void Visit(nzsl::Ast::DeclareExternalStatement& node) override;
			void Visit(nzsl::Ast::DeclareOptionStatement& node) override;
			void Visit(nzsl::Ast::DeclareStructStatement& node) override;

			std::unordered_map<std::string /*tag*/, ExternalBlockData, StringHash<>, std::equal_to<>> m_externalBlocks;
			std::unordered_map<std::string /*name*/, OptionData, StringHash<>, std::equal_to<>> m_options;
			std::unordered_map<std::size_t /*structIndex*/, StructData> m_structs;
			RenderPipelineLayoutInfo m_pipelineLayoutInfo;
			bool m_isConditional;
	};
}

#include <Nazara/Graphics/ShaderReflection.inl>

#endif // NAZARA_GRAPHICS_SHADERREFLECTION_HPP
