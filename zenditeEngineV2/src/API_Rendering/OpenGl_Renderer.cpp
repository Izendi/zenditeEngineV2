#include "../utils.h"
#include "OpenGl_Renderer.h"
#include "../ECS/ECSCoordinator.h"
#include "../Shader.h"
#include "../geometrySetup.h"
#include "../TextureData.h"
#include "../ECS/Components.h"

OpenGL_Renderer::OpenGL_Renderer(std::shared_ptr<Camera> cam) : I_Renderer(cam)
{
	
}

void OpenGL_Renderer::Render(const R_DataHandle& DataHandle, ECSCoordinator& ECScoord, Entity EID)
{
	c_Transform& trans = ECScoord.GetComponentDataFromEntity<c_Transform>(EID);

	(DataHandle.shader)->bindProgram();
	bindVao(DataHandle.VAO);
	glm::mat4 cubeProjection = glm::perspective(glm::radians(cam->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 cubeView = cam->GetViewMatrix();
	(DataHandle.shader)->setUniformMat4("projection", GL_FALSE, glm::value_ptr(cubeProjection));
	(DataHandle.shader)->setUniformMat4("view", GL_FALSE, glm::value_ptr(cubeView));

	//(DataHandle.texture)->changeTexUnit(DataHandle.texUnit); //#unnecessary. Each texture is saved to a texture unit and is not changed throught the programs lifespan
															   //			   This might be useful later if assigned texture units can be modified later during runtime
															   //			   Although, all this does is take a texture and assign it to a texture unit.
	glm::vec3 camPosition = cam->getPosition();
	std::shared_ptr<Shader> shader = DataHandle.shader;
	shader->setUniform3fv("viewPos", camPosition);
	shader->setUniform3fv("lightPos", -8.0f, 7.0f, -4.0f);
	shader->setUniform3fv("dirLight.direction", 8.0f, -7.0f, 4.0f);
	shader->setUniform3fv("dirLight.ambient", 0.5f, 0.5f, 0.5f);
	shader->setUniform3fv("dirLight.diffuse", 0.8f, 0.8f, 0.8f);
	shader->setUniform3fv("dirLight.specular", 0.5f, 0.5f, 0.5f);
	shader->setUniformInt("nrPointLights", 0);
	shader->setUniformInt("nrSpotLights", 0);


	(DataHandle.shader)->setUniformTextureUnit("colorTexture", DataHandle.texUnit);

	for (int i = 0; i < trans.pos.size(); ++i)
	{
		glm::mat4 cubeModel = glm::mat4(1.0f);
		cubeModel = glm::translate(cubeModel, trans.pos[i]);
		cubeModel = glm::scale(cubeModel, trans.scale);
		(DataHandle.shader)->setUniformMat4("model", GL_FALSE, glm::value_ptr(cubeModel));

		GLCALL(glDrawArrays(GL_TRIANGLES, 0, 36));
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

	glm::mat4 cubeModel = glm::mat4(1.0f);
	cubeModel = glm::translate(cubeModel, trans.pos[0]); // Always use position 0
	cubeModel = glm::scale(cubeModel, AABB_Data.scale);
	AABBShader.setUniformMat4("model", GL_FALSE, glm::value_ptr(cubeModel));

	if (AABB_Data.isColliding == true)
	{
		if(AABB_Data.isWallColliding == true)
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

	GLCALL(glDrawArrays(GL_LINES, 0, 24));

}

void OpenGL_Renderer::RenderLighting(const R_DataHandle& DataHandle,
	const c_Transform& trans,
	std::shared_ptr<ECSCoordinator> ECScoord)
{
	std::set<Entity>* SpotLightSet = ECScoord->GetSpotLightEntitiesPtr();

	for (std::set<std::uint32_t>::iterator it = (*SpotLightSet).begin(); it != (*SpotLightSet).end(); ++it)
	{
		const c_SpotLightEmitter& spotLightData = ECScoord->GetComponentDataFromEntity<c_SpotLightEmitter>(*it);
	}
	
	
}
