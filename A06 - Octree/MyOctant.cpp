#include "MyOctant.h"
using namespace Simplex;
uint Simplex::MyOctant::m_uOctantCount = 1;
uint Simplex::MyOctant::m_uIdealEntityCount = 5;
uint Simplex::MyOctant::m_uMaxLevel = 5;
Simplex::MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	m_fSize = 70.0f;
	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;
	Init(1,m_uID);
}

Simplex::MyOctant::MyOctant(vector3 a_v3Center, float a_fSize, uint a_uCurrentLevel, MyOctant* parent, uint id)
{
	m_pParent = parent;
	m_v3Center = a_v3Center;
	m_fSize = a_fSize;
	m_uMaxLevel = m_uMaxLevel;
	m_uIdealEntityCount = m_uIdealEntityCount;
	Init(a_uCurrentLevel, id + 1);
}

Simplex::MyOctant::MyOctant(MyOctant const & other)
{
	m_uID = other.m_uID;
	m_uLevel = other.m_uLevel;
	m_uChildren = other.m_uChildren;
}

Simplex::MyOctant & MyOctant::operator=(MyOctant const & other)
{
	if (this != &other)
	{
		Release();
		Init(m_uLevel,m_uID);
		MyOctant temp(other);
		Swap(temp);
	}
	return *this;
}

Simplex::MyOctant::~MyOctant(void)
{
	Release();
}

void Simplex::MyOctant::Swap(MyOctant & other)
{
	std::swap(m_uID, other.m_uID);
	std::swap(m_uLevel, other.m_uLevel);
	std::swap(m_uChildren, other.m_uChildren);

	std::swap(m_fSize, other.m_fSize);

	std::swap(m_pMeshMngr, other.m_pMeshMngr);
	std::swap(m_pEntityMngr, other.m_pEntityMngr);

	std::swap(m_v3Center, other.m_v3Center);
	std::swap(m_v3Max, other.m_v3Max);
	std::swap(m_v3Min, other.m_v3Min);

	std::swap(m_pParent, other.m_pParent);
	std::swap(m_pChild, other.m_pChild);

	std::swap(m_EntityList, other.m_EntityList);

	std::swap(m_pRoot, other.m_pRoot);
	std::swap(m_lChild, other.m_lChild);
}

float Simplex::MyOctant::GetSize(void)
{
	return m_fSize;
}

vector3 Simplex::MyOctant::GetCenterGlobal(void)
{
	return m_v3Center;
}

vector3 Simplex::MyOctant::GetMinGlobal(void)
{
	return m_v3Min;
}

vector3 Simplex::MyOctant::GetMaxGlobal(void)
{
	return m_v3Max;
}

bool Simplex::MyOctant::IsColliding(uint a_uRBIndex)
{
	MyEntity* entity = m_pEntityMngr->GetEntity(a_uRBIndex);
	MyRigidBody* pRB = entity->GetRigidBody();
	if (pRB->IsColliding(m_pRigidBody))
	{
		return true;
	}
	return false;
}

void Simplex::MyOctant::Display(uint a_nIndex, vector3 a_v3Color)
{
	matrix4 m4Model = glm::translate(m_pRoot->m_lChild[a_nIndex]->m_v3Center);
	m_pMeshMngr->AddWireCubeToRenderList(m4Model * glm::scale(vector3(m_pRoot->m_lChild[a_nIndex]->m_fSize)), a_v3Color, RENDER_WIRE);
}

void Simplex::MyOctant::Display(vector3 a_v3Color)
{
	//display the root octant
	matrix4 m4Model = glm::translate(m_v3Center);
	m_pMeshMngr->AddWireCubeToRenderList(m4Model * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
	//display each child octant of the root node
	for (uint x = 0; x < m_pRoot->m_lChild.size(); x++) {
		m4Model = glm::translate(m_pRoot->m_lChild[x]->GetCenterGlobal());
		m_pMeshMngr->AddWireCubeToRenderList(m4Model * glm::scale(vector3(m_pRoot->m_lChild[x]->GetSize())), a_v3Color, RENDER_WIRE);
	}
}

void Simplex::MyOctant::DisplayLeafs(vector3 a_v3Color)
{
	for (uint x = 0; x < m_lChild.size(); x++) {
		if (m_lChild[x] != nullptr) {
			m_lChild[x]->Display(a_v3Color);
		}
	}
}

void Simplex::MyOctant::ClearEntityList(void)
{
	m_EntityList.clear();
}

void Simplex::MyOctant::Subdivide(void)
{
	//only subdivide if there are more than the ideal entity count in an octant (save the frames)
	//if (ContainsMoreThan(m_uIdealEntityCount)) {
		vector3 newCenter;
		for (uint i = 0; i < 8; i++)
		{
			vector3 halfMax = (m_v3Max - m_v3Center) / 4.0f;
			vector3 halfMin = -halfMax;
			//find the new center based on the index
			//front upper-right
			if (i == 0) {
				newCenter = m_v3Center + halfMax;
			}
			//front upper-left
			else if (i == 1) {
				halfMax.x = -halfMax.x;
				newCenter = m_v3Center + halfMax;
			}
			//front lower-right
			else if (i == 2) {
				halfMax.y = -halfMax.y;
				newCenter = m_v3Center + halfMax;
			}
			//front lower-left
			else if (i == 3) {
				halfMax.x = -halfMax.x;
				halfMax.y = -halfMax.y;
				newCenter = m_v3Center + halfMax;
			}
			//back upper-right
			else if (i == 4) {
				halfMin.x = -halfMin.x;
				halfMin.y = -halfMin.y;
				newCenter = m_v3Center + halfMin;
			}
			//back upper-left
			else if (i == 5) {
				newCenter = m_v3Center + halfMin;
			}
			//back lower-right
			else if (i == 6) {
				newCenter = m_v3Center + halfMin;
			}
			//back lower-left
			else {
				newCenter = m_v3Center + halfMin;
			}

			//create an octant with a new center, half the size of the current octant, and one octant level higher to limit subdivision
			m_pChild[i] = new MyOctant(newCenter, m_fSize / 2.0f, m_uLevel + 1, this, m_uID + i);
		}
	//}
}

MyOctant * Simplex::MyOctant::GetChild(uint a_nChild)
{
	if (a_nChild <= 7 && a_nChild >= 0) {
		return m_lChild[a_nChild];
	}
	return nullptr;
}

MyOctant * Simplex::MyOctant::GetParent(void)
{
	return m_pParent;
}

bool Simplex::MyOctant::IsLeaf(void)
{
	bool leaf = true;
	for (int x = 0; x < 8; x++) {
		if (m_pChild[x] != nullptr) {
			leaf = false;
		}
	}
	return leaf;
}

bool Simplex::MyOctant::ContainsMoreThan(uint a_nEntities)
{
	if (m_EntityList.size() > a_nEntities) {
		return true;
	}
	return false;
}

void Simplex::MyOctant::KillBranches(void)
{
	//delete every child
	for (uint x = 0; x < m_pRoot->m_lChild.size(); x++) {
		SafeDelete(m_pRoot->m_lChild[x]);
	}
}

void Simplex::MyOctant::ConstructTree(uint a_nMaxLevel)
{
	//only subdivide this node if it's not on the max level
	if (m_uLevel < a_nMaxLevel) {
		Subdivide();
	}
	AssignIDtoEntity();
}

void Simplex::MyOctant::AssignIDtoEntity(void)
{
	//clear the dimensions of the entities inside this octant
	for (uint x = 0; x < m_EntityList.size(); x++) 
	{
		m_pEntityMngr->ClearDimensionSet(m_EntityList[x]);
		//add this octants id as a dimension to this entity if its located in it
		m_pEntityMngr->AddDimension(x, m_uID);
	}
}

uint Simplex::MyOctant::GetOctantCount(void)
{
	return m_uOctantCount;
}

void Simplex::MyOctant::SetRigidBody(void)
{
	//Back square
	m_v3Corners.push_back(m_v3Min);
	m_v3Corners.push_back(vector3(m_v3Max.x, m_v3Min.y, m_v3Min.z));
	m_v3Corners.push_back(vector3(m_v3Min.x, m_v3Max.y, m_v3Min.z));
	m_v3Corners.push_back(vector3(m_v3Max.x, m_v3Max.y, m_v3Min.z));

	//Front square
	m_v3Corners.push_back(vector3(m_v3Min.x, m_v3Min.y, m_v3Max.z));
	m_v3Corners.push_back(vector3(m_v3Max.x, m_v3Min.y, m_v3Max.z));
	m_v3Corners.push_back(vector3(m_v3Min.x, m_v3Max.y, m_v3Max.z));
	m_v3Corners.push_back(m_v3Max);

	m_pRigidBody = new MyRigidBody(m_v3Corners);

	for (uint x = 0; x < m_pEntityMngr->GetEntityCount(); x++) {
		if (IsColliding(x)) {
			m_EntityList.push_back(x);
		}
	}
}

void Simplex::MyOctant::Release(void)
{
	//kill all branches and children, then suicide :(
	if (m_uID == 0) {
		KillBranches();
	}
	m_lChild.clear();
	m_pRigidBody = nullptr;
	ClearEntityList();
}

void Simplex::MyOctant::Init(uint a_uCurrentLevel, uint count)
{
	m_uOctantCount++;
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();
	m_uID = count;
	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i] = nullptr;
	}
	m_uLevel = a_uCurrentLevel;
	m_v3Max = m_v3Center + vector3(m_fSize);
	m_v3Min = m_v3Center - vector3(m_fSize);
	SetRigidBody();
	if (m_uID == 0) {
		m_pRoot = this;
	}
	else {
		MyOctant* root = nullptr;
		for (int x = 2; x <= m_uLevel; x++) {
			root = GetParent();
		}
		m_pRoot = root;
		m_pRoot->m_lChild.push_back(this);
	}
	ConstructTree(m_uMaxLevel);
}

void Simplex::MyOctant::ConstructList(void)
{
	
}
