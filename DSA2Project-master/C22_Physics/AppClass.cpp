#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{

	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUpward(
		vector3(0.0f, 5.0f, 25.0f), //Position
		vector3(0.0f, 0.0f, 0.0f),	//Target
		AXIS_Y);					//Up

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)
	m_pMeshMngr->GetInstance();
	//m_pMeshMngr->GenerateIcoSphere(1.0f, 12.0f, C_RED);
	//m_pMeshMngr->AddIcoSphereToRenderList(IDENTITY_M4, C_BLUE, RENDER_SOLID);

	m_pEntityMngr->AddEntity("Cannon\\ball.obj", "Ball");
	m_pEntityMngr->UsePhysicsSolver(false);
	vector3 v_ballPosition = m_pCameraMngr->GetPosition();
	matrix4 m_ballPosition = glm::translate(v_ballPosition);
	m_pEntityMngr->SetModelMatrix(m_ballPosition, "Ball");

	//Draw Cube Tower 
///Need to Apply Physics Collisions between cubes and cubes and cannon ball
	
	std::vector<vector3> points;
	float rad = 3;
	float height = 0;
	float stepHeight= 2.2f;
	int numBlocks = 500;

	float angle = 0;
	float steps = (2 * PI) / static_cast<float>(10);
	int counter = 1;
	
	for (int i = 0; i < numBlocks; i++)
	{

		m_pEntityMngr->AddEntity("Minecraft\\Cube.obj", "Cube_" + std::to_string(i));
		points.push_back(vector3(cos(angle)*rad, 0, sin(angle)*rad));
		angle += steps;
		vector3 v3Position = vector3(points[i]);
		
		if ((i+1) % 10 == 0)
		{
			counter++;
		}
		v3Position.y += counter * stepHeight;


		//v3Position.y += stepHeight;
		/*
		
		if (i >= 10 && i < 20)
		{
			v3Position.y = 2.2;
		}
		if (i >= 20 && i < 30)
		{
			v3Position.y = 4.4;
		}
		if (i >= 30 && i < 40)
		{
			v3Position.y = 6.6;
		}
		if (i >= 40 && i < 50)
		{
			v3Position.y = 8.8;
		}
		if (i >= 50 && i < 60)
		{
			v3Position.y = 11;
		}
		if (i >= 60 && i < 70)
		{
			v3Position.y = 13.2;
		}
		if (i >= 70 && i < 80)
		{
			v3Position.y = 15.4;
		}
		if (i >= 80 && i < 90)
		{
			v3Position.y = 17.6;
		}
		if (i >= 90 && i < 400)
		{
			v3Position.y = 19.8;
		}
		*/
		matrix4 m4Position = glm::translate(v3Position);
		m_pEntityMngr->SetModelMatrix(m4Position * glm::scale(vector3(2.0f)));
		m_pEntityMngr->SetMass(10);
		m_pEntityMngr->UsePhysicsSolver(false, i);

	}
	m_uOctantLevels = 1;
	m_pRoot = new MyOctant(m_uOctantLevels, 5);
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the ArcBall active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
	timer++;
	if (timer > 10)
	{
		timer = 0;
		m_pRoot = new MyOctant(m_uOctantLevels, 5);

	}
	//Update Entity Manager
	m_pEntityMngr->Update();
	

	//Set the model matrix for the main object
	//m_pEntityMngr->SetModelMatrix(m_m4Steve, "Steve");

	//Add objects to render list
	//m_pMeshMngr->AddIcoSphereToRenderList(IDENTITY_M4, C_BLUE, RENDER_SOLID);

	m_pEntityMngr->AddEntityToRenderList(-1, true);
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	//display octree
	if (m_uOctantID == -1)
		m_pRoot->Display();
	else
		m_pRoot->Display(m_uOctantID);

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();

	//draw gui,
	DrawGUI();

	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//Release MyEntityManager
	MyEntityManager::ReleaseInstance();

	//release GUI
	ShutdownGUI();
}