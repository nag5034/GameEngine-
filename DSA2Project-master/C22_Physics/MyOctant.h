#pragma once

#include "MyEntityManager.h"

namespace Simplex
{
	class MyOctant
	{
		static uint octantCount; // number of octants
		static uint maxLevel; // max size
		static uint bestEntityCount; // best size
		float size = 0.0f; // size
		uint ID = 0; // current ID
		uint level = 0; // current level
		uint children = 0; // number of children
		MeshManager* m_pMeshMngr = nullptr; // Mesh manager
		MyEntityManager* m_pEntityMngr = nullptr; // Entity manager
		vector3 m_v3Center = vector3(0.0f); // center
		vector3 m_v3Min = vector3(0.0f); // min
		vector3 m_v3Max = vector3(0.0f); // max
		MyOctant* parent = nullptr; // parent
		MyOctant* childOctant[8]; //child
		std::vector<uint> entityList; // entity list in octant
		MyOctant* root = nullptr; // the root octant
		std::vector<MyOctant*> childList; // list of children

	public:

		MyOctant(uint a_uMaxLevel = 2, uint a_nIdealEntityCount = 5);//Constructor that will create an octant containing all entity instances in the mesh manager

		MyOctant(vector3 a_v3Center, float a_fSize);//Constructor

		MyOctant(MyOctant const& other);//Copy Constructor

		MyOctant& operator=(MyOctant const& other);//Copy Assignment Operator

		~MyOctant(void);//Destructor

		void Swap(MyOctant& other);//Changes object contents for other object's

		float GetSize(void);//Gets this octant's size

		vector3 GetCenterGlobal(void);//Gets octants center in global space

		vector3 GetMinGlobal(void);//Gets the min corner of the octant in global space

		vector3 GetMaxGlobal(void);//Gets the max corner of the octant in global space

		bool IsColliding(uint a_uRBIndex);//Asks if there is a collision with the Entity specified by index from the Bounding Object Manager

		void Display(uint a_nIndex, vector3 a_v3Color = C_YELLOW);//Displays the MyOctant volume specified by index including the objects underneath

		void Display(vector3 a_v3Color = C_YELLOW);//Displays the MyOctant volume in the color specified

		void DisplayLeafs(vector3 a_v3Color = C_YELLOW);//Displays the non empty leafs in the octree

		void ClearEntityList(void);//Clears the Entity List for each node

		void Subdivide(void);//Allocates 8 smaller octants in the child pointers

		MyOctant* GetChild(uint a_nChild);//Returns the child specified in the index

		MyOctant* GetParent(void);//Returns the parent of the octant

		bool IsLeaf(void);//Asks the MyOctant if it does not contain any children (a leaf)

		bool ContainsMoreThan(uint a_nEntities);//Asks the MyOctant if it contains more than this many Bounding Objects

		void KillBranches(void);//Deletes all children and the children of their children

		void ConstructTree(uint a_nMaxLevel = 3);//Creates a tree using subdivisions, the max number of objects and levels

		void AssignIDtoEntity(void);//Traverse the tree up to the leafs and sets the objects in them to the index

		uint GetOctantCount(void);//Gets the total number of octants in the world

	private:

		void Release(void);//Deallocates member fields


		void Init(void);//Allocates member fields


		void ConstructList(void);//Creates the list of all leafs that contain objects
	}; // class
}// namespace Simplex