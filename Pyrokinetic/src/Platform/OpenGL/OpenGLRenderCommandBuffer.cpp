#include "pkpch.h"
#include "OpenGLRenderCommandBuffer.h"

#include "OpenGLPipeline.h"
#include "OpenGLBuffer.h"
#include "OpenGLShader.h"
#include "OpenGLRenderPass.h"


#include <glad/glad.h>

namespace pk
{
	OpenGLRenderCommandBuffer::OpenGLRenderCommandBuffer()
	{
		
	}

	OpenGLRenderCommandBuffer::~OpenGLRenderCommandBuffer()
	{
	
	}

	void OpenGLRenderCommandBuffer::BeginRenderPass(const std::shared_ptr<RenderPass>& renderPass)
	{
		m_RenderPasses.push_back(dynamic_cast<OpenGLRenderPass*>(renderPass.get()));
		m_Queue.push_back([=](OpenGLFramebuffer& framebuffer)
		{
			framebuffer.Bind();
			auto& clearColor = renderPass->GetSpecification().clearColor;
			glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		});
	}

	void OpenGLRenderCommandBuffer::EndRenderPass()
	{
		m_Queue.push_back([=](OpenGLFramebuffer& framebuffer)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			});
	}

	void OpenGLRenderCommandBuffer::Submit(const std::shared_ptr<Pipeline>& pipeline, const std::shared_ptr<VertexBuffer>& vertexBuffer, const std::shared_ptr<IndexBuffer>& indexBuffer, uint32_t indexCount)
	{
		OpenGLPipeline* glPipeline = static_cast<OpenGLPipeline*>(pipeline.get());
		OpenGLShader* glShader = static_cast<OpenGLShader*>(pipeline->GetSpecification().Shader.get());
		OpenGLVertexBuffer* glVertexBuffer = static_cast<OpenGLVertexBuffer*>(vertexBuffer.get());
		OpenGLIndexBuffer* glIndexBuffer = static_cast<OpenGLIndexBuffer*>(indexBuffer.get());

		m_Queue.push_back([=](OpenGLFramebuffer& framebuffer)
			{
				glUseProgram(glShader->GetRendererID());
				glShader->BindNeededResources();
				glBindVertexArray(glPipeline->GetVertexArrayRendererID());
				glVertexBuffer->Bind();
				glIndexBuffer->Bind();

				uint32_t layoutIndex = 0;
				for (const auto& element : glPipeline->GetSpecification().vertexBufferLayout)
				{
					glEnableVertexAttribArray(layoutIndex);
					glVertexAttribPointer(layoutIndex,
						element.GetComponentCount(),
						ShaderDataTypeToOpenGLBaseType(element.Type),
						element.Normalized ? GL_TRUE : GL_FALSE,
						glPipeline->GetSpecification().vertexBufferLayout.GetStride(),
						(const void*)(intptr_t)element.Offset);
					++layoutIndex;
				}

				glDrawElements(GL_TRIANGLES, indexCount == 0 ? glIndexBuffer->GetCount() : indexCount, GL_UNSIGNED_INT, nullptr);
				});
		
	}

	void OpenGLRenderCommandBuffer::Flush()
	{
		for(auto renderPass : m_RenderPasses)
			for(auto framebuffer : renderPass->GetFramebuffers())
				for (auto& func : m_Queue)
					func(*framebuffer);

		m_Queue.clear();
		m_RenderPasses.clear();
	}
}