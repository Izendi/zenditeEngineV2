#pragma once
#include <cstddef>
#include "I_API_Manager.h"
#include "../ECS/Components.h"
#include "../TextureData.h"

class OpenGL_Manager : public I_API_Manager
{
private:
	std::vector<std::shared_ptr<TextureData>> textures;

public:
	OpenGL_Manager()
	{
		No_texUnits = 0;
	}

	void SetupLightingRenderData(Entity EID, std::shared_ptr<ECSCoordinator> ECScoord) override{
		R_DataHandle DH;

		m_Map_ENTITYtoHANDLE[EID] = DH; //Insert modified DH into the map.
	}

	void SetupRenderData(Entity EID, std::shared_ptr<ECSCoordinator> ECScoord) override
	{
		R_DataHandle DH;

		//std::vector<Vertex>& vertexData = 

		c_Renderable& RData = ECScoord->GetComponentDataFromEntity<c_Renderable>(EID);

		//#NOTE Use the DH.bitset value to determine what data to setup for the vertex data passed in
		// (For this version do a simple set up for testing purposes.)

		GLCALL(glGenVertexArrays(1, &(DH.VAO)));
		GLCALL(glBindVertexArray(DH.VAO));

		GLCALL(glGenBuffers(1, &(DH.VBO)));
		GLCALL(glBindBuffer(GL_ARRAY_BUFFER, DH.VBO));

		//Buffer VBO data (Which includes position, normal and texCoord data):
		GLCALL(glBufferData(GL_ARRAY_BUFFER, RData.vertices.size() * sizeof(Vertex), &(RData.vertices[0]), GL_STATIC_DRAW));

		//Buffer EBO data:
		GLCALL(GL_ELEMENT_ARRAY_BUFFER, DH.EBO);
		GLCALL(GL_ELEMENT_ARRAY_BUFFER, RData.indices.size() * sizeof(unsigned int), &(RData.indices[0], GL_STATIC_DRAW));

		//setup position vertex attribute array
		GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0));
		GLCALL(glEnableVertexAttribArray(0));

		//setup normal vertex attribute array:
		GLCALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal)));
		GLCALL(glEnableVertexAttribArray(1));

		//Set up tex vertex attribute array (if texture is a component of the entity)

		short int bitSetPos = ECScoord->GetComponentBitsetPos<c_Texture>();
		std::bitset<32> textureBitset; // Create a bitset of size 32
		textureBitset.set(bitSetPos);

		std::bitset<32> entitySig = ECScoord->GetEntitySignature(EID);

		if ((entitySig & textureBitset) == textureBitset) // If this entity has a texture component
		{
			GLCALL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords)));
			GLCALL(glEnableVertexAttribArray(2));

			c_Texture texData = ECScoord->GetComponentDataFromEntity<c_Texture>(EID);

			DH.texUnit = texData.texUnit; //#TexUNIT_Set
			//DH.texture = textures[texData.texUnit];
		}
		else
		{
			//#Give_a_default tex unit value perhaps. then remove the system requirement for renderables to have a texture.
		}

		m_Map_ENTITYtoHANDLE[EID] = DH; //Insert modified DH into the map.
	
		GLCALL(glBindVertexArray(0)); //Unbind the VAO
	}

	void SetupAABBRenderData(Entity EID, std::shared_ptr<ECSCoordinator> ECScoord)
	{
		R_DataHandle DH;

		GLCALL(glGenVertexArrays(1, &(DH.AABB_VAO)));
		GLCALL(glBindVertexArray(DH.AABB_VAO));

		c_AABB vertexDataAABB = ECScoord->GetComponentDataFromEntity<c_AABB>(EID);
		GLCALL(glGenBuffers(1, &(DH.AABB_posVBO)));
		GLCALL(glBindBuffer(GL_ARRAY_BUFFER, DH.AABB_posVBO));
		GLCALL(glBufferData(GL_ARRAY_BUFFER, 36*3*sizeof(float), vertexDataAABB.vertices, GL_STATIC_DRAW));
		GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));
		GLCALL(glEnableVertexAttribArray(0));

		if(m_Map_ENTITYtoHANDLE.find(EID) == m_Map_ENTITYtoHANDLE.end()) //If EID is not in the map
		{
			m_Map_ENTITYtoHANDLE[EID] = DH;
		}
		else
		{
			m_Map_ENTITYtoHANDLE[EID].AABB_VAO = DH.AABB_VAO;
			m_Map_ENTITYtoHANDLE[EID].AABB_posVBO = DH.AABB_posVBO;
		}
	}

	R_DataHandle const& GetEntityDataHandle(Entity EID) const override
	{
		auto it = m_Map_ENTITYtoHANDLE.find(EID);
		if (it != m_Map_ENTITYtoHANDLE.end()) {
			return it->second;
		}
		else {
			// Handle the case where the EID is not found. For example, throw an exception
			// or return a reference to a static default-constructed R_DataHandle object.
			static R_DataHandle defaultHandle;
			return defaultHandle;
		}
	}

	R_DataHandle& GetNonConstEntityDataHandle(Entity EID) override
	{
		auto it = m_Map_ENTITYtoHANDLE.find(EID);
		if (it != m_Map_ENTITYtoHANDLE.end()) {
			return it->second;
		}
		else {
			// Handle the case where the EID is not found. For example, throw an exception
			// or return a reference to a static default-constructed R_DataHandle object.
			DEBUG_ASSERT(false, "EID data handle Not found"); // Handle map is not retrieving the data handle (find out why)
			static R_DataHandle defaultHandle;
			return defaultHandle;
		}
	}

	std::shared_ptr<Shader> GetEntityShader(Entity EID) const
	{
		DEBUG_ASSERT(m_Map_ENTITYtoSHADER.find(EID) != m_Map_ENTITYtoSHADER.end(), "EID does not exist");

		auto it = m_Map_ENTITYtoSHADER.find(EID);

		return it->second;
	}

	void SetShaderForEntity(Entity EID, std::shared_ptr<Shader> shader) override
	{
		DEBUG_ASSERT(m_Map_ENTITYtoHANDLE.find(EID) != m_Map_ENTITYtoHANDLE.end(), "Entity does not exist");
		
		m_Map_ENTITYtoSHADER[EID] = shader;
	}

	void SetShaderForDataHandle(Entity EID) override
	{
		R_DataHandle& DH = GetNonConstEntityDataHandle(EID);

		DH.shader = m_Map_ENTITYtoSHADER[EID];
	}

	unsigned short int GenerateTexUnit(std::string texFilePath, std::string fileType) override
	{
		DEBUG_ASSERT(No_texUnits <= 16, "Cap on No. Texture Units has been exceeded"); //#IMPROVE Create a system to swap texture units that are not in use in and out
		
		//#SetUp Tex Unit here
		if(m_Map_FILEPATHtoTEXUNIT.find(texFilePath) == m_Map_FILEPATHtoTEXUNIT.end())
		{
			textures.push_back(std::make_shared<TextureData>(texFilePath));

			if(fileType == "JPG")
			{
				(textures[No_texUnits])->setupTextureJPG(No_texUnits, texFilePath);
			}
			else // PNG will be default 
			{
				(textures[No_texUnits])->setupTexturePNG(No_texUnits, texFilePath);
			}

			m_Map_FILEPATHtoTEXUNIT[texFilePath] = No_texUnits;

			++No_texUnits;
		}


		return m_Map_FILEPATHtoTEXUNIT[texFilePath];

	}

};