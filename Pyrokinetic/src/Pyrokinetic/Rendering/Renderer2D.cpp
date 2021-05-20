#include "pkpch.h"
#include "Renderer2D.h"

#include "RenderCommand.h"
#include "Renderer.h"

#include "Shader.h"
#include <glm/gtc/matrix_transform.hpp>

namespace pk
{

	struct QuadVertex
	{
		glm::vec3 pos;
		glm::vec4 color;
		glm::vec2 texCoords;
		float texIndex;
		float tileFactor;
	};

	struct QuadInfo
	{
		glm::vec3 pos;
		glm::vec2 size;
		glm::vec4 color;
		std::shared_ptr<SubTexture2D>& texture;
		float rotation;
		float tilescaling;
	};

	struct RendererData2D
	{
		static const uint32_t maxQuads = 5000;
		static const uint32_t maxVertices = maxQuads * 4;
		static const uint32_t maxIndices = maxQuads * 6;
		static const uint32_t maxTextureSlots = 32; //TODO: renderer capability
		std::shared_ptr<VertexBuffer> quadVertexBuffer;
		std::shared_ptr<IndexBuffer> quadIndexBuffer;
		std::shared_ptr<Shader> shader;
		std::shared_ptr<Texture2D> whiteTexture;

		uint32_t quadIndexCount = 0;
		QuadVertex* quadVertexBufferData = nullptr;
		QuadVertex* quadVertexBufferPtr = nullptr;

		inline static std::array<std::shared_ptr<Texture2D>, maxTextureSlots> textureSlots;
		uint32_t textureSlotIndex = 1; //0 is basic white texture

		glm::vec4 quadVertexPositions[4];

		std::shared_ptr<Pipeline> quadPipeline;

		Renderer2D::Statistics stats;
	};

	static RendererData2D s_Data;

	void Renderer2D::Init()
	{
		PROFILE_FUNCTION();

		s_Data.quadVertexBuffer = VertexBuffer::Create(s_Data.maxVertices * sizeof(QuadVertex));
		BufferLayout quadVBLayout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" }, 
			{ ShaderDataType::Float2, "a_TexCoords" },
			{ ShaderDataType::Float, "a_TexIndex" },
			{ ShaderDataType::Float, "a_TileFactor"}
		};
		s_Data.quadVertexBuffer->SetLayout(quadVBLayout);

		s_Data.quadVertexBufferData = new QuadVertex[s_Data.maxVertices];

		uint32_t* quadIndices = new uint32_t[s_Data.maxIndices];

		uint32_t offset = 0;
		for(uint32_t i = 0; i < s_Data.maxIndices; i+=6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;
			quadIndices[i + 3] = offset + 0;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 2;

			offset += 4;
		}
		s_Data.quadIndexBuffer = IndexBuffer::Create(quadIndices, s_Data.maxIndices);
		delete[] quadIndices;

		s_Data.whiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.whiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		int32_t samplers[s_Data.maxTextureSlots];
		for (uint32_t i = 0; i < s_Data.maxTextureSlots; i++)
			samplers[i] = i;

		PipelineSpecification spec;
		spec.Shader = Shader::Create("assets/shaders/Texture.glsl");

		RenderPassSpecification renderPassSpec;
		renderPassSpec.Attachments = { ImageFormat::RGBA8
			, ImageFormat::Depth
		};
		renderPassSpec.layout = ImageLayout::ShaderReadOnly;
		renderPassSpec.clearColor = glm::vec4{ 0.1f, 0.1f, 0.1f, 1.0f };
		auto renderPass = RenderPass::Create(renderPassSpec);
		Renderer::AddRenderPass("SceneView", renderPass);
		spec.RenderPass = renderPass;
		spec.vertexBufferLayout =
		{
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Float2, "a_TexCoords" },
			{ ShaderDataType::Float, "a_TexIndex" },
			{ ShaderDataType::Float, "a_TileFactor" }
		};
		spec.Shader->AddResource(s_Data.whiteTexture);
		s_Data.quadPipeline = Pipeline::Create(spec);

		s_Data.textureSlots[0] = s_Data.whiteTexture;

		s_Data.quadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.quadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.quadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.quadVertexPositions[3] = {  -0.5f,  0.5f, 0.0f, 1.0f };
	}

	void Renderer2D::Shutdown()
	{
		PROFILE_FUNCTION();
	}

	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		PROFILE_FUNCTION();

		s_Data.quadIndexCount = 0;

		s_Data.quadVertexBufferData = (QuadVertex*)s_Data.quadVertexBuffer->Map();
		s_Data.quadVertexBufferPtr = s_Data.quadVertexBufferData;

		struct{
			glm::mat4 view;
			glm::mat4 projection;
		} cameraBuffer;
		cameraBuffer.view = glm::inverse(transform);
		cameraBuffer.projection = camera.GetProjection();

		s_Data.quadPipeline->GetSpecification().Shader->SetUniformBuffer("Camera", &cameraBuffer, sizeof(cameraBuffer));
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		PROFILE_FUNCTION();
		s_Data.quadIndexCount = 0;

		s_Data.quadVertexBufferData = (QuadVertex*) s_Data.quadVertexBuffer->Map();
		s_Data.quadVertexBufferPtr = s_Data.quadVertexBufferData;
	}

	void Renderer2D::EndScene()
	{
		PROFILE_FUNCTION();

		//if (s_Data.quadIndexCount == 0) return;
		std::ptrdiff_t size = (char*)s_Data.quadVertexBufferPtr - (char*)s_Data.quadVertexBufferData;
		s_Data.quadVertexBuffer->Unmap(size);
		Renderer::Submit(s_Data.quadPipeline, s_Data.quadVertexBuffer, s_Data.quadIndexBuffer, s_Data.quadIndexCount);
	}

	void Renderer2D::Flush()
	{
		PROFILE_FUNCTION();

		/*for (uint32_t i = 0; i < s_Data.textureSlotIndex; i++)
		{
			s_Data.textureSlots[i]->Bind(i);
		}*/

		Renderer::Flush();

		s_Data.stats.drawCalls++;
	}

	void Renderer2D::StartNewBatch()
	{
		EndScene();

		s_Data.quadIndexCount = 0;
		s_Data.textureSlotIndex = 1;
		s_Data.quadVertexBufferPtr = s_Data.quadVertexBufferData;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, const std::shared_ptr<Texture2D>& texture, float rotation, float tileFactor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color, texture, rotation, tileFactor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, const std::shared_ptr<Texture2D>& texture, float rotation, float tileFactor)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		if (rotation != 0) transform *= glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f });

		DrawQuad(transform, color, texture, tileFactor);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, const std::shared_ptr<Texture2D>& texture, float tileFactor)
	{

		if (s_Data.quadIndexCount >= RendererData2D::maxIndices)
		{
			StartNewBatch();
		}
		constexpr size_t quadVertexCount = 4;
		glm::vec2 textureCoords[] = { {0, 0}, {1, 0}, {1, 1}, {0, 1} };

		int textureIndex = 0.0f;
		if(texture != nullptr)
		{
			for (uint32_t i = 1; i < s_Data.textureSlotIndex; ++i)
			{
				if (*s_Data.textureSlots[i].get() == *texture.get())
				{
					textureIndex = i;
					break;
				}
			}

			if (textureIndex == 0.0f)
			{
				textureIndex = s_Data.textureSlotIndex;
				s_Data.textureSlots[s_Data.textureSlotIndex] = texture;
				s_Data.quadPipeline->GetSpecification().Shader->AddResource(texture);
				++s_Data.textureSlotIndex;
			}
		}

		for (size_t i = 0; i < quadVertexCount; ++i)
		{
			s_Data.quadVertexBufferPtr->pos = transform * s_Data.quadVertexPositions[i];
			s_Data.quadVertexBufferPtr->color = color;
			s_Data.quadVertexBufferPtr->texCoords = textureCoords[i];
			s_Data.quadVertexBufferPtr->texIndex = textureIndex;
			s_Data.quadVertexBufferPtr->tileFactor = tileFactor;
			++s_Data.quadVertexBufferPtr;
		}
		s_Data.quadIndexCount += 6;

		++s_Data.stats.quadCount;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const std::shared_ptr<SubTexture2D>& subtexture, float rotation, float tileFactor)
	{
		Renderer2D::DrawQuad({ position.x, position.y, 0.0f }, size, subtexture, rotation, tileFactor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<SubTexture2D>& subtexture, float rotation, float tileFactor)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		if (rotation != 0) transform *= glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f });

		DrawQuad(transform, glm::vec4(1.0f), subtexture, tileFactor);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, const std::shared_ptr<SubTexture2D>& subtexture, float tileFactor)
	{
		if (s_Data.quadIndexCount >= RendererData2D::maxIndices)
		{
			StartNewBatch();
		}

		constexpr size_t quadVertexCount = 4;
		std::array<glm::vec2, 4> textureCoords = { glm::vec2{0, 0}, glm::vec2{1, 0}, glm::vec2{1, 1}, glm::vec2{0, 1} };
		if (subtexture)
		{
			textureCoords = subtexture->GetTexCoords();
		}

		std::shared_ptr<Texture2D> texture = nullptr;
		if(subtexture)
			texture = subtexture->GetTexture();

		float textureIndex = 0.0f;

		if (texture)
		{
			for (uint32_t i = 1; i < s_Data.textureSlotIndex; ++i)
			{
				if (*s_Data.textureSlots[i].get() == *texture.get())
				{
					textureIndex = i;
					break;
				}
			}

			if (textureIndex == 0.0f)
			{
				textureIndex = s_Data.textureSlotIndex;
				s_Data.textureSlots[s_Data.textureSlotIndex] = texture;
				s_Data.quadPipeline->GetSpecification().Shader->AddResource(texture);
				++s_Data.textureSlotIndex;
			}
		}

		for (size_t i = 0; i < quadVertexCount; ++i)
		{
			s_Data.quadVertexBufferPtr->pos = transform * s_Data.quadVertexPositions[i];
			s_Data.quadVertexBufferPtr->color = color;
			s_Data.quadVertexBufferPtr->texCoords = textureCoords[i];
			s_Data.quadVertexBufferPtr->texIndex = textureIndex;
			s_Data.quadVertexBufferPtr->tileFactor = tileFactor;
			++s_Data.quadVertexBufferPtr;
		}
		s_Data.quadIndexCount += 6;

		++s_Data.stats.quadCount;
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_Data.stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics& Renderer2D::GetStats()
	{
		return s_Data.stats;
	}

}