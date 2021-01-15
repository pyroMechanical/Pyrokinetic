#include <Pyrokinetic.h>

#include "Pyrokinetic/Core/EntryPoint.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include <imgui.h>
#include <implot.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <implot_items.cpp>

#include "Viewport2D.h"

//#define PROFILE_SCOPE(name) Pyrokinetic::Benchmark benchmark##__LINE__(name,  [&](BenchmarkResult benchmarkResult) { m_BenchmarkResults.push_back(benchmarkResult); })

/*class ExampleLayer : public Pyrokinetic::Layer 
{
public:
	ExampleLayer()
		: Layer("Example"), m_SquarePosition(0.0f), m_CameraController(1280.0f/720.0f)
	{
		m_VertexArray.reset(Pyrokinetic::VertexArray::Create());

		float vertices[3 * 7] =
		{
			-0.5f, -0.289f, 0.0f, 1.0f, 0.2f, 0.2f, 1.0f,
			0.5f, -0.289f, 0.0f, 0.2f, 1.0f, 0.2f, 1.0f,
			0.0f, 0.577f, 0.0f, 0.2f, 0.2f, 1.0f, 1.0f,
		};

		Pyrokinetic::Ref<Pyrokinetic::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Pyrokinetic::VertexBuffer::Create(vertices, sizeof(vertices)));


		Pyrokinetic::BufferLayout layout = {
			{ Pyrokinetic::ShaderDataType::Float3, "a_Position" },
			{Pyrokinetic::ShaderDataType::Float4, "a_Color"}
		};
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] =
		{
			0, 1, 2
		};

		Pyrokinetic::Ref<Pyrokinetic::IndexBuffer> indexBuffer;
		indexBuffer.reset(Pyrokinetic::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVA.reset(Pyrokinetic::VertexArray::Create());


		float squareVertices[4 * 5] =
		{
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			-0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
			0.5f, 0.5f, 0.0f, 1.0f, 1.0f
		};

		Pyrokinetic::Ref<Pyrokinetic::VertexBuffer> squareVB;
		squareVB.reset(Pyrokinetic::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		Pyrokinetic::BufferLayout squareVBLayout = {
			{ Pyrokinetic::ShaderDataType::Float3, "a_Position" },
			{ Pyrokinetic::ShaderDataType::Float2, "a_TexCoord"}
		};
		squareVB->SetLayout(squareVBLayout);
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] =
		{
			0, 1, 2, 1, 2, 3
		};
		Pyrokinetic::Ref<Pyrokinetic::IndexBuffer> squareIB;
		squareIB.reset(Pyrokinetic::IndexBuffer::Create(squareIndices, sizeof(squareIndices)));
		m_SquareVA->SetIndexBuffer(squareIB);

		std::string VertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}

		)";
		std::string FragmentSrc = R"(
			#version 450 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				color = v_Color;
			}
		)";


		m_Shader = Pyrokinetic::Shader::Create("VertexColor", VertexSrc, FragmentSrc);

		std::string flatColorVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}

		)";
		std::string flatColorShaderFragmentSrc = R"(
			#version 450 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			uniform vec3 u_Color;

			void main()
			{
				color = vec4(u_Color, 1.0);
			}
		)";


		m_FlatColorShader = Pyrokinetic::Shader::Create("FlatColor", flatColorVertexSrc, flatColorShaderFragmentSrc);

		auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

		m_Texture = Pyrokinetic::Texture2D::Create("assets/textures/ferengi.png");

		std::dynamic_pointer_cast<Pyrokinetic::OpenGLShader>(textureShader)->Bind();
		std::dynamic_pointer_cast<Pyrokinetic::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);
	}


	void OnUpdate(Pyrokinetic::Timestep timestep) override
	{
		
		{
			PROFILE_SCOPE("OrthographicCameraController::OnUpdate()");
			m_CameraController.OnUpdate(timestep);
		}

		{
			PROFILE_SCOPE("Render Preparation");
			Pyrokinetic::RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1 });
			Pyrokinetic::RenderCommand::Clear();
		}
		{
			PROFILE_SCOPE("Render Scene");
			Pyrokinetic::Renderer::BeginScene(m_CameraController.GetCamera());

			glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_SquarePosition);

			glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));


			std::dynamic_pointer_cast<Pyrokinetic::OpenGLShader>(m_FlatColorShader)->Bind();
			std::dynamic_pointer_cast<Pyrokinetic::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);

			for (int y = 0; y < 10; y++)
			{
				for (int x = 0; x < 10; x++)
				{
					glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
					glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_SquarePosition);
					transform = glm::translate(transform, pos) * scale;

					Pyrokinetic::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
				}
			}
			glm::mat4 transform2 = glm::scale(glm::mat4(1.0f), glm::vec3(1.2f));

			auto textureShader = m_ShaderLibrary.Get("Texture");

			m_Texture->Bind();
			Pyrokinetic::Renderer::Submit(textureShader, m_SquareVA, transform2);
			//Pyrokinetic::Renderer::Submit(m_Shader, m_VertexArray);

			Pyrokinetic::Renderer::EndScene();
		}
		//Renderer::Flush();
	}

	virtual void OnImGuiRender() override
	{
		PROFILE_SCOPE("EditorApp::OnImGuiRender()");
		ImGui::Begin("Visual Profiler");
		if (ImPlot::BeginPlot("##Pie1", NULL, NULL, ImVec2(250, 250), ImPlotFlags_Equal | ImPlotFlags_NoMousePos, ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations))
		{
			int labelSize = m_BenchmarkResults.size();
			const char** labels = new const char* [labelSize];
			int* data = new int [labelSize];
			for (std::vector<BenchmarkResult>::iterator it = m_BenchmarkResults.begin(); it != m_BenchmarkResults.end(); ++it)
			{
				data[it - m_BenchmarkResults.begin()] = it->Time;
				labels[it - m_BenchmarkResults.begin()] = it->Name;
			}
			ImPlot::PlotPieChart(labels, data, m_BenchmarkResults.size(), 0.5, 0.5, 0.4, true, "%.2f");
			ImPlot::EndPlot();

			delete[] labels;
			delete[] data;
		}

		m_BenchmarkResults.clear();
		ImGui::End();
	}

	void OnEvent(Pyrokinetic::Event& event) override
	{
		m_CameraController.OnEvent(event);
	}

private:
	Pyrokinetic::ShaderLibrary m_ShaderLibrary;
	Pyrokinetic::Ref<Pyrokinetic::Shader> m_Shader;
	Pyrokinetic::Ref<Pyrokinetic::VertexArray> m_VertexArray;
	Pyrokinetic::Ref<Pyrokinetic::Texture2D> m_Texture;
	Pyrokinetic::Ref<Pyrokinetic::Shader> m_FlatColorShader;
	Pyrokinetic::Ref<Pyrokinetic::VertexArray> m_SquareVA;
	Pyrokinetic::OrthographicCameraController m_CameraController;
	glm::vec3 m_SquarePosition;
	glm::vec3 m_SquareColor = { 0.8f, 0.2f, 0.2f };


	struct BenchmarkResult
	{
		const char* Name;
		int Time;
	};

	std::vector<BenchmarkResult> m_BenchmarkResults;
};*/

	class Editor : public Pyrokinetic::Application
	{
	public:
		Editor()
		{
			//PushLayer(new ExampleLayer());
			PushLayer(new Viewport2D());
		}

		~Editor()
		{
		}

	};

	Pyrokinetic::Application* Pyrokinetic::CreateApplication()
	{
		return new Editor();
	}