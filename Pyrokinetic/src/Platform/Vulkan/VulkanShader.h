#pragma once
#include "Pyrokinetic/Rendering/Shader.h"
#include "VulkanTexture.h"
#include "VkTypes.h"
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <spirv_cross/spirv_cross_c.h>

#include <map>

namespace pk
{
	class VulkanShader : public Shader
	{
	public:
		struct UniformBuffer
		{
			AllocatedBuffer Buffer;
			VkDescriptorBufferInfo Descriptor;
			uint32_t Size = 0;
			uint32_t Binding = 0;
			std::string Name;
			VkShaderStageFlagBits ShaderStage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
		};

		struct ImageSampler
		{
			uint32_t Binding = 0;
			uint32_t DescriptorSet = 0;
			uint32_t Count;
			std::string Name;
			VkShaderStageFlagBits ShaderStage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
		};

		struct PushConstantRange
		{
			VkShaderStageFlagBits ShaderStage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
			uint32_t Offset = 0;
			uint32_t Size = 0;
		};

	public:
		VulkanShader(const std::string& path);
		//VulkanShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);

		virtual ~VulkanShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetFloat(const std::string& name, const float value) override;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
		virtual void SetInt(const std::string& name, const int value) override;
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) override;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) override;

		virtual const std::string& GetName() const override { return m_Name; }

		virtual void SetUniformBuffer(const std::string& name, const void* data, uint32_t size) override;

		virtual void AddResource(std::any resource) override 
		{
			std::shared_ptr<VulkanTexture2D>& vkTexture = std::dynamic_pointer_cast<VulkanTexture2D>(std::any_cast<std::shared_ptr<Texture2D>>(resource));
			PK_CORE_INFO("Texture Size: Width: {0}, Height: {1}", vkTexture->GetWidth(), vkTexture->GetHeight());
			m_Textures.push_back(vkTexture->GetDescriptorInfo());
		}

		std::vector<VkDescriptorImageInfo> GetImageInfos()
		{
			return m_Textures;
		}

		void UploadUniformInt(const std::string& name, const int values);
		void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);

		void UploadUniformFloat(const std::string& name, const float values);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& values);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& values);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& values);

		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
		//Vulkan specific
		const std::vector<VkPipelineShaderStageCreateInfo>& GetPipelineShaderStageCreateInfos() const { return m_PipelineShaderStageCreateInfos; }

		void LoadAndCreateShaders(const std::unordered_map<VkShaderStageFlagBits, std::vector<uint32_t>>& data);
		static void* MapUniformBuffer(uint32_t bindingPoint, uint32_t set = 0);
		static void UnmapUniformBuffer(uint32_t bidningPoint, uint32_t set = 0);

		VkDescriptorSet& GetDescriptorSet() { return m_DescriptorSet; };
		VkDescriptorSetLayout GetDescriptorSetLayout(uint32_t set) { return m_DescriptorSetLayouts.at(set); }
		std::vector<VkDescriptorSetLayout> GetAllDescriptorSetLayouts();

		UniformBuffer& GetUniformBuffer(uint32_t binding = 0, uint32_t set = 0)
		{
			PK_CORE_ASSERT(m_ShaderDescriptorSets.at(set).UniformBuffers.size() > binding, "Binding does not exist!");
			return  *m_ShaderDescriptorSets.at(set).UniformBuffers[binding];
		}

		uint32_t GetUniformBufferCount(uint32_t set = 0)
		{
			if (m_ShaderDescriptorSets.find(set) == m_ShaderDescriptorSets.end())
				return 0;

			return m_ShaderDescriptorSets.at(set).UniformBuffers.size();
		}

		struct ShaderDescriptorSet
		{
			std::unordered_map<uint32_t, UniformBuffer*> UniformBuffers;
			std::unordered_map<uint32_t, ImageSampler> ImageSamplers;
			std::unordered_map<uint32_t, ImageSampler> StorageImages;

			std::unordered_map<std::string, VkWriteDescriptorSet> WriteDescriptorSets;
		};

		const std::unordered_map<uint32_t, ShaderDescriptorSet>& GetShaderDescriptorSets() const { return m_ShaderDescriptorSets; }

		const std::vector<PushConstantRange>& GetPushConstantRanges() const { return m_PushConstantRanges; }

		struct ShaderMaterialDescriptorSet
		{
			VkDescriptorPool Pool;
			std::vector<VkDescriptorSet> DescriptorSets;
		};

		ShaderMaterialDescriptorSet CreateDescriptorSets(uint32_t set = 0);
		ShaderMaterialDescriptorSet CreateDescriptorSets(uint32_t set, uint32_t numberOfSets);

		const VkWriteDescriptorSet* GetDescriptorSet(const std::string& name, uint32_t set = 0) const;

		static void ClearUniformBuffers();
	private:
		std::string ReadFile(const std::string& path);
		std::unordered_map<VkShaderStageFlagBits, std::string> PreProcess(const std::string& src);
		void CompileToVulkanSPV(const std::unordered_map<VkShaderStageFlagBits, std::string>& sources);
		void Reflect(std::unordered_map<VkShaderStageFlagBits, std::vector<uint32_t>>& shaderData);

		void CreateDescriptors();

		void AllocateUniformBuffer(UniformBuffer& dst);
	private:
		std::vector<VkPipelineShaderStageCreateInfo> m_PipelineShaderStageCreateInfos;
		std::unordered_map<VkShaderStageFlagBits, std::string> m_ShaderSource;
		std::string m_Name;
		std::string m_Path;

		std::unordered_map<uint32_t, ShaderDescriptorSet> m_ShaderDescriptorSets;

		std::vector<PushConstantRange> m_PushConstantRanges;
		std::unordered_map<std::string, ShaderResourceDeclaration> m_Resources;

		std::unordered_map<std::string, ShaderBuffer> m_Buffers;

		inline static std::unordered_map < uint32_t, std::unordered_map<uint32_t, UniformBuffer*>> s_UniformBuffers;

		std::unordered_map<uint32_t, VkDescriptorSetLayout> m_DescriptorSetLayouts;
		VkDescriptorSet m_DescriptorSet;
		VkDescriptorPool m_DescriptorPool;

		std::vector<VkDescriptorImageInfo> m_Textures;

		std::unordered_map<uint32_t, std::vector<VkDescriptorPoolSize>> m_TypeCounts;
	};
}