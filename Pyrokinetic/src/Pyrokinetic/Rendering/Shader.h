#pragma once
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include <any>

namespace pk
{
	enum class UniformType
	{
		None = 0,
		Float, Float2, Float3, Float4,
		Mat3, Mat4, Int, Uint
	};

	struct Uniform
	{
		UniformType Type;
		std::ptrdiff_t Offset;
		std::string Name;
	};

	/*struct UniformBuffer
	{
		uint8_t* Buffer;
		std::vector<Uniform> Uniforms;
	};*/

	struct UniformBufferBase
	{
		virtual const uint8_t* GetBuffer() const = 0;
		virtual const Uniform* GetUniforms() const = 0;
		virtual unsigned int GetUniformCount() const = 0;
	};

	template<unsigned int N, unsigned int U>
	struct UniformBufferDeclaration : public UniformBufferBase
	{
		uint8_t Buffer[N];
		Uniform Uniforms[U];
		std::ptrdiff_t Cursor = 0;
		int Index = 0;

		virtual const uint8_t* GetBuffer() const override { return Buffer; }
		virtual const Uniform* GetUniforms() const override { return Uniforms; }
		virtual unsigned int GetUniformCount() const override { return U; }

		template<typename T>
		void Push(const std::string& name, const T& data) {
			Uniforms[++Index] = { GetUniformTypeFromObject(std::any_cast<T>(data)), Cursor, name };
			memcpy(Buffer + Cursor, &data, sizeof(T));
			Cursor += sizeof(T);
		}

	private:
		UniformType GetUniformTypeFromObject(std::any object)
		{
			switch(object.type)
			{
			case int32_t: return UniformType::Int;
			case uint32_t: return UniformType::Uint;
			case float: return UniformType::Float;
			case glm::vec2: return UniformType::Float2;
			case glm::vec3: return UniformType::Float3;
			case glm::vec4: return UniformType::Float4;
			case glm::mat3: return UniformType::Mat3;
			case glm::mat4: return UniformType::Mat4;
			}
			PK_CORE_ASSERT(false, "Unrecognized type!")
			return UniformType::None;
		}
	};

	enum class ShaderUniformType
	{
		None = 0, Bool, Int, UInt, Float, Vec2, Vec3, Vec4, Mat3, Mat4
	};

	class ShaderUniform
	{
	public:
		ShaderUniform() = default;
		ShaderUniform(const std::string& name, ShaderUniformType type, uint32_t size, uint32_t offset);

		const std::string& GetName() const { return m_Name; }
		ShaderUniformType GetType() const { return m_Type; }
		uint32_t GetSize() const { return m_Size; }
		uint32_t GetOffset() const { return m_Offset; }

		static const std::string& UniformTypeToString(ShaderUniformType type);
	private:
		std::string m_Name;
		ShaderUniformType m_Type = ShaderUniformType::None;
		uint32_t m_Size = 0;
		uint32_t m_Offset = 0;
	};

	struct ShaderUniformBuffer
	{
		std::string Name;
		uint32_t Index;
		uint32_t BindingPoint;
		uint32_t Size;
		uint32_t RendererID;
		std::vector<ShaderUniform> Uniforms;
	};

	struct ShaderBuffer
	{
		std::string Name;
		uint32_t Size = 0;
		std::unordered_map<std::string, ShaderUniform> Uniforms;
	};


	class ShaderResourceDeclaration
	{
	public:
		ShaderResourceDeclaration() = default;
		ShaderResourceDeclaration(const std::string& name, uint32_t resourceRegister, uint32_t count)
			: m_Name(name), m_Register(resourceRegister), m_Count(count) {}

		virtual const std::string& GetName() const { return m_Name; }
		virtual uint32_t GetRegister() const { return m_Register; }
		virtual uint32_t GetCount() const { return m_Count; }
	private:
		std::string m_Name;
		uint32_t m_Register = 0;
		uint32_t m_Count = 0;
	};

	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetFloat(const std::string& name, const float value) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetInt(const std::string& name, const int value) = 0;
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;
		
		virtual void SetUniformBuffer(const std::string& name, const void* data, uint32_t size) = 0;

		virtual void AddResource(std::any resource) = 0;

		virtual const std::string& GetName() const = 0;

		static std::shared_ptr<Shader> Create(const std::string& path);
		static std::shared_ptr<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		static std::shared_ptr<Shader> Create(const std::string& vertexPath, const std::string& fragmentPath);
	};

	class ShaderLibrary
	{
	public:
		void Add(const std::shared_ptr<Shader>& shader);
		void Add(const std::string& name, const std::shared_ptr<Shader>& shader);
		std::shared_ptr<Shader> Load(const std::string& path);
		std::shared_ptr<Shader> Load(const std::string& path, const std::string& name);

		std::shared_ptr<Shader> Get(const std::string& name);
	private:
		std::unordered_map<std::string, std::shared_ptr<Shader>> m_Shaders;
	};
}