#include "MinimalSceneFactory.h"

void MinimalSceneFactory::ProcessAssimpNode(aiNode* node, const aiScene* scene, EntityNode& entNode, unsigned int NumRenderables)
{
	entNode.SetLocalModelMat(ConvertAssimpMMtoGlmMM(node->mTransformation));

	for(int i = 0; i < node->mNumMeshes; ++i)
	{
		Entity ent;
		ent = COORD.CreateEntity();
		entNode.AddEntity(ent);

		c_Transform c_tr;
		c_Renderable c_rend;
		c_Texture c_tx;
		c_Modified c_md;

		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		ProcessMesh(mesh, scene, c_rend, c_tx);

		c_md.isModifed = true;

		COORD.AddComponentToEntity<c_Transform>(ent, c_tr);
		COORD.AddComponentToEntity<c_Renderable>(ent, c_rend);
		COORD.AddComponentToEntity<c_Texture>(ent, c_tx);
		COORD.AddComponentToEntity<c_Modified>(ent, c_md);

	}

	for(int i = 0; i < node->mNumChildren; ++i)
	{
		EntityNode& childNode = entNode.CreateNewChild();
		ProcessAssimpNode(node->mChildren[i], scene, childNode);
	}

}

//#REFERENCE: large parts of this function were taken from the leanopengl processMesh function example code: https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/model.h
void MinimalSceneFactory::ProcessMesh(aiMesh* mesh, const aiScene* scene, c_Renderable& c_Rd, c_Texture& c_tx)
{
	for(unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		//Position Data:
		glm::vec3 posData;
		glm::vec3 normData(0.0f, 0.0f, 0.0f);
		glm::vec2 txCoord(0.0f, 0.0f);
		glm::vec3 tang(0.0f, 0.0f, 0.0f);
		glm::vec3 btan(0.0f, 0.0f, 0.0f);

		posData.x = mesh->mVertices[i].x;
		posData.x = mesh->mVertices[i].y;
		posData.x = mesh->mVertices[i].z;

		if(mesh->HasNormals())
		{
			normData.x = mesh->mNormals[i].x;
			normData.y = mesh->mNormals[i].y;
			normData.z = mesh->mNormals[i].z;
		}
		
		if(mesh->mTextureCoords[0]) //check if mesh has texture coordinates.
		{
			txCoord.x = mesh->mTextureCoords[0][i].x;
			txCoord.x = mesh->mTextureCoords[0][i].y;

			//#probably_will_not_use_these, but saw them in the learnopengl implementation, they might be useful in the future:
			tang.x = mesh->mTangents[i].x;
			tang.y = mesh->mTangents[i].y;
			tang.z = mesh->mTangents[i].z;

			btan.x = mesh->mBitangents[i].x;
			btan.y = mesh->mBitangents[i].y;
			btan.z = mesh->mBitangents[i].z;

		}

		c_Rd.vertices.emplace_back(Vertex(
			posData,
			normData,
			txCoord,
			tang,
			btan
		));

	}

	for(unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for(unsigned int ii = 0; ii < face.mNumIndices; ++ii)
		{
			c_Rd.indices.push_back(face.mIndices[ii]);
		}
	}


	//Process Materials/Textures:
	//#CONTINUE_HERE: Process textures using https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/model.h process mesh implmemtation as an example.

}

EntityScene MinimalSceneFactory::CreateEntityScene(std::string path, glm::mat4 worldModelMatrix, unsigned int NumRenderables)
{

}

void MinimalSceneFactory::DestroyEntityScene(EntityScene& ES)
{

}
