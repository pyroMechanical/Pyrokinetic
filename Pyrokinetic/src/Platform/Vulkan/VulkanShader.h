#pragma once
#include "Pyrokinetic/Rendering/Shader.h"
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

namespace pk
{
	class VulkanShader : public Shader
	{
	public:
		VulkanShader(const std::string& path);
		VulkanShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);

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

		void UploadUniformInt(const std::string& name, const int values);
		void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);

		void UploadUniformFloat(const std::string& name, const float values);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& values);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& values);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& values);

		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
	private:
		std::string ReadFile(const std::string& path);
		std::unordered_map<VkShaderStageFlagBits, std::string> PreProcess(const std::string& src);
		void Compile(const std::unordered_map<VkShaderStageFlagBits, std::string>& sources);
	private:
		uint32_t m_RendererID;
		std::string m_Name;
	};
}