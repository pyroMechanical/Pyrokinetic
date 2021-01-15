#include "pkpch.h"
#include "Renderer2D.h"

#include "RenderCommand.h"

#include "VertexArray.h"
#include "Shader.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Pyrokinetic
{
	struct RendererData
	{
		Ref<VertexArray> quadVertexArray;
		Ref<Shader> shader;
		Ref<Texture2D> whiteTexture;
	};

	static RendererData* s_Data;

	void Renderer2D::Init()
	{
		PROFILE_FUNCTION();

		s_Data = new RendererData();

		s_Data->quadVertexArray = VertexArray::Create();
		float squareVertices[4 * 5] =
		{
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			-0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
			0.5f, 0.5f, 0.0f, 1.0f, 1.0f
		};

		Ref<VertexBuffer> squareVB;
		squareVB = VertexBuffer::Create(squareVertices, sizeof(squareVertices));
		BufferLayout squareVBLayout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoords" }
		};
		squareVB->SetLayout(squareVBLayout);
		s_Data->quadVertexArray->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 1, 2, 3 };
		Ref<IndexBuffer> squareIB;
		squareIB = IndexBuffer::Create(squareIndices, sizeof(squareIndices));
		s_Data->quadVertexArray->SetIndexBuffer(squareIB);

		s_Data->whiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data->whiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		s_Data->shader = Shader::Create("assets/shaders/Texture.glsl");
		s_Data->shader->Bind();
		s_Data->shader->SetInt("u_Texture", 0);
	}

	void Renderer2D::Shutdown()
	{
		PROFILE_FUNCTION();
		delete s_Data;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		PROFILE_FUNCTION();

		s_Data->shader->Bind();
		s_Data->shader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
		//s_Data->shader->SetMat4("u_Transform", glm::mat4(1.0f));
	}

	void Renderer2D::EndScene()
	{
		PROFILE_FUNCTION();

	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, float rotation)
	{
		Renderer2D::DrawQuad({ position.x, position.y, 0.0f }, size, color, rotation);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, float rotation)
	{
		PROFILE_FUNCTION();

		s_Data->shader->Bind();
		s_Data->shader->SetFloat4("u_Color", color);
		s_Data->shader->SetFloat("u_TileFactor", 1.0f);
		s_Data->whiteTexture->Bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
		transform = glm::rotate(transform, rotation, { 0.0f, 0.0f, 1.0f });
		transform = glm::scale(transform, { size.x, size.y, 0.0f });
		s_Data->shader->SetMat4("u_Transform", transform);
		
		s_Data->quadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->quadVertexArray);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float rotation, float tileFactor)
	{
		Renderer2D::DrawQuad({ position.x, position.y, 0.0f }, size, texture, rotation, tileFactor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float rotation, float tileFactor)
	{
		PROFILE_FUNCTION();

		s_Data->shader->Bind();
		s_Data->shader->SetFloat4("u_Color", glm::vec4(1.0f));
		s_Data->shader->SetFloat("u_TileFactor", tileFactor);
		texture->Bind();
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
		transform = glm::rotate(transform, rotation, { 0.0f, 0.0f, 1.0f });
		transform = glm::scale(transform, { size.x, size.y, 0.0f });
		s_Data->shader->SetMat4("u_Transform", transform);

		s_Data->quadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->quadVertexArray);
	}

}