#include "pkpch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/Vulkan/VulkanShader.h"

namespace pk
{
	std::shared_ptr<Shader> Shader::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
			case API::None:       PK_CORE_ASSERT(false, "RenderAPI::None is currently not supported!"); return nullptr;
			case API::OpenGL:     return std::make_shared<OpenGLShader>(path);
			case API::Vulkan:     return std::make_shared<VulkanShader>(path);
		}

		PK_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;

	}

	std::shared_ptr<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
			case API::None:       PK_CORE_ASSERT(false, "RenderAPI::None is currently not supported!"); return nullptr;
			case API::OpenGL:     return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc);
			case API::Vulkan:     return std::make_shared<VulkanShader>(name, vertexSrc, fragmentSrc);
		}

		PK_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;

	}

	void ShaderLibrary::Add(const std::shared_ptr<Shader>& shader)
	{
		auto& name = shader->GetName();
		Add(name, shader);
	}

	void ShaderLibrary::Add(const std::string& name, const std::shared_ptr<Shader>& shader)
	{
		PK_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end(), "Shader already exists!");
		m_Shaders[name] = shader;
	}

	std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& path)
	{
		auto shader = Shader::Create(path);
		Add(shader);
		return shader;
	}

	std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& path, const std::string& name)
	{
		auto shader = Shader::Create(path);
		Add(name, shader);
		return shader;
	}

	std::shared_ptr<Shader> ShaderLibrary::Get(const std::string& name)
	{
		PK_CORE_ASSERT(m_Shaders.find(name) != m_Shaders.end(), "Shader not found!");
		return m_Shaders[name];
	}
}