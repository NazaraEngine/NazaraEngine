// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_FRAMEPIPELINEPASSREGISTRY_HPP
#define NAZARA_GRAPHICS_FRAMEPIPELINEPASSREGISTRY_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/FramePipelinePass.hpp>
#include <NazaraUtils/StringHash.hpp>
#include <functional>
#include <list>
#include <string>
#include <unordered_map>
#include <vector>

namespace Nz
{
	class ParameterList;

	class FramePipelinePassRegistry
	{
		public:
			using Factory = std::function<std::unique_ptr<FramePipelinePass>(FramePipelinePass::PassData& passData, std::string passName, const ParameterList& parameters)>;

			FramePipelinePassRegistry() = default;
			FramePipelinePassRegistry(const FramePipelinePassRegistry&) = default;
			FramePipelinePassRegistry(FramePipelinePassRegistry&&) = default;
			~FramePipelinePassRegistry() = default;

			inline std::unique_ptr<FramePipelinePass> BuildPass(std::size_t passIndex, FramePipelinePass::PassData& passData, std::string passName, const ParameterList& parameters) const;

			inline std::size_t GetPassIndex(std::string_view passName) const;
			inline std::size_t GetPassInputIndex(std::size_t passIndex, std::string_view inputName) const;
			inline std::size_t GetPassOutputIndex(std::size_t passIndex, std::string_view inputName) const;

			template<typename T, typename... Args> std::size_t RegisterPass(std::string passName, std::vector<std::string> inputs, std::vector<std::string> outputs, Args&&... args);
			inline std::size_t RegisterPass(std::string passName, std::vector<std::string> inputs, std::vector<std::string> outputs, Factory factory);

			FramePipelinePassRegistry& operator=(const FramePipelinePassRegistry&) = default;
			FramePipelinePassRegistry& operator=(FramePipelinePassRegistry&&) = default;

			static constexpr std::size_t InvalidIndex = std::numeric_limits<std::size_t>::max();

		private:
			struct Pass
			{
				Factory factory;
				std::vector<std::string> inputs;
				std::vector<std::string> outputs;
			};

			std::unordered_map<std::string, std::size_t, StringHash<>, std::equal_to<>> m_passIndex;
			std::vector<Pass> m_passes;
	};
}

#include <Nazara/Graphics/FramePipelinePassRegistry.inl>

#endif // NAZARA_GRAPHICS_FRAMEPIPELINEPASSREGISTRY_HPP
