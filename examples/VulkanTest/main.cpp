#include <Nazara/Utility.hpp>
#include <Nazara/Renderer/Renderer.hpp>
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

	VkDebugReportCallbackCreateInfoEXT callbackCreateInfo;
	callbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
	callbackCreateInfo.pNext = nullptr;
	callbackCreateInfo.flags = VK_DEBUG_REPORT_FLAG_BITS_MAX_ENUM_EXT & ~VK_DEBUG_REPORT_INFORMATION_BIT_EXT;
	callbackCreateInfo.pfnCallback = &MyDebugReportCallback;
	callbackCreateInfo.pUserData = nullptr;

	/* Register the callback */
	VkDebugReportCallbackEXT callback;

	instance.vkCreateDebugReportCallbackEXT(instance, &callbackCreateInfo, nullptr, &callback);

	Nz::File shaderFile;
	std::vector<Nz::UInt8> vertexShaderCode;
	std::vector<Nz::UInt8> fragmentShaderCode;

	if (!shaderFile.Open("resources/shaders/triangle.vert.spv", Nz::OpenMode_ReadOnly))
	{
		NazaraError("Failed to open vertex shader code");
		return __LINE__;
	}

	vertexShaderCode.resize(shaderFile.GetSize());
	shaderFile.Read(vertexShaderCode.data(), vertexShaderCode.size());

	if (!shaderFile.Open("resources/shaders/triangle.frag.spv", Nz::OpenMode_ReadOnly))
	{
		NazaraError("Failed to open fragment shader code");
		return __LINE__;
	}

	fragmentShaderCode.resize(shaderFile.GetSize());
	shaderFile.Read(fragmentShaderCode.data(), fragmentShaderCode.size());

	shaderFile.Close();

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

	Nz::Mesh drfreak;
	if (!drfreak.LoadFromFile("resources/drfreak.md2", meshParams))
	{
		NazaraError("Failed to load Dr. Freak");
		return __LINE__;
	}

	Nz::String windowTitle = "Vulkan Test";
	if (!window.Create(Nz::VideoMode(800, 600, 32), windowTitle))
	{
		std::cout << "Failed to create Window" << std::endl;
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

	Nz::Vk::DeviceHandle device = vulkanWindow.GetDevice();

	Nz::Vk::ShaderModule vertexShader;
	if (!vertexShader.Create(device, reinterpret_cast<Nz::UInt32*>(vertexShaderCode.data()), vertexShaderCode.size()))
	{
		NazaraError("Failed to create vertex shader");
		return __LINE__;
	}

	Nz::Vk::ShaderModule fragmentShader;
	if (!fragmentShader.Create(device, reinterpret_cast<Nz::UInt32*>(fragmentShaderCode.data()), fragmentShaderCode.size()))
	{
		NazaraError("Failed to create fragment shader");
		return __LINE__;
	}

	VkMemoryRequirements memRequirement;

	Nz::StaticMesh* drfreakMesh = static_cast<Nz::StaticMesh*>(drfreak.GetSubMesh(0));

	const Nz::VertexBuffer* drfreakVB = drfreakMesh->GetVertexBuffer();
	const Nz::IndexBuffer* drfreakIB = drfreakMesh->GetIndexBuffer();

	// Vertex buffer
	struct Vertex {
		Nz::Vector4f pos;
		Nz::Vector3f col;
	};

	/*std::vector<Vertex> vertexBufferData = {
		{{-1.f,  1.f, 0.0f},  {1.0f, 0.0f, 0.0f}},
		{{1.f,  1.f, 0.0f}, {0.0f, 1.0f, 0.0f}},
		{{0.0f, -1.f, 0.0f},  {0.0f, 0.0f, 1.0f}}
	};

	Nz::Matrix4f projection = Nz::Matrix4f::Perspective(70.f, float(window.GetWidth()) / window.GetHeight(), 1.f, 1000.f);
	Nz::Matrix4f world = Nz::Matrix4f::Translate(Nz::Vector3f::Forward() * 5.f);

	for (unsigned int i = 0; i < 3; ++i)
	{
		Nz::Vector4f pos = vertexBufferData[i].pos;
		vertexBufferData[i].pos = projection * (world * pos);
	}*/

	Nz::BufferMapper<Nz::VertexBuffer> vertexMapper(drfreakVB, Nz::BufferAccess_ReadOnly);
	Nz::MeshVertex* meshVertices = static_cast<Nz::MeshVertex*>(vertexMapper.GetPointer());

	std::size_t vertexCount = drfreakVB->GetVertexCount();

	Nz::Image meshImage;
	if (!meshImage.LoadFromFile("resources/drfreak.tga"))
	{
		NazaraError("Failed to load texture");
		return __LINE__;
	}

	std::vector<Vertex> vertexBufferData;
	vertexBufferData.reserve(vertexCount);
	for (std::size_t i = 0; i < vertexCount; ++i)
	{
		std::size_t texX = meshVertices[i].uv.x * meshImage.GetWidth();
		std::size_t texY = meshVertices[i].uv.y * meshImage.GetHeight();

		Nz::Color c = meshImage.GetPixelColor(texX, texY);

		Vertex vertex = {
			meshVertices[i].position,
			{c.r / 255.f, c.g / 255.f, c.b / 255.f}
		};

		vertexBufferData.push_back(vertex);
	}

	Nz::UInt32 vertexBufferSize = static_cast<Nz::UInt32>(vertexBufferData.size() * sizeof(Vertex));

	Nz::Vk::Buffer vertexBuffer;
	if (!vertexBuffer.Create(device, 0, vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT))
	{
		NazaraError("Failed to create vertex buffer");
		return __LINE__;
	}

	memRequirement = vertexBuffer.GetMemoryRequirements();

	Nz::Vk::DeviceMemory vertexBufferMemory;
	if (!vertexBufferMemory.Create(device, memRequirement.size, memRequirement.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
	{
		NazaraError("Failed to allocate vertex buffer memory");
		return __LINE__;
	}

	if (!vertexBufferMemory.Map(0, vertexBufferSize))
	{
		NazaraError("Failed to map vertex buffer");
		return __LINE__;
	}

	std::memcpy(vertexBufferMemory.GetMappedPointer(), vertexBufferData.data(), vertexBufferSize);

	vertexBufferMemory.Unmap();

	if (!vertexBuffer.BindBufferMemory(vertexBufferMemory))
	{
		NazaraError("Failed to bind vertex buffer to its memory");
		return __LINE__;
	}

	// Index buffer
	Nz::IndexMapper indexMapper(drfreakIB);

	std::size_t indexCount = indexMapper.GetIndexCount();
	std::vector<Nz::UInt32> indexBufferData;
	indexBufferData.reserve(indexCount);

	for (std::size_t i = 0; i < indexCount; ++i)
	{
		indexBufferData.push_back(indexMapper.Get(i));
	}

	Nz::UInt32 indexBufferSize = indexBufferData.size() * sizeof(Nz::UInt32);

	Nz::Vk::Buffer indexBuffer;
	if (!indexBuffer.Create(device, 0, indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT))
	{
		NazaraError("Failed to create vertex buffer");
		return __LINE__;
	}

	memRequirement = indexBuffer.GetMemoryRequirements();

	Nz::Vk::DeviceMemory indexBufferMemory;
	if (!indexBufferMemory.Create(device, memRequirement.size, memRequirement.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
	{
		NazaraError("Failed to allocate vertex buffer memory");
		return __LINE__;
	}

	if (!indexBufferMemory.Map(0, indexBufferSize))
	{
		NazaraError("Failed to map vertex buffer");
		return __LINE__;
	}

	std::memcpy(indexBufferMemory.GetMappedPointer(), indexBufferData.data(), indexBufferSize);

	indexBufferMemory.Unmap();

	if (!indexBuffer.BindBufferMemory(indexBufferMemory))
	{
		NazaraError("Failed to bind vertex buffer to its memory");
		return __LINE__;
	}

	struct
	{
		Nz::Matrix4f projectionMatrix;
		Nz::Matrix4f modelMatrix;
		Nz::Matrix4f viewMatrix;
	}
	ubo;

	ubo.projectionMatrix = Nz::Matrix4f::Perspective(70.f, float(window.GetWidth()) / window.GetHeight(), 0.1f, 1000.f);
	ubo.viewMatrix = Nz::Matrix4f::Translate(Nz::Vector3f::Backward() * 1);
	ubo.modelMatrix = Nz::Matrix4f::Translate(Nz::Vector3f::Forward() * 2 + Nz::Vector3f::Right());

	Nz::UInt32 uniformSize = sizeof(ubo);

	Nz::Vk::Buffer uniformBuffer;
	if (!uniformBuffer.Create(device, 0, uniformSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT))
	{
		NazaraError("Failed to create vertex buffer");
		return __LINE__;
	}

	memRequirement = uniformBuffer.GetMemoryRequirements();

	Nz::Vk::DeviceMemory uniformBufferMemory;
	if (!uniformBufferMemory.Create(device, memRequirement.size, memRequirement.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
	{
		NazaraError("Failed to allocate vertex buffer memory");
		return __LINE__;
	}

	if (!uniformBufferMemory.Map(0, uniformSize))
	{
		NazaraError("Failed to map vertex buffer");
		return __LINE__;
	}

	std::memcpy(uniformBufferMemory.GetMappedPointer(), &ubo, uniformSize);

	uniformBufferMemory.Unmap();

	if (!uniformBuffer.BindBufferMemory(uniformBufferMemory))
	{
		NazaraError("Failed to bind uniform buffer to its memory");
		return __LINE__;
	}


	VkDescriptorSetLayoutBinding layoutBinding = {};
	layoutBinding.binding = 0;
	layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	layoutBinding.descriptorCount = 1;
	layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	layoutBinding.pImmutableSamplers = nullptr;

	Nz::Vk::DescriptorSetLayout descriptorLayout;
	if (!descriptorLayout.Create(device, layoutBinding))
	{
		NazaraError("Failed to create descriptor set layout");
		return __LINE__;
	}

	VkDescriptorPoolSize poolSize;
	poolSize.descriptorCount = 1;
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

	Nz::Vk::DescriptorPool descriptorPool;
	if (!descriptorPool.Create(device, 1, poolSize, VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT))
	{
		NazaraError("Failed to create descriptor pool");
		return __LINE__;
	}

	Nz::Vk::DescriptorSet descriptorSet = descriptorPool.AllocateDescriptorSet(descriptorLayout);

	descriptorSet.WriteUniformDescriptor(0, uniformBuffer, 0, uniformSize);

	std::array<VkPipelineShaderStageCreateInfo, 2> shaderStageCreateInfo = {
		{
			{
				VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				nullptr,
				0,
				VK_SHADER_STAGE_VERTEX_BIT,
				vertexShader,
				"main",
				nullptr
			},
			{
				VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				nullptr,
				0,
				VK_SHADER_STAGE_FRAGMENT_BIT,
				fragmentShader,
				"main",
				nullptr
			}
		}
	};

	VkVertexInputBindingDescription bindingDescription = {
		0,
		sizeof(Vertex),
		VK_VERTEX_INPUT_RATE_VERTEX
	};

	std::array<VkVertexInputAttributeDescription, 2> attributeDescription = 
	{
		{
			{
				0,                          // uint32_t    location
				0,                          // uint32_t    binding;
				VK_FORMAT_R32G32B32A32_SFLOAT, // VkFormat    format;
				0                           // uint32_t    offset;
			},
			{
				1,                          // uint32_t    location
				0,                          // uint32_t    binding;
				VK_FORMAT_R32G32B32_SFLOAT, // VkFormat    format;
				sizeof(float) * 4           // uint32_t    offset;
			}
		}
	};

	VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,    // VkStructureType                                sType
		nullptr,                                                      // const void                                    *pNext
		0,                                                            // VkPipelineVertexInputStateCreateFlags          flags;
		1,                                                            // uint32_t                                       vertexBindingDescriptionCount
		&bindingDescription,                                          // const VkVertexInputBindingDescription         *pVertexBindingDescriptions
		Nz::UInt32(attributeDescription.size()),                      // uint32_t                                       vertexAttributeDescriptionCount
		attributeDescription.data()                                   // const VkVertexInputAttributeDescription       *pVertexAttributeDescriptions
	};

	VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,  // VkStructureType                                sType
		nullptr,                                                      // const void                                    *pNext
		0,                                                            // VkPipelineInputAssemblyStateCreateFlags        flags
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,                          // VkPrimitiveTopology                            topology
		VK_FALSE                                                      // VkBool32                                       primitiveRestartEnable
	};

	VkPipelineViewportStateCreateInfo viewport_state_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,        // VkStructureType                                sType
		nullptr,                                                      // const void                                    *pNext
		0,                                                            // VkPipelineViewportStateCreateFlags             flags
		1,                                                            // uint32_t                                       viewportCount
		nullptr,                                                    // const VkViewport                              *pViewports
		1,                                                            // uint32_t                                       scissorCount
		nullptr                                                      // const VkRect2D                                *pScissors
	};

	VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,   // VkStructureType                                sType
		nullptr,                                                      // const void                                    *pNext
		0,                                                            // VkPipelineRasterizationStateCreateFlags        flags
		VK_FALSE,                                                     // VkBool32                                       depthClampEnable
		VK_FALSE,                                                     // VkBool32                                       rasterizerDiscardEnable
		VK_POLYGON_MODE_FILL,                                         // VkPolygonMode                                  polygonMode
		VK_CULL_MODE_NONE,                                            // VkCullModeFlags                                cullMode
		VK_FRONT_FACE_COUNTER_CLOCKWISE,                              // VkFrontFace                                    frontFace
		VK_FALSE,                                                     // VkBool32                                       depthBiasEnable
		0.0f,                                                         // float                                          depthBiasConstantFactor
		0.0f,                                                         // float                                          depthBiasClamp
		0.0f,                                                         // float                                          depthBiasSlopeFactor
		1.0f                                                          // float                                          lineWidth
	};

	VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,     // VkStructureType                                sType
		nullptr,                                                      // const void                                    *pNext
		0,                                                            // VkPipelineMultisampleStateCreateFlags          flags
		VK_SAMPLE_COUNT_1_BIT,                                        // VkSampleCountFlagBits                          rasterizationSamples
		VK_FALSE,                                                     // VkBool32                                       sampleShadingEnable
		1.0f,                                                         // float                                          minSampleShading
		nullptr,                                                      // const VkSampleMask                            *pSampleMask
		VK_FALSE,                                                     // VkBool32                                       alphaToCoverageEnable
		VK_FALSE                                                      // VkBool32                                       alphaToOneEnable
	};

	VkPipelineColorBlendAttachmentState color_blend_attachment_state = {
		VK_FALSE,                                                     // VkBool32                                       blendEnable
		VK_BLEND_FACTOR_ONE,                                          // VkBlendFactor                                  srcColorBlendFactor
		VK_BLEND_FACTOR_ZERO,                                         // VkBlendFactor                                  dstColorBlendFactor
		VK_BLEND_OP_ADD,                                              // VkBlendOp                                      colorBlendOp
		VK_BLEND_FACTOR_ONE,                                          // VkBlendFactor                                  srcAlphaBlendFactor
		VK_BLEND_FACTOR_ZERO,                                         // VkBlendFactor                                  dstAlphaBlendFactor
		VK_BLEND_OP_ADD,                                              // VkBlendOp                                      alphaBlendOp
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |         // VkColorComponentFlags                          colorWriteMask
		VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
	};

	VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,     // VkStructureType                                sType
		nullptr,                                                      // const void                                    *pNext
		0,                                                            // VkPipelineColorBlendStateCreateFlags           flags
		VK_FALSE,                                                     // VkBool32                                       logicOpEnable
		VK_LOGIC_OP_COPY,                                             // VkLogicOp                                      logicOp
		1,                                                            // uint32_t                                       attachmentCount
		&color_blend_attachment_state,                                // const VkPipelineColorBlendAttachmentState     *pAttachments
		{0.0f, 0.0f, 0.0f, 0.0f}                                    // float                                          blendConstants[4]
	};

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
	pipelineLayout.Create(device, layout_create_info);

	std::array<VkDynamicState, 2> dynamicStates = {
		VK_DYNAMIC_STATE_SCISSOR,
		VK_DYNAMIC_STATE_VIEWPORT,
	};

	VkPipelineDynamicStateCreateInfo dynamicStateInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO, // VkStructureType                      sType;
		nullptr,                                              // const void*                          pNext;
		0,                                                    // VkPipelineDynamicStateCreateFlags    flags;
		Nz::UInt32(dynamicStates.size()),                     // uint32_t                             dynamicStateCount;
		dynamicStates.data()                                  // const VkDynamicState*                pDynamicStates;
	};

	VkPipelineDepthStencilStateCreateInfo depthStencilInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO, // VkStructureType                           sType;
		nullptr, // const void*                               pNext;
		0U, // VkPipelineDepthStencilStateCreateFlags    flags;
		VK_TRUE, // VkBool32                                  depthTestEnable;
		VK_TRUE, // VkBool32                                  depthWriteEnable;
		VK_COMPARE_OP_LESS_OR_EQUAL, // VkCompareOp                               depthCompareOp;
		VK_FALSE, // VkBool32                                  depthBoundsTestEnable;
		VK_FALSE, // VkBool32                                  stencilTestEnable;
		VkStencilOpState{},// VkStencilOpState                          front;
		VkStencilOpState{},// VkStencilOpState                          back;
		0.f, // float                                     minDepthBounds;
		0.f // float                                     maxDepthBounds;
	};

	VkGraphicsPipelineCreateInfo pipeline_create_info = {
		VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,              // VkStructureType                                sType
		nullptr,                                                      // const void                                    *pNext
		0,                                                            // VkPipelineCreateFlags                          flags
		static_cast<uint32_t>(shaderStageCreateInfo.size()),          // uint32_t                                       stageCount
		shaderStageCreateInfo.data(),                                 // const VkPipelineShaderStageCreateInfo         *pStages
		&vertex_input_state_create_info,                              // const VkPipelineVertexInputStateCreateInfo    *pVertexInputState;
		&input_assembly_state_create_info,                            // const VkPipelineInputAssemblyStateCreateInfo  *pInputAssemblyState
		nullptr,                                                      // const VkPipelineTessellationStateCreateInfo   *pTessellationState
		&viewport_state_create_info,                                  // const VkPipelineViewportStateCreateInfo       *pViewportState
		&rasterization_state_create_info,                             // const VkPipelineRasterizationStateCreateInfo  *pRasterizationState
		&multisample_state_create_info,                               // const VkPipelineMultisampleStateCreateInfo    *pMultisampleState
		&depthStencilInfo,                                            // const VkPipelineDepthStencilStateCreateInfo   *pDepthStencilState
		&color_blend_state_create_info,                               // const VkPipelineColorBlendStateCreateInfo     *pColorBlendState
		&dynamicStateInfo,                                            // const VkPipelineDynamicStateCreateInfo        *pDynamicState
		pipelineLayout,                                               // VkPipelineLayout                               layout
		vulkanWindow.GetRenderPass(),                                 // VkRenderPass                                   renderPass
		0,                                                            // uint32_t                                       subpass
		VK_NULL_HANDLE,                                               // VkPipeline                                     basePipelineHandle
		-1                                                            // int32_t                                        basePipelineIndex
	};

	Nz::Vk::Pipeline pipeline;
	if (!pipeline.CreateGraphics(device, pipeline_create_info))
	{
		NazaraError("Failed to create pipeline");
		return __LINE__;
	}

	Nz::Vk::CommandPool cmdPool;
	if (!cmdPool.Create(device, 0, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT))
	{
		NazaraError("Failed to create rendering cmd pool");
		return __LINE__;
	}

	std::array<VkClearValue, 2> clearValues;
	clearValues[0].color = {1.0f, 0.8f, 0.4f, 0.0f};
	clearValues[1].depthStencil = {1.f, 0};

	Nz::Vk::Queue graphicsQueue(device, device->GetEnabledQueues()[0].queues[0].queue);

	Nz::UInt32 imageCount = vulkanWindow.GetFramebufferCount();
	std::vector<Nz::Vk::CommandBuffer> renderCmds = cmdPool.AllocateCommandBuffers(imageCount, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	for (Nz::UInt32 i = 0; i < imageCount; ++i)
	{
		Nz::Vk::CommandBuffer& renderCmd = renderCmds[i];

		VkRect2D renderArea = {
			{                                           // VkOffset2D                     offset
				0,                                          // int32_t                        x
				0                                           // int32_t                        y
			},
			{                                           // VkExtent2D                     extent
				window.GetWidth(),                                        // int32_t                        width
				window.GetHeight(),                                        // int32_t                        height
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

		renderCmd.Begin(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

		vulkanWindow.BuildPreRenderCommands(i, renderCmd);

		renderCmd.BeginRenderPass(render_pass_begin_info);
		//renderCmd.ClearAttachment(clearAttachment, clearRect);
		//renderCmd.ClearAttachment(clearAttachmentDepth, clearRect);
		renderCmd.BindIndexBuffer(indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		renderCmd.BindVertexBuffer(0, vertexBuffer, 0);
		renderCmd.BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, descriptorSet);
		renderCmd.BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		renderCmd.SetScissor(Nz::Recti{0, 0, int(window.GetWidth()), int(window.GetHeight())});
		renderCmd.SetViewport({0.f, 0.f, float(window.GetWidth()), float(window.GetHeight())}, 0.f, 1.f);
		renderCmd.DrawIndexed(indexCount);
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
					Nz::Mouse::SetPosition(window.GetWidth() / 2, window.GetHeight() / 2, window);
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

			if (!uniformBufferMemory.Map(0, uniformSize))
			{
				NazaraError("Failed to map vertex buffer");
				return __LINE__;
			}

			std::memcpy(uniformBufferMemory.GetMappedPointer(), &ubo, uniformSize);

			uniformBufferMemory.Unmap();
		}

		Nz::UInt32 imageIndex;
		if (!vulkanWindow.Acquire(&imageIndex))
		{
			std::cout << "Failed to acquire next image" << std::endl;
			return EXIT_FAILURE;
		}

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

		if (!graphicsQueue.Submit(submit_info))
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

//	instance.vkDestroyDebugReportCallbackEXT(instance, callback, nullptr);

	return EXIT_SUCCESS;
}