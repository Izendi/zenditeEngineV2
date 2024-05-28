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

		float t = 0.0f;

		float degreesAngle = m_Angle * (180.0f / glm::pi<float>());

		if (degreesAngle < 120.0f)
		{
			t = degreesAngle / 120.0f;
			m_SkyColor = glm::mix(m_dawn, m_midday, t);
		}
		else if (degreesAngle < 170.0f)
		{
			t = (degreesAngle - 120.0f) / 50.0f;
			m_SkyColor = glm::mix(m_midday, m_evening, t);
		}
		else if (degreesAngle < 190.0f)
		{
			t = (degreesAngle - 170.0f) / 20.0f;
			m_SkyColor = glm::mix(m_evening, m_sunset, t);
		}
		else if(degreesAngle < 200.0f)
		{
			t = (degreesAngle - 190.0f) / 10.0f;
			m_SkyColor = glm::mix(m_sunset, m_night, t);
		}
		else if(degreesAngle < 260.0f)
		{
			t = (degreesAngle - 200.0f) / 60.0f;
			m_SkyColor = glm::mix(m_night, m_night, t);
		}
		else
		{
			t = (degreesAngle - 260.0f) / 100.0f;
			m_SkyColor = glm::mix(m_night, m_dawn, t);
		}

		m_SunPosition->modelMat[0] = modelMatrix;
	}
}

void DayCycleCoordinator::setSingleCycleDuration(float time)
{
	m_SingleCycleDuration = time;
}

glm::vec3 DayCycleCoordinator::getSkyColor() const
{
	return m_SkyColor;
}

glm::vec3 DayCycleCoordinator::getSunPosition() const
{
	glm::vec3 sunPosition = m_SunPosition->modelMat[0][3];

	return sunPosition;
}

void DayCycleCoordinator::Pause()
{
	m_Halt = true;
}

void DayCycleCoordinator::Resume()
{
	m_Halt = false;
}
