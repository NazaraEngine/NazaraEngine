#include <Nazara/Core/Plugin.hpp>
#include <Nazara/Core/VertexDeclaration.hpp>
#include <Nazara/Platform/Clipboard.hpp>
#include <Nazara/Platform/Window.hpp>
#include <Nazara/Platform/WindowEventHandler.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderPipeline.hpp>
#include <Nazara/Renderer/RenderPipelineLayout.hpp>
#include <Nazara/Renderer/WindowSwapchain.hpp>
#include <Nazara/Renderer/Plugins/ImGuiPlugin.hpp>
#include <NZSL/Math/FieldOffsets.hpp>
#include <frozen/unordered_map.h>
#include <imgui.h>
#include <span>

namespace NzImGui
{
	constexpr auto s_mouseButtonMap = frozen::make_unordered_map<Nz::Mouse::Button, int>({
		{ Nz::Mouse::Button::Left, ImGuiMouseButton_Left },
		{ Nz::Mouse::Button::Right, ImGuiMouseButton_Right },
		{ Nz::Mouse::Button::Max, ImGuiMouseButton_Middle }
	});

	struct PushConstants
	{
		nzsl::FieldOffsets fieldOffsets;

		std::size_t invHalfScreenSizeOffset;

		std::size_t totalSize;

		static constexpr PushConstants Build()
		{
			PushConstants pushConstants = { nzsl::FieldOffsets(nzsl::StructLayout::Std140) };
			pushConstants.invHalfScreenSizeOffset = pushConstants.fieldOffsets.AddField(nzsl::StructFieldType::Float2);

			pushConstants.totalSize = pushConstants.fieldOffsets.GetAlignedSize();

			return pushConstants;
		}
	};

	static constexpr PushConstants PushConstantsFields = PushConstants::Build();

	const Nz::UInt8 r_imguiShader[] = {
		#include <Shader.nzslb.h>
	};

	struct ImGuiPlatformBackend
	{
		std::string clipboardText;
		Nz::Window* window;
	};

	struct ImGuiRendererBackend
	{
		std::shared_ptr<Nz::RenderBuffer> indexBuffer;
		std::shared_ptr<Nz::RenderBuffer> vertexBuffer;
		std::shared_ptr<Nz::RenderBuffer> uniformBuffer;
		std::shared_ptr<Nz::RenderDevice> device;
		std::shared_ptr<Nz::RenderPipeline> renderPipeline;
		std::shared_ptr<Nz::RenderPipelineLayout> renderPipelineLayout;
		std::shared_ptr<Nz::Texture> fontTexture;
		std::shared_ptr<Nz::TextureSampler> fontTextureSampler;
		std::shared_ptr<Nz::VertexDeclaration> vertexDeclaration;
		std::unordered_map<unsigned int, Nz::ShaderBindingPtr> shaderBindings;
		Nz::WindowSwapchain* windowSwapchain;
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
			void Draw(ImGuiContext* context, Nz::CommandBufferBuilder& commandBufferBuilder) override
			{
				ImGui::SetCurrentContext(context);

				ImDrawData* drawData = ImGui::GetDrawData();
				if (!drawData || drawData->CmdListsCount == 0)
					return;

				// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
				int fb_width = int(drawData->DisplaySize.x * drawData->FramebufferScale.x);
				int fb_height = int(drawData->DisplaySize.y * drawData->FramebufferScale.y);
				if (fb_width <= 0 || fb_height <= 0)
					return;

				ImGuiIO& io = ImGui::GetIO();
				ImGuiRendererBackend* rendererBackend = static_cast<ImGuiRendererBackend*>(io.BackendRendererUserData);

				static_assert(sizeof(ImDrawIdx) == sizeof(Nz::UInt16) || sizeof(ImDrawIdx) == sizeof(Nz::UInt32));

				std::array<std::uint8_t, PushConstantsFields.totalSize> pushConstantData;
				Nz::AccessByOffset<Nz::Vector2f&>(&pushConstantData[0], PushConstantsFields.invHalfScreenSizeOffset) = 2.f / Nz::Vector2f(fb_width, fb_height);

				auto ResetStates = [&]
				{
					commandBufferBuilder.BindRenderPipeline(*rendererBackend->renderPipeline);
					commandBufferBuilder.BindIndexBuffer(*rendererBackend->indexBuffer, sizeof(ImDrawIdx) == sizeof(Nz::UInt16) ? Nz::IndexType::U16 : Nz::IndexType::U32);
					commandBufferBuilder.BindVertexBuffer(0, *rendererBackend->vertexBuffer);
					commandBufferBuilder.PushConstants(*rendererBackend->renderPipelineLayout, 0, PushConstantsFields.totalSize, &pushConstantData[0]);
				};

				ResetStates();

				// Will project scissor/clipping rectangles into framebuffer space
				ImVec2 clipOffset = drawData->DisplayPos;         // (0,0) unless using multi-viewports
				ImVec2 clipScale = drawData->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

				Nz::UInt32 globalIndexOffset = 0;
				Nz::UInt32 globalVertexOffset = 0;
				for (const ImDrawList* commandList : drawData->CmdLists)
				{
					for (const ImDrawCmd& command : commandList->CmdBuffer)
					{
						if (command.UserCallback)
						{
							if (command.UserCallback == ImDrawCallback_ResetRenderState)
								ResetStates();
							else
								command.UserCallback(commandList, &command);
						}
						else
						{
							// Project scissor/clipping rectangles into framebuffer space
							ImVec2 clipMin((command.ClipRect.x - clipOffset.x) * clipScale.x, (command.ClipRect.y - clipOffset.y) * clipScale.y);
							ImVec2 clipMax((command.ClipRect.z - clipOffset.x) * clipScale.x, (command.ClipRect.w - clipOffset.y) * clipScale.y);

							// Clamp to viewport as vkCmdSetScissor() won't accept values that are off bounds
							if (clipMin.x < 0.0f) { clipMin.x = 0.0f; }
							if (clipMin.y < 0.0f) { clipMin.y = 0.0f; }
							if (clipMax.x > fb_width) { clipMax.x = fb_width; }
							if (clipMax.y > fb_height) { clipMax.y = fb_height; }
							if (clipMax.x <= clipMin.x || clipMax.y <= clipMin.y)
								continue;

							commandBufferBuilder.SetScissor(Nz::Recti(Nz::Rectf::FromExtends({ clipMin.x, clipMin.y }, { clipMax.x, clipMax.y })));

							// Bind descriptor set
							unsigned int textureIndex = Nz::PointerToInteger<unsigned int>(command.GetTexID());
							if (auto it = rendererBackend->shaderBindings.find(textureIndex); it != rendererBackend->shaderBindings.end())
								commandBufferBuilder.BindRenderShaderBinding(0, *it->second);

							commandBufferBuilder.DrawIndexed(command.ElemCount, 1, command.IdxOffset, command.VtxOffset);
						}
					}
					globalIndexOffset += commandList->IdxBuffer.size();
					globalVertexOffset += commandList->VtxBuffer.size();
				}
			}

			void NewFrame(ImGuiContext* context, Nz::Time updateTime) override
			{
				ImGui::SetCurrentContext(context);

				ImGuiIO& io = ImGui::GetIO();
				IMGUI_CHECKVERSION();

				NazaraAssertMsg(io.BackendPlatformUserData != nullptr, "backend platform has not been initialized for this context");
				ImGuiPlatformBackend* platformBackend = static_cast<ImGuiPlatformBackend*>(io.BackendPlatformUserData);

				NazaraAssertMsg(io.BackendRendererUserData != nullptr, "backend renderer has not been initialized for this context");
				ImGuiRendererBackend* rendererBackend = static_cast<ImGuiRendererBackend*>(io.BackendRendererUserData);

				Nz::Vector2ui windowSize = platformBackend->window->GetSize();
				Nz::Vector2ui swapchainSize = rendererBackend->windowSwapchain->GetSize();

				io.DisplaySize = ImVec2(float(windowSize.x), float(windowSize.y));
				if (windowSize.x > 0 && windowSize.y > 0)
					io.DisplayFramebufferScale = ImVec2(float(swapchainSize.x) / windowSize.x, float(swapchainSize.y) / windowSize.y);

				io.DeltaTime = updateTime.AsSeconds();
			}

			void Prepare(ImGuiContext* context, Nz::RenderResources& renderResources) override
			{
				ImDrawData* drawData = ImGui::GetDrawData();
				if (!drawData || drawData->CmdListsCount == 0)
					return;

				std::size_t totalIndexCount = 0;
				std::size_t totalVertexCount = 0;
				for (const ImDrawList* commandList : drawData->CmdLists)
				{
					totalIndexCount += commandList->IdxBuffer.size();
					totalVertexCount += commandList->VtxBuffer.size();
				}

				auto& indexAllocation = renderResources.GetUploadPool().Allocate(totalIndexCount * sizeof(ImDrawIdx));
				auto& vertexAllocation = renderResources.GetUploadPool().Allocate(totalVertexCount * sizeof(ImDrawVert));

				ImDrawIdx* indexCopyPtr = static_cast<ImDrawIdx*>(indexAllocation.mappedPtr);
				ImDrawVert* vertexCopyPtr = static_cast<ImDrawVert*>(vertexAllocation.mappedPtr);

				std::size_t indexCount = 0;
				std::size_t vertexCount = 0;
				for (const ImDrawList* commandList : drawData->CmdLists)
				{
					std::memcpy(indexCopyPtr, commandList->IdxBuffer.Data, commandList->IdxBuffer.size_in_bytes());
					std::memcpy(vertexCopyPtr, commandList->VtxBuffer.Data, commandList->VtxBuffer.size_in_bytes());
				}

				ImGuiIO& io = ImGui::GetIO();
				ImGuiRendererBackend* rendererBackend = static_cast<ImGuiRendererBackend*>(io.BackendRendererUserData);

				// now that we have macro buffers, allocate them on gpu
				if (rendererBackend->indexBuffer)
					renderResources.PushForRelease(std::move(rendererBackend->indexBuffer));

				if (rendererBackend->vertexBuffer)
					renderResources.PushForRelease(std::move(rendererBackend->vertexBuffer));

				rendererBackend->indexBuffer = rendererBackend->device->InstantiateBuffer(Nz::BufferType::Index, totalIndexCount * sizeof(ImDrawIdx), Nz::BufferUsage::DeviceLocal | Nz::BufferUsage::Dynamic, nullptr);
				rendererBackend->vertexBuffer = rendererBackend->device->InstantiateBuffer(Nz::BufferType::Vertex, totalVertexCount * sizeof(ImDrawVert), Nz::BufferUsage::DeviceLocal | Nz::BufferUsage::Dynamic, nullptr);
				renderResources.Execute([&](Nz::CommandBufferBuilder& builder)
				{
					builder.CopyBuffer(indexAllocation, Nz::RenderBufferView(rendererBackend->indexBuffer.get()));
					builder.CopyBuffer(vertexAllocation, Nz::RenderBufferView(rendererBackend->vertexBuffer.get()));

					builder.MemoryBarrier(Nz::PipelineStage::Transfer, Nz::PipelineStage::VertexInput, Nz::MemoryAccess::TransferWrite, Nz::MemoryAccess::VertexBufferRead);
				}, Nz::QueueType::Transfer);
			}

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
				ImGuiPlatformBackend* backend = static_cast<ImGuiPlatformBackend*>(io.BackendPlatformUserData);
				backend->window = &window;

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

				SetupInputs(io, window.GetEventHandler());
			}

			void SetupFontTexture(ImGuiIO& io, ImGuiRendererBackend* rendererBackend)
			{
				unsigned char* pixels;
				int width, height;
				io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);

				Nz::TextureInfo textureInfo;
				textureInfo.pixelFormat = Nz::PixelFormat::L8;
				textureInfo.width = width;
				textureInfo.height = height;
				textureInfo.type = Nz::ImageType::E2D;

				rendererBackend->fontTexture = rendererBackend->device->InstantiateTexture(textureInfo, pixels, true);
				rendererBackend->fontTextureSampler = rendererBackend->device->InstantiateTextureSampler({});

				rendererBackend->shaderBindings[0] = rendererBackend->renderPipelineLayout->AllocateShaderBinding(0);
				rendererBackend->shaderBindings[0]->Update({
					{
						0,
						Nz::ShaderBinding::SampledTextureBinding {
							rendererBackend->fontTexture.get(),
							rendererBackend->fontTextureSampler.get()
						}
					}
				});
				io.Fonts->SetTexID(Nz::IntegerToPointer<ImTextureID>(0));
			}

			void SetupInputs(ImGuiIO& io, Nz::WindowEventHandler& eventHandler)
			{
				onMouseButtonPressed.Connect(eventHandler.OnMouseButtonPressed, [&io](const Nz::WindowEventHandler* /*eventHandler*/, const Nz::WindowEvent::MouseButtonEvent& event)
				{
					auto it = s_mouseButtonMap.find(event.button);
					if (it != s_mouseButtonMap.end())
						io.AddMouseButtonEvent(it->second, true);
				});

				onMouseButtonReleased.Connect(eventHandler.OnMouseButtonReleased, [&io](const Nz::WindowEventHandler* /*eventHandler*/, const Nz::WindowEvent::MouseButtonEvent& event)
				{
					auto it = s_mouseButtonMap.find(event.button);
					if (it != s_mouseButtonMap.end())
						io.AddMouseButtonEvent(it->second, false);
				});
				
				onMouseMoved.Connect(eventHandler.OnMouseMoved, [&io](const Nz::WindowEventHandler* /*eventHandler*/, const Nz::WindowEvent::MouseMoveEvent& event)
				{
					io.AddMousePosEvent(event.x, event.y);
				});
			}

			void SetupPipeline(ImGuiRendererBackend* rendererBackend)
			{
				rendererBackend->renderPipelineLayout = rendererBackend->device->InstantiateRenderPipelineLayout({
					.bindings = {
						{
							.setIndex = 0,
							.bindingIndex = 0,
							.type = Nz::ShaderBindingType::Sampler,
							.shaderStageFlags = nzsl::ShaderStageType::Fragment
						}
					},
					.pushConstantSize = Nz::UInt32(PushConstantsFields.totalSize)
				});

				Nz::RenderPipelineInfo renderPipelineInfo;
				renderPipelineInfo.blending = true;
				renderPipelineInfo.blend = {
					.dstAlpha = Nz::BlendFunc::InvSrcAlpha,
					.dstColor = Nz::BlendFunc::InvSrcAlpha,
					.srcAlpha = Nz::BlendFunc::One,
					.srcColor = Nz::BlendFunc::SrcAlpha
				};
				renderPipelineInfo.depthBuffer = false;
				renderPipelineInfo.frontFace = Nz::FrontFace::Clockwise;

				renderPipelineInfo.pipelineLayout = rendererBackend->renderPipelineLayout;
				renderPipelineInfo.vertexBuffers.push_back({
					.binding = 0,
					.declaration = rendererBackend->vertexDeclaration
				});
				renderPipelineInfo.shaderModules.push_back(rendererBackend->device->InstantiateShaderModule(nzsl::ShaderStageType::Fragment | nzsl::ShaderStageType::Vertex, Nz::ShaderLanguage::NazaraBinary, r_imguiShader, sizeof(r_imguiShader), { .optimize = true }));

				rendererBackend->renderPipeline = rendererBackend->device->InstantiateRenderPipeline(std::move(renderPipelineInfo));
			}

			void SetupRenderer(ImGuiContext* context, Nz::WindowSwapchain& windowSwapchain) override
			{
				ImGui::SetCurrentContext(context);

				ImGuiIO& io = ImGui::GetIO();
				IMGUI_CHECKVERSION();
				NazaraAssertMsg(io.BackendRendererUserData == nullptr, "context has already been initialized");

				io.BackendRendererName = "NazaraImGuiPlugin";

				io.BackendRendererUserData = IM_NEW(ImGuiRendererBackend);
				ImGuiRendererBackend* backend = static_cast<ImGuiRendererBackend*>(io.BackendRendererUserData);
				backend->windowSwapchain = &windowSwapchain;
				backend->device = backend->windowSwapchain->GetRenderDevice();
				if (backend->device->GetEnabledFeatures().drawBaseVertex)
					io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

				backend->vertexDeclaration = std::make_shared<Nz::VertexDeclaration>(Nz::VertexInputRate::Vertex, sizeof(ImDrawVert), std::initializer_list<Nz::VertexDeclaration::Component>{
					{
						.type = Nz::ComponentType::Float2,
							.component = Nz::VertexComponent::Position,
							.componentIndex = 0,
							.offset = offsetof(ImDrawVert, pos)
					},
					{
						.type = Nz::ComponentType::Float2,
						.component = Nz::VertexComponent::TexCoord,
						.componentIndex = 0,
						.offset = offsetof(ImDrawVert, uv)
					},
					{
						.type = Nz::ComponentType::Byte4,
						.component = Nz::VertexComponent::Color,
						.componentIndex = 0,
						.offset = offsetof(ImDrawVert, col)
					}
				});

				SetupPipeline(backend);
				SetupFontTexture(io, backend);
			}

			void ShutdownContext(ImGuiContext* context) override
			{
				ImGui::SetCurrentContext(context);

				ImGuiIO& io = ImGui::GetIO();
				IM_DELETE(static_cast<ImGuiPlatformBackend*>(io.BackendPlatformUserData));
				io.BackendPlatformUserData = nullptr;
			}

			void ShutdownRenderer(ImGuiContext* context) override
			{
				ImGui::SetCurrentContext(context);

				ImGuiIO& io = ImGui::GetIO();
				IM_DELETE(static_cast<ImGuiRendererBackend*>(io.BackendRendererUserData));
				io.BackendRendererUserData = nullptr;
			}

			NazaraSlot(Nz::WindowEventHandler, OnMouseButtonPressed,  onMouseButtonPressed);
			NazaraSlot(Nz::WindowEventHandler, OnMouseButtonReleased, onMouseButtonReleased);
			NazaraSlot(Nz::WindowEventHandler, OnMouseMoved,          onMouseMoved);
			NazaraSlot(Nz::WindowEventHandler, OnMouseWheelMoved,     onMouseWheelMoved);
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
