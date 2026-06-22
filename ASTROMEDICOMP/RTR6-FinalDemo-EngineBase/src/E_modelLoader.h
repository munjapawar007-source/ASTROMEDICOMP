#pragma once
#include "E_scene.h"

// Loads a GLB/GLTF file, adds it to the scene, and returns the root sceneObject.
// name overrides the root node name; pass NULL to use the filename.
// Returns NULL on failure.
E_sceneObject* E_loadModel(const char* path, E_scene* scene, const char* name);

// Loads only the first mesh primitive from a GLB as a plain E_mesh (positions + indices).
// No scene object is created. Caller owns the returned mesh; call E_destroyMesh when done.
E_mesh E_loadMeshOnly(const char* path);
