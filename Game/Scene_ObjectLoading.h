#pragma once
#include <Scene.h>

class Scene_ObjectLoading final : public Scene
{
	public:
		Scene_ObjectLoading();
		virtual ~Scene_ObjectLoading() override;

		Scene_ObjectLoading(const Scene_ObjectLoading& other) = delete;
		Scene_ObjectLoading(Scene_ObjectLoading&& other) = delete;
		Scene_ObjectLoading& operator=(const Scene_ObjectLoading& other) = delete;
		Scene_ObjectLoading& operator=(Scene_ObjectLoading&& other) = delete;

		virtual void Initialize() override;
		virtual void PostInitialize() override;
		virtual void Update(const float elapsedSec) override;
		virtual void FixedUpdate(const float timeEachUpdate) override;
		virtual void LateUpdate() override;
		virtual void Render() const override;

	private:
		/** Function that creates a landscape from a 2D plane */
		void CreateLandscape_2DPlane();
		///** Function that creates a landscape with voxels on the CPU */
		//void CreateLandscape_VoxelsCPU();
		///** Function that creates a landscape with voxels on the GPU */
		//void CreateLandscape_VoxelsGPU();
		///** Function that creates a landscape from a 2D plane with the Octree data structure */
		//void CreateLandscape_2DPlaneOctree(const bool visualizeDataStructure);
		///** Function that creates a landscape from a 2D plane with the KDTree data structure */
		//void CreateLandscape_2DPlaneKDtree();
};