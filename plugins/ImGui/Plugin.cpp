#include <Nazara/Core/Plugin.hpp>
#include <Nazara/Platform/Clipboard.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/Plugins/ImGuiPlugin.hpp>
#include <imgui.h>

namespace NzImGui
{
	struct ImGuiPlatformBackend
	{
		std::string clipboardText;
	};

	struct ImGuiRendererBackend
	{
		
	};

	class ImGuiPluginImpl final : public Nz::ImGuiPlugin
	{
		public:
			bool ActivateImpl() override
			{
				Nz::Renderer* renderer = Nz::Renderer::Instance();
				NazaraAssertMsg(renderer, "Renderer module is not instancied");

				return true;
			}

			void DeactivateImpl() override
			{
				Nz::Renderer* renderer = Nz::Renderer::Instance();
				NazaraAssertMsg(renderer, "Renderer module is not instanced");
			}

			std::string_view GetDescription() const override
			{
				return "ImGui backend using NazaraRenderer to render ImGui surfaces";
			}

			std::string_view GetName() const override
			{
				return "ImGui backend";
			}

			Nz::UInt32 GetVersion() const override
			{
				return 100;
			}

		private:
			void SetupContext(ImGuiContext* context, Nz::Window& window) override
			{
				ImGui::SetCurrentContext(context);

				ImGuiIO& io = ImGui::GetIO();
				IMGUI_CHECKVERSION();
				NazaraAssertMsg(io.BackendPlatformUserData == nullptr, "context has already been initialized");

				io.BackendPlatformName = "NazaraImGuiPlugin";
				io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
				io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

				io.BackendPlatformUserData = IM_NEW(ImGuiPlatformBackend);

				ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
				platformIO.Platform_GetClipboardTextFn = [](ImGuiContext* /*context*/) -> const char*
				{
					ImGuiPlatformBackend* backend = static_cast<ImGuiPlatformBackend*>(ImGui::GetIO().BackendPlatformUserData);
					backend->clipboardText = Nz::Clipboard::GetString();

					return backend->clipboardText.c_str();
				};

				platformIO.Platform_SetClipboardTextFn = [](ImGuiContext* /*context*/, const char* clipboardText)
				{
					ImGuiPlatformBackend* backend = static_cast<ImGuiPlatformBackend*>(ImGui::GetIO().BackendPlatformUserData);
					Nz::Clipboard::SetString(clipboardText);
				};
			}

			void SetupRenderer(ImGuiContext* context, Nz::RenderDevice& renderDevice) override
			{
				ImGui::SetCurrentContext(context);

				ImGuiIO& io = ImGui::GetIO();
				IMGUI_CHECKVERSION();
				NazaraAssertMsg(io.BackendRendererUserData == nullptr, "context has already been initialized");

				io.BackendRendererName = "NazaraImGuiPlugin";
				io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

				io.BackendRendererUserData = IM_NEW(ImGuiRendererBackend);
			}

			void ShutdownContext(ImGuiContext* context) override
			{
				ImGui::SetCurrentContext(context);

				ImGuiIO& io = ImGui::GetIO();
				IM_DELETE(static_cast<ImGuiPlatformBackend*>(io.BackendPlatformUserData));
			}

			void ShutdownRenderer(ImGuiContext* context) override
			{
				ImGui::SetCurrentContext(context);

				ImGuiIO& io = ImGui::GetIO();
				IM_DELETE(static_cast<ImGuiRendererBackend*>(io.BackendRendererUserData));
			}
	};
}

#ifdef NAZARA_PLUGINS_STATIC
namespace Nz
{
	std::unique_ptr<ImGuiPlugin> StaticPluginProvider<ImGuiPlugin>::Instantiate()
	{
		return std::make_unique<NzImGui::ImGuiPluginImpl>();
	}
}
#else
extern "C"
{
	NAZARA_EXPORT Nz::PluginInterface* PluginLoad()
	{
		Nz::Renderer* renderer = Nz::Renderer::Instance();
		if (!renderer)
		{
			NazaraError("Renderer module must be initialized");
			return nullptr;
		}

		std::unique_ptr<NzImGui::ImGuiPluginImpl> plugin = std::make_unique<NzImGui::ImGuiPluginImpl>();
		return plugin.release();
	}
}
#endif
