#include "MyOctant.h"
using namespace Simplex;

uint MyOctant::octantCount = 0;
uint MyOctant::maxLevel = 3;
uint MyOctant::bestEntityCount = 5;
uint MyOctant::GetOctantCount(void) { return octantCount; }
void MyOctant::Init(void) {
	children = 0;

	//initialized variables
	size = 0.0f;
	ID = octantCount;
	level = 0;

	m_v3Center = vector3(0.0f);
	m_v3Min = vector3(0.0f);
	m_v3Max = vector3(0.0f);

	// get managers
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	// set null pointers
	root = nullptr;
	parent = nullptr;

	//set all children as null
	for (uint n = 0; n < 8; n++) {
		childOctant[n] = nullptr;
	}
}

void MyOctant::Release(void) {
	if (level == 0) {
		KillBranches();
	}
	//Get rid of branches
	children = 0;
	size = 0.0f;
	entityList.clear();
	childList.clear();
}
void MyOctant::Swap(MyOctant& other) {
	std::swap(children, other.children);

	// swap this with other object
	std::swap(size, other.size);
	std::swap(ID, other.ID);
	std::swap(root, other.root);
	std::swap(childList, other.childList);

	std::swap(m_v3Center, other.m_v3Center);
	std::swap(m_v3Min, other.m_v3Min);
	std::swap(m_v3Max, other.m_v3Max);

	// set managers
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	// swap nodes
	std::swap(level, other.level);
	std::swap(parent, other.parent);
	for (uint i = 0; i < 8; i++) {
		std::swap(childOctant[i], other.childOctant[i]);
	}
}
MyOctant * MyOctant::GetParent(void) { return parent; }


MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount) {
	// initialize variables 
	Init();
	octantCount = 0;
	maxLevel = a_nMaxLevel;
	bestEntityCount = a_nIdealEntityCount;
	ID = octantCount;
	root = this;
	childList.clear();
	std::vector<vector3> pointList;
	uint objects = m_pEntityMngr->GetEntityCount();

	// add global min and max to the list
	for (uint i = 0; i < objects; i++) {
		MyEntity* entity = m_pEntityMngr->GetEntity(i);
		MyRigidBody* rigidBody = entity->GetRigidBody();
		pointList.push_back(rigidBody->GetMinGlobal());
		pointList.push_back(rigidBody->GetMaxGlobal());
	}

	// create a rigidbody for the octant
	MyRigidBody* rigidBody = new MyRigidBody(pointList);
	vector3 halfWidth = rigidBody->GetHalfWidth();
	float max = halfWidth.x;

	// set the maximum length
	for (int i = 1; i < 3; i++) {
		if (max < halfWidth[i])
			max = halfWidth[i];
	}

	// set the center
	vector3 center = rigidBody->GetCenterLocal();
	//delete the rigidbody
	pointList.clear();
	SafeDelete(rigidBody);

	// set the variables using size and center
	size = max * 2.0f;
	m_v3Center = center;
	m_v3Min = m_v3Center - (vector3(max));
	m_v3Max = m_v3Center + (vector3(max));

	// increment the octant count
	octantCount++;
	//make tree
	ConstructTree(maxLevel);
}
MyOctant::MyOctant(vector3 a_v3Center, float a_fSize) {
	// initialize variables
	Init();
	m_v3Center = a_v3Center;
	size = a_fSize;

	// set variables
	m_v3Min = m_v3Center - (vector3(size) / 2.0f);
	m_v3Max = m_v3Center + (vector3(size) / 2.0f);

	// increment count of octants
	octantCount++;
}
MyOctant::MyOctant(MyOctant const& other) {
	// set variables as other's member variables
	m_v3Center = other.m_v3Center;
	m_v3Min = other.m_v3Min;
	m_v3Max = other.m_v3Max;

	size = other.size;
	ID = other.ID;
	level = other.level;

	children = other.children;
	parent = other.parent;
	root, other.root;
	childList, other.childList;

	// set managers
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	for (uint i = 0; i < 8; i++) {
		childOctant[i] = other.childOctant[i];
	}
}
MyOctant& MyOctant::operator=(MyOctant const& other) {
	// not equal to other, perform operation
	if (this != &other) {
		Release();
		Init();
		MyOctant temp(other);
		Swap(temp);
	}
	return *this;
}
MyOctant::~MyOctant() { Release(); };

//Getters
float MyOctant::GetSize(void) { return size; }

vector3 MyOctant::GetCenterGlobal(void) { return m_v3Center; }

vector3 MyOctant::GetMinGlobal(void) { return m_v3Min; }

vector3 MyOctant::GetMaxGlobal(void) { return m_v3Max; }

void MyOctant::Display(uint a_nIndex, vector3 a_v3Color) {
	// display if equal to the current
	if (ID == a_nIndex) {
		m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) *
			glm::scale(vector3(size)), a_v3Color, RENDER_WIRE);
		return;
	}
	// recursively find the octant to display
	for (uint i = 0; i < children; i++) {
		childOctant[i]->Display(a_nIndex);
	}
}
void MyOctant::Display(vector3 a_v3Color) {
	// recursively display octant volume
	for (uint i = 0; i < children; i++) {
		childOctant[i]->Display(a_v3Color);
	}
	// use color
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) *
		glm::scale(vector3(size)), a_v3Color, RENDER_WIRE);
}
void MyOctant::Subdivide(void) {
	// return if max
	if (level >= maxLevel)
		return;

	// return if subdivided
	if (children != 0)
		return;

	// create children
	children = 8;

	// set size of new octant
	float sizeH = size / 4.0f;
	float sideF = sizeH * 2.0f;
	vector3 center;

	// bottom left back
	center = m_v3Center;
	center.x -= sizeH;
	center.y -= sizeH;
	center.z -= sizeH;
	childOctant[0] = new MyOctant(center, sideF);

	// bottom right back
	center.x += sideF;
	childOctant[1] = new MyOctant(center, sideF);

	// bottom right front
	center.z += sideF;
	childOctant[2] = new MyOctant(center, sideF);

	// bottom left front
	center.x -= sideF;
	childOctant[3] = new MyOctant(center, sideF);

	// top left front
	center.y += sideF;
	childOctant[4] = new MyOctant(center, sideF);

	// top left back
	center.z -= sideF;
	childOctant[5] = new MyOctant(center, sideF);

	// top right back
	center.x += sideF;
	childOctant[6] = new MyOctant(center, sideF);

	// top right front
	center.z += sideF;
	childOctant[7] = new MyOctant(center, sideF);

	// set new root, parent, and level
	// subdivide if not best size
	for (uint i = 0; i < 8; i++) {
		childOctant[i]->root = root;
		childOctant[i]->parent = this;
		childOctant[i]->level = level + 1;
		if (childOctant[i]->ContainsMoreThan(bestEntityCount)) {
			childOctant[i]->Subdivide();
		}
	}
}
MyOctant * MyOctant::GetChild(uint a_nChild) {
	// get child from list
	if (a_nChild > 7) return nullptr;
	return childOctant[a_nChild];
}

bool MyOctant::IsLeaf(void) { return children == 0; }

bool MyOctant::IsColliding(uint a_uRBIndex) {
	uint objects = m_pEntityMngr->GetEntityCount();
	// if index is out of range return
	if (a_uRBIndex >= objects)
		return false;

	// get global max and min
	MyEntity* entity = m_pEntityMngr->GetEntity(a_uRBIndex);
	MyRigidBody* rigidBody = entity->GetRigidBody();
	vector3 min = rigidBody->GetMinGlobal();
	vector3 max = rigidBody->GetMaxGlobal();

	// check if its in x boubds
	if (m_v3Max.x < min.x)
		return false;
	if (m_v3Min.x > max.x)
		return false;

	// check if its in y bounds
	if (m_v3Max.y < min.y)
		return false;
	if (m_v3Min.y > max.y)
		return false;

	// check if its in z bounds
	if (m_v3Max.z < min.z)
		return false;
	if (m_v3Min.z > max.z)
		return false;

	// its colliding
	return true;

}

bool MyOctant::ContainsMoreThan(uint a_nEntities) {
	uint count = 0;
	uint objects = m_pEntityMngr->GetEntityCount();
	// checks if objects are colliding with octant
	for (uint n = 0; n < objects; n++) {
		if (IsColliding(n))
			count++;
		if (count > a_nEntities)
			return true;
	}
	return false;
}
void MyOctant::KillBranches(void) {
	// Gets rid of nodes and children
	for (uint i = 0; i < children; i++) {
		childOctant[i]->KillBranches();
		delete childOctant[i];
		childOctant[i] = nullptr;
	}
	children = 0;
}
void MyOctant::DisplayLeafs(vector3 a_v3Color) {
	uint leafs = childList.size();
	// show leaves
	for (uint i = 0; i < leafs; i++) {
		childList[i]->DisplayLeafs(a_v3Color);
	}
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) *
		glm::scale(vector3(size)), a_v3Color, RENDER_WIRE);
}
void MyOctant::ClearEntityList(void) {
	// clear list
	for (uint i = 0; i < children; i++) {
		childOctant[i]->ClearEntityList();
	}
	entityList.clear();
}
void MyOctant::ConstructTree(uint a_nMaxLevel) {
	// check if its the root
	if (level != 0)
		return;

	// set counts
	maxLevel = a_nMaxLevel;
	octantCount = 1;

	// clear the entity list
	entityList.clear();

	// clear the tree
	KillBranches();
	childList.clear();

	// subdivide if more than best count
	if (ContainsMoreThan(bestEntityCount)) {
		Subdivide();
	}

	// set id to entityList
	AssignIDtoEntity();

	// creates lists
	ConstructList();
}
void MyOctant::AssignIDtoEntity(void) {
	// set id to entities
	for (uint i = 0; i < children; i++) {
		childOctant[i]->AssignIDtoEntity();
	}
	// if its a leaf find its objects and add to list
	if (children == 0) {
		uint objects = m_pEntityMngr->GetEntityCount();
		for (uint i = 0; i < objects; i++) {
			if (IsColliding(i)) {
				entityList.push_back(i);
				m_pEntityMngr->AddDimension(i, ID);
			}
		}
	}
}
void MyOctant::ConstructList(void) {
	// set list for each child
	for (uint i = 0; i < children; i++) {
		childOctant[i]->ConstructList();
	}

	// send to root
	if (entityList.size() > 0) {
		root->childList.push_back(this);
	}
}