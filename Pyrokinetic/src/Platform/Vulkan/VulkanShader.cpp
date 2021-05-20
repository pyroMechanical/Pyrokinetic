#include "pkpch.h"
#include "VulkanShader.h"
#include "VulkanContext.h"
#include "VulkanInitializer.h"
#include <fstream>
#include <vulkan/vulkan.h>

#include <glm/gtc/type_ptr.hpp>

#include <filesystem>
#include <utility>

#include <shaderc/shaderc.hpp>

namespace pk
{

	namespace util
	{
		static VkShaderModule LoadShaderModuleFromFile(const std::string& filePath)
		{
			PROFILE_FUNCTION();
			VulkanContext* context = VulkanContext::Get();

			std::ifstream file(filePath, std::ios::ate | std::ios::binary);

			PK_CORE_ASSERT(file.is_open(), "Failed to open file!");

			size_t fileSize = (size_t)file.tellg();

			std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

			file.seekg(0);

			file.read((char*)buffer.data(), fileSize);

			file.close();

			VkShaderModuleCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.pNext = nullptr;

			createInfo.codeSize = buffer.size() * sizeof(uint32_t);
			createInfo.pCode = buffer.data();

			VkShaderModule shaderModule;

			CHECK_VULKAN(vkCreateShaderModule(context->GetDevice()->GetVulkanDevice(), &createInfo, nullptr, &shaderModule));

			return shaderModule;
		}

		static const char* GetCacheDirectory()
		{
			// TODO: make sure the assets directory is valid
			return "assets/cache/shader/opengl";
		}

		static void CreateCacheDirectoryIfNeeded()
		{
			std::string cacheDirectory = GetCacheDirectory();
			if (!std::filesystem::exists(cacheDirectory))
				std::filesystem::create_directories(cacheDirectory);
		}

		static const char* ShaderStageCachedSPVFileExtension(VkShaderStageFlagBits stage)
		{
			switch (stage)
			{
			case VK_SHADER_STAGE_VERTEX_BIT: return ".vert.spv";
			case VK_SHADER_STAGE_FRAGMENT_BIT: return ".frag.spv";
			}

			PK_CORE_ASSERT(false, "Stage not currently supported or recognized!")
				return "";
		}

		static shaderc_shader_kind ShaderStageToShaderCStage(VkShaderStageFlagBits stage)
		{
			switch (stage)
			{
			case VK_SHADER_STAGE_VERTEX_BIT: return shaderc_vertex_shader;
			case VK_SHADER_STAGE_FRAGMENT_BIT: return shaderc_fragment_shader;
			}

			PK_CORE_ASSERT(false, "Stage not currently supported or recognized!")
				return (shaderc_shader_kind)0;
		}

		static std::string ShaderStageToString(VkShaderStageFlagBits stage)
		{
			switch (stage)
			{
			case VK_SHADER_STAGE_VERTEX_BIT: return "Vertex";
			case VK_SHADER_STAGE_FRAGMENT_BIT: return "Fragment";
			}

			PK_CORE_ASSERT(false, "Stage not currently supported or recognized!")
				return "";
		}

		static ShaderUniformType SPIRTypeToShaderUniformType(spvc_type type)
		{
			PK_CORE_INFO("{0}, {1}, {2}", spvc_type_get_basetype(type), spvc_type_get_vector_size(type), spvc_type_get_columns(type));

			switch (spvc_type_get_basetype(type))
			{
			case SPVC_BASETYPE_BOOLEAN:    return ShaderUniformType::Bool;
			case SPVC_BASETYPE_INT32:      return ShaderUniformType::Int;
			case SPVC_BASETYPE_UINT32:     return ShaderUniformType::UInt;
			case SPVC_BASETYPE_FP32:
				if (spvc_type_get_vector_size(type) == 1)            return ShaderUniformType::Float;
				if (spvc_type_get_vector_size(type) == 2)            return ShaderUniformType::Vec2;
				if (spvc_type_get_vector_size(type) == 3)            return ShaderUniformType::Vec3;
				if (spvc_type_get_vector_size(type) == 4)            return ShaderUniformType::Vec4;

				if (spvc_type_get_columns(type) == 3)            return ShaderUniformType::Mat3;
				if (spvc_type_get_columns(type) == 4)            return ShaderUniformType::Mat4;
				break;
			}
			PK_CORE_ASSERT(false, "Unknown type!");
			return ShaderUniformType::None;
		}

		static uint32_t SpvDimTypeToDimensionNumber(uint32_t dim)
		{
			switch (dim)
			{
			case SpvDim1D: return 1; break;
			case SpvDim2D: return 2; break;
			case SpvDim3D: return 3; break;
			}
			PK_ASSERT(false, "Dimension type not supported!");
			return -1;
		}
	}

	static VkShaderStageFlagBits ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return VK_SHADER_STAGE_VERTEX_BIT;
		if (type == "fragment" || type == "pixel" || type == "frag")
			return VK_SHADER_STAGE_FRAGMENT_BIT;

		PK_CORE_ASSERT(false, "Unknown shader type!");
	}

	VulkanShader::VulkanShader(const std::string& path)
	{
		PROFILE_FUNCTION();

		std::string src = ReadFile(path);
		auto shaderSources = PreProcess(src);
		CompileToVulkanSPV(shaderSources);
		

		// pull shader name from source file
		auto lastSlash = path.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = path.rfind(".");
		auto count = lastDot == std::string::npos ? path.size() - lastSlash : lastDot - lastSlash;
		m_Name = path.substr(lastSlash, count);
		m_Path = path;
	}

	/*VulkanShader::VulkanShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath)
	{
		PROFILE_FUNCTION();

		m_Name = name;

		m_ShaderModules[VK_SHADER_STAGE_VERTEX_BIT] = util::LoadShaderModuleFromFile(vertexPath);
		m_ShaderModules[VK_SHADER_STAGE_FRAGMENT_BIT] = util::LoadShaderModuleFromFile(fragmentPath);

		VkPipelineLayoutCreateInfo layoutCreateInfo = {};
		layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutCreateInfo.pNext = nullptr;
		layoutCreateInfo.flags = 0;
		//layoutCreateInfo.setLayoutCount = 1;
		//layoutCreateInfo.pSetLayouts = &m_DescriptorLayout;
		layoutCreateInfo.setLayoutCount = 0;
		layoutCreateInfo.pSetLayouts = nullptr;
		layoutCreateInfo.pushConstantRangeCount = 0;
		layoutCreateInfo.pPushConstantRanges = nullptr;

		CHECK_VULKAN(vkCreatePipelineLayout(VulkanContext::Get()->GetDevice()->GetVulkanDevice(), &layoutCreateInfo, nullptr, &m_Layout));
	}*/

	VulkanShader::~VulkanShader()
	{
		PROFILE_FUNCTION();

		//glDeleteProgram(m_RendererID);
	}

	std::string VulkanShader::ReadFile(const std::string& path)
	{
		PROFILE_FUNCTION();

		std::string result;
		std::ifstream in(path, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else
		{
			PK_CORE_ERROR("Could not open file '{0}'", path);
		}

		return result;
	}

	std::unordered_map<VkShaderStageFlagBits, std::string> VulkanShader::PreProcess(const std::string& src)
	{
		PROFILE_FUNCTION();

		std::unordered_map<VkShaderStageFlagBits, std::string> sources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = src.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = src.find_first_of("\r\n", pos);
			PK_CORE_ASSERT(eol != std::string::npos, "Syntax Error!");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = src.substr(begin, eol - begin);
			PK_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified!");

			size_t nextLinePos = src.find_first_not_of("\r\n", eol);
			PK_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
			pos = src.find(typeToken, nextLinePos);
			sources[ShaderTypeFromString(type)] = src.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? src.size() - 1 : nextLinePos));
		}

		return sources;
	}

	void VulkanShader::CompileToVulkanSPV(const std::unordered_map<VkShaderStageFlagBits, std::string>& sources)
	{
		PROFILE_FUNCTION();

		std::unordered_map<VkShaderStageFlagBits, std::vector<uint32_t>> output;

		std::filesystem::path cacheDir = util::GetCacheDirectory();
		for (auto [stage, source] : sources)
		{
			auto extension = util::ShaderStageCachedSPVFileExtension(stage);
			if (false)
			{
				auto path = cacheDir / (m_Name + extension);

				std::string cachePath = path.string();

				FILE* f = fopen(cachePath.c_str(), "rb");
				if (f)
				{
					fseek(f, 0, SEEK_END);
					size_t size = ftell(f);
					fseek(f, 0, SEEK_SET);
					output[stage] = std::vector<uint32_t>(size / sizeof(uint32_t));
					fread(output[stage].data(), sizeof(uint32_t), output[stage].size(), f);
					fclose(f);
				}
			}

			if (output[stage].size() == 0)
			{
				shaderc::Compiler compiler;
				shaderc::CompileOptions options;
				options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_1);
				options.AddMacroDefinition("OPENGL");
				{
					auto& source = sources.at(stage);
					shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, util::ShaderStageToShaderCStage(stage), m_Path.c_str(), options);

					if (module.GetCompilationStatus() != shaderc_compilation_status_success)
					{
						PK_CORE_ERROR("shaderc error: {0}", module.GetErrorMessage());
						PK_CORE_ASSERT(false, "Failed to compile to Vulkan SPV!");
					}

					const uint8_t* begin = (const uint8_t*)module.cbegin();
					const uint8_t* end = (const uint8_t*)module.cend();
					const ptrdiff_t size = end - begin;

					output[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

					auto path = cacheDir / (m_Name + extension);
					std::string cachedPath = path.string();

					FILE* f = fopen(cachedPath.c_str(), "wb");
					fwrite(output[stage].data(), sizeof(uint32_t), output[stage].size(), f);
					fclose(f);
				}
			}

		}
		LoadAndCreateShaders(output);
		Reflect(output);
		CreateDescriptors();
	}

	void VulkanShader::Reflect(std::unordered_map<VkShaderStageFlagBits, std::vector<uint32_t>>& shaderData)
	{
		PROFILE_FUNCTION();
		PK_CORE_TRACE("Vulkan Shader Reflection for: {0}", m_Path);

		for (auto& [stage, data] : shaderData)
		{
			spvc_context context = nullptr;
			spvc_context_create(&context);

			spvc_parsed_ir ir = nullptr;
			spvc_compiler compiler = nullptr;
			spvc_context_parse_spirv(context, data.data(), data.size(), &ir);
			spvc_context_create_compiler(context, SPVC_BACKEND_NONE, ir, SPVC_CAPTURE_MODE_COPY, &compiler);

			spvc_resources resources = nullptr;
			const spvc_reflected_resource* list = nullptr;
			size_t count;
			spvc_compiler_create_shader_resources(compiler, &resources);
			spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_UNIFORM_BUFFER, &list, &count);
			PK_CORE_INFO("Uniform Buffers:");
			for (size_t i = 0; i < count; i++)
			{
				const std::string& bufferName = spvc_compiler_get_name(compiler, list[i].base_type_id);
				auto bufferType = spvc_compiler_get_type_handle(compiler, list[i].type_id);
				size_t bufferSize;
				spvc_compiler_get_declared_struct_size(compiler, bufferType, &bufferSize);
				int memberCount = spvc_type_get_num_member_types(bufferType);
				uint32_t bindingPoint = spvc_compiler_get_decoration(compiler, list[i].id, SpvDecorationBinding);
				uint32_t descriptorSet = spvc_compiler_get_decoration(compiler, list[i].id, SpvDecorationDescriptorSet);

				ShaderDescriptorSet& shaderDescriptorSet = m_ShaderDescriptorSets[descriptorSet];

				if (s_UniformBuffers[descriptorSet].find(bindingPoint) == s_UniformBuffers[descriptorSet].end())
				{
					UniformBuffer* uniformBuffer = new UniformBuffer();
					uniformBuffer->Binding = bindingPoint;
					uniformBuffer->Size = bufferSize;
					uniformBuffer->ShaderStage = stage;
					uniformBuffer->Name = bufferName;
					s_UniformBuffers.at(descriptorSet)[bindingPoint] = uniformBuffer;

					AllocateUniformBuffer(*uniformBuffer);
				}
				else
				{
					UniformBuffer* uniformBuffer = s_UniformBuffers.at(descriptorSet).at(bindingPoint);
					if (bufferSize > uniformBuffer->Size)
					{
						PK_CORE_TRACE("Resizing Uniform Buffer (binding = {0}, set ={1}) to {2} bytes", bindingPoint, descriptorSet, bufferSize);
						uniformBuffer->Size = bufferSize;
						AllocateUniformBuffer(*uniformBuffer);
					}
				}

				shaderDescriptorSet.UniformBuffers[bindingPoint] = s_UniformBuffers.at(descriptorSet).at(bindingPoint);

				PK_CORE_TRACE("Buffer {0} ({1}, {2})", bufferName, descriptorSet, bindingPoint);
				PK_CORE_TRACE("Member Count: {0}", memberCount);
			}

			spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_PUSH_CONSTANT, &list, &count);
			PK_CORE_INFO("Push Constants:");
			for (uint32_t i = 0; i < count; i++)
			{
				const std::string& bufferName = spvc_compiler_get_name(compiler, list[i].base_type_id);
				auto bufferType = spvc_compiler_get_type_handle(compiler, list[i].type_id);
				size_t bufferSize;
				spvc_compiler_get_declared_struct_size(compiler, bufferType, &bufferSize);
				int memberCount = spvc_type_get_num_member_types(bufferType);
				uint32_t bufferOffset = 0;
				if (m_PushConstantRanges.size())
					bufferOffset = m_PushConstantRanges.back().Offset + m_PushConstantRanges.back().Size;

				auto& pushConstantRange = m_PushConstantRanges.emplace_back();
				pushConstantRange.ShaderStage = stage;
				pushConstantRange.Size = bufferSize;
				pushConstantRange.Offset = bufferOffset;

				if (bufferName.empty() || bufferName == "u_Renderer")
					continue;

				ShaderBuffer& buffer = m_Buffers[bufferName];
				buffer.Name = bufferName;
				buffer.Size = bufferSize - bufferOffset;

				PK_CORE_TRACE("Name: {0}, Members: {1}, Size: {2}", bufferName, memberCount, bufferSize);

				for (int i = 0; i < memberCount; i++)
				{
					auto typeID = spvc_type_get_member_type(bufferType, i);
					auto memberType = spvc_compiler_get_type_handle(compiler, typeID);
					const std::string& memberName = spvc_compiler_get_member_name(compiler, list[i].base_type_id, i);
					size_t size;
					spvc_compiler_get_declared_struct_member_size(compiler, bufferType, i, &size);
					uint32_t offset;
					spvc_compiler_type_struct_member_offset(compiler, bufferType, i, &offset);
					offset -= bufferOffset;
					PK_CORE_INFO(" MemberName: {0}, {1}, {2}", bufferName, memberName, bufferName + "." + memberName);
					std::string uniformName = memberName;
					buffer.Uniforms[uniformName] = ShaderUniform(uniformName, util::SPIRTypeToShaderUniformType(memberType), size, offset);
				}
			}


			spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_SAMPLED_IMAGE, &list, &count);
			PK_CORE_INFO("Sampled Images:");
			for (uint32_t i = 0; i < count; i++)
			{
				auto resourceType = spvc_compiler_get_type_handle(compiler, list[i].type_id);
				auto resourceBinding = spvc_compiler_get_decoration(compiler, list[i].id, SpvDecorationBinding);
				auto descriptorSet = spvc_compiler_get_decoration(compiler, list[i].id, SpvDecorationDescriptorSet);
				std::string resourceName = std::string(list[i].name);
				uint32_t dimension = util::SpvDimTypeToDimensionNumber(spvc_type_get_image_dimension(resourceType));
				uint32_t arrayLength = spvc_type_get_array_dimension(resourceType, 0);

				ShaderDescriptorSet& shaderDescriptorSet = m_ShaderDescriptorSets[descriptorSet];
				auto& imageSampler = shaderDescriptorSet.ImageSamplers[resourceBinding];
				imageSampler.Binding = resourceBinding;
				imageSampler.DescriptorSet = descriptorSet;
				imageSampler.Name = resourceName;
				imageSampler.ShaderStage = stage;
				imageSampler.Count = arrayLength;

				PK_CORE_INFO("Resource dimension: {0}, Resource Array Size: {1}", dimension, arrayLength);

				m_Resources[resourceName] = ShaderResourceDeclaration(resourceName, resourceBinding, arrayLength);
			}

			spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_STORAGE_IMAGE, &list, &count);
			PK_CORE_INFO("Storage Images:");
			for (uint32_t i = 0; i < count; i++)
			{
				auto resourceType = spvc_compiler_get_type_handle(compiler, list[i].type_id);
				auto resourceBinding = spvc_compiler_get_decoration(compiler, list[i].id, SpvDecorationBinding);
				auto descriptorSet = spvc_compiler_get_decoration(compiler, list[i].id, SpvDecorationDescriptorSet);
				std::string resourceName = std::string(list[i].name);
				uint32_t dimension = util::SpvDimTypeToDimensionNumber(spvc_type_get_image_dimension(resourceType));
				uint32_t arrayLength = spvc_type_get_array_dimension(resourceType, 0);

				ShaderDescriptorSet& shaderDescriptorSet = m_ShaderDescriptorSets[descriptorSet];
				auto& imageSampler = shaderDescriptorSet.ImageSamplers[resourceBinding];
				imageSampler.Binding = resourceBinding;
				imageSampler.DescriptorSet = descriptorSet;
				imageSampler.Name = resourceName;
				imageSampler.ShaderStage = stage;
				imageSampler.Count = arrayLength;

				PK_CORE_INFO("Resource dimension: {0}, Resource Array Size: {1}", dimension, arrayLength);

				m_Resources[resourceName] = ShaderResourceDeclaration(resourceName, resourceBinding, arrayLength);
			}
		}
	}

	void VulkanShader::LoadAndCreateShaders(const std::unordered_map<VkShaderStageFlagBits, std::vector<uint32_t>>& stageData)
	{
		PROFILE_FUNCTION();
		VkDevice device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();
		m_PipelineShaderStageCreateInfos.clear();
		for (auto [stage, data] : stageData)
		{
			PK_CORE_ASSERT(data.size(), "Shader stage does not exist!");

			VkShaderModuleCreateInfo moduleCreateInfo{};
			moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			moduleCreateInfo.codeSize = data.size() * sizeof(uint32_t);
			moduleCreateInfo.pCode = data.data();

			VkShaderModule shaderModule;
			CHECK_VULKAN(vkCreateShaderModule(device, &moduleCreateInfo, nullptr, &shaderModule));

			VkPipelineShaderStageCreateInfo& shaderStage = m_PipelineShaderStageCreateInfos.emplace_back();
			shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStage.stage = stage;
			shaderStage.module = shaderModule;
			shaderStage.pName = "main";
		}
	}

	void VulkanShader::CreateDescriptors()
	{
		PROFILE_FUNCTION();
		VkDevice device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();

		m_TypeCounts.clear();
		for (auto&& [set, shaderDescriptorSet] : m_ShaderDescriptorSets)
		{
			if (shaderDescriptorSet.UniformBuffers.size())
			{
				VkDescriptorPoolSize& typeCount = m_TypeCounts[set].emplace_back();
				typeCount.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				typeCount.descriptorCount = shaderDescriptorSet.UniformBuffers.size();
			}
			if (shaderDescriptorSet.ImageSamplers.size())
			{
				VkDescriptorPoolSize& typeCount = m_TypeCounts[set].emplace_back();
				typeCount.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				uint32_t count = 0;
				for (auto& [binding, sampler] : shaderDescriptorSet.ImageSamplers)
					count += sampler.Count;
				typeCount.descriptorCount = count;
			}
			if (shaderDescriptorSet.StorageImages.size())
			{
				VkDescriptorPoolSize& typeCount = m_TypeCounts[set].emplace_back();
				typeCount.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
				uint32_t count = 0;
				for (auto& [binding, sampler] : shaderDescriptorSet.StorageImages)
					count += sampler.Count;
				typeCount.descriptorCount = count;
			}

			std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
			for (auto& [binding, uniformBuffer] : shaderDescriptorSet.UniformBuffers)
			{
				VkDescriptorSetLayoutBinding& layoutBinding = layoutBindings.emplace_back();
				layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				layoutBinding.descriptorCount = 1;
				layoutBinding.stageFlags = uniformBuffer->ShaderStage;
				layoutBinding.pImmutableSamplers = nullptr;
				layoutBinding.binding = binding;

				VkWriteDescriptorSet& set = shaderDescriptorSet.WriteDescriptorSets[uniformBuffer->Name];
				set = {};
				set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				set.descriptorType = layoutBinding.descriptorType;
				set.descriptorCount = layoutBinding.descriptorCount;
				set.dstBinding = layoutBinding.binding;
			}

			for (auto& [binding, imageSampler] : shaderDescriptorSet.ImageSamplers)
			{
				VkDescriptorSetLayoutBinding& layoutBinding = layoutBindings.emplace_back();
				layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				layoutBinding.descriptorCount = imageSampler.Count;
				layoutBinding.stageFlags = imageSampler.ShaderStage;
				layoutBinding.pImmutableSamplers = nullptr;
				layoutBinding.binding = binding;

				VkWriteDescriptorSet& set = shaderDescriptorSet.WriteDescriptorSets[imageSampler.Name];
				set = {};
				set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				set.descriptorType = layoutBinding.descriptorType;
				set.descriptorCount = layoutBinding.descriptorCount;
				set.dstBinding = layoutBinding.binding;
			}

			for (auto& [bindingAndSet, imageSampler] : shaderDescriptorSet.StorageImages)
			{
				VkDescriptorSetLayoutBinding& layoutBinding = layoutBindings.emplace_back();
				layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				layoutBinding.descriptorCount = imageSampler.Count;
				layoutBinding.stageFlags = imageSampler.ShaderStage;
				layoutBinding.pImmutableSamplers = nullptr;

				uint32_t binding = bindingAndSet & 0xffffffff;
				uint32_t descriptorSet = (bindingAndSet >> 32);
				layoutBinding.binding = binding;

				PK_CORE_ASSERT(shaderDescriptorSet.UniformBuffers.find(binding) == shaderDescriptorSet.UniformBuffers.end(), "Uniform Binding is already present!");
				PK_CORE_ASSERT(shaderDescriptorSet.ImageSamplers.find(binding) == shaderDescriptorSet.ImageSamplers.end(), "Image Sampler Binding is already present!");

				VkWriteDescriptorSet& set = shaderDescriptorSet.WriteDescriptorSets[imageSampler.Name];
				set = {};
				set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				set.descriptorType = layoutBinding.descriptorType;
				set.descriptorCount = layoutBinding.descriptorCount;
				set.dstBinding = layoutBinding.binding;
			}

			VkDescriptorSetLayoutCreateInfo descriptorLayout = {};
			descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			descriptorLayout.pNext = nullptr;
			descriptorLayout.bindingCount = layoutBindings.size();
			descriptorLayout.pBindings = layoutBindings.data();

			PK_CORE_INFO("Creating Descriptor set {0} with {1} uniform buffers, {2} samplers, and {3} storage images",
				set,
				shaderDescriptorSet.UniformBuffers.size(),
				shaderDescriptorSet.ImageSamplers.size(),
				shaderDescriptorSet.StorageImages.size());
			CHECK_VULKAN(vkCreateDescriptorSetLayout(device, &descriptorLayout, nullptr, &m_DescriptorSetLayouts[set]));
		}
	}

	VulkanShader::ShaderMaterialDescriptorSet VulkanShader::CreateDescriptorSets(uint32_t set)
	{
		PROFILE_FUNCTION();
		ShaderMaterialDescriptorSet result;

		VkDevice device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();

		PK_CORE_ASSERT(m_TypeCounts.find(set) != m_TypeCounts.end(), "Descriptor Set not described!");

		VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
		descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.pNext = nullptr;
		descriptorPoolInfo.poolSizeCount = m_TypeCounts.at(set).size();
		descriptorPoolInfo.pPoolSizes = m_TypeCounts.at(set).data();
		descriptorPoolInfo.maxSets = 1;

		CHECK_VULKAN(vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &result.Pool));

		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = result.Pool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &m_DescriptorSetLayouts[set];

		result.DescriptorSets.emplace_back();
		CHECK_VULKAN(vkAllocateDescriptorSets(device, &allocInfo, result.DescriptorSets.data()));

		return result;
	}

	VulkanShader::ShaderMaterialDescriptorSet VulkanShader::CreateDescriptorSets(uint32_t set, uint32_t numOfSets)
	{
		PROFILE_FUNCTION();
		ShaderMaterialDescriptorSet result;

		VkDevice device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();

		std::unordered_map<uint32_t, std::vector<VkDescriptorPoolSize>> poolSizes;
		for (auto&& [set, shaderDescriptorSet] : m_ShaderDescriptorSets)
		{
			if (shaderDescriptorSet.UniformBuffers.size())
			{
				VkDescriptorPoolSize& typeCount = poolSizes[set].emplace_back();
				typeCount.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				typeCount.descriptorCount = shaderDescriptorSet.UniformBuffers.size();
			}
			if (shaderDescriptorSet.ImageSamplers.size())
			{
				VkDescriptorPoolSize& typeCount = poolSizes[set].emplace_back();
				typeCount.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				typeCount.descriptorCount = shaderDescriptorSet.ImageSamplers.size();
			}
			if (shaderDescriptorSet.StorageImages.size())
			{
				VkDescriptorPoolSize& typeCount = poolSizes[set].emplace_back();
				typeCount.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
				typeCount.descriptorCount = shaderDescriptorSet.StorageImages.size();
			}
		}

		PK_CORE_ASSERT(poolSizes.find(set) != poolSizes.end(), "Set contains no descriptors!");

		VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
		descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.pNext = nullptr;
		descriptorPoolInfo.poolSizeCount = poolSizes.at(set).size();
		descriptorPoolInfo.pPoolSizes = poolSizes.at(set).data();
		descriptorPoolInfo.maxSets = numOfSets;

		CHECK_VULKAN(vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &result.Pool));

		result.DescriptorSets.resize(numOfSets);

		for (uint32_t i = 0; i < numOfSets; i++)
		{
			VkDescriptorSetAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = result.Pool;
			allocInfo.descriptorSetCount = 1;
			allocInfo.pSetLayouts = &m_DescriptorSetLayouts[set];

			CHECK_VULKAN(vkAllocateDescriptorSets(device, &allocInfo, &result.DescriptorSets[i]));
		}
		return result;
	}

	const VkWriteDescriptorSet* VulkanShader::GetDescriptorSet(const std::string& name, uint32_t set) const
	{
		PK_CORE_ASSERT(m_ShaderDescriptorSets.find(set) != m_ShaderDescriptorSets.end(), "Descriptor Set not found!");
		if (m_ShaderDescriptorSets.at(set).WriteDescriptorSets.find(name) == m_ShaderDescriptorSets.at(set).WriteDescriptorSets.end())
		{
			PK_CORE_WARN("Shader {0} does not contain requested descriptor set {1}", m_Name, name);
			return nullptr;
		}
		return &m_ShaderDescriptorSets.at(set).WriteDescriptorSets.at(name);
	}

	std::vector<VkDescriptorSetLayout> VulkanShader::GetAllDescriptorSetLayouts()
	{
		std::vector<VkDescriptorSetLayout> result;
		result.reserve(m_DescriptorSetLayouts.size());
		for (auto [set, layout] : m_DescriptorSetLayouts)
			result.emplace_back(layout);

		return result;
	}

	void VulkanShader::AllocateUniformBuffer(UniformBuffer& dst)
	{
		PROFILE_FUNCTION();
		VmaAllocator* allocator = VulkanContext::Get()->GetAllocator();

		UniformBuffer& uniformBuffer = dst;

		uniformBuffer.Buffer = vkinit::allocate_buffer(*allocator, uniformBuffer.Size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

		uniformBuffer.Descriptor.buffer = uniformBuffer.Buffer.buffer;
		uniformBuffer.Descriptor.offset = 0;
		uniformBuffer.Descriptor.range = uniformBuffer.Size;
	}

	void VulkanShader::Bind() const
	{
	}

	void VulkanShader::Unbind() const
	{
	}

	void VulkanShader::SetUniformBuffer(const std::string& name, const void* data, uint32_t size)
	{
		PROFILE_FUNCTION();
		//this is a very inefficient way of doing things, will find a better one later!
		for(auto&& [set, descriptorSet] : m_ShaderDescriptorSets)
		{
			for (auto&& [binding, uniformBuffer] : descriptorSet.UniformBuffers)
			{
				if(uniformBuffer->Name == name)
				{
					PK_CORE_ASSERT(uniformBuffer->Size >= size, "Buffer not large enough for data!");

					void* bufferPtr = VulkanShader::MapUniformBuffer(binding, set);
					memcpy(bufferPtr, data, size);
					VulkanShader::UnmapUniformBuffer(binding, set);
					break;
				}
			}
		}
	}

	void VulkanShader::SetFloat(const std::string& name, const float value)
	{
		PROFILE_FUNCTION();

		UploadUniformFloat(name, value);
	}

	void VulkanShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		PROFILE_FUNCTION();

		UploadUniformFloat3(name, value);
	}

	void VulkanShader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		PROFILE_FUNCTION();

		UploadUniformFloat4(name, value);
	}

	void VulkanShader::SetInt(const std::string& name, const int value)
	{
		PROFILE_FUNCTION();

		UploadUniformInt(name, value);
	}

	void VulkanShader::SetIntArray(const std::string& name, int* values, uint32_t count)
	{
		UploadUniformIntArray(name, values, count);
	}

	void VulkanShader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		PROFILE_FUNCTION();

		UploadUniformMat4(name, value);
	}
	
	void VulkanShader::UploadUniformInt(const std::string& name, const int values)
	{
		//GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		//glUniform1i(location, values);
	}

	void VulkanShader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
	{
		//GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		//glUniform1iv(location, count, values);
	}

	void VulkanShader::UploadUniformFloat(const std::string& name, const float values)
	{
		//GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		//glUniform1f(location, values);
	}

	void VulkanShader::UploadUniformFloat2(const std::string& name, const glm::vec2& values)
	{
		//GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		//glUniform2f(location, values.x, values.y);
	}

	void VulkanShader::UploadUniformFloat3(const std::string& name, const glm::vec3& values)
	{
		//GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		//glUniform3f(location, values.x, values.y, values.z);
	}

	void VulkanShader::UploadUniformFloat4(const std::string& name, const glm::vec4& values)
	{
		//GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		//glUniform4f(location, values.x, values.y, values.z, values.w);
	}

	void VulkanShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		//GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		//glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void VulkanShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		
	}

	void* VulkanShader::MapUniformBuffer(uint32_t bindingPoint, uint32_t set)
	{
		PROFILE_FUNCTION();
		PK_CORE_ASSERT(s_UniformBuffers.find(set) != s_UniformBuffers.end(), "Uniform Buffer not found!");
		PK_CORE_ASSERT(s_UniformBuffers.at(set).find(bindingPoint) != s_UniformBuffers.at(set).end(), "Binding Point not found!");
		PK_CORE_ASSERT(s_UniformBuffers.at(set).at(bindingPoint), "Buffer at binding point not initialized!");

		VmaAllocator* allocator = VulkanContext::Get()->GetAllocator();
		void* pData;
		CHECK_VULKAN(vmaMapMemory(*allocator, s_UniformBuffers.at(set).at(bindingPoint)->Buffer.allocation, &pData));
		return pData;
	}

	void VulkanShader::UnmapUniformBuffer(uint32_t bindingPoint, uint32_t set)
	{
		PROFILE_FUNCTION();
		PK_CORE_ASSERT(s_UniformBuffers.find(set) != s_UniformBuffers.end(), "Uniform Buffer not found!");
		PK_CORE_ASSERT(s_UniformBuffers.at(set).find(bindingPoint) != s_UniformBuffers.at(set).end(), "Binding Point not found!");
		PK_CORE_ASSERT(s_UniformBuffers.at(set).at(bindingPoint), "Buffer at binding point not initialized!");

		VmaAllocator* allocator = VulkanContext::Get()->GetAllocator();
		vmaUnmapMemory(*allocator, s_UniformBuffers.at(set).at(bindingPoint)->Buffer.allocation);
	}

}