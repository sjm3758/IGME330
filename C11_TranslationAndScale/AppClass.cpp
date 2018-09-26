#include "AppClass.h"
void Application::InitVariables(void)
{
	//init the mesh
	//m_pMesh = new MyMesh();
	//m_pMesh->GenerateCube(1.0f, C_WHITE);
	//m_pMesh->GenerateSphere(1.0f, 5, C_WHITE);
	//m_pMesh->GenerateCube(1.0f, C_BLUE);

	for (unsigned int x = 0; x < m_uMeshCount; x++) {
		MyMesh* m_pMesh = new MyMesh();
		cubes.push_back(m_pMesh);
		cubes[x]->GenerateCube(1.0f, C_BLACK);
	}
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	static float value = 0.0f;
	static float change = 0.02f;
	//changing value makes the mesh move
	//if value reaches a certain point, change to the opposite velocity
	if (value > 2.0f) {
		change = -0.02f;
	}
	if (value < -2.0f) {
		change = 0.02f;
	}
	value += change;

	//matrix4 m4Model = m4Translate * m4Scale;


	cubes[0]->m4Translate = glm::translate(IDENTITY_M4, vector3(value - 3.0f, 4.0f, 3.0f)); cubes[0]->visible = true;
	cubes[1]->m4Translate = glm::translate(IDENTITY_M4, vector3(value + 3.0f, 4.0f, 3.0f)); cubes[1]->visible = true;

	cubes[2]->m4Translate = glm::translate(IDENTITY_M4, vector3(value - 2.0f, 3.0f, 3.0f)); cubes[2]->visible = true;
	cubes[3]->m4Translate = glm::translate(IDENTITY_M4, vector3(value + 2.0f, 3.0f, 3.0f)); cubes[3]->visible = true;

	for (int x = 4; x < 11; x++) {
		cubes[x]->m4Translate = glm::translate(IDENTITY_M4, vector3(value + (-3.0f + (x - 4)), 2.0f, 3.0f));
		cubes[x]->visible = true;
	}

	for (int x = 11; x < 20; x++) {
		if (x == 13 || x == 17) {
		}
		else {
			cubes[x]->m4Translate = glm::translate(IDENTITY_M4, vector3(value + (-4.0f + (x - 11)), 1.0f, 3.0f));
			cubes[x]->visible = true;
		}
	}

	for (int x = 20; x < 31; x++) {
		cubes[x]->m4Translate = glm::translate(IDENTITY_M4, vector3(value + (-5.0f + (x - 20)), 0.0f, 3.0f));
		cubes[x]->visible = true;
	}

	for (int x = 31; x < 42; x++) {
		if (x == 32 || x == 40) {

		}
		else {
			cubes[x]->m4Translate = glm::translate(IDENTITY_M4, vector3(value + (-5.0f + (x - 31)), -1.0f, 3.0f));
			cubes[x]->visible = true;
		}
	}

	for (int x = 42; x < 53; x++) {
		if (x == 43 || x == 45 || x == 46 || x == 47 || x == 48 || x == 49 || x == 51) {

		}
		else {
			cubes[x]->m4Translate = glm::translate(IDENTITY_M4, vector3(value + (-5.0f + (x - 42)), -2.0f, 3.0f));
			cubes[x]->visible = true;
		}
	}

	for (int x = 53; x < 58; x++) {
		if (x == 55) {

		}
		else {
			cubes[x]->m4Translate = glm::translate(IDENTITY_M4, vector3(value + (-2.0f + (x - 53)), -3.0f, 3.0f));
			cubes[x]->visible = true;
		}
	}

	for (unsigned int x = 0; x < m_uMeshCount; x++) {
		if (cubes[x]->visible == true) {
			cubes[x]->m4Scale = glm::scale(IDENTITY_M4, vector3(2.0f, 2.0f, 2.0f));
			cubes[x]->m4Model = cubes[x]->m4Scale * cubes[x]->m4Translate;
			ModelRender(x, cubes[x]->m4Translate, cubes[x]->m4Scale);
		}
	}

	//m_pMesh->Render(m4Projection, m4View, m4Model);
	
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}

void Application::ModelRender(int currentMesh, matrix4 m4Translate, matrix4 m4Scale) {
	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	matrix4 m4Model = m4Scale * m4Translate;
	cubes[currentMesh]->Render(m4Projection, m4View, m4Model);
}

void Application::Release(void)
{
	for (unsigned int x = 0; x < cubes.size(); x++) {
		SafeDelete(cubes[x]);
	}

	//release GUI
	ShutdownGUI();
}