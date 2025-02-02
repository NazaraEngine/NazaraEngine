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
#include <NazaraUtils/FixedVector.hpp>
#include <imgui.h>

namespace NzImGui
{
	constexpr Nz::EnumArray<Nz::Mouse::Button, int> s_mouseButtonMap = {
		ImGuiMouseButton_Left,
		ImGuiMouseButton_Right,
		ImGuiMouseButton_Middle,
		-1,
		-1
	};

	constexpr Nz::EnumArray<Nz::Keyboard::VKey, ImGuiKey> s_virtualKeyMap = {
		ImGuiKey_A, // Nz::Keyboard::VKey::A,
		ImGuiKey_B, // Nz::Keyboard::VKey::B,
		ImGuiKey_C, // Nz::Keyboard::VKey::C,
		ImGuiKey_D, // Nz::Keyboard::VKey::D,
		ImGuiKey_E, // Nz::Keyboard::VKey::E,
		ImGuiKey_F, // Nz::Keyboard::VKey::F,
		ImGuiKey_G, // Nz::Keyboard::VKey::G,
		ImGuiKey_H, // Nz::Keyboard::VKey::H,
		ImGuiKey_I, // Nz::Keyboard::VKey::I,
		ImGuiKey_J, // Nz::Keyboard::VKey::J,
		ImGuiKey_K, // Nz::Keyboard::VKey::K,
		ImGuiKey_L, // Nz::Keyboard::VKey::L,
		ImGuiKey_M, // Nz::Keyboard::VKey::M,
		ImGuiKey_N, // Nz::Keyboard::VKey::N,
		ImGuiKey_O, // Nz::Keyboard::VKey::O,
		ImGuiKey_P, // Nz::Keyboard::VKey::P,
		ImGuiKey_Q, // Nz::Keyboard::VKey::Q,
		ImGuiKey_R, // Nz::Keyboard::VKey::R,
		ImGuiKey_S, // Nz::Keyboard::VKey::S,
		ImGuiKey_T, // Nz::Keyboard::VKey::T,
		ImGuiKey_U, // Nz::Keyboard::VKey::U,
		ImGuiKey_V, // Nz::Keyboard::VKey::V,
		ImGuiKey_W, // Nz::Keyboard::VKey::W,
		ImGuiKey_X, // Nz::Keyboard::VKey::X,
		ImGuiKey_Y, // Nz::Keyboard::VKey::Y,
		ImGuiKey_Z, // Nz::Keyboard::VKey::Z,

		// Functional keys
		ImGuiKey_F1,  // Nz::Keyboard::VKey::F1,
		ImGuiKey_F2,  // Nz::Keyboard::VKey::F2,
		ImGuiKey_F3,  // Nz::Keyboard::VKey::F3,
		ImGuiKey_F4,  // Nz::Keyboard::VKey::F4,
		ImGuiKey_F5,  // Nz::Keyboard::VKey::F5,
		ImGuiKey_F6,  // Nz::Keyboard::VKey::F6,
		ImGuiKey_F7,  // Nz::Keyboard::VKey::F7,
		ImGuiKey_F8,  // Nz::Keyboard::VKey::F8,
		ImGuiKey_F9,  // Nz::Keyboard::VKey::F9,
		ImGuiKey_F10, // Nz::Keyboard::VKey::F10,
		ImGuiKey_F11, // Nz::Keyboard::VKey::F11,
		ImGuiKey_F12, // Nz::Keyboard::VKey::F12,
		ImGuiKey_F13, // Nz::Keyboard::VKey::F13,
		ImGuiKey_F14, // Nz::Keyboard::VKey::F14,
		ImGuiKey_F15, // Nz::Keyboard::VKey::F15,

		// Directional keys
		ImGuiKey_DownArrow,  // Nz::Keyboard::VKey::Down,
		ImGuiKey_LeftArrow,  // Nz::Keyboard::VKey::Left,
		ImGuiKey_RightArrow, // Nz::Keyboard::VKey::Right,
		ImGuiKey_UpArrow,    // Nz::Keyboard::VKey::Up,

		// Numerical pad
		ImGuiKey_KeypadAdd,      // Nz::Keyboard::VKey::Add,
		ImGuiKey_KeypadDecimal,  // Nz::Keyboard::VKey::Decimal,
		ImGuiKey_KeypadDivide,   // Nz::Keyboard::VKey::Divide,
		ImGuiKey_KeypadMultiply, // Nz::Keyboard::VKey::Multiply,
		ImGuiKey_KeypadEnter,    // Nz::Keyboard::VKey::NumpadReturn,
		ImGuiKey_Keypad0,        // Nz::Keyboard::VKey::Numpad0,
		ImGuiKey_Keypad1,        // Nz::Keyboard::VKey::Numpad1,
		ImGuiKey_Keypad2,        // Nz::Keyboard::VKey::Numpad2,
		ImGuiKey_Keypad3,        // Nz::Keyboard::VKey::Numpad3,
		ImGuiKey_Keypad4,        // Nz::Keyboard::VKey::Numpad4,
		ImGuiKey_Keypad5,        // Nz::Keyboard::VKey::Numpad5,
		ImGuiKey_Keypad6,        // Nz::Keyboard::VKey::Numpad6,
		ImGuiKey_Keypad7,        // Nz::Keyboard::VKey::Numpad7,
		ImGuiKey_Keypad8,        // Nz::Keyboard::VKey::Numpad8,
		ImGuiKey_Keypad9,        // Nz::Keyboard::VKey::Numpad9,
		ImGuiKey_KeypadSubtract, // Nz::Keyboard::VKey::Subtract,

		// Various
		ImGuiKey_Backslash,    // Nz::Keyboard::VKey::Backslash,
		ImGuiKey_Backspace,    // Nz::Keyboard::VKey::Backspace,
		ImGuiKey_None,         // Nz::Keyboard::VKey::Clear,
		ImGuiKey_Comma,        // Nz::Keyboard::VKey::Comma,
		ImGuiKey_Minus,        // Nz::Keyboard::VKey::Dash,
		ImGuiKey_Delete,       // Nz::Keyboard::VKey::Delete,
		ImGuiKey_End,          // Nz::Keyboard::VKey::End,
		ImGuiKey_Equal,        // Nz::Keyboard::VKey::Equal,
		ImGuiKey_Escape,       // Nz::Keyboard::VKey::Escape,
		ImGuiKey_Home,         // Nz::Keyboard::VKey::Home,
		ImGuiKey_Insert,       // Nz::Keyboard::VKey::Insert,
		ImGuiKey_LeftAlt,      // Nz::Keyboard::VKey::LAlt,
		ImGuiKey_LeftBracket,  // Nz::Keyboard::VKey::LBracket,
		ImGuiKey_LeftCtrl,     // Nz::Keyboard::VKey::LControl,
		ImGuiKey_LeftShift,    // Nz::Keyboard::VKey::LShift,
		ImGuiKey_LeftSuper,    // Nz::Keyboard::VKey::LSystem,
		ImGuiKey_0,            // Nz::Keyboard::VKey::Num0,
		ImGuiKey_1,            // Nz::Keyboard::VKey::Num1,
		ImGuiKey_2,            // Nz::Keyboard::VKey::Num2,
		ImGuiKey_3,            // Nz::Keyboard::VKey::Num3,
		ImGuiKey_4,            // Nz::Keyboard::VKey::Num4,
		ImGuiKey_5,            // Nz::Keyboard::VKey::Num5,
		ImGuiKey_6,            // Nz::Keyboard::VKey::Num6,
		ImGuiKey_7,            // Nz::Keyboard::VKey::Num7,
		ImGuiKey_8,            // Nz::Keyboard::VKey::Num8,
		ImGuiKey_9,            // Nz::Keyboard::VKey::Num9,
		ImGuiKey_PageDown,     // Nz::Keyboard::VKey::PageDown,
		ImGuiKey_PageUp,       // Nz::Keyboard::VKey::PageUp,
		ImGuiKey_Pause,        // Nz::Keyboard::VKey::Pause,
		ImGuiKey_Period,       // Nz::Keyboard::VKey::Period,
		ImGuiKey_None,         // Nz::Keyboard::VKey::Print,
		ImGuiKey_PrintScreen,  // Nz::Keyboard::VKey::PrintScreen,
		ImGuiKey_Apostrophe,   // Nz::Keyboard::VKey::Quote,
		ImGuiKey_RightAlt,     // Nz::Keyboard::VKey::RAlt,
		ImGuiKey_RightBracket, // Nz::Keyboard::VKey::RBracket,
		ImGuiKey_RightCtrl,    // Nz::Keyboard::VKey::RControl,
		ImGuiKey_Enter,        // Nz::Keyboard::VKey::Return,
		ImGuiKey_RightShift,   // Nz::Keyboard::VKey::RShift,
		ImGuiKey_RightSuper,   // Nz::Keyboard::VKey::RSystem,
		ImGuiKey_Semicolon,    // Nz::Keyboard::VKey::Semicolon,
		ImGuiKey_Slash,        // Nz::Keyboard::VKey::Slash,
		ImGuiKey_Space,        // Nz::Keyboard::VKey::Space,
		ImGuiKey_Tab,          // Nz::Keyboard::VKey::Tab,
		ImGuiKey_GraveAccent,  // Nz::Keyboard::VKey::Tilde,
		ImGuiKey_Menu,         // Nz::Keyboard::VKey::Menu,
		ImGuiKey_None,         // Nz::Keyboard::VKey::ISOBackslash102,

		// Navigator keys
		ImGuiKey_AppBack,    // Nz::Keyboard::VKey::Browser_Back,
		ImGuiKey_None,       // Nz::Keyboard::VKey::Browser_Favorites,
		ImGuiKey_AppForward, // Nz::Keyboard::VKey::Browser_Forward,
		ImGuiKey_None,       // Nz::Keyboard::VKey::Browser_Home,
		ImGuiKey_None,       // Nz::Keyboard::VKey::Browser_Refresh,
		ImGuiKey_None,       // Nz::Keyboard::VKey::Browser_Search,
		ImGuiKey_None,       // Nz::Keyboard::VKey::Browser_Stop,

		// Lecture control keys
		ImGuiKey_None, // Nz::Keyboard::VKey::Media_Next,
		ImGuiKey_None, // Nz::Keyboard::VKey::Media_Play,
		ImGuiKey_None, // Nz::Keyboard::VKey::Media_Previous,
		ImGuiKey_None, // Nz::Keyboard::VKey::Media_Stop,

		// Volume control keys
		ImGuiKey_None, // Nz::Keyboard::VKey::Volume_Down,
		ImGuiKey_None, // Nz::Keyboard::VKey::Volume_Mute,
		ImGuiKey_None, // Nz::Keyboard::VKey::Volume_Up,

		// Locking keys
		ImGuiKey_CapsLock,   // Nz::Keyboard::VKey::CapsLock,
		ImGuiKey_NumLock,    // Nz::Keyboard::VKey::NumLock,
		ImGuiKey_ScrollLock, // Nz::Keyboard::VKey::ScrollLock,
	};

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

	constexpr PushConstants PushConstantsFields = PushConstants::Build();

	constexpr Nz::UInt8 r_imguiShader[] = {
		#include <Shader.nzslb.h>
	};

	struct ImGuiPool
	{
		Nz::HybridVector<std::shared_ptr<Nz::RenderBuffer>, 4> indexBuffers;
		Nz::HybridVector<std::shared_ptr<Nz::RenderBuffer>, 4> vertexBuffers;
	};

	struct ImGuiPlatformBackend
	{
		std::string clipboardText;
		Nz::Window* window;

		NazaraSlot(Nz::WindowEventHandler, OnKeyPressed, onKeyPressed);
		NazaraSlot(Nz::WindowEventHandler, OnKeyReleased, onKeyReleased);
		NazaraSlot(Nz::WindowEventHandler, OnMouseButtonPressed, onMouseButtonPressed);
		NazaraSlot(Nz::WindowEventHandler, OnMouseButtonReleased, onMouseButtonReleased);
		NazaraSlot(Nz::WindowEventHandler, OnMouseMoved, onMouseMoved);
		NazaraSlot(Nz::WindowEventHandler, OnMouseWheelMoved, onMouseWheelMoved);
		NazaraSlot(Nz::WindowEventHandler, OnTextEntered, onTextEntered);
	};

	struct ImGuiRendererBackend
	{
		std::shared_ptr<Nz::RenderBuffer> indexBuffer;
		std::shared_ptr<Nz::RenderBuffer> vertexBuffer;
		std::shared_ptr<Nz::RenderDevice> device;
		std::shared_ptr<Nz::RenderPipeline> renderPipeline;
		std::shared_ptr<Nz::RenderPipelineLayout> renderPipelineLayout;
		std::shared_ptr<Nz::Texture> fontTexture;
		std::shared_ptr<Nz::TextureSampler> fontTextureSampler;
		std::shared_ptr<Nz::VertexDeclaration> vertexDeclaration;
		std::shared_ptr<ImGuiPool> pool;
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

							commandBufferBuilder.DrawIndexed(command.ElemCount, 1, globalIndexOffset + command.IdxOffset, globalVertexOffset + command.VtxOffset);
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

				for (const ImDrawList* commandList : drawData->CmdLists)
				{
					std::memcpy(indexCopyPtr, commandList->IdxBuffer.Data, commandList->IdxBuffer.size_in_bytes());
					std::memcpy(vertexCopyPtr, commandList->VtxBuffer.Data, commandList->VtxBuffer.size_in_bytes());

					indexCopyPtr += commandList->IdxBuffer.size();
					vertexCopyPtr += commandList->VtxBuffer.size();
				}

				ImGuiIO& io = ImGui::GetIO();
				ImGuiRendererBackend* rendererBackend = static_cast<ImGuiRendererBackend*>(io.BackendRendererUserData);

				// now that we have macro buffers, allocate them on gpu
				if (rendererBackend->indexBuffer)
				{
					renderResources.PushReleaseCallback([pool = rendererBackend->pool, indexBuffer = std::move(rendererBackend->indexBuffer)]() mutable
					{
						pool->indexBuffers.push_back(std::move(indexBuffer));
					});
					rendererBackend->indexBuffer.reset();
				}

				if (rendererBackend->vertexBuffer)
				{
					renderResources.PushReleaseCallback([pool = rendererBackend->pool, vertexBuffer = std::move(rendererBackend->vertexBuffer)]() mutable
					{
						pool->vertexBuffers.push_back(std::move(vertexBuffer));
					});
					rendererBackend->vertexBuffer.reset();
				}

				if (!rendererBackend->pool->indexBuffers.empty())
				{
					rendererBackend->indexBuffer = std::move(rendererBackend->pool->indexBuffers.back());
					rendererBackend->pool->indexBuffers.pop_back();

					Nz::UInt64 indexCount = rendererBackend->indexBuffer->GetSize() / sizeof(ImDrawIdx);
					if (totalIndexCount > indexCount)
						rendererBackend->indexBuffer.reset(); // Release buffer and force reallocation if it doesn't have enough space 
				}

				if (!rendererBackend->pool->vertexBuffers.empty())
				{
					rendererBackend->vertexBuffer = std::move(rendererBackend->pool->vertexBuffers.back());
					rendererBackend->pool->vertexBuffers.pop_back();

					Nz::UInt64 vertexCount = rendererBackend->vertexBuffer->GetSize() / sizeof(ImDrawVert);
					if (totalVertexCount > vertexCount)
						rendererBackend->vertexBuffer.reset(); // Release buffer and force reallocation if it doesn't have enough space 
				}

				if (!rendererBackend->indexBuffer)
					rendererBackend->indexBuffer = rendererBackend->device->InstantiateBuffer(Nz::BufferType::Index, totalIndexCount * sizeof(ImDrawIdx), Nz::BufferUsage::DeviceLocal | Nz::BufferUsage::Dynamic, nullptr);

				if (!rendererBackend->vertexBuffer)
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

				SetupInputs(io, backend, window.GetEventHandler());
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

			void SetupInputs(ImGuiIO& io, ImGuiPlatformBackend* platformBackend, Nz::WindowEventHandler& eventHandler)
			{
				platformBackend->onKeyPressed.Connect(eventHandler.OnKeyPressed, [&io](const Nz::WindowEventHandler* /*eventHandler*/, const Nz::WindowEvent::KeyEvent& event)
					{
						if (event.virtualKey == Nz::Keyboard::VKey::Undefined)
							return;

					ImGuiKey keyCode = s_virtualKeyMap[event.virtualKey];
					if (keyCode != ImGuiKey_None)
					{
						io.AddKeyEvent(ImGuiMod_Alt, event.alt);
						io.AddKeyEvent(ImGuiMod_Ctrl, event.control);
						io.AddKeyEvent(ImGuiMod_Shift, event.shift);
						io.AddKeyEvent(ImGuiMod_Super, event.system);
						io.AddKeyEvent(keyCode, true);
					}
				});
				
				platformBackend->onKeyReleased.Connect(eventHandler.OnKeyReleased, [&io](const Nz::WindowEventHandler* /*eventHandler*/, const Nz::WindowEvent::KeyEvent& event)
				{
					if (event.virtualKey == Nz::Keyboard::VKey::Undefined)
						return;

					ImGuiKey keyCode = s_virtualKeyMap[event.virtualKey];
					if (keyCode != ImGuiKey_None)
					{
						io.AddKeyEvent(ImGuiMod_Alt, event.alt);
						io.AddKeyEvent(ImGuiMod_Ctrl, event.control);
						io.AddKeyEvent(ImGuiMod_Shift, event.shift);
						io.AddKeyEvent(ImGuiMod_Super, event.system);
						io.AddKeyEvent(keyCode, false);
					}
				});
				
				platformBackend->onMouseButtonPressed.Connect(eventHandler.OnMouseButtonPressed, [&io](const Nz::WindowEventHandler* /*eventHandler*/, const Nz::WindowEvent::MouseButtonEvent& event)
				{
					int mouseCode = s_mouseButtonMap[event.button];
					if (mouseCode >= 0)
						io.AddMouseButtonEvent(mouseCode, true);
				});

				platformBackend->onMouseButtonReleased.Connect(eventHandler.OnMouseButtonReleased, [&io](const Nz::WindowEventHandler* /*eventHandler*/, const Nz::WindowEvent::MouseButtonEvent& event)
				{
					int mouseCode = s_mouseButtonMap[event.button];
					if (mouseCode >= 0)
						io.AddMouseButtonEvent(mouseCode, false);
				});
				
				platformBackend->onMouseMoved.Connect(eventHandler.OnMouseMoved, [&io](const Nz::WindowEventHandler* /*eventHandler*/, const Nz::WindowEvent::MouseMoveEvent& event)
				{
					io.AddMousePosEvent(event.x, event.y);
				});
				
				platformBackend->onTextEntered.Connect(eventHandler.OnTextEntered, [&io](const Nz::WindowEventHandler* /*eventHandler*/, const Nz::WindowEvent::TextEvent& event)
				{
					io.AddInputCharacter(event.character);
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
				backend->pool = std::make_shared<ImGuiPool>();
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
