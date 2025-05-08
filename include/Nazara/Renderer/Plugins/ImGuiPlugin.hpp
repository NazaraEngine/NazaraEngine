// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_PLUGINS_IMGUIPLUGIN_HPP
#define NAZARA_RENDERER_PLUGINS_IMGUIPLUGIN_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/PluginInterface.hpp>
#include <Nazara/Core/Time.hpp>
#include <Nazara/Renderer/Export.hpp>

struct ImGuiContext;

namespace Nz
{
	class CommandBufferBuilder;
	class RenderDevice;
	class RenderResources;
	class Window;
	class WindowSwapchain;
	struct ImGuiFunctions;

	// Don't export class due to MinGW bug, export every method instead
	class ImGuiPlugin : public PluginInterface
	{
		public:
#ifdef NAZARA_DEBUG
			static constexpr inline std::string_view Filename = NazaraPluginPrefix "PluginImGui-d";
#else
			static constexpr inline std::string_view Filename = NazaraPluginPrefix "PluginImGui";
#endif

			ImGuiPlugin() = default;
			ImGuiPlugin(const ImGuiPlugin&) = delete;
			ImGuiPlugin(ImGuiPlugin&&) = delete;
			~ImGuiPlugin() = default;

			virtual void Draw(ImGuiContext* context, CommandBufferBuilder& commandBufferBuilder) = 0;

			virtual const ImGuiFunctions& GetFunctions() const = 0;

			virtual void Prepare(ImGuiContext* context, RenderResources& renderResources) = 0;

			virtual void NewFrame(ImGuiContext* context, Nz::Time updateTime) = 0;

			virtual void SetupContext(ImGuiContext* context, Window& window) = 0;
			virtual void SetupRenderer(ImGuiContext* context, WindowSwapchain& windowSwapchain) = 0;
			virtual void ShutdownContext(ImGuiContext* context) = 0;
			virtual void ShutdownRenderer(ImGuiContext* context) = 0;

			ImGuiPlugin& operator=(const ImGuiPlugin&) = delete;
			ImGuiPlugin& operator=(ImGuiPlugin&&) = delete;
	};

#ifdef NAZARA_PLUGINS_STATIC
	template<>
	struct StaticPluginProvider<ImGuiPlugin>
	{
		static std::unique_ptr<ImGuiPlugin> Instantiate();
	};
#endif
}

#include <Nazara/Renderer/Plugins/ImGuiPlugin.inl>

#endif // NAZARA_RENDERER_PLUGINS_IMGUIPLUGIN_HPP
