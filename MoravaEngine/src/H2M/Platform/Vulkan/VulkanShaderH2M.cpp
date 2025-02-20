/**
 * @package H2M
 * @author  Yan Chernikov (TheCherno)
 * @licence Apache License 2.0
 */

#include "VulkanShaderH2M.h"

#include "H2M/Core/AssertH2M.h"
#include "H2M/Platform/Vulkan/VulkanContextH2M.h"
#include "H2M/Platform/Vulkan/VulkanTextureH2M.h"
#include "H2M/Renderer/RendererH2M.h"

#include "Core/Log.h"

#include <shaderc/shaderc.hpp>
#include <spirv_glsl.hpp>

#include <filesystem>


namespace H2M
{

	static ShaderUniformTypeH2M SPIRTypeToShaderUniformType(spirv_cross::SPIRType type)
	{
		switch (type.basetype)
		{
			case spirv_cross::SPIRType::Boolean:  return ShaderUniformTypeH2M::Bool;
			case spirv_cross::SPIRType::Int:      return ShaderUniformTypeH2M::Int;
			case spirv_cross::SPIRType::Float:
				if (type.vecsize == 1)            return ShaderUniformTypeH2M::Float;
				if (type.vecsize == 2)            return ShaderUniformTypeH2M::Vec2;
				if (type.vecsize == 3)            return ShaderUniformTypeH2M::Vec3;
				if (type.vecsize == 4)            return ShaderUniformTypeH2M::Vec4;

				if (type.columns == 3)            return ShaderUniformTypeH2M::Mat3;
				if (type.columns == 4)            return ShaderUniformTypeH2M::Mat4;
				break;
		}
		H2M_CORE_ASSERT(false, "Unknown type!");
		return ShaderUniformTypeH2M::None;
	}

	static std::unordered_map<uint32_t, std::unordered_map<uint32_t, VulkanShaderH2M::UniformBufferH2M*>> s_UniformBuffers; // set -> binding point -> buffer

	// Very temporary attribute in Vulkan Week Day 5 Part 1
	// Hazel::Ref<Hazel::HazelTexture2D> VulkanShaderH2M::s_AlbedoTexture;
	// Hazel::Ref<Hazel::HazelTexture2D> VulkanShaderH2M::s_NormalTexture;

	VulkanShaderH2M::VulkanShaderH2M(const std::string& path, bool forceCompile)
		: m_AssetPath(path)
	{
		// TODO: This should be more "general"
		size_t found = path.find_last_of("/\\");
		m_Name = found != std::string::npos ? path.substr(found + 1) : path;
		found = m_Name.find_last_of(".");
		m_Name = found != std::string::npos ? m_Name.substr(0, found) : m_Name;

		Reload();
	}

	VulkanShaderH2M::~VulkanShaderH2M() {}

	static std::string ReadShaderFromFile(const std::string& filepath)
	{
		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
		}
		else
		{
			H2M_CORE_ASSERT(false, "Could not load shader!");
		}
		in.close();
		return result;
	}

	void VulkanShaderH2M::Reload(bool forceCompile)
	{
		// Ref<VulkanShader> instance = this;
		// HazelRenderer::Submit([instance]() mutable
		// {
		// });
		{
			// Vertex and Fragment for now
			std::string source = ReadShaderFromFile(m_AssetPath);
			m_ShaderSource = PreProcess(source);
			std::unordered_map<VkShaderStageFlagBits, std::vector<uint32_t>> shaderData;
			CompileOrGetVulkanBinary(shaderData, forceCompile);
			LoadAndCreateShaders(shaderData);
			ReflectAllShaderStages(shaderData);
			CreateDescriptors();
		}
	}

	void VulkanShaderH2M::LoadAndCreateShaders(const std::unordered_map<VkShaderStageFlagBits, std::vector<uint32_t>>& shaderData)
	{
		VkDevice device = VulkanContextH2M::GetCurrentDevice()->GetVulkanDevice();

		m_PipelineShaderStageCreateInfos.clear();

		for (auto [stage, data] : shaderData)
		{
			H2M_CORE_ASSERT(data.size());

			// Create a new shader module that will be used for pipeline creation
			VkShaderModuleCreateInfo moduleCreateInfo{};
			moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			moduleCreateInfo.codeSize = data.size() * sizeof(uint32_t);
			moduleCreateInfo.pCode = data.data();

			VkShaderModule shaderModule;
			VK_CHECK_RESULT_H2M(vkCreateShaderModule(device, &moduleCreateInfo, NULL, &shaderModule));

			VkPipelineShaderStageCreateInfo& shaderStage = m_PipelineShaderStageCreateInfos.emplace_back();
			shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStage.stage = stage;
			shaderStage.module = shaderModule;
			shaderStage.pName = "main";
		}
	}

	void VulkanShaderH2M::ReflectAllShaderStages(const std::unordered_map<VkShaderStageFlagBits, std::vector<uint32_t>>& shaderData)
	{
		for (auto [stage, data] : shaderData)
		{
			Reflect(stage, data);
		}
	}

	void VulkanShaderH2M::Reflect(VkShaderStageFlagBits shaderStage, const std::vector<uint32_t>& shaderData)
	{
		VkDevice device = VulkanContextH2M::GetCurrentDevice()->GetVulkanDevice();

		std::string shaderStageName = "UNKNOWN";
		switch (shaderStage)
		{
			case VK_SHADER_STAGE_VERTEX_BIT:   shaderStageName = "VERTEX";   break;
			case VK_SHADER_STAGE_FRAGMENT_BIT: shaderStageName = "FRAGMENT"; break;
			case VK_SHADER_STAGE_COMPUTE_BIT:  shaderStageName = "COMPUTE";  break;
		}

		MORAVA_CORE_TRACE("==========================");
		MORAVA_CORE_TRACE(" Vulkan Shader Reflection (Stage: " + shaderStageName  + ")");
		MORAVA_CORE_TRACE(" {0}", m_AssetPath);
		MORAVA_CORE_TRACE("==========================");

		// Vertex Shader
		spirv_cross::Compiler compiler(shaderData);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		MORAVA_CORE_TRACE("Uniform Buffers:");
		for (const spirv_cross::Resource& resource : resources.uniform_buffers)
		{
			const auto& name = resource.name;
			auto& bufferType = compiler.get_type(resource.base_type_id);
			int memberCount = static_cast<uint32_t>(bufferType.member_types.size());
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			uint32_t descriptorSet = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
			uint32_t size = static_cast<uint32_t>(compiler.get_declared_struct_size(bufferType));

			ShaderDescriptorSetH2M& shaderDescriptorSet = m_ShaderDescriptorSets[descriptorSet];
			H2M_CORE_ASSERT(shaderDescriptorSet.UniformBuffers.find(binding) == shaderDescriptorSet.UniformBuffers.end());

			// UniformBuffer& buffer = shaderDescriptorSet.UniformBuffers[bindingPoint];
			UniformBufferH2M& buffer = shaderDescriptorSet.UniformBuffers[binding];
			// UniformBuffer buffer;
			buffer.BindingPoint = binding;
			buffer.DescriptorSet = descriptorSet;
			buffer.Size = size;
			// AllocateUniformBuffer(buffer);
			buffer.Name = name;
			buffer.ShaderStage = shaderStage;
			// m_UniformBuffers.insert(std::pair(bindingPoint, buffer));

			MORAVA_CORE_TRACE("    {0} ({1}, {2})", name, descriptorSet, binding);

			MORAVA_CORE_TRACE("  Name: {0}", name);
			MORAVA_CORE_TRACE("  Member Count: {0}", memberCount);
			MORAVA_CORE_TRACE("  Descriptor Set: {0}", descriptorSet);
			MORAVA_CORE_TRACE("  Binding Point: {0}", binding);
			MORAVA_CORE_TRACE("  Size: {0}", size);
			MORAVA_CORE_TRACE("--------------------------");
		}

		MORAVA_CORE_TRACE("Push Constant Buffers:");
		for (const auto& resource : resources.push_constant_buffers)
		{
			const auto& bufferName = resource.name;
			auto& bufferType = compiler.get_type(resource.base_type_id);
			auto bufferSize = compiler.get_declared_struct_size(bufferType);
			int memberCount = static_cast<int>(bufferType.member_types.size());

			uint32_t bufferOffset = 0;
			if (m_PushConstantRanges.size())
			{
				bufferOffset = m_PushConstantRanges.back().Offset + m_PushConstantRanges.back().Size;
			}

			auto& pushConstantRange = m_PushConstantRanges.emplace_back();
			pushConstantRange.ShaderStage = shaderStage;
			pushConstantRange.Size = static_cast<uint32_t>(bufferSize);
			pushConstantRange.Offset = bufferOffset;

			// Skip empty push constant buffers - these are for the renderer only
			if (bufferName.empty())
			{
				continue;
			}

			ShaderBufferH2M& buffer = m_Buffers[bufferName];
			buffer.Name = bufferName;
			buffer.Size = static_cast<uint32_t>(bufferSize - bufferOffset);

			// MORAVA_CORE_TRACE("    {0} ({1}, {2})", name, descriptorSet, binding);

			MORAVA_CORE_TRACE("  Name: {0}", bufferName);
			MORAVA_CORE_TRACE("  Member Count: {0}", memberCount);
			// MORAVA_CORE_TRACE("  Binding Point: {0}", bindingPoint);
			MORAVA_CORE_TRACE("  Buffer size: {0}", bufferSize);
			MORAVA_CORE_TRACE("--------------------------");

			for (int i = 0; i < memberCount; i++)
			{
				auto type = compiler.get_type(bufferType.member_types[i]);
				const auto& memberName = compiler.get_member_name(bufferType.self, i);
				auto size = compiler.get_declared_struct_member_size(bufferType, i);
				auto offset = compiler.type_struct_member_offset(bufferType, i) - bufferOffset;

				std::string uniformName = bufferName + "." + memberName;
				buffer.Uniforms[uniformName] = ShaderUniformH2M(uniformName, SPIRTypeToShaderUniformType(type), static_cast<uint32_t>(size), offset);
			}
		}

		MORAVA_CORE_TRACE("Sampled Images:");
		for (const auto& resource : resources.sampled_images)
		{
			const auto& name = resource.name;
			auto& type = compiler.get_type(resource.base_type_id);
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			uint32_t descriptorSet = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
			uint32_t dimension = type.image.dim;

			ShaderDescriptorSetH2M& shaderDescriptorSet = m_ShaderDescriptorSets[descriptorSet];
			H2M_CORE_ASSERT(shaderDescriptorSet.ImageSamplers.find(binding) == shaderDescriptorSet.ImageSamplers.end());

			auto& imageSampler = shaderDescriptorSet.ImageSamplers[binding];
			// ImageSampler imageSampler;
			imageSampler.BindingPoint = binding;
			imageSampler.DescriptorSet = descriptorSet;
			imageSampler.Name = name;
			imageSampler.ShaderStage = shaderStage;
			// m_ImageSamplers.insert(std::pair(bindingPoint, imageSampler));

			MORAVA_CORE_TRACE("    {0} ({1}, {2})", name, descriptorSet, binding);

			MORAVA_CORE_TRACE("  Name: {0}", name);
			// MORAVA_CORE_TRACE("  Member Count: {0}", memberCount);
			MORAVA_CORE_TRACE("  Descriptor Set: {0}", descriptorSet);
			MORAVA_CORE_TRACE("  Binding Point: {0}", binding);
			// MORAVA_CORE_TRACE("  Size: {0}", size);
			MORAVA_CORE_TRACE("--------------------------");
		}

		MORAVA_CORE_TRACE("Storage Images:");
		for (const auto& resource : resources.storage_images)
		{
			const auto& name = resource.name;
			auto& type = compiler.get_type(resource.base_type_id);
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			uint32_t descriptorSet = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
			uint32_t dimension = type.image.dim;

			ShaderDescriptorSetH2M& shaderDescriptorSet = m_ShaderDescriptorSets[descriptorSet];
			H2M_CORE_ASSERT(shaderDescriptorSet.StorageImages.find(binding) == shaderDescriptorSet.StorageImages.end());

			auto& imageSampler = shaderDescriptorSet.StorageImages[binding];
			// ImageSampler imageSampler;
			imageSampler.BindingPoint = binding;
			imageSampler.DescriptorSet = descriptorSet;
			imageSampler.Name = name;
			imageSampler.ShaderStage = shaderStage;
			// m_ImageSamplers.insert(std::pair(bindingPoint, imageSampler));

			MORAVA_CORE_TRACE("    {0} ({1}, {2})", name, descriptorSet, binding);

			MORAVA_CORE_TRACE("  Name: {0}", name);
			// MORAVA_CORE_TRACE("  Member Count: {0}", memberCount);
			MORAVA_CORE_TRACE("  Descriptor Set: {0}", descriptorSet);
			MORAVA_CORE_TRACE("  Binding Point: {0}", binding);
			// MORAVA_CORE_TRACE("  Size: {0}", size);
			MORAVA_CORE_TRACE("--------------------------");
		}

		MORAVA_CORE_TRACE("==========================");
	}

	void VulkanShaderH2M::CreateDescriptors()
	{
		VkDevice device = VulkanContextH2M::GetCurrentDevice()->GetVulkanDevice();

		//////////////////////////////////////////////////////////////////////
		// Descriptor Pool
		//////////////////////////////////////////////////////////////////////

		// We need to tell the API the number of max. requested descriptors per type
		m_TypeCounts.clear();
		for (auto&& [set, shaderDescriptorSet] : m_ShaderDescriptorSets)
		{
			// m_TypeCounts.insert(std::make_pair(0, std::vector<VkDescriptorPoolSize>()));
			if (shaderDescriptorSet.UniformBuffers.size())
			{
				VkDescriptorPoolSize& typeCount = m_TypeCounts[set].emplace_back();
				typeCount.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				typeCount.descriptorCount = static_cast<uint32_t>(shaderDescriptorSet.UniformBuffers.size());
			}

			if (shaderDescriptorSet.ImageSamplers.size())
			{
				VkDescriptorPoolSize& typeCount = m_TypeCounts[set].emplace_back();
				typeCount.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				typeCount.descriptorCount = static_cast<uint32_t>(shaderDescriptorSet.ImageSamplers.size());
			}

			if (shaderDescriptorSet.StorageImages.size())
			{
				VkDescriptorPoolSize& typeCount = m_TypeCounts[set].emplace_back();
				typeCount.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
				typeCount.descriptorCount = static_cast<uint32_t>(shaderDescriptorSet.StorageImages.size());
			}

#if 0
			// TODO: Move this to the centralized renderer
			// Create the global descriptor pool
			// All descriptors used in this example are allocated from this pool
			VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
			descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			// Once you bind a descriptor set and use it in a vkCmdDraw() function, you can no longer modify it unless you specify the
			// descriptorPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
			descriptorPoolInfo.pNext = nullptr;
			descriptorPoolInfo.poolSizeCount = (uint32_t)m_TypeCounts.at(set).size();
			descriptorPoolInfo.pPoolSizes = m_TypeCounts.at(set).data();
			descriptorPoolInfo.maxSets = 1;

			VK_CHECK_RESULT_H2M(vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &m_DescriptorPool));
#endif

			//////////////////////////////////////////////////////////////////////
			// Descriptor Set Layout
			//////////////////////////////////////////////////////////////////////

			std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
			for (auto& [binding, uniformBuffer] : shaderDescriptorSet.UniformBuffers)
			{
				VkDescriptorSetLayoutBinding& layoutBinding = layoutBindings.emplace_back();
				layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				layoutBinding.descriptorCount = 1;
				layoutBinding.stageFlags = uniformBuffer.ShaderStage;
				layoutBinding.pImmutableSamplers = nullptr;
				layoutBinding.binding = binding;

				VkWriteDescriptorSet& set = shaderDescriptorSet.WriteDescriptorSets[uniformBuffer.Name];
				set = {};
				set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				set.descriptorType = layoutBinding.descriptorType; // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				set.descriptorCount = 1;
				set.dstBinding = layoutBinding.binding;

				AllocateUniformBuffer(uniformBuffer);
			}

			for (auto& [binding, imageSampler] : shaderDescriptorSet.ImageSamplers)
			{
				VkDescriptorSetLayoutBinding& layoutBinding = layoutBindings.emplace_back();
				layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				layoutBinding.descriptorCount = 1;
				layoutBinding.stageFlags = imageSampler.ShaderStage;
				layoutBinding.pImmutableSamplers = nullptr;
				layoutBinding.binding = binding;

				H2M_CORE_ASSERT(shaderDescriptorSet.UniformBuffers.find(binding) == shaderDescriptorSet.UniformBuffers.end(), "Binding is already present in m_UniformBuffers!");

				VkWriteDescriptorSet& set = shaderDescriptorSet.WriteDescriptorSets[imageSampler.Name];
				set = {};
				set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				set.descriptorType = layoutBinding.descriptorType; // VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				set.descriptorCount = 1;
				set.dstBinding = layoutBinding.binding;
			}

			for (auto& [binding, storageImage] : shaderDescriptorSet.StorageImages)
			{
				VkDescriptorSetLayoutBinding& layoutBinding = layoutBindings.emplace_back();
				layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
				layoutBinding.descriptorCount = 1;
				layoutBinding.stageFlags = storageImage.ShaderStage;
				layoutBinding.pImmutableSamplers = nullptr;

				// uint32_t binding = bindingAndSet & 0xffffffff;
				// uint32_t descriptorSet = (bindingAndSet >> 32);
				layoutBinding.binding = binding;

				H2M_CORE_ASSERT(shaderDescriptorSet.UniformBuffers.find(binding) == shaderDescriptorSet.UniformBuffers.end(), "Binding is already present in m_UniformBuffers!");
				H2M_CORE_ASSERT(shaderDescriptorSet.ImageSamplers.find(binding) == shaderDescriptorSet.ImageSamplers.end(), "Binding is already present in m_ImageSamplers!");

				VkWriteDescriptorSet& set = shaderDescriptorSet.WriteDescriptorSets[storageImage.Name];
				set = {};
				set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				set.descriptorType = layoutBinding.descriptorType; // VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
				set.descriptorCount = 1;
				set.dstBinding = layoutBinding.binding;
				// set.dstSet = descriptorSet;
			}

			VkDescriptorSetLayoutCreateInfo descriptorLayout = {};
			descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			descriptorLayout.pNext = nullptr;
			descriptorLayout.bindingCount = static_cast<uint32_t>(layoutBindings.size());
			descriptorLayout.pBindings = layoutBindings.data();

			MORAVA_CORE_INFO("Creating descriptor set {0} with {1} ubos, {2} samplers and {3} storage images", set,
				shaderDescriptorSet.UniformBuffers.size(),
				shaderDescriptorSet.ImageSamplers.size(),
				shaderDescriptorSet.StorageImages.size());

			VK_CHECK_RESULT_H2M(vkCreateDescriptorSetLayout(device, &descriptorLayout, nullptr, &m_DescriptorSetLayouts[set]));
		}
	}

	void VulkanShaderH2M::AllocateUniformBuffer(UniformBufferH2M& dst)
	{
		VkDevice device = VulkanContextH2M::GetCurrentDevice()->GetVulkanDevice();

		UniformBufferH2M& uniformBuffer = dst;

		// Prepare and initialize an uniform buffer block containing shader uniforms
		// Single uniforms like in OpenGL are no longer present in Vulkan. All Shader uniforms are passed via uniform buffer blocks

		// Vertex shader uniform buffer block
		VkBufferCreateInfo bufferInfo = {};
		VkMemoryAllocateInfo allocInfo = {};

		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.pNext = nullptr;
		allocInfo.allocationSize = 0;
		allocInfo.memoryTypeIndex = 0;

		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = uniformBuffer.Size;
		// This buffer will be used as an uniform buffer
		bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

		VulkanAllocatorH2M allocator(std::string("UniformBuffer"));

		// Create a new buffer
		VK_CHECK_RESULT_H2M(vkCreateBuffer(device, &bufferInfo, nullptr, &uniformBuffer.Buffer));

		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(device, uniformBuffer.Buffer, &memoryRequirements);
		allocInfo.allocationSize = memoryRequirements.size;

		allocator.Allocate(memoryRequirements, &uniformBuffer.Memory, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		VK_CHECK_RESULT_H2M(vkBindBufferMemory(device, uniformBuffer.Buffer, uniformBuffer.Memory, 0));

		// Store information in the uniform's descriptor that is used by the descriptor set
		uniformBuffer.Descriptor.buffer = uniformBuffer.Buffer;
		uniformBuffer.Descriptor.offset = 0;
		uniformBuffer.Descriptor.range = uniformBuffer.Size;
	}

	//	ShaderMaterialDescriptorSet VulkanShaderH2M::AllocateDescriptorSet(uint32_t set)
	//	{
	//		return ShaderMaterialDescriptorSet();
	//	}

	VulkanShaderH2M::ShaderMaterialDescriptorSet VulkanShaderH2M::CreateDescriptorSets(uint32_t set)
	{
		ShaderMaterialDescriptorSet result;

		VkDevice device = VulkanContextH2M::GetCurrentDevice()->GetVulkanDevice();

		H2M_CORE_ASSERT(m_TypeCounts.find(set) != m_TypeCounts.end());

		// TODO: Move this to the centralized renderer
		VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
		descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.pNext = nullptr;
		descriptorPoolInfo.poolSizeCount = (uint32_t)m_TypeCounts.at(set).size();
		descriptorPoolInfo.pPoolSizes = m_TypeCounts.at(set).data();
		descriptorPoolInfo.maxSets = 1;

		VK_CHECK_RESULT_H2M(vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &result.Pool));

		// Allocate a new descriptor set from the global descriptor pool
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = result.Pool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &m_DescriptorSetLayouts[set];

		result.DescriptorSets.emplace_back();
		VK_CHECK_RESULT_H2M(vkAllocateDescriptorSets(device, &allocInfo, result.DescriptorSets.data()));
		return result;
	}

	VulkanShaderH2M::ShaderMaterialDescriptorSet VulkanShaderH2M::CreateDescriptorSets(uint32_t set, uint32_t numberOfSets)
	{
		ShaderMaterialDescriptorSet result{};

		VkDevice device = VulkanContextH2M::GetCurrentDevice()->GetVulkanDevice();

		std::unordered_map<uint32_t, std::vector<VkDescriptorPoolSize>> poolSizes;
		for (auto&& [set, shaderDescriptorSet] : m_ShaderDescriptorSets)
		{
			if (shaderDescriptorSet.UniformBuffers.size())
			{
				VkDescriptorPoolSize& typeCount = poolSizes[set].emplace_back();
				typeCount.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				typeCount.descriptorCount = static_cast<uint32_t>(shaderDescriptorSet.UniformBuffers.size()) * numberOfSets;
			}
			if (shaderDescriptorSet.ImageSamplers.size())
			{
				VkDescriptorPoolSize& typeCount = poolSizes[set].emplace_back();
				typeCount.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				typeCount.descriptorCount = static_cast<uint32_t>(shaderDescriptorSet.ImageSamplers.size()) * numberOfSets;
			}
			if (shaderDescriptorSet.StorageImages.size())
			{
				VkDescriptorPoolSize& typeCount = poolSizes[set].emplace_back();
				typeCount.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
				typeCount.descriptorCount = static_cast<uint32_t>(shaderDescriptorSet.StorageImages.size()) * numberOfSets;
			}

		}

		if (poolSizes.find(set) == poolSizes.end())
		{
			// H2M_CORE_ASSERT(poolSizes.find(set) != poolSizes.end());
			Log::GetLogger()->error("VulkanShaderH2M::CreateDescriptorSets('{0}, {1}') - descriptor set not found in 'poolSizes'!", set, numberOfSets);
		}

		// TODO: Move this to the centralized renderer
		VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
		descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.pNext = nullptr;
		descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.at(set).size());
		descriptorPoolInfo.pPoolSizes = poolSizes.at(set).data();
		descriptorPoolInfo.maxSets = numberOfSets;

		VK_CHECK_RESULT_H2M(vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &result.Pool));

		result.DescriptorSets.resize(numberOfSets);

		for (uint32_t i = 0; i < numberOfSets; i++)
		{
			VkDescriptorSetAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = result.Pool;
			allocInfo.descriptorSetCount = 1;
			allocInfo.pSetLayouts = &m_DescriptorSetLayouts[set];

			VK_CHECK_RESULT_H2M(vkAllocateDescriptorSets(device, &allocInfo, &result.DescriptorSets[i]));
		}
		return result;
	}

	const VkWriteDescriptorSet* VulkanShaderH2M::GetDescriptorSet(const std::string& name, uint32_t set) const
	{
		H2M_CORE_ASSERT(m_ShaderDescriptorSets.find(set) != m_ShaderDescriptorSets.end());
		if (m_ShaderDescriptorSets.at(set).WriteDescriptorSets.find(name) == m_ShaderDescriptorSets.at(set).WriteDescriptorSets.end())
		{
			// HZ_CORE_WARN("Shader {0} does not contain requested descriptor set {1}", m_Name, name);
			Log::GetLogger()->warn("Shader {0} does not contain requested descriptor set {1}", m_Name, name);
			return nullptr;
		}
		return &m_ShaderDescriptorSets.at(set).WriteDescriptorSets.at(name);
	}

	// does not exist in Vulkan Week version, added later
	std::vector<VkDescriptorSetLayout> VulkanShaderH2M::GetAllDescriptorSetLayouts()
	{
		std::vector<VkDescriptorSetLayout> result;
		result.reserve(m_DescriptorSetLayouts.size());
		for (auto [set, layout] : m_DescriptorSetLayouts)
		{
			result.emplace_back(layout);
		}

		return result;
	}

	VulkanShaderH2M::UniformBufferH2M& VulkanShaderH2M::GetUniformBuffer(uint32_t binding, uint32_t set)
	{
		H2M_CORE_ASSERT(m_ShaderDescriptorSets.at(set).UniformBuffers.size() > binding);
		return m_ShaderDescriptorSets.at(set).UniformBuffers[binding];
	}

	static const char* VkShaderStageCachedFileExtension(VkShaderStageFlagBits stage)
	{
		switch (stage)
		{
		case VK_SHADER_STAGE_VERTEX_BIT:   return ".cached_vulkan.vert";
		case VK_SHADER_STAGE_FRAGMENT_BIT: return ".cached_vulkan.frag";
		case VK_SHADER_STAGE_COMPUTE_BIT:  return ".cached_vulkan.comp";
		}
		Log::GetLogger()->error("Invalid VkShaderStageFlagBits value '{0}'!", stage);
		H2M_CORE_ASSERT(false, "Invalid VkShaderStageFlagBits value!");
		return "";
	}

	static shaderc_shader_kind VkShaderStageToShaderC(VkShaderStageFlagBits stage)
	{
		switch (stage)
		{
		case VK_SHADER_STAGE_VERTEX_BIT:   return shaderc_vertex_shader;
		case VK_SHADER_STAGE_FRAGMENT_BIT: return shaderc_fragment_shader;
		case VK_SHADER_STAGE_COMPUTE_BIT:  return shaderc_compute_shader;
		}
		Log::GetLogger()->error("Invalid VkShaderStageFlagBits value '{0}'!", stage);
		H2M_CORE_ASSERT(false, "Invalid VkShaderStageFlagBits value!");
		return (shaderc_shader_kind)-1;
	}

	void VulkanShaderH2M::CompileOrGetVulkanBinary(std::unordered_map<VkShaderStageFlagBits, std::vector<uint32_t>>& outputBinary, bool forceCompile)
	{
		for (auto [stage, source] : m_ShaderSource)
		{
			auto extension = VkShaderStageCachedFileExtension(stage);

			if (!forceCompile)
			{
				// Retrieve shader code from cache, if available
				std::filesystem::path p = m_AssetPath;
				auto path = p.parent_path() / "cached" / (p.filename().string() + extension);
				std::string cachedFilePath = path.string();

				FILE* f = fopen(cachedFilePath.c_str(), "rb");
				if (f)
				{
					fseek(f, 0, SEEK_END);
					uint64_t size = ftell(f);
					fseek(f, 0, SEEK_SET);
					outputBinary[stage] = std::vector<uint32_t>(size / sizeof(uint32_t));
					fread(outputBinary[stage].data(), sizeof(uint32_t), outputBinary[stage].size(), f);
					fclose(f);
				}
			}

			if (outputBinary[stage].size() == 0)
			{
				// TODO: Do we need to init a compiler for each stage?
				shaderc::Compiler compiler;
				shaderc::CompileOptions options;
				options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);

				const bool optimize = false;
				if (optimize)
				{
					options.SetOptimizationLevel(shaderc_optimization_level_performance);
				}

				if (m_ShaderSource.find(stage) == m_ShaderSource.end()) return;

				// Compile shader
				{
					auto& shaderSource = m_ShaderSource.at(stage); // e.g. VK_SHADER_STAGE_VERTEX_BIT
					shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(shaderSource, VkShaderStageToShaderC(stage), m_AssetPath.c_str(), options);

					if (module.GetCompilationStatus() != shaderc_compilation_status_success)
					{
						H2M_CORE_ERROR(module.GetErrorMessage());
						H2M_CORE_ASSERT(false);
					}

					const uint8_t* begin = (const uint8_t*)module.cbegin();
					const uint8_t* end = (const uint8_t*)module.cend();
					const ptrdiff_t size = end - begin;

					outputBinary[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());
				}

				// Cache compiled shader
				{
					std::filesystem::path p = m_AssetPath;
					auto path = p.parent_path() / "cached" / (p.filename().string() + extension);
					std::string cachedFilePath = path.string();

					FILE* f = fopen(cachedFilePath.c_str(), "wb");
					fwrite(outputBinary[stage].data(), sizeof(uint32_t), outputBinary[stage].size(), f);
					fclose(f);
				}
			}
		}
	}

	static VkShaderStageFlagBits ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")                       return VK_SHADER_STAGE_VERTEX_BIT;
		if (type == "fragment" || type == "pixel")  return VK_SHADER_STAGE_FRAGMENT_BIT;
		if (type == "compute")                      return VK_SHADER_STAGE_COMPUTE_BIT;
		
		return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
	}

	std::unordered_map<VkShaderStageFlagBits, std::string> VulkanShaderH2M::PreProcess(const std::string& source)
	{
		std::unordered_map<VkShaderStageFlagBits, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			H2M_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			H2M_CORE_ASSERT(type == "vertex" || type == "fragment" || type == "pixel" || type == "compute", "Invalid shader type specified");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);
			auto shaderType = ShaderTypeFromString(type);
			shaderSources[shaderType] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}

		return shaderSources;
	}

	void VulkanShaderH2M::Bind() {}

	uint32_t VulkanShaderH2M::GetRendererID() const { return 0; }

	void VulkanShaderH2M::ClearUniformBuffers()
	{
		s_UniformBuffers.clear();
	}

	size_t VulkanShaderH2M::GetHash() const
	{
		return std::hash<std::string>{}(m_AssetPath);
	}

	void VulkanShaderH2M::SetUniformBuffer(const std::string& name, const void* data, uint32_t size) {}

	/****
	const std::vector<VkPipelineShaderStageCreateInfo>& VulkanShaderH2M::GetShaderStages() const
	{
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
		for (auto [stage, pipelineShaderStageCreateInfo] : m_ShaderStages)
		{
			shaderStages.push_back(pipelineShaderStageCreateInfo);
		}
		return shaderStages;
	}
	****/

	void VulkanShaderH2M::SetUniform(const std::string& fullname, float value) {}

	void VulkanShaderH2M::SetUniform(const std::string& fullname, int value) {}

	void VulkanShaderH2M::SetUniform(const std::string& fullname, const glm::vec2& value) {}

	void VulkanShaderH2M::SetUniform(const std::string& fullname, const glm::vec3& value) {}

	void VulkanShaderH2M::SetUniform(const std::string& fullname, const glm::vec4& value) {}

	void VulkanShaderH2M::SetUniform(const std::string& fullname, const glm::mat3& value) {}

	void VulkanShaderH2M::SetUniform(const std::string& fullname, const glm::mat4& value) {}

	void VulkanShaderH2M::SetUniform(const std::string& fullname, uint32_t value) {}

	void VulkanShaderH2M::SetInt(const std::string& name, int value) {}

	void VulkanShaderH2M::SetUInt(const std::string& name, uint32_t value) {}

	void VulkanShaderH2M::SetFloat(const std::string& name, float value) {}

	void VulkanShaderH2M::SetFloat2(const std::string& name, const glm::vec2& value) {}

	void VulkanShaderH2M::SetFloat3(const std::string& name, const glm::vec3& value) {}

	void VulkanShaderH2M::SetFloat4(const std::string& name, const glm::vec4& value) {}

	void VulkanShaderH2M::SetMat4(const std::string& name, const glm::mat4& value) {}

	void VulkanShaderH2M::SetMat4FromRenderThread(const std::string& name, const glm::mat4& value, bool bind /*= true*/) {}

	void VulkanShaderH2M::SetIntArray(const std::string& name, int* values, uint32_t size) {}

	// const std::unordered_map<std::string, Hazel::ShaderBuffer>& VulkanShaderH2M::GetShaderBuffers() const { return {}; }

	const std::unordered_map<std::string, ShaderResourceDeclarationH2M>& VulkanShaderH2M::GetResources() const
	{
		return {};
	}

	void VulkanShaderH2M::AddShaderReloadedCallback(const ShaderReloadedCallback& callback) {}

	void* VulkanShaderH2M::MapUniformBuffer(uint32_t bindingPoint, uint32_t set)
	{
		H2M_CORE_ASSERT(m_ShaderDescriptorSets.find(set) != m_ShaderDescriptorSets.end());
		VkDevice device = VulkanContextH2M::GetCurrentDevice()->GetVulkanDevice();

		uint8_t* pData;
		VK_CHECK_RESULT_H2M(vkMapMemory(device, m_ShaderDescriptorSets.at(set).UniformBuffers.at(bindingPoint).Memory, 0, m_ShaderDescriptorSets.at(set).UniformBuffers.at(bindingPoint).Size, 0, (void**)&pData));
		return pData;
	}

	void VulkanShaderH2M::UnmapUniformBuffer(uint32_t bindingPoint, uint32_t set)
	{
		H2M_CORE_ASSERT(m_ShaderDescriptorSets.find(set) != m_ShaderDescriptorSets.end());
		VkDevice device = VulkanContextH2M::GetCurrentDevice()->GetVulkanDevice();
		vkUnmapMemory(device, m_ShaderDescriptorSets.at(set).UniformBuffers.at(bindingPoint).Memory);
	}

}
