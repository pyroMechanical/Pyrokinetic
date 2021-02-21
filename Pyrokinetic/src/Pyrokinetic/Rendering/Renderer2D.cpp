#include "pkpch.h"
#include "Renderer2D.h"

#include "RenderCommand.h"

#include "VertexArray.h"
#include "Shader.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Pyrokinetic
{

	struct QuadVertex
	{
		glm::vec3 pos;
		glm::vec4 color;
		glm::vec2 texCoords;
		int texIndex;
	};

	struct RendererData2D
	{
		static const uint32_t maxQuads = 5000;
		static const uint32_t maxVertices = maxQuads * 4;
		static const uint32_t maxIndices = maxQuads * 6;
		static const uint32_t maxTextureSlots = 32; //TODO: renderer capability

		std::shared_ptr<VertexArray> quadVertexArray;
		std::shared_ptr<VertexBuffer> quadVertexBuffer;
		std::shared_ptr<Shader> shader;
		std::shared_ptr<Texture2D> whiteTexture;

		uint32_t quadIndexCount = 0;
		QuadVertex* quadVertexBufferStart = nullptr;
		QuadVertex* quadVertexBufferPtr = nullptr;

		std::array<std::shared_ptr<Texture2D>, maxTextureSlots> textureSlots;
		uint32_t textureSlotIndex = 1; //0 is basic white texture

		glm::vec4 quadVertexPositions[4];

		Renderer2D::Statistics stats;
	};

	static RendererData2D s_Data;

	void Renderer2D::Init()
	{
		PROFILE_FUNCTION();

		/*s_Data.quadVertexArray = VertexArray::Create();

		s_Data.quadVertexBuffer = VertexBuffer::Create(s_Data.maxVertices * sizeof(QuadVertex));
		BufferLayout quadVBLayout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" }, 
			{ ShaderDataType::Float2, "a_TexCoords" },
			{ ShaderDataType::Int, "a_TexIndex" }
		};
		s_Data.quadVertexBuffer->SetLayout(quadVBLayout);
		s_Data.quadVertexArray->AddVertexBuffer(s_Data.quadVertexBuffer);

		s_Data.quadVertexBufferStart = new QuadVertex[s_Data.maxVertices];

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
		std::shared_ptr<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_Data.maxIndices);
		s_Data.quadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		s_Data.whiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.whiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		int32_t samplers[s_Data.maxTextureSlots];
		for (uint32_t i = 0; i < s_Data.maxTextureSlots; i++)
			samplers[i] = i;

		s_Data.shader = Shader::Create("assets/shaders/Texture.glsl");
		s_Data.shader->Bind();
		s_Data.shader->SetIntArray("u_Textures", samplers, s_Data.maxTextureSlots);

		s_Data.textureSlots[0] = s_Data.whiteTexture;

		s_Data.quadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.quadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.quadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.quadVertexPositions[3] = {  -0.5f,  0.5f, 0.0f, 1.0f };*/
	}

	void Renderer2D::Shutdown()
	{
		PROFILE_FUNCTION();
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		PROFILE_FUNCTION();

		/*s_Data.shader->Bind();
		s_Data.shader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
		//s_Data.shader->SetMat4("u_Transform", glm::mat4(1.0f));

		s_Data.quadIndexCount = 0;
		s_Data.textureSlotIndex = 1;
		s_Data.quadVertexBufferPtr = s_Data.quadVertexBufferStart;*/
	}

	void Renderer2D::EndScene()
	{
		PROFILE_FUNCTION();

		/*uint32_t dataSize = (uint8_t*)s_Data.quadVertexBufferPtr - (uint8_t*)s_Data.quadVertexBufferStart;
		s_Data.quadVertexBuffer->SetData(s_Data.quadVertexBufferStart, dataSize);

		Flush();*/
	}

	void Renderer2D::Flush()
	{
		PROFILE_FUNCTION();

		for (uint32_t i = 0; i < s_Data.textureSlotIndex; i++)
		{
			s_Data.textureSlots[i]->Bind(i);
		}

		RenderCommand::DrawIndexed(s_Data.quadVertexArray, s_Data.quadIndexCount);

		s_Data.stats.drawCalls++;
	}

	void Renderer2D::StartNewBatch()
	{
		EndScene();

		s_Data.quadIndexCount = 0;
		s_Data.textureSlotIndex = 1;
		s_Data.quadVertexBufferPtr = s_Data.quadVertexBufferStart;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, float rotation)
	{
		Renderer2D::DrawQuad({ position.x, position.y, 0.0f }, size, color, rotation);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, float rotation)
	{
		if (s_Data.quadIndexCount >= RendererData2D::maxIndices)
		{
			StartNewBatch();
		}

		const float textureIndex = 0.0f;
	
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		if(rotation != 0) transform *= glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f });

		s_Data.quadVertexBufferPtr->pos = transform * s_Data.quadVertexPositions[0];
		s_Data.quadVertexBufferPtr->color = color;
		s_Data.quadVertexBufferPtr->texCoords = { 0.0f, 0.0f };
		s_Data.quadVertexBufferPtr->texIndex = textureIndex;
		s_Data.quadVertexBufferPtr++;

		s_Data.quadVertexBufferPtr->pos = transform * s_Data.quadVertexPositions[1];
		s_Data.quadVertexBufferPtr->color = color;
		s_Data.quadVertexBufferPtr->texCoords = { 1.0f, 0.0f };
		s_Data.quadVertexBufferPtr->texIndex = textureIndex;
		s_Data.quadVertexBufferPtr++;

		s_Data.quadVertexBufferPtr->pos = transform * s_Data.quadVertexPositions[2];
		s_Data.quadVertexBufferPtr->color = color;
		s_Data.quadVertexBufferPtr->texCoords = { 1.0f, 1.0f };
		s_Data.quadVertexBufferPtr->texIndex = textureIndex;
		s_Data.quadVertexBufferPtr++;

		s_Data.quadVertexBufferPtr->pos = transform * s_Data.quadVertexPositions[3];
		s_Data.quadVertexBufferPtr->color = color;
		s_Data.quadVertexBufferPtr->texCoords = { 0.0f, 1.0f };
		s_Data.quadVertexBufferPtr->texIndex = textureIndex;
		s_Data.quadVertexBufferPtr++;

		s_Data.quadIndexCount += 6;

		s_Data.stats.quadCount++;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const std::shared_ptr<Texture2D>& texture, float rotation, float tileFactor)
	{
		Renderer2D::DrawQuad({ position.x, position.y, 0.0f }, size, texture, rotation, tileFactor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<Texture2D>& texture, float rotation, float tileFactor)
	{

		if(s_Data.quadIndexCount >= RendererData2D::maxIndices)
		{
			StartNewBatch();
		}

		const glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

		float textureIndex = 0.0f;
		
		for (uint32_t i = 1; i < s_Data.textureSlotIndex; i++)
		{
			if (*s_Data.textureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if(textureIndex == 0.0f)
		{
			textureIndex = (float)s_Data.textureSlotIndex;
			s_Data.textureSlots[s_Data.textureSlotIndex] = texture;
			s_Data.textureSlotIndex++;
		}
		
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		if (rotation != 0) transform *= glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f });

		s_Data.quadVertexBufferPtr->pos = transform * s_Data.quadVertexPositions[0];
		s_Data.quadVertexBufferPtr->color = color;
		s_Data.quadVertexBufferPtr->texCoords = { 0.0f, 0.0f };
		s_Data.quadVertexBufferPtr->texIndex = textureIndex;
		s_Data.quadVertexBufferPtr++;

		s_Data.quadVertexBufferPtr->pos = transform * s_Data.quadVertexPositions[1];
		s_Data.quadVertexBufferPtr->color = color;
		s_Data.quadVertexBufferPtr->texCoords = { 1.0f, 0.0f };
		s_Data.quadVertexBufferPtr->texIndex = textureIndex;
		s_Data.quadVertexBufferPtr++;

		s_Data.quadVertexBufferPtr->pos = transform * s_Data.quadVertexPositions[2];
		s_Data.quadVertexBufferPtr->color = color;
		s_Data.quadVertexBufferPtr->texCoords = { 1.0f, 1.0f };
		s_Data.quadVertexBufferPtr->texIndex = textureIndex;
		s_Data.quadVertexBufferPtr++;

		s_Data.quadVertexBufferPtr->pos = transform * s_Data.quadVertexPositions[3];
		s_Data.quadVertexBufferPtr->color = color;
		s_Data.quadVertexBufferPtr->texCoords = { 0.0f, 1.0f };
		s_Data.quadVertexBufferPtr->texIndex = textureIndex;
		s_Data.quadVertexBufferPtr++;

		s_Data.quadIndexCount += 6;

		s_Data.stats.quadCount++;
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