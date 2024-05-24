#pragma once

#include "../ECS/Components.h"
#include "../utils.h"
#include "../ECS/ECSUtils.h"

#include "../Coordinator.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class DayCycleCoordinator
{
private:
	Coordinator& COORD;

	Entity m_Sun;

	float m_SunRadius;

	float m_Angle = 0.0f;

	bool m_Halt = false;

	c_Transform* m_SunPosition;

	glm::vec3 m_SkyColor;
	glm::vec3 m_lightDirection;

	float m_SingleCycleDuration;
	float m_AngularSpeed;

	void SetUpCycle();

public:
	DayCycleCoordinator(Coordinator& coordinator, Entity Sun, float sunRadius, float singleCycleDuration);

	void Update(float deltaTime, float currentTime);

	void setSingleCycleDuration(float time);

	void Pause();

	void Resume();

};