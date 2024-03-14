#pragma once
#include "../utils.h"
#include "../ECS/ECSUtils.h"
#include "../ECS/ECSCoordinator.h"
#include <unordered_map>

class I_API_Manager
{
protected:
	std::unordered_map<Entity, R_DataHandle> m_Map_ENTITYtoHANDLE;
	std::unordered_map<Entity, std::shared_ptr<Shader>> m_Map_ENTITYtoSHADER;
	std::unordered_map<std::string, unsigned int short> m_Map_FILEPATHtoTEXUNIT;

	virtual R_DataHandle& GetNonConstEntityDataHandle(Entity EID) = 0;
	unsigned short int No_texUnits;

public:
	virtual void SetupRenderData(Entity EID, std::shared_ptr<ECSCoordinator> ECScoord) = 0;
	virtual void SetupAABBRenderData(Entity EID, std::shared_ptr<ECSCoordinator> ECScoord) = 0;
	virtual R_DataHandle const& GetEntityDataHandle(Entity EID) const = 0;
	virtual std::shared_ptr<Shader> GetEntityShader(Entity EID) const = 0;
	virtual void SetShaderForDataHandle(Entity EID) = 0;
	virtual void SetShaderForEntity(Entity EID, std::shared_ptr<Shader> shader) = 0;

	virtual unsigned short int GenerateTexUnit(std::string texFilePath, std::string fileType) = 0;


};