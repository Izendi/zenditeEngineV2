#include "DayCycleCoordinator.h"

DayCycleCoordinator::DayCycleCoordinator(Coordinator& coordinator, Entity Sun, float sunRadius, float singleCycleDuration) : COORD(coordinator)
{
	m_Sun = Sun;
	m_SunRadius = sunRadius;


	setSingleCycleDuration(singleCycleDuration);

	SetUpCycle();
}

void DayCycleCoordinator::SetUpCycle()
{
	//m_AngularSpeed = glm::radians(360.0f / m_SingleCycleDuration);
	m_SunPosition = &COORD.GetComponentDataFromEntity<c_Transform>(m_Sun);

}

void DayCycleCoordinator::Update(float deltaTime, float currentTime)
{
	if(m_Halt)
	{

	}
	else
	{
		m_AngularSpeed = glm::two_pi<float>() / m_SingleCycleDuration;

		m_Angle = m_Angle + m_AngularSpeed * deltaTime;

		if (m_Angle > glm::two_pi<float>())
		{
			m_Angle -= glm::two_pi<float>();
		}

		float x = 0.0f;
		float y = m_SunRadius * std::sin(m_Angle);
		float z = m_SunRadius * std::cos(m_Angle);;

		glm::mat4 modelMatrix = glm::mat4(1.0f);  // Initialize to identity matrix
		modelMatrix = glm::translate(modelMatrix, glm::vec3(x, y, z));

		m_SunPosition->modelMat[0] = modelMatrix;
	}
}

void DayCycleCoordinator::setSingleCycleDuration(float time)
{
	m_SingleCycleDuration = time;
}

void DayCycleCoordinator::Pause()
{
	m_Halt = true;
}

void DayCycleCoordinator::Resume()
{
	m_Halt = false;
}
