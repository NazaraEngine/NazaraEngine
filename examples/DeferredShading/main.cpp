#include <Nazara/Core.hpp>
#include <Nazara/Platform.hpp>
#include <Nazara/Math/Angle.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Renderer.hpp>
#include <NZSL/Math/FieldOffsets.hpp>
#include <NZSL/Parser.hpp>
#include <array>
#include <chrono>
#include <iostream>
#include <random>
#include <thread>

NAZARA_REQUEST_DEDICATED_GPU()

constexpr std::size_t BloomSubdivisionCount = 5;

/*
[layout(std140)]
struct PointLight
{
	color: vec3[f32],
	position: vec3[f32],

	constant: f32,
	linear: f32,
	quadratic: f32,
}

[layout(std140)]
struct SpotLight
{
	color: vec3[f32],
	position: vec3[f32],
	direction: vec3[f32],

	constant: f32,
	linear: f32,
	quadratic: f32,

	innerAngle: f32,
	outerAngle: f32,
}
*/

struct PointLight
{
	Nz::Color color = Nz::Color::White();
	Nz::Vector3f position = Nz::Vector3f::Zero();

	float radius = 1.f;
};

struct SpotLight
{
	Nz::Color color = Nz::Color::White();
	Nz::Matrix4f transformMatrix;
	Nz::Vector3f position = Nz::Vector3f::Zero();
	Nz::Vector3f direction = Nz::Vector3f::Forward();

	float radius = 1.f;

	Nz::RadianAnglef innerAngle = Nz::DegreeAnglef(15.f);
	Nz::RadianAnglef outerAngle = Nz::DegreeAnglef(20.f);
};

int main(int argc, char* argv[])
{
	std::filesystem::path resourceDir = "assets/examples";
	if (!std::filesystem::is_directory(resourceDir) && std::filesystem::is_directory("../.." / resourceDir))
		resourceDir = "../.." / resourceDir;

	std::filesystem::path shaderDir = "assets/shaders";
	if (!std::filesystem::is_directory(shaderDir) && std::filesystem::is_directory("../.." / shaderDir))
		shaderDir = "../.." / shaderDir;

	Nz::Application<Nz::Graphics> app(argc, argv);

	nzsl::BackendParameters states;
	states.shaderModuleResolver = Nz::Graphics::Instance()->GetShaderModuleResolver();

	Nz::MeshParams meshParams;
	meshParams.center = true;
	meshParams.vertexRotation = Nz::EulerAnglesf(0.f, -90.f, 0.f);
	meshParams.vertexScale = Nz::Vector3f(0.002f);
	meshParams.vertexDeclaration = Nz::VertexDeclaration::Get(Nz::VertexLayout::XYZ_Normal_UV);

	std::shared_ptr<Nz::RenderDevice> device = Nz::Graphics::Instance()->GetRenderDevice();
	const Nz::RenderDeviceInfo& deviceInfo = device->GetDeviceInfo();

	auto& windowing = app.AddComponent<Nz::WindowingAppComponent>();

	std::string windowTitle = "Graphics Test";
	Nz::Window& window = windowing.CreateWindow(Nz::VideoMode(1920, 1080), windowTitle);
	Nz::WindowSwapchain windowSwapchain(device, window);

	std::shared_ptr<Nz::Mesh> spaceship = Nz::Mesh::LoadFromFile(resourceDir / "Spaceship/spaceship.obj", meshParams);
	if (!spaceship)
	{
		NazaraError("failed to load model");
		return __LINE__;
	}

	std::shared_ptr<Nz::GraphicalMesh> gfxMesh = Nz::GraphicalMesh::BuildFromMesh(*spaceship);

	Nz::TextureParams texParams;
	texParams.renderDevice = device;
	texParams.loadFormat = Nz::PixelFormat::RGBA8_SRGB;

	// Plane
	Nz::MeshParams meshPrimitiveParams;
	meshPrimitiveParams.vertexDeclaration = Nz::VertexDeclaration::Get(Nz::VertexLayout::XYZ_Normal_UV);

	std::shared_ptr<Nz::Mesh> planeMesh = std::make_shared<Nz::Mesh>();
	planeMesh->CreateStatic();
	planeMesh->BuildSubMesh(Nz::Primitive::Plane(Nz::Vector2f(25.f, 25.f), Nz::Vector2ui(0u), Nz::Matrix4f::Identity(), Nz::Rectf(0.f, 0.f, 10.f, 10.f)), meshPrimitiveParams);
	//planeMesh->BuildSubMesh(Nz::Primitive::Cone(1.f, 1.f, 16, Nz::Matrix4f::Rotate(Nz::EulerAnglesf(90.f, 0.f, 0.f))), planeParams);
	planeMesh->SetMaterialCount(1);

	std::shared_ptr<Nz::GraphicalMesh> planeMeshGfx = Nz::GraphicalMesh::BuildFromMesh(*planeMesh);

	// Skybox
	meshPrimitiveParams.vertexDeclaration = Nz::VertexDeclaration::Get(Nz::VertexLayout::XYZ);

	std::shared_ptr<Nz::Mesh> cubeMesh = std::make_shared<Nz::Mesh>();
	cubeMesh->CreateStatic();
	cubeMesh->BuildSubMesh(Nz::Primitive::Box(Nz::Vector3f::Unit(), Nz::Vector3ui(0), Nz::Matrix4f::Scale({ 1.f, -1.f, 1.f })), meshPrimitiveParams);
	cubeMesh->SetMaterialCount(1);

	std::shared_ptr<Nz::GraphicalMesh> cubeMeshGfx = Nz::GraphicalMesh::BuildFromMesh(*cubeMesh);

	Nz::RenderPipelineLayoutInfo skyboxPipelineLayoutInfo;
	skyboxPipelineLayoutInfo.bindings.push_back({
		0, 0, 1,
		Nz::ShaderBindingType::UniformBuffer,
		nzsl::ShaderStageType_All
	});

	auto& textureBinding = skyboxPipelineLayoutInfo.bindings.emplace_back();
	textureBinding.setIndex = 0;
	textureBinding.bindingIndex = 1;
	textureBinding.shaderStageFlags = nzsl::ShaderStageType::Fragment;
	textureBinding.type = Nz::ShaderBindingType::Sampler;

	std::shared_ptr<Nz::RenderPipelineLayout> skyboxPipelineLayout = device->InstantiateRenderPipelineLayout(std::move(skyboxPipelineLayoutInfo));

	Nz::RenderPipelineInfo skyboxPipelineInfo;
	skyboxPipelineInfo.depthBuffer = true;
	skyboxPipelineInfo.depthCompare = Nz::RendererComparison::Equal;
	skyboxPipelineInfo.faceCulling = Nz::FaceCulling::Front;
	skyboxPipelineInfo.pipelineLayout = skyboxPipelineLayout;
	skyboxPipelineInfo.shaderModules.push_back(device->InstantiateShaderModule(nzsl::ShaderStageType::Fragment | nzsl::ShaderStageType::Vertex, Nz::ShaderLanguage::NazaraShader, shaderDir / "skybox.nzsl", states));
	skyboxPipelineInfo.vertexBuffers.push_back({
		0,
		meshPrimitiveParams.vertexDeclaration
	});

	std::shared_ptr<Nz::RenderPipeline> skyboxPipeline = device->InstantiateRenderPipeline(std::move(skyboxPipelineInfo));

	// Skybox
	std::shared_ptr<Nz::Texture> skyboxTexture;
	{
		Nz::Image skyboxImage(Nz::ImageType::Cubemap, Nz::PixelFormat::RGBA8, 2048, 2048);
		skyboxImage.LoadFaceFromFile(Nz::CubemapFace::PositiveX, resourceDir / "purple_nebula_skybox/purple_nebula_skybox_right1.png");
		skyboxImage.LoadFaceFromFile(Nz::CubemapFace::PositiveY, resourceDir / "purple_nebula_skybox/purple_nebula_skybox_top3.png");
		skyboxImage.LoadFaceFromFile(Nz::CubemapFace::PositiveZ, resourceDir / "purple_nebula_skybox/purple_nebula_skybox_front5.png");
		skyboxImage.LoadFaceFromFile(Nz::CubemapFace::NegativeX, resourceDir / "purple_nebula_skybox/purple_nebula_skybox_left2.png");
		skyboxImage.LoadFaceFromFile(Nz::CubemapFace::NegativeY, resourceDir / "purple_nebula_skybox/purple_nebula_skybox_bottom4.png");
		skyboxImage.LoadFaceFromFile(Nz::CubemapFace::NegativeZ, resourceDir / "purple_nebula_skybox/purple_nebula_skybox_back6.png");

		skyboxTexture = Nz::Texture::CreateFromImage(skyboxImage, texParams);
	}


	// Cone mesh
	std::shared_ptr<Nz::Mesh> coneMesh = std::make_shared<Nz::Mesh>();
	coneMesh->CreateStatic();
	coneMesh->BuildSubMesh(Nz::Primitive::Cone(1.f, 1.f, 16, Nz::Matrix4f::Rotate(Nz::EulerAnglesf(90.f, 0.f, 0.f))), meshPrimitiveParams);
	coneMesh->SetMaterialCount(1);

	std::shared_ptr<Nz::GraphicalMesh> coneMeshGfx = Nz::GraphicalMesh::BuildFromMesh(*coneMesh);

	Nz::MaterialSettings settings;
	Nz::PredefinedMaterials::AddBasicSettings(settings);

	Nz::MaterialPass customForwardPass;
	customForwardPass.states.depthBuffer = true;
	customForwardPass.shaders.emplace_back(std::make_shared<Nz::UberShader>(nzsl::ShaderStageType::Fragment, nzsl::ParseFromFile(shaderDir / "deferred_frag.nzsl")));
	customForwardPass.shaders.emplace_back(std::make_shared<Nz::UberShader>(nzsl::ShaderStageType::Vertex, nzsl::ParseFromFile(shaderDir / "deferred_vert.nzsl")));
	settings.AddPass("ForwardPass", customForwardPass);

	Nz::MaterialPass customDepthPass = customForwardPass;
	customDepthPass.options[Nz::CRC32("DepthPass")] = true;
	settings.AddPass("DepthPass", customDepthPass);

	auto deferredMaterial = std::make_shared<Nz::Material>(std::move(settings), "BasicMaterial");

	std::shared_ptr<Nz::MaterialInstance> spaceshipMat = deferredMaterial->Instantiate();
	spaceshipMat->SetTextureProperty("AlphaMap", Nz::TextureAsset::OpenFromFile(resourceDir / "alphatile.png"));
	spaceshipMat->SetTextureProperty("BaseColorMap", Nz::TextureAsset::OpenFromFile(resourceDir / "Spaceship/Texture/diffuse.png", { .sRGB = true }));

	std::shared_ptr<Nz::MaterialInstance> flareMaterial = deferredMaterial->Instantiate();
	flareMaterial->UpdatePassStates("ForwardPass", [](Nz::RenderStates& renderStates)
	{
		renderStates.depthClamp = true;
		renderStates.depthWrite = false;
		renderStates.faceCulling = Nz::FaceCulling::None;
		renderStates.blending = true;
		renderStates.blend.modeColor = Nz::BlendEquation::Add;
		renderStates.blend.modeAlpha = Nz::BlendEquation::Add;
		renderStates.blend.srcColor = Nz::BlendFunc::SrcAlpha;
		renderStates.blend.dstColor = Nz::BlendFunc::InvSrcAlpha;
		renderStates.blend.srcAlpha = Nz::BlendFunc::One;
		renderStates.blend.dstAlpha = Nz::BlendFunc::One;
		return true;
	});
	flareMaterial->UpdatePassFlags("ForwardPass", Nz::MaterialPassFlag::SortByDistance);
	flareMaterial->SetTextureProperty("BaseColorMap", Nz::TextureAsset::OpenFromFile(resourceDir / "flare1.png", { .sRGB = true }));

	Nz::TextureSamplerInfo planeSampler;
	planeSampler.anisotropyLevel = 16;
	planeSampler.wrapModeU = Nz::SamplerWrap::Repeat;
	planeSampler.wrapModeV = Nz::SamplerWrap::Repeat;

	std::shared_ptr<Nz::MaterialInstance> planeMat = deferredMaterial->Instantiate();
	planeMat->SetTextureProperty("BaseColorMap", Nz::TextureAsset::OpenFromFile(resourceDir / "dev_grey.png", { .sRGB = true }), planeSampler);

	Nz::Model spaceshipModel(std::move(gfxMesh));
	for (std::size_t i = 0; i < spaceshipModel.GetSubMeshCount(); ++i)
		spaceshipModel.SetMaterial(i, spaceshipMat);

	Nz::Model planeModel(std::move(planeMeshGfx));
	for (std::size_t i = 0; i < planeModel.GetSubMeshCount(); ++i)
		planeModel.SetMaterial(i, planeMat);

	Nz::Vector2ui windowSize = window.GetSize();

	Nz::ViewerInstance viewerInstance;
	viewerInstance.UpdateTargetSize(Nz::Vector2f(windowSize));
	viewerInstance.UpdateProjViewMatrices(Nz::Matrix4f::Perspective(Nz::DegreeAnglef(70.f), float(windowSize.x) / windowSize.y, 0.1f, 1000.f), Nz::Matrix4f::Translate(Nz::Vector3f::Up() * 1));
	viewerInstance.UpdateNearFarPlanes(0.1f, 1000.f);

	Nz::WorldInstance modelInstance1;
	modelInstance1.UpdateWorldMatrix(Nz::Matrix4f::Translate(Nz::Vector3f::Left() + Nz::Vector3f::Up()));

	Nz::WorldInstance modelInstance2;
	modelInstance2.UpdateWorldMatrix(Nz::Matrix4f::Translate(Nz::Vector3f::Right() + Nz::Vector3f::Up()));

	Nz::WorldInstance planeInstance;


	Nz::RenderPipelineLayoutInfo lightingPipelineLayoutInfo;
	lightingPipelineLayoutInfo.bindings.push_back({
		0, 0, 1,
		Nz::ShaderBindingType::UniformBuffer,
		nzsl::ShaderStageType_All
	});

	for (unsigned int i = 0; i < 3; ++i)
	{
		lightingPipelineLayoutInfo.bindings.push_back({
			0,
			i + 1,
			1,
			Nz::ShaderBindingType::Sampler,
			nzsl::ShaderStageType::Fragment,
		});
	}

	lightingPipelineLayoutInfo.bindings.push_back({
		1,
		0,
		1,
		Nz::ShaderBindingType::UniformBuffer,
		nzsl::ShaderStageType::Fragment | nzsl::ShaderStageType::Vertex,
	});

	/*nzsl::FieldOffsets pointLightOffsets(nzsl::StructLayout::Std140);
	std::size_t colorOffset = pointLightOffsets.AddField(nzsl::StructFieldType::Float3);
	std::size_t positionOffset = pointLightOffsets.AddField(nzsl::StructFieldType::Float3);
	std::size_t constantOffset = pointLightOffsets.AddField(nzsl::StructFieldType::Float1);
	std::size_t linearOffset = pointLightOffsets.AddField(nzsl::StructFieldType::Float1);
	std::size_t quadraticOffset = pointLightOffsets.AddField(nzsl::StructFieldType::Float1);

	std::size_t alignedPointLightSize = Nz::Align(pointLightOffsets.GetSize(), static_cast<std::size_t>(deviceInfo.limits.minUniformBufferOffsetAlignment));*/

	/*
	[layout(std140)]
	struct SpotLight
	{
		color: vec3[f32],
		position: vec3[f32],
		direction: vec3[f32],

		constant: f32,
		linear: f32,
		quadratic: f32,

		innerAngle: f32,
		outerAngle: f32,
	}
	*/

	nzsl::FieldOffsets spotLightOffsets(nzsl::StructLayout::Std140);
	std::size_t transformMatrixOffset = spotLightOffsets.AddMatrix(nzsl::StructFieldType::Float1, 4, 4, true);
	std::size_t colorOffset = spotLightOffsets.AddField(nzsl::StructFieldType::Float3);
	std::size_t positionOffset = spotLightOffsets.AddField(nzsl::StructFieldType::Float3);
	std::size_t directionOffset = spotLightOffsets.AddField(nzsl::StructFieldType::Float3);
	std::size_t radiusOffset = spotLightOffsets.AddField(nzsl::StructFieldType::Float1);
	std::size_t invRadiusOffset = spotLightOffsets.AddField(nzsl::StructFieldType::Float1);
	std::size_t innerAngleOffset = spotLightOffsets.AddField(nzsl::StructFieldType::Float1);
	std::size_t outerAngleOffset = spotLightOffsets.AddField(nzsl::StructFieldType::Float1);

	std::size_t alignedSpotLightSize = Nz::AlignPow2(spotLightOffsets.GetAlignedSize(), static_cast<std::size_t>(deviceInfo.limits.minUniformBufferOffsetAlignment));

	constexpr std::size_t MaxPointLight = 2000;

	std::shared_ptr<Nz::RenderBuffer> lightUbo = device->InstantiateBuffer(Nz::BufferType::Uniform, MaxPointLight * alignedSpotLightSize, Nz::BufferUsage::DeviceLocal | Nz::BufferUsage::Dynamic | Nz::BufferUsage::Write);

	std::vector<SpotLight> spotLights;
	/*auto& firstSpot = spotLights.emplace_back();
	firstSpot.position = Nz::Vector3f::Right() + Nz::Vector3f::Forward();
	firstSpot.direction = Nz::Vector3f::Up();*/

	std::random_device rng;
	std::mt19937 randomEngine(rng());
	std::uniform_real_distribution<float> colorDis(0.f, 1.f);
	std::uniform_real_distribution<float> heightDis(0.15f, 1.f);
	std::uniform_real_distribution<float> posDis(-10.f, 10.f);
	std::uniform_real_distribution<float> dirDis(-1.f, 1.f);
	std::uniform_real_distribution<float> dirYDis(-0.33f, 0.f);
	std::uniform_real_distribution<float> radiusDis(1.f, 5.f);

	for (std::size_t i = 0; i < 1000; ++i)
	{
		auto& light = spotLights.emplace_back();
		light.color = Nz::Color(colorDis(randomEngine), colorDis(randomEngine), colorDis(randomEngine));
		light.position = Nz::Vector3f(posDis(randomEngine), heightDis(randomEngine), posDis(randomEngine));
		light.direction = Nz::Vector3f(dirDis(randomEngine), dirYDis(randomEngine), dirDis(randomEngine)).GetNormal();
		light.radius = radiusDis(randomEngine);
	}

	// Bloom data

	Nz::RenderPipelineLayoutInfo fullscreenPipelineLayoutInfoViewer;
	fullscreenPipelineLayoutInfoViewer.bindings.push_back({
		0, 0, 1,
		Nz::ShaderBindingType::UniformBuffer,
		nzsl::ShaderStageType_All
	});

	fullscreenPipelineLayoutInfoViewer.bindings.push_back({
		0, 1, 1,
		Nz::ShaderBindingType::Sampler,
		nzsl::ShaderStageType::Fragment,
	});

	Nz::RenderPipelineInfo fullscreenPipelineInfoViewer;
	fullscreenPipelineInfoViewer.primitiveMode = Nz::PrimitiveMode::TriangleList;
	fullscreenPipelineInfoViewer.pipelineLayout = device->InstantiateRenderPipelineLayout(fullscreenPipelineLayoutInfoViewer);

	fullscreenPipelineInfoViewer.shaderModules.push_back(device->InstantiateShaderModule(nzsl::ShaderStageType::Fragment | nzsl::ShaderStageType::Vertex, Nz::ShaderLanguage::NazaraShader, shaderDir / "bloom_bright.nzsl", states));

	Nz::ShaderBindingPtr bloomBrightShaderBinding;

	std::shared_ptr<Nz::RenderPipeline> bloomBrightPipeline = device->InstantiateRenderPipeline(fullscreenPipelineInfoViewer);

	// Gaussian Blur

	Nz::RenderPipelineLayoutInfo gaussianBlurPipelineLayoutInfo = fullscreenPipelineLayoutInfoViewer;

	gaussianBlurPipelineLayoutInfo.bindings.push_back({
		0, 2, 1,
		Nz::ShaderBindingType::UniformBuffer,
		nzsl::ShaderStageType::Fragment,
	});

	Nz::RenderPipelineInfo gaussianBlurPipelineInfo = fullscreenPipelineInfoViewer;
	gaussianBlurPipelineInfo.pipelineLayout = device->InstantiateRenderPipelineLayout(gaussianBlurPipelineLayoutInfo);

	nzsl::FieldOffsets gaussianBlurDataOffsets(nzsl::StructLayout::Std140);
	std::size_t gaussianBlurDataDirection = gaussianBlurDataOffsets.AddField(nzsl::StructFieldType::Float2);
	std::size_t gaussianBlurDataSize = gaussianBlurDataOffsets.AddField(nzsl::StructFieldType::Float1);

	gaussianBlurPipelineInfo.shaderModules.clear();
	gaussianBlurPipelineInfo.shaderModules.push_back(device->InstantiateShaderModule(nzsl::ShaderStageType::Fragment | nzsl::ShaderStageType::Vertex, Nz::ShaderLanguage::NazaraShader, shaderDir / "gaussian_blur.nzsl", states));

	std::shared_ptr<Nz::RenderPipeline> gaussianBlurPipeline = device->InstantiateRenderPipeline(gaussianBlurPipelineInfo);
	std::vector<Nz::ShaderBindingPtr> gaussianBlurShaderBinding(BloomSubdivisionCount * 2);

	std::vector<Nz::UInt8> gaussianBlurData(gaussianBlurDataOffsets.GetSize());
	std::vector<std::shared_ptr<Nz::RenderBuffer>> gaussianBlurUbos;

	float sizeFactor = 2.f;
	for (std::size_t i = 0; i < BloomSubdivisionCount; ++i)
	{
		Nz::AccessByOffset<Nz::Vector2f&>(gaussianBlurData.data(), gaussianBlurDataDirection) = Nz::Vector2f(1.f, 0.f);
		Nz::AccessByOffset<float&>(gaussianBlurData.data(), gaussianBlurDataSize) = sizeFactor;

		std::shared_ptr<Nz::RenderBuffer> horizontalBlurData = device->InstantiateBuffer(Nz::BufferType::Uniform, gaussianBlurDataOffsets.GetSize(), Nz::BufferUsage::DeviceLocal | Nz::BufferUsage::Dynamic | Nz::BufferUsage::Write, gaussianBlurData.data());

		Nz::AccessByOffset<Nz::Vector2f&>(gaussianBlurData.data(), gaussianBlurDataDirection) = Nz::Vector2f(0.f, 1.f);

		std::shared_ptr<Nz::RenderBuffer> verticalBlurData = device->InstantiateBuffer(Nz::BufferType::Uniform, gaussianBlurDataOffsets.GetSize(), Nz::BufferUsage::DeviceLocal | Nz::BufferUsage::Dynamic | Nz::BufferUsage::Write, gaussianBlurData.data());

		sizeFactor *= 2.f;

		gaussianBlurUbos.push_back(horizontalBlurData);
		gaussianBlurUbos.push_back(verticalBlurData);
	}

	// Tone mapping
	Nz::ShaderBindingPtr toneMappingShaderBinding;

	fullscreenPipelineInfoViewer.shaderModules.clear();
	fullscreenPipelineInfoViewer.shaderModules.push_back(device->InstantiateShaderModule(nzsl::ShaderStageType::Fragment | nzsl::ShaderStageType::Vertex, Nz::ShaderLanguage::NazaraShader, shaderDir / "tone_mapping.nzsl", states));

	std::shared_ptr<Nz::RenderPipeline> toneMappingPipeline = device->InstantiateRenderPipeline(fullscreenPipelineInfoViewer);

	// Bloom blend

	Nz::ShaderBindingPtr bloomBlitBinding;

	Nz::RenderPipelineLayoutInfo bloomBlendPipelineLayoutInfo;
	bloomBlendPipelineLayoutInfo.bindings.push_back({
		0, 0, 1,
		Nz::ShaderBindingType::UniformBuffer,
		nzsl::ShaderStageType_All
	});

	/*bloomBlendPipelineLayoutInfo.bindings.push_back({
		0, 1, 1,
		Nz::ShaderBindingType::Texture,
		nzsl::ShaderStageType::Fragment,
	});*/

	bloomBlendPipelineLayoutInfo.bindings.push_back({
		0, 2, 1,
		Nz::ShaderBindingType::Sampler,
		nzsl::ShaderStageType::Fragment,
	});


	Nz::RenderPipelineInfo bloomBlendPipelineInfo;
	bloomBlendPipelineInfo.blending = true;
	bloomBlendPipelineInfo.blend.dstColor = Nz::BlendFunc::One;
	bloomBlendPipelineInfo.blend.srcColor = Nz::BlendFunc::One;
	bloomBlendPipelineInfo.primitiveMode = Nz::PrimitiveMode::TriangleList;
	bloomBlendPipelineInfo.pipelineLayout = device->InstantiateRenderPipelineLayout(bloomBlendPipelineLayoutInfo);

	bloomBlendPipelineInfo.shaderModules.push_back(device->InstantiateShaderModule(nzsl::ShaderStageType::Fragment | nzsl::ShaderStageType::Vertex, Nz::ShaderLanguage::NazaraShader, shaderDir / "bloom_final.nzsl", states));

	std::shared_ptr<Nz::RenderPipeline> bloomBlendPipeline = device->InstantiateRenderPipeline(bloomBlendPipelineInfo);

	std::vector<Nz::ShaderBindingPtr> bloomBlendShaderBinding(BloomSubdivisionCount);

	// Gamma correction

	Nz::RenderPipelineLayoutInfo fullscreenPipelineLayoutInfo;

	fullscreenPipelineLayoutInfo.bindings.push_back({
		0, 0, 1,
		Nz::ShaderBindingType::Sampler,
		nzsl::ShaderStageType::Fragment,
	});

	Nz::RenderPipelineInfo fullscreenPipelineInfo;
	fullscreenPipelineInfo.primitiveMode = Nz::PrimitiveMode::TriangleList;
	fullscreenPipelineInfo.pipelineLayout = device->InstantiateRenderPipelineLayout(fullscreenPipelineLayoutInfo);

	fullscreenPipelineInfo.shaderModules.push_back(device->InstantiateShaderModule(nzsl::ShaderStageType::Fragment | nzsl::ShaderStageType::Vertex, Nz::ShaderLanguage::NazaraShader, shaderDir / "gamma.nzsl", states));

	// God rays

	Nz::RenderPipelineLayoutInfo godraysPipelineLayoutInfo;

	godraysPipelineLayoutInfo.bindings = {
		{
			{
				0, 0, 1,
				Nz::ShaderBindingType::UniformBuffer,
				nzsl::ShaderStageType::Fragment,
			},
			{
				0, 1, 1,
				Nz::ShaderBindingType::UniformBuffer,
				nzsl::ShaderStageType::Fragment,
			},
			{
				0, 2, 1,
				Nz::ShaderBindingType::Sampler,
				nzsl::ShaderStageType::Fragment,
			},
		}
	};

	Nz::RenderPipelineInfo godraysPipelineInfo;
	godraysPipelineInfo.primitiveMode = Nz::PrimitiveMode::TriangleList;
	godraysPipelineInfo.pipelineLayout = device->InstantiateRenderPipelineLayout(godraysPipelineLayoutInfo);

	godraysPipelineInfo.shaderModules.push_back(device->InstantiateShaderModule(nzsl::ShaderStageType::Fragment | nzsl::ShaderStageType::Vertex, Nz::ShaderLanguage::NazaraShader, shaderDir / "god_rays.nzsl", states));

	std::shared_ptr<Nz::RenderPipeline> godraysPipeline = device->InstantiateRenderPipeline(godraysPipelineInfo);

	nzsl::FieldOffsets godraysFieldOffsets(nzsl::StructLayout::Std140);
	std::size_t gr_exposureOffset = godraysFieldOffsets.AddField(nzsl::StructFieldType::Float1);
	std::size_t gr_decayOffset = godraysFieldOffsets.AddField(nzsl::StructFieldType::Float1);
	std::size_t gr_densityOffset = godraysFieldOffsets.AddField(nzsl::StructFieldType::Float1);
	std::size_t gr_weightOffset = godraysFieldOffsets.AddField(nzsl::StructFieldType::Float1);
	std::size_t gr_lightPositionOffset = godraysFieldOffsets.AddField(nzsl::StructFieldType::Float2);

	Nz::ShaderBindingPtr godRaysShaderBinding = godraysPipelineInfo.pipelineLayout->AllocateShaderBinding(0);

	/*
		uniformExposure = 0.0034f;
		uniformDecay = 1.0f;
		uniformDensity = 0.84f;
		uniformWeight = 5.65f;
	*/

	std::vector<Nz::UInt8> godRaysData(godraysFieldOffsets.GetSize());
	Nz::AccessByOffset<float&>(godRaysData.data(), gr_exposureOffset) = 0.0034f;
	Nz::AccessByOffset<float&>(godRaysData.data(), gr_decayOffset) = 0.99f;
	Nz::AccessByOffset<float&>(godRaysData.data(), gr_densityOffset) = 0.95f;
	Nz::AccessByOffset<float&>(godRaysData.data(), gr_weightOffset) = 5.65f;
	Nz::AccessByOffset<Nz::Vector2f&>(godRaysData.data(), gr_lightPositionOffset) = Nz::Vector2f(0.5f, 0.1f);

	std::shared_ptr<Nz::RenderBuffer> godRaysUBO = device->InstantiateBuffer(Nz::BufferType::Uniform, godRaysData.size(), Nz::BufferUsage::DeviceLocal | Nz::BufferUsage::Dynamic | Nz::BufferUsage::Write, godRaysData.data());

	Nz::ShaderBindingPtr godRaysBlitShaderBinding;


	std::shared_ptr<Nz::RenderPipeline> fullscreenPipeline = device->InstantiateRenderPipeline(fullscreenPipelineInfo);

	Nz::RenderPipelineInfo lightingPipelineInfo;
	lightingPipelineInfo.blending = true;
	lightingPipelineInfo.blend.dstColor = Nz::BlendFunc::One;
	lightingPipelineInfo.blend.srcColor = Nz::BlendFunc::One;
	lightingPipelineInfo.primitiveMode = Nz::PrimitiveMode::TriangleList;
	lightingPipelineInfo.pipelineLayout = device->InstantiateRenderPipelineLayout(lightingPipelineLayoutInfo);
	lightingPipelineInfo.vertexBuffers.push_back({
		0,
		meshPrimitiveParams.vertexDeclaration
	});
	lightingPipelineInfo.depthBuffer = false;
	lightingPipelineInfo.faceCulling = Nz::FaceCulling::Front;
	lightingPipelineInfo.stencilTest = true;
	lightingPipelineInfo.stencilBack.compare = Nz::RendererComparison::NotEqual;
	lightingPipelineInfo.stencilBack.fail = Nz::StencilOperation::Zero;
	lightingPipelineInfo.stencilBack.depthFail = Nz::StencilOperation::Zero;
	lightingPipelineInfo.stencilBack.pass = Nz::StencilOperation::Zero;

	lightingPipelineInfo.shaderModules.push_back(device->InstantiateShaderModule(nzsl::ShaderStageType::Fragment | nzsl::ShaderStageType::Vertex, Nz::ShaderLanguage::NazaraShader, shaderDir / "lighting.nzsl", states));

	std::shared_ptr<Nz::RenderPipeline> lightingPipeline = device->InstantiateRenderPipeline(lightingPipelineInfo);

	Nz::RenderPipelineInfo stencilPipelineInfo;
	stencilPipelineInfo.primitiveMode = Nz::PrimitiveMode::TriangleList;
	stencilPipelineInfo.pipelineLayout = device->InstantiateRenderPipelineLayout(lightingPipelineLayoutInfo);
	stencilPipelineInfo.vertexBuffers.push_back({
		0,
		meshPrimitiveParams.vertexDeclaration
	});

	stencilPipelineInfo.colorWriteMask = 0;
	stencilPipelineInfo.depthBuffer = true;
	stencilPipelineInfo.depthWrite = false;
	stencilPipelineInfo.faceCulling = Nz::FaceCulling::Front;
	stencilPipelineInfo.stencilTest = true;
	stencilPipelineInfo.stencilFront.compare = Nz::RendererComparison::Always;
	stencilPipelineInfo.stencilFront.depthFail = Nz::StencilOperation::Invert;
	stencilPipelineInfo.stencilBack.compare = Nz::RendererComparison::Always;
	stencilPipelineInfo.stencilBack.depthFail = Nz::StencilOperation::Invert;

	stencilPipelineInfo.shaderModules.push_back(device->InstantiateShaderModule(nzsl::ShaderStageType::Vertex, Nz::ShaderLanguage::NazaraShader, shaderDir / "lighting.nzsl", states));

	std::shared_ptr<Nz::RenderPipeline> stencilPipeline = device->InstantiateRenderPipeline(stencilPipelineInfo);


	std::vector<Nz::ShaderBindingPtr> lightingShaderBindings;

	Nz::ShaderBindingPtr bloomSkipBlit;
	Nz::ShaderBindingPtr finalBlitBinding;

	bool lightUpdate = true;

	std::shared_ptr<Nz::TextureSampler> textureSampler = device->InstantiateTextureSampler({});

	Nz::ShaderBindingPtr skyboxShaderBinding = skyboxPipelineLayout->AllocateShaderBinding(0);
	skyboxShaderBinding->Update({
		{
			0,
			Nz::ShaderBinding::UniformBufferBinding {
				viewerInstance.GetViewerBuffer().get(),
				0, viewerInstance.GetViewerBuffer()->GetSize()
			}
		},
		{
			1,
			Nz::ShaderBinding::SampledTextureBinding {
				skyboxTexture.get(),
				textureSampler.get()
			}
		}
	});

	Nz::ShaderBindingPtr gbufferShaderBinding;

	bool bloomEnabled = true;
	bool forwardEnabled = true;
	bool lightAnimation = true;

	std::size_t colorTexture;
	std::size_t normalTexture;
	std::size_t positionTexture;
	std::size_t depthBuffer1;
	std::size_t depthBuffer2;
	std::size_t bloomBrightOutput;
	std::size_t bloomOutput;
	std::vector<std::size_t> bloomTextures(BloomSubdivisionCount * 2);
	std::size_t lightOutput;
	std::size_t occluderTexture;
	std::size_t godRaysTexture;

	std::size_t toneMappingOutput;

	Nz::SpriteChainRenderer spritechainRenderer(*device);
	std::unique_ptr<Nz::ElementRendererData> spriteRendererData = spritechainRenderer.InstanciateData();

	Nz::Sprite flareSprite(flareMaterial);
	flareSprite.SetSize({ 128.f, 128.f });
	flareSprite.SetSize(flareSprite.GetSize() * 0.1f);
	flareSprite.SetOrigin({ 0.5f, 0.5f });

	Nz::Vector3f flarePosition = { 0.f, 6.f, 100.f };

	Nz::WorldInstance flareInstance;
	flareInstance.UpdateWorldMatrix(Nz::Matrix4f::Translate(flarePosition));

	Nz::SubmeshRenderer submeshRenderer;
	std::unique_ptr<Nz::ElementRendererData> submeshRendererData = submeshRenderer.InstanciateData();

	std::size_t forwardPassIndex = Nz::Graphics::Instance()->GetMaterialPassRegistry().GetPassIndex("ForwardPass");

	Nz::RenderFrame* currentFrame = nullptr;

	Nz::ElementRendererRegistry elementRegistry;

	Nz::BakedFrameGraph bakedGraph = [&]
	{
		Nz::PixelFormat depthStencilFormat = Nz::PixelFormat::Undefined;
		for (Nz::PixelFormat candidate : { Nz::PixelFormat::Depth24Stencil8, Nz::PixelFormat::Depth32FStencil8, Nz::PixelFormat::Depth16Stencil8 })
		{
			if (device->IsTextureFormatSupported(candidate, Nz::TextureUsage::DepthStencilAttachment))
			{
				depthStencilFormat = candidate;
				break;
			}
		}

		if (depthStencilFormat == Nz::PixelFormat::Undefined)
		{
			std::cerr << "no depth-stencil format found" << std::endl;
			std::exit(__LINE__);
		}

		Nz::FrameGraph graph;

		colorTexture = graph.AddAttachment({
			"Color",
			Nz::PixelFormat::RGBA8
		});

		normalTexture = graph.AddAttachment({
			"Normal",
			Nz::PixelFormat::RGBA8
		});

		positionTexture = graph.AddAttachment({
			"Position",
			Nz::PixelFormat::RGBA32F
		});

		depthBuffer1 = graph.AddAttachment({
			"Depth buffer",
			depthStencilFormat
		});

		depthBuffer2 = graph.AddAttachmentProxy("Depth buffer 2", depthBuffer1);

		lightOutput = graph.AddAttachment({
			"Light output",
			Nz::PixelFormat::RGBA16F
		});

		occluderTexture = graph.AddAttachment({
			"Occluder texture",
			Nz::PixelFormat::RGBA8,
		});

		godRaysTexture = graph.AddAttachment({
			.name   = "God rays texture",
			.format = Nz::PixelFormat::RGBA16F,
			.size   = Nz::FramePassAttachmentSize::ViewerTargetFactor,
			.width  = 50'000,
			.height = 50'000
		});

		bloomOutput = graph.AddAttachmentProxy("Bloom output", lightOutput);

		unsigned int bloomSize = 50'000;
		bloomBrightOutput = graph.AddAttachment({
			.name   = "Bloom bright output",
			.format = Nz::PixelFormat::RGBA16F,
			.size   = Nz::FramePassAttachmentSize::ViewerTargetFactor,
			.width  = bloomSize,
			.height = bloomSize
		});

		for (std::size_t i = 0; i < BloomSubdivisionCount; ++i)
		{
			bloomTextures[i * 2 + 0] = graph.AddAttachment({
				.name   = "Bloom texture #" + std::to_string(i),
				.format = Nz::PixelFormat::RGBA16F,
				.size   = Nz::FramePassAttachmentSize::ViewerTargetFactor,
				.width  = bloomSize,
				.height = bloomSize
			});


			bloomTextures[i * 2 + 1] = graph.AddAttachment({
				.name   = "Bloom texture #" + std::to_string(i),
				.format = Nz::PixelFormat::RGBA16F,
				.size   = Nz::FramePassAttachmentSize::ViewerTargetFactor,
				.width  = bloomSize,
				.height = bloomSize
			});

			bloomSize /= 2;
		}

		toneMappingOutput = graph.AddAttachment({
			.name   = "Tone mapping",
			.format = Nz::PixelFormat::RGBA8,
			.size   = Nz::FramePassAttachmentSize::ViewerTargetFactor,
			.width  = 100'000,
			.height = 100'000
		});

		Nz::FramePass& gbufferPass = graph.AddPass("GBuffer");

		std::size_t geometryAlbedo = gbufferPass.AddOutput(colorTexture);
		gbufferPass.SetClearColor(geometryAlbedo, Nz::Color::Black());

		std::size_t geometryNormal = gbufferPass.AddOutput(normalTexture);
		gbufferPass.SetClearColor(geometryNormal, Nz::Color::Black());

		std::size_t positionAttachment = gbufferPass.AddOutput(positionTexture);
		gbufferPass.SetClearColor(positionAttachment, Nz::Color::Black());

		gbufferPass.SetDepthStencilClear(1.f, 0);

		gbufferPass.SetDepthStencilOutput(depthBuffer1);

		gbufferPass.SetExecutionCallback([&]
		{
			return Nz::FramePassExecution::Execute;
		});

		gbufferPass.SetCommandCallback([&](Nz::CommandBufferBuilder& builder, const Nz::FramePassEnvironment& env)
		{
			builder.SetViewport(env.renderRect);

			Nz::InstancedRenderable::ElementData elementData;
			elementData.scissorBox = &env.renderRect;
			elementData.skeletonInstance = nullptr;

			std::vector<Nz::RenderElementOwner> elements;

			elementData.worldInstance = &modelInstance1;
			spaceshipModel.BuildElement(elementRegistry, elementData, forwardPassIndex, elements);

			elementData.worldInstance = &modelInstance2;
			spaceshipModel.BuildElement(elementRegistry, elementData, forwardPassIndex, elements);

			elementData.worldInstance = &planeInstance;
			planeModel.BuildElement(elementRegistry, elementData, forwardPassIndex, elements);

			std::vector<Nz::Pointer<const Nz::RenderElement>> elementPointers;
			std::vector<Nz::ElementRenderer::RenderStates> renderStates(elements.size());
			elementPointers.reserve(elements.size());
			for (const auto& elementOwner : elements)
				elementPointers.emplace_back(elementOwner.GetElement());

			submeshRenderer.Prepare(viewerInstance, *submeshRendererData, *currentFrame, elementPointers.size(), elementPointers.data(), renderStates.data());
			submeshRenderer.PrepareEnd(*currentFrame, *spriteRendererData);
			submeshRenderer.Render(viewerInstance, *submeshRendererData, builder, elementPointers.size(), elementPointers.data());
		});

		Nz::FramePass& lightingPass = graph.AddPass("Lighting pass");
		lightingPass.SetExecutionCallback([&]
		{
			return (lightUpdate) ? Nz::FramePassExecution::UpdateAndExecute : Nz::FramePassExecution::Execute;
		});

		lightingPass.SetCommandCallback([&](Nz::CommandBufferBuilder& builder, const Nz::FramePassEnvironment& env)
		{
			builder.SetScissor(env.renderRect);
			builder.SetViewport(env.renderRect);

			//builder.BindVertexBuffer(0, vertexBuffer.get());
			builder.BindIndexBuffer(*coneMeshGfx->GetIndexBuffer(0).get(), Nz::IndexType::U16);
			builder.BindVertexBuffer(0, *coneMeshGfx->GetVertexBuffer(0).get());

			builder.BindRenderShaderBinding(0, *gbufferShaderBinding);

			for (std::size_t i = 0; i < spotLights.size(); ++i)
			{
				builder.BindRenderShaderBinding(1, *lightingShaderBindings[i]);

				builder.BindRenderPipeline(*stencilPipeline);
				builder.DrawIndexed(coneMeshGfx->GetIndexCount(0));

				builder.BindRenderPipeline(*lightingPipeline);
				builder.DrawIndexed(coneMeshGfx->GetIndexCount(0));
			}
		});

		lightingPass.AddInput(colorTexture);
		lightingPass.AddInput(normalTexture);
		lightingPass.AddInput(positionTexture);

		lightingPass.SetClearColor(lightingPass.AddOutput(lightOutput), Nz::Color::Black());
		lightingPass.SetDepthStencilInput(depthBuffer1);
		lightingPass.SetDepthStencilOutput(depthBuffer1);

		Nz::FramePass& forwardPass = graph.AddPass("Forward pass");
		forwardPass.SetCommandCallback([&](Nz::CommandBufferBuilder& builder, const Nz::FramePassEnvironment& env)
		{
			builder.SetScissor(env.renderRect);
			builder.SetViewport(env.renderRect);

			builder.BindRenderShaderBinding(0, *skyboxShaderBinding);

			builder.BindIndexBuffer(*cubeMeshGfx->GetIndexBuffer(0), Nz::IndexType::U16);
			builder.BindVertexBuffer(0, *cubeMeshGfx->GetVertexBuffer(0));
			builder.BindRenderPipeline(*skyboxPipeline);

			builder.DrawIndexed(Nz::SafeCast<Nz::UInt32>(cubeMeshGfx->GetIndexCount(0)));

			Nz::InstancedRenderable::ElementData elementData;
			elementData.scissorBox = &env.renderRect;
			elementData.skeletonInstance = nullptr;
			elementData.worldInstance = &flareInstance;

			std::vector<Nz::RenderElementOwner> elements;
			flareSprite.BuildElement(elementRegistry, elementData, forwardPassIndex, elements);

			std::vector<Nz::Pointer<const Nz::RenderElement>> elementPointers;
			std::vector<Nz::ElementRenderer::RenderStates> renderStates(elements.size());

			elementPointers.reserve(elements.size());
			for (const auto& element : elements)
				elementPointers.emplace_back(element.GetElement());

			spritechainRenderer.Prepare(viewerInstance, *spriteRendererData, *currentFrame, elementPointers.size(), elementPointers.data(), renderStates.data());
			spritechainRenderer.Render(viewerInstance, *spriteRendererData, builder, elementPointers.size(), elementPointers.data());
		});
		forwardPass.SetExecutionCallback([&]
		{
			return (forwardEnabled) ? Nz::FramePassExecution::Execute : Nz::FramePassExecution::Skip;
		});

		forwardPass.AddInput(lightOutput);
		forwardPass.AddOutput(lightOutput);
		forwardPass.SetDepthStencilInput(depthBuffer1);
		forwardPass.SetDepthStencilOutput(depthBuffer2);


		Nz::FramePass& occluderPass = graph.AddPass("Occluder pass");
		occluderPass.SetCommandCallback([&](Nz::CommandBufferBuilder& builder, const Nz::FramePassEnvironment& env)
		{
			builder.SetViewport(env.renderRect);

			Nz::InstancedRenderable::ElementData elementData;
			elementData.scissorBox = &env.renderRect;
			elementData.skeletonInstance = nullptr;
			elementData.worldInstance = &flareInstance;

			std::vector<Nz::RenderElementOwner> elements;
			flareSprite.BuildElement(elementRegistry, elementData, forwardPassIndex, elements);

			std::vector<Nz::Pointer<const Nz::RenderElement>> elementPointers;
			std::vector<Nz::ElementRenderer::RenderStates> renderStates(elements.size());

			elementPointers.reserve(elements.size());
			for (const auto& element : elements)
				elementPointers.emplace_back(element.GetElement());

			spritechainRenderer.Prepare(viewerInstance, *spriteRendererData, *currentFrame, elementPointers.size(), elementPointers.data(), renderStates.data());
			spritechainRenderer.PrepareEnd(*currentFrame, *spriteRendererData);
			spritechainRenderer.Render(viewerInstance, *spriteRendererData, builder, elementPointers.size(), elementPointers.data());
		});

		occluderPass.AddOutput(occluderTexture);
		occluderPass.SetClearColor(0, Nz::Color::Black());
		occluderPass.SetDepthStencilInput(depthBuffer1);

		Nz::FramePass& godraysPass = graph.AddPass("Light scattering pass");
		godraysPass.SetCommandCallback([&](Nz::CommandBufferBuilder& builder, const Nz::FramePassEnvironment& env)
		{
			builder.SetScissor(env.renderRect);
			builder.SetViewport(env.renderRect);

			builder.BindRenderShaderBinding(0, *godRaysShaderBinding);

			builder.BindRenderPipeline(*godraysPipeline);

			builder.Draw(3);
		});

		godraysPass.AddInput(occluderTexture);
		godraysPass.AddOutput(godRaysTexture);

		Nz::FramePass& bloomBrightPass = graph.AddPass("Bloom pass - extract bright pixels");
		bloomBrightPass.SetCommandCallback([&](Nz::CommandBufferBuilder& builder, const Nz::FramePassEnvironment& env)
		{
			builder.SetScissor(env.renderRect);
			builder.SetViewport(env.renderRect);

			builder.BindRenderShaderBinding(0, *bloomBrightShaderBinding);

			builder.BindRenderPipeline(*bloomBrightPipeline);

			builder.Draw(3);
		});
		bloomBrightPass.SetExecutionCallback([&]
		{
			return (bloomEnabled) ? Nz::FramePassExecution::Execute : Nz::FramePassExecution::Skip;
		});

		bloomBrightPass.AddInput(lightOutput);
		bloomBrightPass.AddOutput(bloomBrightOutput);

		std::size_t bloomTextureIndex = 0;
		for (std::size_t i = 0; i < BloomSubdivisionCount; ++i)
		{
			Nz::FramePass& bloomBlurPassHorizontal = graph.AddPass("Bloom pass - gaussian blur #" + std::to_string(i) + " - horizontal");
			bloomBlurPassHorizontal.SetCommandCallback([&, i](Nz::CommandBufferBuilder& builder, const Nz::FramePassEnvironment& env)
			{
				builder.SetScissor(env.renderRect);
				builder.SetViewport(env.renderRect);

				builder.BindRenderShaderBinding(0, *gaussianBlurShaderBinding[i * 2 + 0]);
				builder.BindRenderPipeline(*gaussianBlurPipeline);

				builder.Draw(3);
			});

			bloomBlurPassHorizontal.SetExecutionCallback([&]
			{
				return (bloomEnabled) ? Nz::FramePassExecution::Execute : Nz::FramePassExecution::Skip;
			});

			bloomBlurPassHorizontal.AddInput((i == 0) ? bloomBrightOutput : bloomTextures[bloomTextureIndex++]);
			bloomBlurPassHorizontal.AddOutput(bloomTextures[bloomTextureIndex]);

			Nz::FramePass& bloomBlurPassVertical = graph.AddPass("Bloom pass - gaussian blur #" + std::to_string(i) + " - vertical");
			bloomBlurPassVertical.SetCommandCallback([&, i](Nz::CommandBufferBuilder& builder, const Nz::FramePassEnvironment& env)
			{
				builder.SetScissor(env.renderRect);
				builder.SetViewport(env.renderRect);

				builder.BindRenderShaderBinding(0, *gaussianBlurShaderBinding[i * 2 + 1]);
				builder.BindRenderPipeline(*gaussianBlurPipeline);

				builder.Draw(3);
			});

			bloomBlurPassVertical.SetExecutionCallback([&]
			{
				return (bloomEnabled) ? Nz::FramePassExecution::Execute : Nz::FramePassExecution::Skip;
			});

			bloomBlurPassVertical.AddInput(bloomTextures[bloomTextureIndex++]);
			bloomBlurPassVertical.AddOutput(bloomTextures[bloomTextureIndex]);
		}

		Nz::FramePass& bloomBlendPass = graph.AddPass("Bloom pass - blend");
		bloomBlendPass.SetCommandCallback([&](Nz::CommandBufferBuilder& builder, const Nz::FramePassEnvironment& env)
		{
			builder.SetScissor(env.renderRect);
			builder.SetViewport(env.renderRect);

			// Blend bloom
			builder.BindRenderPipeline(*bloomBlendPipeline);
			for (std::size_t i = 0; i < BloomSubdivisionCount; ++i)
			{
				builder.BindRenderShaderBinding(0, *bloomBlendShaderBinding[i]);
				builder.Draw(3);
			}

			// God rays
			builder.BindRenderShaderBinding(0, *godRaysBlitShaderBinding);
			builder.Draw(3);
		});

		bloomBlendPass.SetExecutionCallback([&]
		{
			return (bloomEnabled) ? Nz::FramePassExecution::Execute : Nz::FramePassExecution::Skip;
		});

		bloomBlendPass.AddInput(lightOutput);
		bloomBlendPass.AddInput(godRaysTexture);
		bloomBlendPass.SetReadInput(0, false);

		for (std::size_t i = 0; i < BloomSubdivisionCount; ++i)
			bloomBlendPass.AddInput(bloomTextures[i * 2 + 1]);

		bloomBlendPass.AddOutput(bloomOutput);

		Nz::FramePass& toneMappingPass = graph.AddPass("Tone mapping");
		toneMappingPass.AddInput(bloomOutput);
		toneMappingPass.AddOutput(toneMappingOutput);
		toneMappingPass.SetCommandCallback([&](Nz::CommandBufferBuilder& builder, const Nz::FramePassEnvironment& env)
		{
			builder.SetScissor(env.renderRect);
			builder.SetViewport(env.renderRect);

			builder.BindRenderShaderBinding(0, *toneMappingShaderBinding);
			builder.BindRenderPipeline(*toneMappingPipeline);

			builder.Draw(3);
		});

		graph.AddOutput(toneMappingOutput);

		return graph.Bake();
	}();

	Nz::Vector3f viewerPos = Nz::Vector3f::Backward() * 10.f + Nz::Vector3f::Up() * 3.f;

	Nz::EulerAnglesf camAngles(-30.f, 0.f, 0.f);
	Nz::Quaternionf camQuat(camAngles);

	Nz::MillisecondClock updateClock;
	Nz::MillisecondClock fpsClock;
	unsigned int fps = 0;

	std::size_t totalFrameCount = 0;

	window.SetRelativeMouseMode(true);

	Nz::Time elapsedTime = Nz::Time::Zero();
	Nz::Time time = Nz::GetElapsedNanoseconds();

	auto ComputeLightAnimationSpeed = [](const Nz::Vector3f& position)
	{
		return position.GetLength() / 15.f;
	};

	auto AnimateLightPosition = [](const Nz::Vector3f& position, float rotationSpeed, float elapsedTime)
	{
		rotationSpeed *= 45.f;

		return Nz::Matrix4f::Rotate(Nz::EulerAnglesf(0.f, elapsedTime * rotationSpeed, 0.f)) * position;
	};

	auto AnimateLightDirection = [](const Nz::Vector3f& direction, float rotationSpeed, float elapsedTime)
	{
		rotationSpeed *= 90.f;

		return Nz::Matrix4f::Rotate(Nz::EulerAnglesf(0.f, elapsedTime * rotationSpeed, 0.f)) * direction;
	};

	window.GetEventHandler().OnEvent.Connect([&](const Nz::WindowEventHandler*, const Nz::WindowEvent& event)
	{
		switch (event.type)
		{
			case Nz::WindowEventType::MouseMoved: // La souris a bougé
			{
				// Gestion de la caméra free-fly (Rotation)
				float sensitivity = 0.3f; // Sensibilité de la souris

				// On modifie l'angle de la caméra grâce au déplacement relatif sur X de la souris
				camAngles.yaw = camAngles.yaw - event.mouseMove.deltaX*sensitivity;
				camAngles.yaw.Normalize();

				// Idem, mais pour éviter les problèmes de calcul de la matrice de vue, on restreint les angles
				camAngles.pitch = Nz::Clamp(camAngles.pitch - event.mouseMove.deltaY*sensitivity, -89.f, 89.f);

				camQuat = camAngles;
				break;
			}

			case Nz::WindowEventType::KeyPressed:
			{
				if (event.key.scancode == Nz::Keyboard::Scancode::Space)
				{
					float elapsedSeconds = elapsedTime.AsSeconds();
					float rotationSpeed = ComputeLightAnimationSpeed(viewerPos);

					auto& spotLight = spotLights.emplace_back();
					spotLight.color = Nz::Color(0.4f, 0.4f, 1.f);
					spotLight.radius = 5.f;
					spotLight.position = AnimateLightPosition(viewerPos, rotationSpeed, -elapsedSeconds);
					spotLight.direction = AnimateLightDirection(camQuat * Nz::Vector3f::Forward(), rotationSpeed, -elapsedSeconds);

					lightUpdate = true;
				}
				else if (event.key.virtualKey == Nz::Keyboard::VKey::F)
					forwardEnabled = !forwardEnabled;
				else if (event.key.virtualKey == Nz::Keyboard::VKey::A)
					lightAnimation = !lightAnimation;
				else if (event.key.virtualKey == Nz::Keyboard::VKey::B)
					bloomEnabled = !bloomEnabled;
				else if (event.key.virtualKey == Nz::Keyboard::VKey::E)
					modelInstance1.UpdateWorldMatrix(Nz::Matrix4f::Transform(viewerPos, camQuat));

				break;
			}

			case Nz::WindowEventType::Resized:
			{
				Nz::Vector2ui newSize = window.GetSize();
				viewerInstance.UpdateProjectionMatrix(Nz::Matrix4f::Perspective(Nz::DegreeAnglef(70.f), float(newSize.x) / newSize.y, 0.1f, 1000.f));
				break;
			}

			default:
				break;
		}
	});

	app.AddUpdaterFunc([&](Nz::Time deltaTime)
	{
		if (lightAnimation)
			elapsedTime += deltaTime;

		if (std::optional<Nz::Time> movementDuration = updateClock.RestartIfOver(Nz::Time::TickDuration(60)))
		{
			float cameraSpeed = 2.f * movementDuration->AsSeconds();

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Up) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Z))
				viewerPos += camQuat * Nz::Vector3f::Forward() * cameraSpeed;

			// Si la flèche du bas ou la touche S est pressée, on recule
			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Down) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::S))
				viewerPos += camQuat * Nz::Vector3f::Backward() * cameraSpeed;

			// Etc...
			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Left) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Q))
				viewerPos += camQuat * Nz::Vector3f::Left() * cameraSpeed;

			// Etc...
			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Right) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::D))
				viewerPos += camQuat * Nz::Vector3f::Right() * cameraSpeed;

			// Majuscule pour monter, notez l'utilisation d'une direction globale (Non-affectée par la rotation)
			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::LShift) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::RShift))
				viewerPos += Nz::Vector3f::Up() * cameraSpeed;

			// Contrôle (Gauche ou droite) pour descendre dans l'espace global, etc...
			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::LControl) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::RControl))
				viewerPos += Nz::Vector3f::Down() * cameraSpeed;

			viewerInstance.UpdateViewMatrix(Nz::Matrix4f::TransformInverse(viewerPos, camQuat));
		}

		Nz::RenderFrame frame = windowSwapchain.AcquireFrame();
		if (!frame)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			return;
		}

		currentFrame = &frame;

		std::array<Nz::Vector2ui, 1> sizes = { currentFrame->GetSize() };

		if (bakedGraph.Resize(frame, sizes))
		{
			frame.PushForRelease(std::move(gbufferShaderBinding));

			gbufferShaderBinding = lightingPipelineInfo.pipelineLayout->AllocateShaderBinding(0);
			gbufferShaderBinding->Update({
				{
					0,
					Nz::ShaderBinding::UniformBufferBinding {
						viewerInstance.GetViewerBuffer().get(),
						0, viewerInstance.GetViewerBuffer()->GetSize()
					}
				},
				{
					1,
					Nz::ShaderBinding::SampledTextureBinding {
						bakedGraph.GetAttachmentTexture(colorTexture).get(),
						textureSampler.get()
					}
				},
				{
					2,
					Nz::ShaderBinding::SampledTextureBinding {
						bakedGraph.GetAttachmentTexture(normalTexture).get(),
						textureSampler.get()
					}
				},
				{
					3,
					Nz::ShaderBinding::SampledTextureBinding {
						bakedGraph.GetAttachmentTexture(positionTexture).get(),
						textureSampler.get()
					}
				}
			});

			frame.PushForRelease(std::move(lightingShaderBindings));
			lightingShaderBindings.clear();

			for (std::size_t i = 0; i < MaxPointLight; ++i)
			{
				Nz::ShaderBindingPtr lightingShaderBinding = lightingPipelineInfo.pipelineLayout->AllocateShaderBinding(1);
				lightingShaderBinding->Update({
					{
						0,
						Nz::ShaderBinding::UniformBufferBinding {
							lightUbo.get(),
							i * alignedSpotLightSize, spotLightOffsets.GetAlignedSize()
						}
					}
				});

				lightingShaderBindings.emplace_back(std::move(lightingShaderBinding));
			}

			frame.PushForRelease(std::move(bloomBrightShaderBinding));

			bloomBrightShaderBinding = fullscreenPipelineInfoViewer.pipelineLayout->AllocateShaderBinding(0);
			bloomBrightShaderBinding->Update({
				{
					0,
					Nz::ShaderBinding::UniformBufferBinding {
						viewerInstance.GetViewerBuffer().get(),
						0, viewerInstance.GetViewerBuffer()->GetSize()
					}
				},
				{
					1,
					Nz::ShaderBinding::SampledTextureBinding {
						bakedGraph.GetAttachmentTexture(lightOutput).get(),
						textureSampler.get()
					}
				}
			});

			std::size_t bloomTextureIndex = 0;
			for (std::size_t i = 0; i < BloomSubdivisionCount; ++i)
			{
				for (std::size_t j = 0; j < 2; ++j)
				{
					frame.PushForRelease(std::move(gaussianBlurShaderBinding[i * 2 + j]));

					gaussianBlurShaderBinding[i * 2 + j] = gaussianBlurPipeline->GetPipelineInfo().pipelineLayout->AllocateShaderBinding(0);
					gaussianBlurShaderBinding[i * 2 + j]->Update({
						{
							0,
							Nz::ShaderBinding::UniformBufferBinding {
								viewerInstance.GetViewerBuffer().get(),
								0, viewerInstance.GetViewerBuffer()->GetSize()
							}
						},
						{
							1,
							Nz::ShaderBinding::SampledTextureBinding {
								bakedGraph.GetAttachmentTexture((i == 0 && j == 0) ? bloomBrightOutput : bloomTextures[bloomTextureIndex++]).get(),
								textureSampler.get()
							}
						},
						{
							2,
							Nz::ShaderBinding::UniformBufferBinding {
								gaussianBlurUbos[i * 2 + j].get(),
								0, gaussianBlurUbos[i * 2 + j]->GetSize()
							}
						}
					});
				}

				frame.PushForRelease(std::move(bloomBlendShaderBinding[i]));

				bloomBlendShaderBinding[i] = bloomBlendPipelineInfo.pipelineLayout->AllocateShaderBinding(0);
				bloomBlendShaderBinding[i]->Update({
					{
						0,
						Nz::ShaderBinding::UniformBufferBinding {
							viewerInstance.GetViewerBuffer().get(),
							0, viewerInstance.GetViewerBuffer()->GetSize()
						}
					},
					/*{
						1,
						Nz::ShaderBinding::TextureBinding {
							bakedGraph.GetAttachmentTexture(lightOutput).get(),
							textureSampler.get()
						}
					},*/
					{
						2,
						Nz::ShaderBinding::SampledTextureBinding {
							bakedGraph.GetAttachmentTexture(bloomTextures[i * 2 + 1]).get(),
							textureSampler.get()
						}
					}
				});
			}

			frame.PushForRelease(std::move(bloomBlitBinding));

			bloomBlitBinding = Nz::Graphics::Instance()->GetBlitPipelineLayout()->AllocateShaderBinding(0);
			bloomBlitBinding->Update({
				{
					0,
					Nz::ShaderBinding::SampledTextureBinding {
						bakedGraph.GetAttachmentTexture(lightOutput).get(),
						textureSampler.get()
					}
				}
			});

			frame.PushForRelease(std::move(bloomSkipBlit));

			bloomSkipBlit = fullscreenPipelineInfo.pipelineLayout->AllocateShaderBinding(0);
			bloomSkipBlit->Update({
				{
					0,
					Nz::ShaderBinding::SampledTextureBinding {
						bakedGraph.GetAttachmentTexture(lightOutput).get(),
						textureSampler.get()
					}
				}
			});

			frame.PushForRelease(std::move(godRaysShaderBinding));

			godRaysShaderBinding = godraysPipelineInfo.pipelineLayout->AllocateShaderBinding(0);
			godRaysShaderBinding->Update({
				{
					0,
					Nz::ShaderBinding::UniformBufferBinding {
						viewerInstance.GetViewerBuffer().get(),
						0, viewerInstance.GetViewerBuffer()->GetSize()
					}
				},
				{
					1,
					Nz::ShaderBinding::UniformBufferBinding {
						godRaysUBO.get(),
						0, godRaysUBO->GetSize()
					}
				},
				{
					2,
					Nz::ShaderBinding::SampledTextureBinding {
						bakedGraph.GetAttachmentTexture(occluderTexture).get(),
						textureSampler.get()
					}
				}
			});

			frame.PushForRelease(std::move(toneMappingShaderBinding));

			toneMappingShaderBinding = fullscreenPipelineInfoViewer.pipelineLayout->AllocateShaderBinding(0);
			toneMappingShaderBinding->Update({
				{
					0,
					Nz::ShaderBinding::UniformBufferBinding {
						viewerInstance.GetViewerBuffer().get(),
						0, viewerInstance.GetViewerBuffer()->GetSize()
					}
				},
				{
					1,
					Nz::ShaderBinding::SampledTextureBinding {
						bakedGraph.GetAttachmentTexture(bloomOutput).get(),
						textureSampler.get()
					}
				}
			});

			frame.PushForRelease(std::move(godRaysBlitShaderBinding));

			godRaysBlitShaderBinding = bloomBlendPipelineInfo.pipelineLayout->AllocateShaderBinding(0);
			godRaysBlitShaderBinding->Update({
				{
					0,
					Nz::ShaderBinding::UniformBufferBinding {
						viewerInstance.GetViewerBuffer().get(),
						0, viewerInstance.GetViewerBuffer()->GetSize()
					}
				},
				/*{
					1,
					Nz::ShaderBinding::TextureBinding {
						bakedGraph.GetAttachmentTexture(lightOutput).get(),
						textureSampler.get()
					}
				},*/
				{
					2,
					Nz::ShaderBinding::SampledTextureBinding {
						bakedGraph.GetAttachmentTexture(godRaysTexture).get(),
						textureSampler.get()
					}
				}
			});

			frame.PushForRelease(std::move(finalBlitBinding));

			finalBlitBinding = fullscreenPipelineInfo.pipelineLayout->AllocateShaderBinding(0);
			finalBlitBinding->Update({
				{
					0,
					Nz::ShaderBinding::SampledTextureBinding {
						bakedGraph.GetAttachmentTexture(toneMappingOutput).get(),
						textureSampler.get()
					}
				}
			});
		}

		Nz::UploadPool& uploadPool = frame.GetUploadPool();

		frame.Execute([&](Nz::CommandBufferBuilder& builder)
		{
			builder.BeginDebugRegion("UBO Update", Nz::Color::Yellow());
			{
				builder.PreTransferBarrier();

				modelInstance1.OnTransfer(frame, builder);
				modelInstance2.OnTransfer(frame, builder);
				planeInstance.OnTransfer(frame, builder);

				float elapsedSeconds = elapsedTime.AsSeconds();

				Nz::EulerAnglesf flareRotation(0.f, 0.f, elapsedSeconds * 10.f);
				flareInstance.UpdateWorldMatrix(Nz::Matrix4f::Transform(viewerPos + flarePosition, flareRotation));
				flareInstance.OnTransfer(frame, builder);

				viewerInstance.OnTransfer(frame, builder);

				// Update light buffer
				if (!spotLights.empty() && (lightUpdate || lightAnimation))
				{
					auto& lightDataAllocation = uploadPool.Allocate(alignedSpotLightSize * spotLights.size());
					Nz::UInt8* lightDataPtr = static_cast<Nz::UInt8*>(lightDataAllocation.mappedPtr);

					for (const SpotLight& spotLight : spotLights)
					{
						float rotationSpeed = ComputeLightAnimationSpeed(spotLight.position);

						Nz::Vector3f position = AnimateLightPosition(spotLight.position, rotationSpeed, elapsedSeconds);
						Nz::Vector3f direction = AnimateLightDirection(spotLight.direction, rotationSpeed, elapsedSeconds);

						Nz::AccessByOffset<Nz::Vector3f&>(lightDataPtr, colorOffset) = Nz::Vector3f(spotLight.color.r, spotLight.color.g, spotLight.color.b);
						Nz::AccessByOffset<Nz::Vector3f&>(lightDataPtr, positionOffset) = position;
						Nz::AccessByOffset<Nz::Vector3f&>(lightDataPtr, directionOffset) = direction;
						Nz::AccessByOffset<float&>(lightDataPtr, radiusOffset) = spotLight.radius;
						Nz::AccessByOffset<float&>(lightDataPtr, invRadiusOffset) = 1.f / spotLight.radius;
						Nz::AccessByOffset<float&>(lightDataPtr, innerAngleOffset) = spotLight.innerAngle.GetCos();
						Nz::AccessByOffset<float&>(lightDataPtr, outerAngleOffset) = spotLight.outerAngle.GetCos();

						float baseRadius = spotLight.radius * spotLight.outerAngle.GetTan() * 1.1f;
						Nz::Matrix4f transformMatrix = Nz::Matrix4f::Transform(position, Nz::Quaternionf::RotationBetween(Nz::Vector3f::Forward(), direction), Nz::Vector3f(baseRadius, baseRadius, spotLight.radius));
						Nz::AccessByOffset<Nz::Matrix4f&>(lightDataPtr, transformMatrixOffset) = transformMatrix;

						lightDataPtr += alignedSpotLightSize;
					}

					builder.CopyBuffer(lightDataAllocation, lightUbo.get());
				}

				// Update light scattering buffer
				{
					Nz::Vector4f pos(flareInstance.GetWorldMatrix().GetTranslation(), 1.f);
					pos = viewerInstance.GetViewMatrix() * pos;
					pos = viewerInstance.GetProjectionMatrix() * pos;
					pos /= pos.w;

					Nz::Vector2f& lightPosition = Nz::AccessByOffset<Nz::Vector2f&>(godRaysData.data(), gr_lightPositionOffset);
					lightPosition = Nz::Vector2f(pos.x * 0.5f + 0.5f, pos.y * 0.5f + 0.5f);
					lightPosition.x = Nz::Clamp(std::abs(lightPosition.x), -0.5f, 1.5f);
					lightPosition.y = Nz::Clamp(std::abs(lightPosition.y), -0.5f, 1.5f);

					auto& lightScatteringAllocation = uploadPool.Allocate(godRaysData.size());
					Nz::UInt8* dataPtr = static_cast<Nz::UInt8*>(lightScatteringAllocation.mappedPtr);

					std::memcpy(dataPtr, godRaysData.data(), godRaysData.size());

					builder.CopyBuffer(lightScatteringAllocation, godRaysUBO.get());
				}

				spaceshipMat->OnTransfer(frame, builder);
				planeMat->OnTransfer(frame, builder);
				flareMaterial->OnTransfer(frame, builder);

				builder.PostTransferBarrier();
			}
			builder.EndDebugRegion();
		}, Nz::QueueType::Transfer);

		bakedGraph.Execute(frame);

		const Nz::WindowSwapchain* windowRT = &windowSwapchain;
		frame.Execute([&](Nz::CommandBufferBuilder& builder)
		{
			Nz::Recti windowRenderRect(0, 0, window.GetSize().x, window.GetSize().y);

			builder.TextureBarrier(Nz::PipelineStage::ColorOutput, Nz::PipelineStage::FragmentShader, Nz::MemoryAccess::ColorWrite, Nz::MemoryAccess::ShaderRead, Nz::TextureLayout::ColorOutput, Nz::TextureLayout::ColorInput, *bakedGraph.GetAttachmentTexture(toneMappingOutput));

			Nz::CommandBufferBuilder::ClearValues clearValues[2];
			clearValues[0].color = Nz::Color::Black();
			clearValues[1].depth = 1.f;
			clearValues[1].stencil = 0;

			builder.BeginRenderPass(windowRT->GetFramebuffer(frame.GetImageIndex()), windowRT->GetRenderPass(), windowRenderRect, { clearValues[0], clearValues[1] });
			{
				builder.BeginDebugRegion("Main window rendering", Nz::Color::Green());
				{
					builder.SetScissor(Nz::Recti{ 0, 0, int(windowSize.x), int(windowSize.y) });
					builder.SetViewport(Nz::Recti{ 0, 0, int(windowSize.x), int(windowSize.y) });

					builder.BindRenderShaderBinding(0, *finalBlitBinding);
					builder.BindRenderPipeline(*fullscreenPipeline);

					builder.Draw(3);
				}
				builder.EndDebugRegion();
			}
			builder.EndRenderPass();
		}, Nz::QueueType::Graphics);

		frame.Present();

		lightUpdate = false;

		// On incrémente le compteur de FPS improvisé
		fps++;
		totalFrameCount++;

		if (fpsClock.RestartIfOver(Nz::Time::Second()))
		{
			window.SetTitle(windowTitle + " - " + Nz::NumberToString(fps) + " FPS");
			fps = 0;
		}
	});

	return app.Run();
}
