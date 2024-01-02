// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_WRAPPER_AUTOFREE_HPP
#define NAZARA_VULKANRENDERER_WRAPPER_AUTOFREE_HPP

#include <NazaraUtils/Prerequisites.hpp>

namespace Nz::Vk
{
	template<typename T>
	class AutoFree
	{
		public:
			template<typename... Args> AutoFree(Args&&... args);
			AutoFree(const AutoFree&) = default;
			AutoFree(AutoFree&&) = default;
			~AutoFree();

			T& Get();
			const T& Get() const;

			T* operator->();
			const T* operator->() const;

			operator T&();
			operator const T&() const;

			AutoFree& operator=(const AutoFree&) = delete;
			AutoFree& operator=(AutoFree&&) = default;

		private:
			T m_object;
	};
}

#include <Nazara/VulkanRenderer/Wrapper/AutoFree.inl>

#endif // NAZARA_VULKANRENDERER_WRAPPER_AUTOFREE_HPP
