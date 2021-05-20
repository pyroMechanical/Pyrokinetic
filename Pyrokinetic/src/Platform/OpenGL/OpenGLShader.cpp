#include "pkpch.h"
#include "OpenGLShader.h"

#include <fstream>
#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

#include <shaderc/shaderc.hpp>

#include <filesystem>
#include <utility>

namespace pk
{

	namespace util {

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

		static const char* ShaderStageCachedSPVFileExtension(uint32_t stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER: return ".vert.spv";
			case GL_FRAGMENT_SHADER: return ".frag.spv";
			}

			PK_CORE_ASSERT(false, "Stage not currently supported or recognized!")
			return "";
		}

		static const char* ShaderStageCachedGLSLFileExtension(uint32_t stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER: return ".vert";
			case GL_FRAGMENT_SHADER: return ".frag";
			}

			PK_CORE_ASSERT(false, "Stage not currently supported or recognized!")
				return "";
		}

		static shaderc_shader_kind ShaderStageToShaderCStage(uint32_t stage)
		{
			switch(stage)
			{
			case GL_VERTEX_SHADER: return shaderc_vertex_shader;
			case GL_FRAGMENT_SHADER: return shaderc_fragment_shader;
			}

			PK_CORE_ASSERT(false, "Stage not currently supported or recognized!")
				return (shaderc_shader_kind)0;
		}

		static std::string ShaderStageToString(uint32_t stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER: return "Vertex";
			case GL_FRAGMENT_SHADER: return "Fragment";
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
			switch(dim)
			{
			case SpvDim1D: return 1; break;
			case SpvDim2D: return 2; break;
			case SpvDim3D: return 3; break;
			}
			PK_ASSERT(false, "Dimension type not supported!");
			return -1;
		}
	}


	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel" || type == "frag")
			return GL_FRAGMENT_SHADER;

		PK_CORE_ASSERT(false, "Unknown shader type!");
	}

	OpenGLShader::OpenGLShader(const std::string& path)
		: m_Path(path)
	{
		PROFILE_FUNCTION();

		// assets/shaders/Texture.glsl
		auto lastSlash = path.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = path.rfind(".");
		auto count = lastDot == std::string::npos ? path.size() - lastSlash : lastDot - lastSlash;
		m_Name = path.substr(lastSlash, count);

		m_Path = path;

		util::CreateCacheDirectoryIfNeeded();

		std::string src = ReadFile(path);
		Load(src, true);
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
		: m_Name(name)
	{
		PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;
		std::unordered_map<uint32_t, std::vector<uint32_t>> outputResult;
		CompileToVulkanSPV(sources, outputResult);
		CompileToOpenGLSPV(outputResult);
	}

	OpenGLShader::~OpenGLShader()
	{
		PROFILE_FUNCTION();

		glDeleteProgram(m_RendererID);
	}

	void OpenGLShader::Load(const std::string& source, bool compile)
	{
		auto shaderSources = PreProcess(source);
		std::unordered_map<uint32_t, std::vector<uint32_t>> outputResult;
		CompileToVulkanSPV(shaderSources, outputResult);
		CompileToOpenGLSPV(outputResult);
	}

	std::string OpenGLShader::ReadFile(const std::string& path)
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

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& src)
	{
		PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> sources;

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

	std::unordered_map<uint32_t, std::vector<uint32_t>> OpenGLShader::CompileToVulkanSPV(std::unordered_map<GLenum, std::string> sources, std::unordered_map<uint32_t, std::vector<uint32_t>>& output)
	{
		PROFILE_FUNCTION();

		std::filesystem::path cacheDir = util::GetCacheDirectory();
		for (auto [stage, source] : sources)
		{
			auto extension = util::ShaderStageCachedSPVFileExtension(stage);
			if (false)
			{
				auto path = cacheDir / (m_Name + extension);

				std::string cachePath = path.string();

				FILE* f = fopen(cachePath.c_str(), "rb");
				if(f)
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
				options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
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
		return output;
	}

	void OpenGLShader::CompileToOpenGLSPV(std::unordered_map<uint32_t, std::vector<uint32_t>>& shaderBinaries)
	{
		if (m_RendererID)
			glDeleteProgram(m_RendererID);

		GLuint program = glCreateProgram();
		m_RendererID = program;

		std::vector<GLuint> shaderIDs;
		shaderIDs.reserve(shaderBinaries.size());

		std::filesystem::path cacheDirectory = util::GetCacheDirectory();

		m_ConstantBufferOffset = 0;
		std::vector<std::string> shaderSources;
		for (auto& [stage, binary] : shaderBinaries)
		{
			{
				spvc_context context = nullptr;
				spvc_context_create(&context);
				//spvc_context_set_error_callback(context, error_callback)
				
				spvc_parsed_ir ir = nullptr;
				spvc_compiler glsl = nullptr;
				spvc_context_parse_spirv(context, binary.data(), binary.size(), &ir);
				spvc_context_create_compiler(context, SPVC_BACKEND_GLSL, ir, SPVC_CAPTURE_MODE_COPY, &glsl);

				ParseConstantBuffers(glsl);

				auto path = cacheDirectory / (m_Name + util::ShaderStageCachedGLSLFileExtension(stage));
				std::string cachedPath = path.string();

				std::string& source = shaderSources.emplace_back();

				if (false)
				{
					FILE* f = fopen(cachedPath.c_str(), "rb");
					if(f)
					{
						fseek(f, 0, SEEK_END);
						size_t size = ftell(f);
						fseek(f, 0, SEEK_SET);
						source = std::string(size / sizeof(char), (char)0);
						fread(source.data(), sizeof(char), source.length(), f);
						fclose(f);
					}
				}

				if (!source.length())
				{
					spvc_compiler_options spvc_options = nullptr;
					spvc_compiler_create_compiler_options(glsl, &spvc_options);
					spvc_compiler_options_set_uint(spvc_options, SPVC_COMPILER_OPTION_GLSL_VERSION, 450);
					spvc_compiler_options_set_bool(spvc_options, SPVC_COMPILER_OPTION_GLSL_ES, SPVC_FALSE);
					spvc_compiler_install_compiler_options(glsl, spvc_options);
					const char* result = nullptr;
					auto success = spvc_compiler_compile(glsl, &result);
					if (success != SPVC_SUCCESS)
						PK_CORE_ERROR("Shader Complilation failed: {0}", success);
					source = std::string(result);

					PK_CORE_TRACE("Shader Source GLSL:\n {0}\n", source.c_str());

					spvc_context_destroy(context);
					{
						auto path = cacheDirectory / (m_Name + util::ShaderStageCachedGLSLFileExtension(stage));
						std::string cachedPath = path.string();

						FILE* f = fopen(cachedPath.c_str(), "wb");
						fwrite(source.c_str(), sizeof(char), source.length(), f);
						fclose(f);
					}
				}

				GLuint shaderID = glCreateShader(stage);
				const char* sourceCStr = source.c_str();

				glShaderSource(shaderID, 1, &sourceCStr, 0);
				glCompileShader(shaderID);
				GLint compiled = 0;
				glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compiled);
				if(compiled == GL_FALSE)
				{
					GLint maxLength = 0;
					glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

					std::vector<GLchar> infoLog(maxLength);
					glGetShaderInfoLog(program, maxLength, &maxLength, &infoLog[0]);
					PK_CORE_ERROR("Shader linking failed({0}):\n{1}", m_Name, &infoLog[0]);

					glDeleteShader(shaderID);
				}
				glAttachShader(program, shaderID);

				shaderIDs.emplace_back(shaderID);
			}
		}

		glLinkProgram(program);

		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
			PK_CORE_ERROR("Shader linking failed({0}):\n{1}", m_Name, &infoLog[0]);
			
			glDeleteProgram(program);
			for (auto id : shaderIDs)
				glDeleteShader(id);
		}

		for (auto id : shaderIDs)
		{
			glDetachShader(program, id);
			glDeleteShader(id);
		}

		glUseProgram(program);		

		/*for (auto& [bufferName, buffer] : m_Buffers)
		{
			for (auto& [name, uniform] : buffer.Uniforms)
			{
				//TODO: find out why ViewProjection uniform doesn't actually work here! suspect the issue is name/object type related, check what the uniform names are for shader tomorrow.
				PK_CORE_ERROR("{0}", bufferName);
				GLint index = glGetProgramResourceIndex(program, GL_UNIFORM, bufferName.c_str());
				PK_CORE_ERROR("Uniform Index: {0}", index);
				GLenum props[] = { GL_LOCATION };
				GLint values[1];
				glGetProgramResourceiv(program, GL_UNIFORM, index, 1, &props[0], 1, NULL, &values[0]);
				GLint location = values[0];
				if (location == -1)
				{
					PK_CORE_WARN("{0}: could not find uniform location {0}", bufferName.c_str());
				}
				else 
				{
					PK_CORE_TRACE("Uniform name: {0}, Location: {1}", bufferName.c_str(), location);
					m_UniformLocations[bufferName] = location;
				}
			}
		}*/

		size_t resourceCount = 0;

		for (auto& [name, resource] : m_Resources)
		{
			PK_CORE_INFO("Resource name: {0}", name);
			GLint location = glGetUniformLocation(m_RendererID, name.c_str());
			if (location == -1) 
			{
				PK_CORE_WARN("{0}: could not find Resource location {0}", name.c_str());
				m_Resources.erase(name);
			}
			else
			{
				glUniform1i(location, resource.GetRegister());
				resourceCount += resource.GetCount();
			}
		}

		for (auto& [stage, binary] : shaderBinaries)
		{
			spvc_context context = nullptr;
			spvc_context_create(&context);
			//spvc_context_set_error_callback(context, error_callback)

			spvc_parsed_ir ir = nullptr;
			spvc_compiler parser = nullptr;
			spvc_context_parse_spirv(context, binary.data(), binary.size(), &ir);
			spvc_context_create_compiler(context, SPVC_BACKEND_NONE, ir, SPVC_CAPTURE_MODE_COPY, &parser);

			spvc_resources res = nullptr;
			const spvc_reflected_resource* list = nullptr;
			size_t count;

			spvc_compiler_create_shader_resources(parser, &res);
			spvc_resources_get_resource_list_for_type(res, SPVC_RESOURCE_TYPE_UNIFORM_BUFFER, &list, &count);

			for (size_t i = 0; i < count; ++i)
			{
				const std::string bufferName = spvc_compiler_get_name(parser, list[i].base_type_id);
				auto bufferType = spvc_compiler_get_type_handle(parser, list[i].type_id);
				size_t bufferSize;
				spvc_compiler_get_declared_struct_size(parser, bufferType, &bufferSize);
				auto bindingPoint = spvc_compiler_get_decoration(parser, list[i].id, SpvDecorationBinding);
				if (s_UniformBuffers.find(bindingPoint) == s_UniformBuffers.end())
				{
					ShaderUniformBuffer& uniformBuffer = s_UniformBuffers[bindingPoint];
					uniformBuffer.Name = bufferName;
					uniformBuffer.BindingPoint = bindingPoint;
					uniformBuffer.Size = bufferSize;

					glCreateBuffers(1, &uniformBuffer.RendererID);
					glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer.RendererID);
					glBufferData(GL_UNIFORM_BUFFER, uniformBuffer.Size, nullptr, GL_DYNAMIC_DRAW);
					glBindBufferBase(GL_UNIFORM_BUFFER, uniformBuffer.BindingPoint, uniformBuffer.RendererID);

					PK_CORE_TRACE("Created Uniform buffer at binding point {0} with name '{1}', size {2}", uniformBuffer.BindingPoint, uniformBuffer.Name, uniformBuffer.Size);

					glBindBuffer(GL_UNIFORM_BUFFER, 0);
				}
				else
				{
					ShaderUniformBuffer& uniformBuffer = s_UniformBuffers.at(bindingPoint);
					PK_CORE_ASSERT(uniformBuffer.Name == bufferName, "Buffer at wrong location!");
					if (bufferSize > uniformBuffer.Size)
					{
						uniformBuffer.Size = bufferSize;

						glDeleteBuffers(1, &uniformBuffer.RendererID);
						glCreateBuffers(1, &uniformBuffer.RendererID);
						glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer.RendererID);
						glBufferData(GL_UNIFORM_BUFFER, uniformBuffer.Size, nullptr, GL_DYNAMIC_DRAW);
						glBindBufferBase(GL_UNIFORM_BUFFER, uniformBuffer.BindingPoint, uniformBuffer.RendererID); 
						
						PK_CORE_TRACE("Resized Uniform buffer at binding point {0} with name '{1}', size {2}", uniformBuffer.BindingPoint, uniformBuffer.Name, uniformBuffer.Size);

						glBindBuffer(GL_UNIFORM_BUFFER, 0);
					}
				}
			}
		}

		glUseProgram(0);
	}

	void OpenGLShader::ParseConstantBuffers(spvc_compiler& compiler)
	{
		spvc_resources resources = nullptr;
		const spvc_reflected_resource* list = nullptr;
		size_t count;
		spvc_compiler_create_shader_resources(compiler, &resources);
		spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_UNIFORM_BUFFER, &list, &count);

		for (size_t i = 0; i < count; ++i)
		{
			const std::string bufferName = spvc_compiler_get_name(compiler, list[i].base_type_id);
			auto bufferType = spvc_compiler_get_type_handle(compiler, list[i].type_id);
			size_t bufferSize;
			spvc_compiler_get_declared_struct_size(compiler, bufferType, &bufferSize);
			auto location = spvc_compiler_get_decoration(compiler, list[i].id, SpvDecorationLocation);
			auto bindingPoint = spvc_compiler_get_decoration(compiler, list[i].id, SpvDecorationBinding);
			int memberCount = spvc_type_get_num_member_types(bufferType);

			if (bufferName == "u_Renderer")
			{
				m_ConstantBufferOffset += bufferSize;
				continue;
			}

			ShaderBuffer& buffer = m_Buffers[bufferName];
			buffer.Name = bufferName;
			buffer.Size = bufferSize - m_ConstantBufferOffset;
			for (int j = 0; j < memberCount; j++)
			{
				auto typeID = spvc_type_get_member_type(bufferType, j);
				auto memberType = spvc_compiler_get_type_handle(compiler, typeID);
				const std::string& memberName(spvc_compiler_get_member_name(compiler, spvc_type_get_base_type_id(bufferType), j));

				PK_CORE_INFO("Shader Uniform Info: MemberName: {0}, {1}, {2}", bufferName, memberName, bufferName + "." + memberName);
				size_t size;
				unsigned int offset;
				spvc_compiler_get_declared_struct_member_size(compiler, bufferType, j, &size);
				spvc_compiler_type_struct_member_offset(compiler, bufferType, j, &offset);
				offset -= m_ConstantBufferOffset;
				std::string uniformName = memberName;
				buffer.Uniforms[uniformName] = ShaderUniform(uniformName, util::SPIRTypeToShaderUniformType(memberType), size, offset);
				PK_CORE_WARN("Uniform Name: {0}", uniformName);
			}
			m_ConstantBufferOffset += bufferSize;
		}

		const spvc_reflected_resource* imageList = nullptr;
		size_t imageCount;

		spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_SAMPLED_IMAGE, &imageList, &imageCount);

		for (size_t i = 0; i < imageCount; ++i)
		{
			auto resourceType = spvc_compiler_get_type_handle(compiler, imageList[i].type_id);
			auto resourceBinding = spvc_compiler_get_decoration(compiler, imageList[i].id, SpvDecorationBinding);
			std::string resourceName = std::string(imageList[i].name);
			uint32_t dimension = util::SpvDimTypeToDimensionNumber(spvc_type_get_image_dimension(resourceType));
			uint32_t arrayLength = spvc_type_get_array_dimension(resourceType, 0);
			PK_CORE_INFO("Resource dimension: {0}, Resource Array Size: {1}", dimension, arrayLength);

			m_Resources[resourceName] = ShaderResourceDeclaration(resourceName, resourceBinding, arrayLength);
		}
	}

	void OpenGLShader::Bind() const
	{
		PROFILE_FUNCTION();

		glUseProgram(m_RendererID);
	}

	void OpenGLShader::Unbind() const
	{
		PROFILE_FUNCTION();

		glUseProgram(0);
	}

	void OpenGLShader::SetUniformBuffer(const std::string& name, const void* data, uint32_t size)
	{
		ShaderUniformBuffer* uniformBuffer = nullptr;
		for (auto& [bindingPoint, ub] : s_UniformBuffers)
		{
			if (ub.Name == name)
			{
				uniformBuffer = &ub;
				break;
			}
		}

		PK_CORE_ASSERT(uniformBuffer, "Buffer does not exist!");
		PK_CORE_ASSERT(uniformBuffer->Size >= size, "Buffer too small for data sent!");
		glNamedBufferSubData(uniformBuffer->RendererID, 0, size, data);
	}

	void OpenGLShader::SetFloat(const std::string& name, const float value)
	{
		PROFILE_FUNCTION();

		UploadUniformFloat(name, value);
	}

	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		PROFILE_FUNCTION();

		UploadUniformFloat3(name, value);
	}

	void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		PROFILE_FUNCTION();

		UploadUniformFloat4(name, value);
	}

	void OpenGLShader::SetInt(const std::string& name, const int value)
	{
		PROFILE_FUNCTION();

		UploadUniformInt(name, value);
	}

	void OpenGLShader::SetIntArray(const std::string& name, int* values, uint32_t count)
	{
		UploadUniformIntArray(name, values, count);
	}

	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		PROFILE_FUNCTION();

		UploadUniformMat4(name, value);
	}

	void OpenGLShader::UploadUniformInt(const std::string& name, const int values)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, values);
	}

	void OpenGLShader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1iv(location, count, values);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& name, const float values)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1f(location, values);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& values)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2f(location, values.x, values.y);
	}

	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& values)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3f(location, values.x, values.y, values.z);
	}

	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& values)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4f(location, values.x, values.y, values.z, values.w);
	}

	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}


}