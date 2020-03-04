#include <Nazara/Utility.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderBuffer.hpp>
#include <Nazara/Renderer/RenderWindow.hpp>
#include <Nazara/VulkanRenderer.hpp>
#include <array>
#include <iostream>

VKAPI_ATTR VkBool32 VKAPI_CALL MyDebugReportCallback(
	VkDebugReportFlagsEXT       flags,
	VkDebugReportObjectTypeEXT  objectType,
	uint64_t                    object,
	size_t                      location,
	int32_t                     messageCode,
	const char*                 pLayerPrefix,
	const char*                 pMessage,
	void*                       pUserData)
{
	std::cerr << pMessage << std::endl;
	return VK_FALSE;
}

int main()
{
	Nz::ParameterList params;
	params.SetParameter("VkInstanceInfo_EnabledExtensionCount", 1LL);
	params.SetParameter("VkInstanceInfo_EnabledExtension0", "VK_EXT_debug_report");

	params.SetParameter("VkDeviceInfo_EnabledLayerCount", 1LL);
	params.SetParameter("VkDeviceInfo_EnabledLayer0", "VK_LAYER_LUNARG_standard_validation");
	params.SetParameter("VkInstanceInfo_EnabledLayerCount", 1LL);
	params.SetParameter("VkInstanceInfo_EnabledLayer0", "VK_LAYER_LUNARG_standard_validation");

	Nz::Renderer::SetParameters(params);

	Nz::Initializer<Nz::Renderer> loader;
	if (!loader)
	{
		std::cout << "Failed to initialize Vulkan" << std::endl;;
		return __LINE__;
	}

	Nz::VulkanRenderer* rendererImpl = static_cast<Nz::VulkanRenderer*>(Nz::Renderer::GetRendererImpl());

	Nz::Vk::Instance& instance = Nz::Vulkan::GetInstance();

	VkDebugReportCallbackCreateInfoEXT callbackCreateInfo = {};
	callbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
	callbackCreateInfo.flags = VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT ;
	callbackCreateInfo.pfnCallback = &MyDebugReportCallback;

	/* Register the callback */
	VkDebugReportCallbackEXT callback;

	instance.vkCreateDebugReportCallbackEXT(instance, &callbackCreateInfo, nullptr, &callback);


	std::vector<VkLayerProperties> layerProperties;
	if (!Nz::Vk::Loader::EnumerateInstanceLayerProperties(&layerProperties))
	{
		NazaraError("Failed to enumerate instance layer properties");
		return __LINE__;
	}

	for (const VkLayerProperties& properties : layerProperties)
	{
		std::cout << properties.layerName << ": \t" << properties.description << std::endl;
	}
	/*
	std::vector<VkExtensionProperties> extensionProperties;
	if (!Nz::Vk::Loader::EnumerateInstanceExtensionProperties(&extensionProperties))
	{
		NazaraError("Failed to enumerate instance extension properties");
		return __LINE__;
	}

	for (const VkExtensionProperties& properties : extensionProperties)
		std::cout << properties.extensionName << ": \t" << properties.specVersion << std::endl;

	std::vector<VkPhysicalDevice> devices;
	if (!instance.EnumeratePhysicalDevices(&devices))
	{
		NazaraError("Failed to enumerate physical devices");
		return __LINE__;
	}
	*/
	Nz::RenderWindow window;

	Nz::MeshParams meshParams;
	meshParams.matrix = Nz::Matrix4f::Rotate(Nz::EulerAnglesf(0.f, 90.f, 180.f));
	meshParams.vertexDeclaration = Nz::VertexDeclaration::Get(Nz::VertexLayout_XYZ_Normal);

	Nz::String windowTitle = "Vulkan Test";
	if (!window.Create(Nz::VideoMode(800, 600, 32), windowTitle))
	{
		std::cout << "Failed to create Window" << std::endl;
		return __LINE__;
	}

	std::shared_ptr<Nz::RenderDevice> device = window.GetRenderDevice();

	auto fragmentShader = device->InstantiateShaderStage(Nz::ShaderStageType::Fragment, Nz::ShaderLanguage::SpirV, "resources/shaders/triangle.frag.spv");
	if (!fragmentShader)
	{
		std::cout << "Failed to instantiate fragment shader" << std::endl;
		return __LINE__;
	}

	auto vertexShader = device->InstantiateShaderStage(Nz::ShaderStageType::Vertex, Nz::ShaderLanguage::SpirV, "resources/shaders/triangle.vert.spv");
	if (!vertexShader)
	{
		std::cout << "Failed to instantiate fragment shader" << std::endl;
		return __LINE__;
	}

	Nz::VkRenderWindow& vulkanWindow = *static_cast<Nz::VkRenderWindow*>(window.GetImpl());

	/*VkPhysicalDeviceFeatures features;
	instance.GetPhysicalDeviceFeatures(physDevice, &features);

	VkPhysicalDeviceMemoryProperties memoryProperties;
	instance.GetPhysicalDeviceMemoryProperties(physDevice, &memoryProperties);

	VkPhysicalDeviceProperties properties;
	instance.GetPhysicalDeviceProperties(physDevice, &properties);

	std::vector<VkQueueFamilyProperties> queues;
	instance.GetPhysicalDeviceQueueFamilyProperties(physDevice, &queues);*/

	Nz::VulkanDevice& vulkanDevice = vulkanWindow.GetDevice();

	Nz::MeshRef drfreak = Nz::Mesh::LoadFromFile("resources/drfreak.md2", meshParams);

	if (!drfreak)
	{
		NazaraError("Failed to load model");
		return __LINE__;
	}

	Nz::StaticMesh* drfreakMesh = static_cast<Nz::StaticMesh*>(drfreak->GetSubMesh(0));

	const Nz::VertexBuffer* drfreakVB = drfreakMesh->GetVertexBuffer();
	const Nz::IndexBuffer* drfreakIB = drfreakMesh->GetIndexBuffer();

	// Index buffer
	std::cout << "Index count: " << drfreakIB->GetIndexCount() << std::endl;

	Nz::RenderBuffer* renderBufferIB = static_cast<Nz::RenderBuffer*>(drfreakIB->GetBuffer()->GetImpl());
	if (!renderBufferIB->Synchronize(&vulkanDevice))
	{
		NazaraError("Failed to synchronize render buffer");
		return __LINE__;
	}

	Nz::VulkanBuffer* indexBufferImpl = static_cast<Nz::VulkanBuffer*>(renderBufferIB->GetHardwareBuffer(&vulkanDevice));

	// Vertex buffer
	std::cout << "Vertex count: " << drfreakVB->GetVertexCount() << std::endl;

	Nz::RenderBuffer* renderBufferVB = static_cast<Nz::RenderBuffer*>(drfreakVB->GetBuffer()->GetImpl());
	if (!renderBufferVB->Synchronize(&vulkanDevice))
	{
		NazaraError("Failed to synchronize render buffer");
		return __LINE__;
	}

	Nz::VulkanBuffer* vertexBufferImpl = static_cast<Nz::VulkanBuffer*>(renderBufferVB->GetHardwareBuffer(&vulkanDevice));

	struct
	{
		Nz::Matrix4f projectionMatrix;
		Nz::Matrix4f modelMatrix;
		Nz::Matrix4f viewMatrix;
	}
	ubo;

	Nz::Vector2ui windowSize = window.GetSize();
	ubo.projectionMatrix = Nz::Matrix4f::Perspective(70.f, float(windowSize.x) / windowSize.y, 0.1f, 1000.f);
	ubo.viewMatrix = Nz::Matrix4f::Translate(Nz::Vector3f::Backward() * 1);
	ubo.modelMatrix = Nz::Matrix4f::Translate(Nz::Vector3f::Forward() * 2 + Nz::Vector3f::Right());

	Nz::UInt32 uniformSize = sizeof(ubo);

	Nz::UniformBuffer uniformBuffer(uniformSize, Nz::DataStorage_Hardware, Nz::BufferUsage_Dynamic);
	uniformBuffer.Fill(&ubo, 0, uniformSize);

	Nz::RenderBuffer* renderBufferUB = static_cast<Nz::RenderBuffer*>(uniformBuffer.GetBuffer()->GetImpl());
	if (!renderBufferUB->Synchronize(&vulkanDevice))
	{
		NazaraError("Failed to synchronize render buffer");
		return __LINE__;
	}

	Nz::VulkanBuffer* uniformBufferImpl = static_cast<Nz::VulkanBuffer*>(renderBufferUB->GetHardwareBuffer(&vulkanDevice));

	VkDescriptorSetLayoutBinding layoutBinding = {};
	layoutBinding.binding = 0;
	layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	layoutBinding.descriptorCount = 1;
	layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	layoutBinding.pImmutableSamplers = nullptr;

	Nz::Vk::DescriptorSetLayout descriptorLayout;
	if (!descriptorLayout.Create(vulkanDevice.shared_from_this(), layoutBinding))
	{
		NazaraError("Failed to create descriptor set layout");
		return __LINE__;
	}

	VkDescriptorPoolSize poolSize;
	poolSize.descriptorCount = 1;
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

	Nz::Vk::DescriptorPool descriptorPool;
	if (!descriptorPool.Create(vulkanDevice.shared_from_this(), 1, poolSize, VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT))
	{
		NazaraError("Failed to create descriptor pool");
		return __LINE__;
	}

	Nz::Vk::DescriptorSet descriptorSet = descriptorPool.AllocateDescriptorSet(descriptorLayout);

	descriptorSet.WriteUniformDescriptor(0, uniformBufferImpl->GetBufferHandle(), 0, uniformSize);

	Nz::RenderPipelineInfo pipelineInfo;
	pipelineInfo.depthBuffer = true;
	pipelineInfo.shaderStages.emplace_back(fragmentShader);
	pipelineInfo.shaderStages.emplace_back(vertexShader);

	auto& vertexBuffer = pipelineInfo.vertexBuffers.emplace_back();
	vertexBuffer.binding = 0;
	vertexBuffer.declaration = drfreakVB->GetVertexDeclaration();


	//std::unique_ptr<Nz::RenderPipeline> pipeline = device->InstantiateRenderPipeline(pipelineInfo);

	VkDescriptorSetLayout descriptorSetLayout = descriptorLayout;

	VkPipelineLayoutCreateInfo layout_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,  // VkStructureType                sType
		nullptr,                                        // const void                    *pNext
		0,                                              // VkPipelineLayoutCreateFlags    flags
		1U,                                              // uint32_t                       setLayoutCount
		&descriptorSetLayout,                                        // const VkDescriptorSetLayout   *pSetLayouts
		0,                                              // uint32_t                       pushConstantRangeCount
		nullptr                                         // const VkPushConstantRange     *pPushConstantRanges
	};

	Nz::Vk::PipelineLayout pipelineLayout;
	pipelineLayout.Create(vulkanDevice.shared_from_this(), layout_create_info);

	Nz::VulkanRenderPipeline::CreateInfo pipelineCreateInfo = Nz::VulkanRenderPipeline::BuildCreateInfo(pipelineInfo);
	pipelineCreateInfo.pipelineInfo.layout = pipelineLayout;
	pipelineCreateInfo.pipelineInfo.renderPass = vulkanWindow.GetRenderPass();

	Nz::Vk::Pipeline vkPipeline;
	if (!vkPipeline.CreateGraphics(vulkanDevice.shared_from_this(), pipelineCreateInfo.pipelineInfo))
	{
		NazaraError("Failed to create pipeline");
		return __LINE__;
	}

	Nz::Vk::CommandPool cmdPool;
	if (!cmdPool.Create(vulkanDevice.shared_from_this(), 0, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT))
	{
		NazaraError("Failed to create rendering cmd pool");
		return __LINE__;
	}

	std::array<VkClearValue, 2> clearValues;
	clearValues[0].color = {1.0f, 0.8f, 0.4f, 0.0f};
	clearValues[1].depthStencil = {1.f, 0};

	Nz::Vk::Queue graphicsQueue(vulkanDevice.shared_from_this(), vulkanDevice.GetEnabledQueues()[0].queues[0].queue);

	Nz::UInt32 imageCount = vulkanWindow.GetFramebufferCount();
	std::vector<Nz::Vk::CommandBuffer> renderCmds = cmdPool.AllocateCommandBuffers(imageCount, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	std::vector<Nz::Vk::Fence> fences(imageCount);
	for (auto& fence : fences)
		fence.Create(vulkanDevice.shared_from_this());

	for (Nz::UInt32 i = 0; i < imageCount; ++i)
	{
		Nz::Vk::CommandBuffer& renderCmd = renderCmds[i];

		VkRect2D renderArea = {
			{                                           // VkOffset2D                     offset
				0,                                          // int32_t                        x
				0                                           // int32_t                        y
			},
			{                                           // VkExtent2D                     extent
				windowSize.x,                                        // int32_t                        width
				windowSize.y,                                        // int32_t                        height
			}
		};

		VkRenderPassBeginInfo render_pass_begin_info = {
			VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,     // VkStructureType                sType
			nullptr,                                      // const void                    *pNext
			vulkanWindow.GetRenderPass(),                            // VkRenderPass                   renderPass
			vulkanWindow.GetFrameBuffer(i),                       // VkFramebuffer                  framebuffer
			renderArea,
			2U,                                            // uint32_t                       clearValueCount
			clearValues.data()                                  // const VkClearValue            *pClearValues
		};

		VkClearAttachment clearAttachment = {
			VK_IMAGE_ASPECT_COLOR_BIT,
			0U,
			clearValues[0]
		};

		VkClearAttachment clearAttachmentDepth = {
			VK_IMAGE_ASPECT_DEPTH_BIT,
			0U,
			clearValues[1]
		};

		VkClearRect clearRect = {
			renderArea,
			0U,
			1U
		};

		renderCmd.Begin();

		vulkanWindow.BuildPreRenderCommands(i, renderCmd);

		renderCmd.BeginRenderPass(render_pass_begin_info);
		//renderCmd.ClearAttachment(clearAttachment, clearRect);
		//renderCmd.ClearAttachment(clearAttachmentDepth, clearRect);
		renderCmd.BindIndexBuffer(indexBufferImpl->GetBufferHandle(), 0, VK_INDEX_TYPE_UINT16);
		renderCmd.BindVertexBuffer(0, vertexBufferImpl->GetBufferHandle(), 0);
		renderCmd.BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, descriptorSet);
		renderCmd.BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline);
		renderCmd.SetScissor(Nz::Recti{0, 0, int(windowSize.x), int(windowSize.y)});
		renderCmd.SetViewport({0.f, 0.f, float(windowSize.x), float(windowSize.y)}, 0.f, 1.f);
		renderCmd.DrawIndexed(drfreakIB->GetIndexCount());
		renderCmd.EndRenderPass();

		vulkanWindow.BuildPostRenderCommands(i, renderCmd);

		if (!renderCmd.End())
		{
			NazaraError("Failed to specify render cmd");
			return __LINE__;
		}
	}

	Nz::Vector3f viewerPos = Nz::Vector3f::Zero();

	Nz::EulerAnglesf camAngles(0.f, 0.f, 0.f);
	Nz::Quaternionf camQuat(camAngles);

	window.EnableEventPolling(true);

	Nz::Clock updateClock;
	Nz::Clock secondClock;
	unsigned int fps = 0;
	while (window.IsOpen())
	{
		bool updateUniforms = false;

		Nz::WindowEvent event;
		while (window.PollEvent(&event))
		{
			switch (event.type)
			{
				case Nz::WindowEventType_Quit:
					window.Close();
					break;

				case Nz::WindowEventType_MouseMoved: // La souris a bougé
				{
					// Gestion de la caméra free-fly (Rotation)
					float sensitivity = 0.3f; // Sensibilité de la souris

					// On modifie l'angle de la caméra grâce au déplacement relatif sur X de la souris
					camAngles.yaw = Nz::NormalizeAngle(camAngles.yaw - event.mouseMove.deltaX*sensitivity);

					// Idem, mais pour éviter les problèmes de calcul de la matrice de vue, on restreint les angles
					camAngles.pitch = Nz::Clamp(camAngles.pitch + event.mouseMove.deltaY*sensitivity, -89.f, 89.f);

					camQuat = camAngles;

					// Pour éviter que le curseur ne sorte de l'écran, nous le renvoyons au centre de la fenêtre
					// Cette fonction est codée de sorte à ne pas provoquer d'évènement MouseMoved
					Nz::Mouse::SetPosition(windowSize.x / 2, windowSize.y / 2, window);
					updateUniforms = true;
					break;
				}
			}
		}

		if (updateClock.GetMilliseconds() > 1000 / 60)
		{
			float elapsedTime = updateClock.GetSeconds();
			updateClock.Restart();

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Up))
			{
				viewerPos += camQuat * Nz::Vector3f::Forward() * elapsedTime;
				updateUniforms = true;
			}

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Down))
			{
				viewerPos += camQuat * Nz::Vector3f::Backward() * elapsedTime;
				updateUniforms = true;
			}
		}

		if (updateUniforms)
		{
			ubo.viewMatrix = Nz::Matrix4f::ViewMatrix(viewerPos, camAngles);

			uniformBuffer.Fill(&ubo, 0, uniformSize);

			Nz::RenderBuffer* renderBufferUB = static_cast<Nz::RenderBuffer*>(uniformBuffer.GetBuffer()->GetImpl());
			if (!renderBufferUB->Synchronize(&vulkanDevice))
			{
				NazaraError("Failed to synchronize render buffer");
				return __LINE__;
			}
		}

		Nz::UInt32 imageIndex;
		if (!vulkanWindow.Acquire(&imageIndex))
		{
			std::cout << "Failed to acquire next image" << std::endl;
			return EXIT_FAILURE;
		}

		fences[imageIndex].Wait();
		fences[imageIndex].Reset();

		VkCommandBuffer renderCmdBuffer = renderCmds[imageIndex];
		VkSemaphore waitSemaphore = vulkanWindow.GetRenderSemaphore();

		VkPipelineStageFlags wait_dst_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		VkSubmitInfo submit_info = {
			VK_STRUCTURE_TYPE_SUBMIT_INFO,                // VkStructureType              sType
			nullptr,                                      // const void                  *pNext
			1U,                                            // uint32_t                     waitSemaphoreCount
			&waitSemaphore,              // const VkSemaphore           *pWaitSemaphores
			&wait_dst_stage_mask,                         // const VkPipelineStageFlags  *pWaitDstStageMask;
			1,                                            // uint32_t                     commandBufferCount
			&renderCmdBuffer,  // const VkCommandBuffer       *pCommandBuffers
			0,                                            // uint32_t                     signalSemaphoreCount
			nullptr            // const VkSemaphore           *pSignalSemaphores
		};

		if (!graphicsQueue.Submit(submit_info, fences[imageIndex]))
			return false;

		vulkanWindow.Present(imageIndex);

		// On incrémente le compteur de FPS improvisé
		fps++;

		if (secondClock.GetMilliseconds() >= 1000) // Toutes les secondes
		{
			// Et on insère ces données dans le titre de la fenêtre
			window.SetTitle(windowTitle + " - " + Nz::String::Number(fps) + " FPS");

			/*
			Note: En C++11 il est possible d'insérer de l'Unicode de façon standard, quel que soit l'encodage du fichier,
			via quelque chose de similaire à u8"Cha\u00CEne de caract\u00E8res".
			Cependant, si le code source est encodé en UTF-8 (Comme c'est le cas dans ce fichier),
			cela fonctionnera aussi comme ceci : "Chaîne de caractères".
			*/

			// Et on réinitialise le compteur de FPS
			fps = 0;

			// Et on relance l'horloge pour refaire ça dans une seconde
			secondClock.Restart();
		}
	}

	instance.vkDestroyDebugReportCallbackEXT(instance, callback, nullptr);

	return EXIT_SUCCESS;
}
