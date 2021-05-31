#pragma once
#include "Pyrokinetic/Rendering/Shader.h"
#include "OpenGLTexture.h"
#include <glm/glm.hpp>
#include <spirv_cross/spirv_cross_c.h>


namespace pk
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& path);
		OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);

		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetFloat(const std::string& name, const float value) override;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
		virtual void SetInt(const std::string& name, const int value) override;
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) override;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) override;

		virtual const std::string& GetName() const override { return m_Name; }

		uint32_t GetRendererID() const { return m_RendererID; }

		virtual void SetUniformBuffer(const std::string& name, const void* data, uint32_t size) override;

		virtual void AddResource(std::any resource) override
		{
			m_Textures.push_back(std::any_cast<std::shared_ptr<Texture2D>>(resource));
		}

		void BindNeededResources()
		{
			for (size_t i = 0; i < m_Textures.size(); i++)
			{
					m_Textures[i]->Bind(i+1);
			}
		}

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
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& src);
		std::unordered_map<uint32_t, std::vector<uint32_t>> CompileToVulkanSPV(std::unordered_map<GLenum, std::string> sources, std::unordered_map<uint32_t, std::vector<uint32_t>>& output);
		void CompileToOpenGLSPV(std::unordered_map<uint32_t, std::vector<uint32_t>>& shaderBinaries);
		void Load(const std::string& source, bool compile);
		void ParseConstantBuffers(spvc_compiler& compiler);

	private:
		uint32_t m_RendererID;
		std::string m_Name;

		std::string m_Path;

		uint32_t m_ConstantBufferOffset = 0;

		inline static std::unordered_map<uint32_t, ShaderUniformBuffer> s_UniformBuffers;

		std::unordered_map<std::string, ShaderBuffer> m_Buffers;
		std::unordered_map<std::string, ShaderResourceDeclaration> m_Resources;
		std::vector<std::shared_ptr<Texture2D>> m_Textures;
		std::unordered_map<std::string, GLint> m_UniformLocations;
	};
}