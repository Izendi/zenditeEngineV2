#include "MinimalSceneFactory.h"
#include "../Coordinator.h"
#include "../ECS/Components.h"


void MinimalSceneFactory::ProcessAssimpNode(std::string dir, aiNode* node, const aiScene* scene, EntityNode& entNode, unsigned int NumRenderables)
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

		for (int i = 0; i < NumRenderables; ++i)
		{
			c_tr.modelMat.push_back(glm::mat4(1.0f));
		}

		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		ProcessMesh(dir, mesh, scene, c_rend, c_tx);

		c_md.isModifed = true;

		COORD.AddComponentToEntity<c_Transform>(ent, c_tr);
		COORD.AddComponentToEntity<c_Renderable>(ent, c_rend);
		COORD.AddComponentToEntity<c_Texture>(ent, c_tx);
		COORD.AddComponentToEntity<c_Modified>(ent, c_md);
		COORD.SetUpRenderData(ent);
	}

	std::cout << node->mNumChildren << std::endl;
	for(int i = 0; i < node->mNumChildren; ++i)
	{
		std::shared_ptr<EntityNode> childNode = entNode.CreateNewChild();
		ProcessAssimpNode(dir, node->mChildren[i], scene, *childNode, NumRenderables);
	}

}

//#REFERENCE: large parts of this function were taken from the leanopengl processMesh function example code: https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/model.h
void MinimalSceneFactory::ProcessMesh(std::string dir, aiMesh* mesh, const aiScene* scene, c_Renderable& c_Rd, c_Texture& c_tx)
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
		posData.y = mesh->mVertices[i].y;
		posData.z = mesh->mVertices[i].z;

		if(mesh->HasNormals())
		{
			normData.x = mesh->mNormals[i].x;
			normData.y = mesh->mNormals[i].y;
			normData.z = mesh->mNormals[i].z;
		}
		
		if(mesh->mTextureCoords[0]) //check if mesh has texture coordinates.
		{
			txCoord.x = mesh->mTextureCoords[0][i].x;
			txCoord.y = mesh->mTextureCoords[0][i].y;

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
	//#Improve_THIS_IS_A_BASIC_VERSION_FOR_TESTING: Process textures using https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/model.h process mesh implmemtation as an example.
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	std::string texFilePath = "res/textures/rockySurface.png"; //default tex filepath.
	//Only load diffuse for now:
	if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) 
	{
		aiString path; // Path to the texture

		// Get the path of the first diffuse texture
		if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
		{
			std::string std_path = path.C_Str();
			texFilePath = dir + std_path;
			
		}
	}

	unsigned int texUnit = COORD.GenerateTexUnit(texFilePath, "JPG"); //Assume jpg but will need to make dynamic in future.
	
	c_tx.texUnit = texUnit;
	c_tx.type = "JPG";

}

MinimalSceneFactory::MinimalSceneFactory(Coordinator& coorinator) : I_SceneFactory(coorinator)
{

}

EntityScene MinimalSceneFactory::CreateEntityScene(std::string dir, std::string objFile, glm::mat4 worldModelMatrix, std::shared_ptr<Shader> shader, unsigned int NumRenderables)
{
	EntityNode RootNode;

	Assimp::Importer importer;
	std::string dir_objFile = dir + objFile;
	const aiScene* scene = importer.ReadFile(dir_objFile, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		DEBUG_ASSERT(false, "Unable to load assimp file");
	}

	ProcessAssimpNode(dir, scene->mRootNode, scene, RootNode, NumRenderables);
	RootNode.SetAllTransformCompoennts(worldModelMatrix, COORD);

	EntityScene SceneObj(RootNode, worldModelMatrix);

	SceneObj.SetShaderForAllSceneEntities(COORD, shader);

	return SceneObj;

}

void MinimalSceneFactory::DestroyEntityScene(EntityScene& ES)
{

}
