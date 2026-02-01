#include "raylib.h"
#include "raymath.h"
#include <float.h>

#define WORLD_SIZE 8

int main(void) {
	const int screenWidth = 960;
	const int screenHeight = 540;

	InitWindow(screenWidth, screenHeight, "My Voxel Engine");

	// Camera
	Camera3D camera = { 0 };
	camera.position = (Vector3){0.0f, 10.0f, 10.0f};
	// camera.target = (Vector3){0.0f, 0.0f, 0.0f};
	camera.up = (Vector3){0.0f, 1.0f, 0.0f};
	camera.fovy = 45.0f;
	camera.projection = CAMERA_PERSPECTIVE;

	// The cube
	Vector3 cubePos = {0.0f, 0.0f, 0.0f};

	// Mesh
	Mesh cubeMesh = GenMeshCube(1.0f, 1.0f, 1.0f);
	Model CubeModel = LoadModelFromMesh(cubeMesh);
	CubeModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = RED;

	// Voxel World
	bool voxels[WORLD_SIZE][WORLD_SIZE][WORLD_SIZE] = {false};
	for (int x = 0; x < WORLD_SIZE; x++) {
		for (int y = 0; y < WORLD_SIZE; y++) {
			for (int z = 0; z < WORLD_SIZE; z++) {
				voxels[x][y][z] = true;
			}
		}
	}

	DisableCursor();

	SetTargetFPS(60);

	while(!WindowShouldClose()) {

		// moving, zooming, paning
		UpdateCamera(&camera, CAMERA_FREE);

		BeginDrawing();

		ClearBackground(RAYWHITE);

		BeginMode3D(camera);

		DrawGrid(10, 1.0f);

		// Drawing Voxels
		for (int x = 0; x < WORLD_SIZE; x++) {
			for (int y = 0; y < WORLD_SIZE; y++) {
				for (int z = 0; z < WORLD_SIZE; z++) {
					
					if (!voxels[x][y][z]) { continue;}

					Vector3 pos = {(float)x, (float)y, (float)z};
					DrawModel(CubeModel, pos, 1.0f, RED);
					DrawCubeWires(pos, 1.0f, 1.0f, 1.0f, BLACK);
				}
			}
		}


		EndMode3D();

		DrawFPS(2, 2);

		EndDrawing();
	}

	UnloadModel(CubeModel);
	CloseWindow();

	return 0;
}
