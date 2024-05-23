#include "../utils.h"
#include "OpenGl_Renderer.h"
#include "../ECS/ECSCoordinator.h"
#include "../geometrySetup.h"
#include "../TextureData.h"
#include "../ECS/Components.h"

OpenGL_Renderer::OpenGL_Renderer(std::shared_ptr<Camera> cam) : I_Renderer(cam)
{
	m_sh_SingleColor = std::make_shared<Shader>("res/shaders/simple/vs_shaderSingleColor.glsl",
		"res/shaders/simple/fs_shaderSingleColor.glsl");
}

void OpenGL_Renderer::Render(const R_DataHandle& DataHandle, ECSCoordinator& ECScoord, Entity EID, float deltaTime, float time)
{
	c_Transform& trans = ECScoord.GetComponentDataFromEntity<c_Transform>(EID);
	c_Renderable& rendData = ECScoord.GetComponentDataFromEntity<c_Renderable>(EID);
	c_Texture& texComponentData = ECScoord.GetComponentDataFromEntity<c_Texture>(EID);

	std::shared_ptr<Shader> shader = DataHandle.shader;
	shader->bindProgram();
	bindVao(DataHandle.VAO);
	glm::mat4 cubeProjection = glm::perspective(glm::radians(cam->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 cubeView = cam->GetViewMatrix();
	shader->setUniformMat4("projection", GL_FALSE, glm::value_ptr(cubeProjection));
	shader->setUniformMat4("view", GL_FALSE, glm::value_ptr(cubeView));

	if(texComponentData.is3Dtex)
	{
		shader->setUniformFloat("iTime", deltaTime);
	}

	if(rendData.emReflection == true)
	{
		//#Continue_from_here!
		shader->setUniformInt("skybox", DataHandle.texUnit);
		shader->setUniform3fv("cameraPos", cam->Position);
	}
	else
	{

		shader->setUniformTextureUnit("colorTexture", DataHandle.texUnit);

		//Temporary uniform setter for height map blended texture tests:
		shader->setUniformTextureUnit("highTexture", 0);
		shader->setUniformTextureUnit("lowTexture", 1);

		if (rendData.outline == false)
		{
			//glStencilFunc(GL_ALWAYS, 1, 0xFF); // all fragments should pass the stencil test	
			//glStencilMask(0xFF); // enable writing to the stencil buffer
			glStencilMask(0x00);
		}
		else
		{
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);
		}

		if (rendData.blending == true)
		{

		}

	}

		for (int i = 0; i < trans.modelMat.size(); ++i)
		{
			shader->setUniformMat4("model", GL_FALSE, glm::value_ptr((trans.modelMat)[i]));

			shader->setUniformFloat("baseNoGrassValue", 0.04);
			//layerHeight
			shader->setUniformFloat("layerHeight", (float)i);
			shader->setUniformFloat("time", time);

			GLCALL(glDrawElements(GL_TRIANGLES, (rendData.indices).size(), GL_UNSIGNED_INT, 0));
		}

		if (rendData.outline == true)
		{
			m_sh_SingleColor->bindProgram();
			//glDisable(GL_STENCIL_TEST);

			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilMask(0x00); // disable writing to the stencil buffer
			glDisable(GL_DEPTH_TEST);

			float scaleFactor = 1.1;
			glm::mat4 outlineMM;
			outlineMM = glm::scale(trans.modelMat[0], glm::vec3(scaleFactor, scaleFactor, scaleFactor));

			m_sh_SingleColor->setUniformMat4("projection", GL_FALSE, glm::value_ptr(cubeProjection));
			m_sh_SingleColor->setUniformMat4("view", GL_FALSE, glm::value_ptr(cubeView));
			(m_sh_SingleColor)->setUniformMat4("model", GL_FALSE, glm::value_ptr(outlineMM));

			GLCALL(glDrawElements(GL_TRIANGLES, (rendData.indices).size(), GL_UNSIGNED_INT, 0));

			//glDisable(GL_STENCIL_TEST);
			glStencilMask(0xFF);
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glEnable(GL_DEPTH_TEST);
		}
	
}

void OpenGL_Renderer::RenderAABB(const R_DataHandle& DataHandle, 
	Shader& AABBShader, 
	const c_AABB& AABB_Data, 
	const c_Transform& trans)
{
	AABBShader.bindProgram();
	bindVao(DataHandle.AABB_VAO);

	glm::mat4 cubeProjection = glm::perspective(glm::radians(cam->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 cubeView = cam->GetViewMatrix();
	AABBShader.setUniformMat4("projection", GL_FALSE, glm::value_ptr(cubeProjection));
	AABBShader.setUniformMat4("view", GL_FALSE, glm::value_ptr(cubeView));

	glm::vec3 AABBpos;

	// Always use position 0 in the first []
	AABBpos.x = trans.modelMat[0][3][0];
	AABBpos.y = trans.modelMat[0][3][1];
	AABBpos.z = trans.modelMat[0][3][2]; //#NOTE: We extract the x, y and z data to avoid also attempting to copy the rotation.

	glm::mat4 cubeModel = glm::mat4(1.0f);
	cubeModel = glm::translate(cubeModel, AABBpos);
	cubeModel = glm::scale(cubeModel, AABB_Data.scale);
	AABBShader.setUniformMat4("model", GL_FALSE, glm::value_ptr(cubeModel));

	if (AABB_Data.isColliding == true)
	{
		if (AABB_Data.isWallColliding == true)
		{
			AABBShader.setUniform4f("lineColor", 0.0f, 1.0f, 0.0f, 1.0f);
		}
		else
		{
			AABBShader.setUniform4f("lineColor", 1.0f, 0.0f, 0.0f, 1.0f);
		}
	}
	else
	{
		AABBShader.setUniform4f("lineColor", 1.0f, 0.08f, 0.58f, 1.0f);
	}

	if (AABB_Data.active == true)
	{
		GLCALL(glDrawArrays(GL_LINES, 0, 24));
	}
}

