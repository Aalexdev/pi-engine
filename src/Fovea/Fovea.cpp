#include "Fovea/core.hpp"
#include "Fovea/Fovea.h"

#include "Fovea/Pipeline.hpp"
#include "Fovea/RenderTarget.hpp"

using namespace Fovea;

VkFormat FoveaImageFormatToVkFormat(FoveaImageFormat format){
	switch (format){
		case FoveaImageFormat_R8: return VK_FORMAT_R8_UINT;
		case FoveaImageFormat_R8G8: return VK_FORMAT_R8G8_UINT;
		case FoveaImageFormat_R8G8B8: return VK_FORMAT_R8G8B8_UINT;
		case FoveaImageFormat_R8G8B8A8: return VK_FORMAT_R8G8B8A8_UINT;
		case FoveaImageFormat_R16: return VK_FORMAT_R16_UINT;
		case FoveaImageFormat_R16G16: return VK_FORMAT_R16G16_UINT;
		case FoveaImageFormat_R16G16B16: return VK_FORMAT_R16G16B16_UINT;
		case FoveaImageFormat_R16G16B16A16: return VK_FORMAT_R16G16B16A16_UINT;
		case FoveaImageFormat_R32: return VK_FORMAT_R32_UINT;
		case FoveaImageFormat_R32G32: return VK_FORMAT_R32G32_UINT;
		case FoveaImageFormat_R32G32B32: return VK_FORMAT_R32G32B32_UINT;
		case FoveaImageFormat_R32G32B32A32: return VK_FORMAT_R32G32B32A32_UINT;
		case FoveaImageFormat_R64: return VK_FORMAT_R64_UINT;
		case FoveaImageFormat_R64G64: return VK_FORMAT_R64G64_UINT;
		case FoveaImageFormat_R64G64B64: return VK_FORMAT_R64G64B64_UINT;
		case FoveaImageFormat_R64G64B64A64: return VK_FORMAT_R64G64B64A64_UINT;
		case FoveaImageFormat_R4G4: return VK_FORMAT_R4G4_UNORM_PACK8;
		case FoveaImageFormat_R4G4B4A4: return VK_FORMAT_R4G4B4A4_UNORM_PACK16;
		case FoveaImageFormat_R5G5B5A1: return VK_FORMAT_R5G5B5A1_UNORM_PACK16;
		case FoveaImageFormat_R5B6G5: return VK_FORMAT_R5G6B5_UNORM_PACK16;
		case FoveaImageFormat_D32: return VK_FORMAT_D32_SFLOAT;
		case FoveaImageFormat_D32_S8: return VK_FORMAT_D32_SFLOAT_S8_UINT;
		case FoveaImageFormat_D24_S8: return VK_FORMAT_D24_UNORM_S8_UINT;
	}
	return VK_FORMAT_R8G8B8_UINT;
}

VkImageTiling FoveaImageTilingToVulkanImageTiling(FoveaImageTiling tiling){
	switch (tiling){
		case FoveaImageTiling_Linear: VK_IMAGE_TILING_LINEAR;
		case FoveaImageTiling_Nearest: VK_IMAGE_TILING_OPTIMAL;
	}
	return VK_IMAGE_TILING_LINEAR;
}

VkSampleCountFlagBits FoveaSampleToVkSample(FoveaSample sample){
	switch (sample){
		case FoveaSample_1: return VK_SAMPLE_COUNT_1_BIT;
		case FoveaSample_2: return VK_SAMPLE_COUNT_2_BIT;
		case FoveaSample_4: return VK_SAMPLE_COUNT_4_BIT;
		case FoveaSample_8: return VK_SAMPLE_COUNT_8_BIT;
		case FoveaSample_16: return VK_SAMPLE_COUNT_16_BIT;
		case FoveaSample_32: return VK_SAMPLE_COUNT_32_BIT;
		case FoveaSample_64: return VK_SAMPLE_COUNT_64_BIT;
	}
	return VK_SAMPLE_COUNT_1_BIT;
}

int FoveaShaderTypeToPipelineStage(FoveaShaderType type){
	switch (type){
		case FoveaShaderType_Graphic: return VK_SHADER_STAGE_ALL_GRAPHICS;
		case FoveaShaderType_Compute: return VK_SHADER_STAGE_COMPUTE_BIT;
	}
	return 0;
}

RenderTarget::ClearColor FoveaColorToRenderTargetClearColor(FoveaColor color){
	return {color.r, color.g, color.b, color.a};
}

VkExtent2D FoveaUIVec2ToVkExtent(FoveaUIVec2 vec){
	return {vec.width, vec.height};
}

void initializeInstance(void *window){
	InstanceBuilder builder;

	builder.enableValidationLayers(true);
	builder.requireValidationLayer("VK_LAYER_KHRONOS_validation");

	getInstance().instance.initialize(builder, window);
}

void initializePhysicalDevice(){
	PhysicalDeviceBuidler builder;

	builder.requireExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	builder.requireFeature(PhysicalDeviceFeature::WIDE_LINES);
	builder.requireFeature(PhysicalDeviceFeature::LARGE_POINTS);
	builder.requireFeature(PhysicalDeviceFeature::SHADER_FLOAT_64);

	builder.requireFamily(PhysicalDeviceFamily::FAMILY_GRAPHIC);
	builder.requireFamily(PhysicalDeviceFamily::FAMILY_PRESENT);
	getInstance().physicalDevice.initialize(builder);
}

void initializeLogicalDevice(){
	LogicalDeviceBuilder builder;
	builder.requireQueue(PhysicalDeviceFamily::FAMILY_GRAPHIC);
	builder.requireQueue(PhysicalDeviceFamily::FAMILY_PRESENT);
	builder.requireExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	builder.setQueuePriority(PhysicalDeviceFamily::FAMILY_GRAPHIC, 1);
	getInstance().logicalDevice.initialize(builder);
}

void initializeRenderCommandPool(){
	CommandPoolBuilder builder;

	builder.setFamily(PhysicalDeviceFamily::FAMILY_GRAPHIC);
	builder.setFlag(CommandPoolFlags::FLAG_TRANSIENT | CommandPoolFlags::FLAG_RESET_BUFFER);
	getInstance().renderCommandPool.initialize(builder);
}

void initializeRenderer(){
	Renderer &renderer = getInstance().renderer;

	renderer.initialize();

	renderer.setClearColor(0.1, 0.1, 0.1, 1.0);
}
struct PushConstant{
	int a;
};

void FoveaInitialize(void* window){
	initializeInstance(window);
	initializePhysicalDevice();
	initializeLogicalDevice();
	initializeRenderCommandPool();
	initializeRenderer();
}

void FoveaSetClearColor(const FoveaColor color){
	getInstance().renderer.setClearColor(color.r, color.g, color.b, color.a);
}

void FoveaOnWindowResized(uint32_t width, uint32_t height){
	getInstance().renderer.windowResized(width, height);
}

VkCommandBuffer& frameCommandBuffer(){
	static VkCommandBuffer commandBuffer;
	return commandBuffer;
}

void FoveaBeginFrame(){
	frameCommandBuffer() = getInstance().renderer.beginFrame();
}

void FoveaEndFrame(){
	Renderer &renderer = getInstance().renderer;
	{
		VkCommandBuffer& commandBuffer = frameCommandBuffer();
		renderer.beginSwapChainRenderPass(commandBuffer);

		auto pipeline = getInstance().pipelineLibrary.get(0);
		pipeline->bind(frameCommandBuffer());

		vkCmdDraw(frameCommandBuffer(), 3, 1, 0, 0);

		renderer.endSwapChainRenderPass(commandBuffer);
	}

	renderer.endFrame();
}

void FoveaDefaultShaderCreateInfo(FoveaShaderCreateInfo *createInfo){
	createInfo->sample = FoveaSample_1;
	createInfo->type = FoveaShaderType_Graphic;
	createInfo->pushConstantSize = 0;
	createInfo->base = FOVEA_NONE;
	createInfo->target = FOVEA_NONE;
}

FoveaShader FoveaCreateShader(const char *name, FoveaShaderCreateInfo *createInfo){
	PipelineBuilder builder;

	if (createInfo->vertexFilepath != nullptr) builder.setShaderStage(PipelineStage::VERTEX, createInfo->vertexFilepath);
	if (createInfo->fragmentFilepath != nullptr) builder.setShaderStage(PipelineStage::FRAGMENT, createInfo->fragmentFilepath);
	if (createInfo->geometryFilepath != nullptr) builder.setShaderStage(PipelineStage::GEOMETRY, createInfo->geometryFilepath);
	if (createInfo->computeFilepath != nullptr) builder.setShaderStage(PipelineStage::COMPUTE, createInfo->computeFilepath);
	builder->multisampleInfo.rasterizationSamples = FoveaSampleToVkSample(createInfo->sample);

	if (createInfo->target != FOVEA_NONE){
		builder.setRenderPass(getInstance().renderTargetLibrary.get(createInfo->target)->getRenderPass());
	} else {
		builder.setRenderPass(getInstance().renderer.getSwapChain().getRenderPass());
	}
	
	builder.setPushConstant(createInfo->pushConstantSize, FoveaShaderTypeToPipelineStage(createInfo->type));

	if (createInfo->base != FOVEA_NONE){
		builder.setBase(getInstance().pipelineLibrary.get(createInfo->base));
	}

	return static_cast<FoveaShader>(getInstance().pipelineLibrary.push(&builder, name));
}

void FoveaDestroyShader(FoveaShader shader){
	getInstance().pipelineLibrary.erase(shader);
}

FoveaShader FoveaGetShaderFromName(const char *name){
	return {getInstance().pipelineLibrary.getIDFromName(name)};
}

void FoveaUseShader(FoveaShader shader){
	auto pipeline = getInstance().pipelineLibrary.get(shader);
	pipeline->bind(frameCommandBuffer());
}

void FoveaSetShaderPushConstant(FoveaShader shader, void *data){
	getInstance().pipelineLibrary.get(shader)->bindPushConstant(frameCommandBuffer(), data);
}

FoveaRenderTarget FoveaCreateRenderTarget(const char *name, FoveaRenderTargetCreateInfo *createInfo){
	RenderTargetBuilder builder;

	FramebufferAttachments attachments;

	attachments.attachments.resize(createInfo->colorAttachmentCount);

	for (uint32_t i=0; i<createInfo->colorAttachmentCount; i++){
		auto &attachment = attachments.attachments[i];
		auto &info = createInfo->colorAttachments[i];

		attachment.format = FoveaImageFormatToVkFormat(info.format);
		attachment.tiling = FoveaImageTilingToVulkanImageTiling(info.tiling);
		attachment.samples = FoveaSampleToVkSample(info.sample);
	}

	builder->setAttachments(attachments);
	builder->setExtent(FoveaUIVec2ToVkExtent(createInfo->size));

	for (uint32_t i=0; i<createInfo->colorAttachmentCount; i++){
		builder.setAttachmentClearColor(i, FoveaColorToRenderTargetClearColor(createInfo->colorAttachments[i].clearColor));
	}

	if (createInfo->depthStencilEnabled == Fovea_True){
		builder->enableDepthAttachment(FoveaImageFormatToVkFormat(createInfo->depthFormat));
		builder.setDepthStencilClearColor(createInfo->depthClearValue, createInfo->stencilClearValue);
	}

	return static_cast<FoveaRenderTarget>(getInstance().renderTargetLibrary.push(&builder, name));
}

void FoveaDestroyRenderTarget(FoveaRenderTarget renderTarget){
	getInstance().renderTargetLibrary.erase(renderTarget);
}

FoveaRenderTarget getRenderTargetFromName(const char *name){
	return getInstance().renderTargetLibrary.getIDFromName(name);
}

void FoveaBeginRenderTarget(FoveaRenderTarget renderTarget){
	getInstance().renderTargetLibrary.get(renderTarget)->beginRenderPass(frameCommandBuffer());
}

void FoveaEndRenderTarget(FoveaRenderTarget renderTarget){
	getInstance().renderTargetLibrary.get(renderTarget)->endRenderPass(frameCommandBuffer());
}