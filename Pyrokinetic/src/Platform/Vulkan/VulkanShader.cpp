#include "pkpch.h"
#include "VulkanShader.h"
#include "VulkanContext.h"

#include <fstream>
#include <vulkan/vulkan.h>

#include <glm/gtc/type_ptr.hpp>

#include <shaderc/shaderc.hpp>

namespace pk
{

	namespace util
	{
		static VkShaderModule LoadShaderModuleFromFile(const std::string& filePath)
		{
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
		Compile(shaderSources);

		// pull shader name from source file
		auto lastSlash = path.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = path.rfind(".");
		auto count = lastDot == std::string::npos ? path.size() - lastSlash : lastDot - lastSlash;
		m_Name = path.substr(lastSlash, count);
	}

	VulkanShader::VulkanShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath)
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
	}

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

	void VulkanShader::Compile(const std::unordered_map<VkShaderStageFlagBits, std::string>& sources)
	{
		PROFILE_FUNCTION();
		
	}


	void VulkanShader::Bind() const
	{
		PROFILE_FUNCTION();

		//glUseProgram(m_RendererID);
	}

	void VulkanShader::Unbind() const
	{
		PROFILE_FUNCTION();

		//glUseProgram(0);
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

}