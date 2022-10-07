// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_SHADERREFLECTION_HPP
#define NAZARA_GRAPHICS_SHADERREFLECTION_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Renderer/RenderPipelineLayout.hpp>
#include <NZSL/Ast/Module.hpp>
#include <NZSL/Ast/RecursiveVisitor.hpp>
#include <NZSL/Math/FieldOffsets.hpp>
#include <unordered_map>

namespace Nz
{
	class NAZARA_GRAPHICS_API ShaderReflection : nzsl::Ast::RecursiveVisitor
	{
		public:
			struct ExternalBlockData;
			struct StructData;

			ShaderReflection(nzsl::Ast::Module& module);
			ShaderReflection(const ShaderReflection&) = delete;
			ShaderReflection(ShaderReflection&&) = delete;
			~ShaderReflection() = default;

			inline const RenderPipelineLayoutInfo& GetPipelineLayoutInfo() const;
			inline const ExternalBlockData* GetExternalBlockByTag(const std::string& tag) const;
			inline const StructData* GetStructByIndex(std::size_t structIndex) const;

			ShaderReflection& operator=(const ShaderReflection&) = delete;
			ShaderReflection& operator=(ShaderReflection&&) = delete;

			struct ExternalData
			{
				UInt32 bindingSet;
				UInt32 bindingIndex;
			};

			struct ExternalStorageBuffer : ExternalData
			{
				std::size_t structIndex;
			};

			struct ExternalTexture : ExternalData
			{
				nzsl::ImageType imageType;
				nzsl::Ast::PrimitiveType sampledType;
			};

			struct ExternalUniformBuffer : ExternalData
			{
				std::size_t structIndex;
			};

			struct ExternalBlockData
			{
				std::unordered_map<std::string /*tag*/, ExternalStorageBuffer> storageBuffers;
				std::unordered_map<std::string /*tag*/, ExternalTexture> textures;
				std::unordered_map<std::string /*tag*/, ExternalUniformBuffer> uniformBuffers;
			};

			struct StructMemberData
			{
				std::size_t offset;
				nzsl::Ast::ExpressionType type;
			};

			struct StructData
			{
				StructData(nzsl::StructLayout layout) : fieldOffsets(layout) {}

				std::unordered_map<std::string /*tag*/, StructMemberData> members;
				nzsl::FieldOffsets fieldOffsets;
			};

		private:
			void Visit(nzsl::Ast::DeclareExternalStatement& node) override;
			void Visit(nzsl::Ast::DeclareStructStatement& node) override;

			std::unordered_map<std::string /*tag*/, ExternalBlockData> m_externalBlocks;
			std::unordered_map<std::size_t /*structIndex*/, StructData> m_structs;
			RenderPipelineLayoutInfo m_pipelineLayoutInfo;
	};
}

#include <Nazara/Graphics/ShaderReflection.inl>

#endif // NAZARA_GRAPHICS_SHADERREFLECTION_HPP
