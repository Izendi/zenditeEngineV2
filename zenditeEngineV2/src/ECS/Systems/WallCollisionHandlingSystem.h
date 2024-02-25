#pragma once

#include "../ECSCoordinator.h"
#include "../I_System.h"
#include "../../EventManager/I_Subject.h"
#include "../../API_Rendering/I_Renderer.h"
#include "../../API_Rendering/I_API_Manager.h"
#include "../../ECS/Components.h"

#include "../../Shader.h"


class WallCollisionHandlingSystem : public I_System, public I_Subject
{
private:
	//std::shared_ptr<ECSCoordinator> ECScoord;

	void NotifyObservers(std::shared_ptr<ECSCoordinator> ECS_Coord, int* i) override
	{
		for (size_t j = 0; j < observerList.size(); ++j) //Iterate through every item in observerList
		{
			observerList[j]->UpdateObservers(ECS_Coord, i);
		}

		//delete[] i;
	}

	//Will return true if a collision is occurring, false if not
	bool testCollision(Entity E0, Entity E1, std::shared_ptr<ECSCoordinator> ECScoord)
	{
		glm::vec3 AABBpos_0;
		glm::vec3 AABBpos_1;
		c_Transform& trans0 = ECScoord->GetComponentDataFromEntity<c_Transform>(E0);
		c_Transform& trans1 = ECScoord->GetComponentDataFromEntity<c_Transform>(E1);

		// Always use position 0 in the first []
		AABBpos_0.x = trans0.modelMat[0][3][0];
		AABBpos_0.y = trans0.modelMat[0][3][1];
		AABBpos_0.z = trans0.modelMat[0][3][2];

		AABBpos_1.x = trans1.modelMat[0][3][0];
		AABBpos_1.y = trans1.modelMat[0][3][1];
		AABBpos_1.z = trans1.modelMat[0][3][2];

		//#CONTINUE_HERE
		glm::mat4 E0_Model = glm::mat4(1.0f);
		glm::mat4 E1_Model = glm::mat4(1.0f);
		E0_Model = glm::translate(E0_Model, AABBpos_0);
		E0_Model = glm::scale(E0_Model, ECScoord->GetComponentDataFromEntity<c_AABB>(E0).scale);

		E1_Model = glm::translate(E1_Model, AABBpos_1);
		E1_Model = glm::scale(E1_Model, ECScoord->GetComponentDataFromEntity<c_AABB>(E1).scale);


		float* E0_AABBVertices = ECScoord->GetComponentDataFromEntity<c_AABB>(E0).vertices;
		float* E1_AABBVertices = ECScoord->GetComponentDataFromEntity<c_AABB>(E1).vertices;

		//If the rules were followed correctly, the first 6 array entries should give us the 3 axis we need:
		//We can use these to get out x, y and z mins and maxs.

		//#IMPROVE: This method of array indexing is highly prone to failure if the vertecies for the AABB are modified. Try to think of a better implementaiton
		glm::vec4 E0_x0(E0_AABBVertices[0], E0_AABBVertices[1], E0_AABBVertices[2], 1.0f);
		glm::vec4 E0_x1(E0_AABBVertices[3], E0_AABBVertices[4], E0_AABBVertices[5], 1.0f);

		glm::vec4 E1_x0(E1_AABBVertices[0], E1_AABBVertices[1], E1_AABBVertices[2], 1.0f);
		glm::vec4 E1_x1(E1_AABBVertices[3], E1_AABBVertices[4], E1_AABBVertices[5], 1.0f);

		glm::vec4 E0_y0(E0_AABBVertices[6], E0_AABBVertices[7], E0_AABBVertices[8], 1.0f);
		glm::vec4 E0_y1(E0_AABBVertices[9], E0_AABBVertices[10], E0_AABBVertices[11], 1.0f);

		glm::vec4 E1_y0(E1_AABBVertices[6], E1_AABBVertices[7], E1_AABBVertices[8], 1.0f);
		glm::vec4 E1_y1(E1_AABBVertices[9], E1_AABBVertices[10], E1_AABBVertices[11], 1.0f);

		glm::vec4 E0_z0(E0_AABBVertices[12], E0_AABBVertices[13], E0_AABBVertices[14], 1.0f);
		glm::vec4 E0_z1(E0_AABBVertices[15], E0_AABBVertices[16], E0_AABBVertices[17], 1.0f);

		glm::vec4 E1_z0(E1_AABBVertices[12], E1_AABBVertices[13], E1_AABBVertices[14], 1.0f);
		glm::vec4 E1_z1(E1_AABBVertices[15], E1_AABBVertices[16], E1_AABBVertices[17], 1.0f);

		//Convert it to world space:
		E0_x0 = E0_Model * E0_x0;
		E0_x1 = E0_Model * E0_x1;
		E1_x0 = E1_Model * E1_x0;
		E1_x1 = E1_Model * E1_x1;

		E0_y0 = E0_Model * E0_y0;
		E0_y1 = E0_Model * E0_y1;
		E1_y0 = E1_Model * E1_y0;
		E1_y1 = E1_Model * E1_y1;

		E0_z0 = E0_Model * E0_z0;
		E0_z1 = E0_Model * E0_z1;
		E1_z0 = E1_Model * E1_z0;
		E1_z1 = E1_Model * E1_z1;

		float E0_xMax;
		float E0_xMin;
		float E1_xMax;
		float E1_xMin;

		float E0_yMax;
		float E0_yMin;
		float E1_yMax;
		float E1_yMin;

		float E0_zMax;
		float E0_zMin;
		float E1_zMax;
		float E1_zMin;

		if (E0_x0.x > E0_x1.x)
		{
			E0_xMax = E0_x0.x;
			E0_xMin = E0_x1.x;
		}
		else
		{
			E0_xMax = E0_x1.x;
			E0_xMin = E0_x0.x;
		}

		if (E1_x0.x > E1_x1.x)
		{
			E1_xMax = E1_x0.x;
			E1_xMin = E1_x1.x;
		}
		else
		{
			E1_xMax = E1_x1.x;
			E1_xMin = E1_x0.x;
		}

		if (E0_y0.y > E0_y1.y)
		{
			E0_yMax = E0_y0.y;
			E0_yMin = E0_y1.y;
		}
		else
		{
			E0_yMax = E0_y1.y;
			E0_yMin = E0_y0.y;
		}

		if (E1_y0.y > E1_y1.y)
		{
			E1_yMax = E1_y0.y;
			E1_yMin = E1_y1.y;
		}
		else
		{
			E1_yMax = E1_y1.y;
			E1_yMin = E1_y0.y;
		}

		if (E0_z0.z > E0_z1.z)
		{
			E0_zMax = E0_z0.z;
			E0_zMin = E0_z1.z;
		}
		else
		{
			E0_zMax = E0_z1.z;
			E0_zMin = E0_z0.z;
		}

		if (E1_z0.z > E1_z1.z)
		{
			E1_zMax = E1_z0.z;
			E1_zMin = E1_z1.z;
		}
		else
		{
			E1_zMax = E1_z1.z;
			E1_zMin = E1_z0.z;
		}

		//Return true if the two AABBs are intersecting
		return (E0_xMin <= E1_xMax &&
			E0_xMax >= E1_xMin &&
			E0_yMin <= E1_yMax &&
			E0_yMax >= E1_yMin &&
			E0_zMin <= E1_zMax &&
			E0_zMax >= E1_zMin
			);
	}

public:
	//res/shaders/AABB/vs_BasicAABB.glsl //res/shaders/AABB/fs_BasicAABB.glsl
	WallCollisionHandlingSystem()
	{

	}

	void checkCollisions(std::shared_ptr<ECSCoordinator> ECScoord)
	{
		//Check all entites that are colliding;

		int NoOfEntitesColliding = 0;

		std::set<std::uint32_t>* WallAABBs = ECScoord->GetWallAABBEntitiesSetPtr();
		std::set<std::uint32_t>* WallCollidingAABBs = ECScoord->GetWallCollidableAABBEntitiesSetPtr();

		if (WallCollidingAABBs && WallAABBs && !WallAABBs->empty() && !WallCollidingAABBs->empty())
		{
			//#Parallelism_Potential?
			for (std::set<std::uint32_t>::iterator it = (*WallAABBs).begin(); it != (*WallAABBs).end(); ++it)
			{
				for (std::set<std::uint32_t>::iterator iit = (*WallCollidingAABBs).begin(); iit != (*WallCollidingAABBs).end(); ++iit)
				{
					bool isColliding = testCollision(*it, *iit, ECScoord);

					if (isColliding)
					{
						int collidingEnts[2];
						collidingEnts[0] = *it;
						collidingEnts[1] = *iit;

						(ECScoord->GetComponentDataFromEntity<c_AABB>(*it)).isColliding = true;
						(ECScoord->GetComponentDataFromEntity<c_AABB>(*iit)).isColliding = true;

						ECScoord->GetComponentDataFromEntity<c_AABB>(*iit).isWallColliding = true;

						c_Transform& CollidingObjTransData = (ECScoord->GetComponentDataFromEntity<c_Transform>(*iit));


						CollidingObjTransData.modelMat[0] = (CollidingObjTransData.prevModelMat);

						NotifyObservers(ECScoord, collidingEnts);
					}
					else
					{
						(ECScoord->GetComponentDataFromEntity<c_AABB>(*it)).isColliding = false;
						(ECScoord->GetComponentDataFromEntity<c_AABB>(*iit)).isColliding = false;
					}

				}
			}
		}
	}

};