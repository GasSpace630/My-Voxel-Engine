#include "raylib.h"
#include "raymath.h"

#define WORLD_X 8
#define WORLD_Y 4
#define WORLD_Z 8

Vector3 VoxelToWorld(int x, int y, int z) {
    return (Vector3){
        x - WORLD_X/2.0f,
        y - WORLD_Y/2.0f,
        z - WORLD_Z/2.0f
    };
}


int main(void) {
	const int screenWidth = 960;
	const int screenHeight = 540;

	InitWindow(screenWidth, screenHeight, "My Voxel Engine");

	// Camera
	Camera3D camera = { 0 };
	camera.position = (Vector3){0.0f, 10.0f, 10.0f};
	camera.up = (Vector3){0.0f, 1.0f, 0.0f};
	camera.fovy = 45.0f;
	camera.projection = CAMERA_PERSPECTIVE;

	// Mesh
	Mesh cubeMesh = GenMeshCube(1.0f, 1.0f, 1.0f);
	Model CubeModel = LoadModelFromMesh(cubeMesh);
	CubeModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = GREEN;

	// Voxel World
	bool voxels[WORLD_X][WORLD_Y][WORLD_Z] = {false};
	int hx = WORLD_X / 2;
	int hy = WORLD_Y / 2;
	int hz = WORLD_Z / 2;

	for (int x = -hx; x < hx; x++) {
	    for (int y = -hy; y < hy; y++) {
	        for (int z = -hz; z < hz; z++) {
	            voxels[x + hx][y + hy][z + hz] = true;
	        }
	    }
	}

	DisableCursor();

	SetTargetFPS(60);

	while(!WindowShouldClose()) {

		// moving, zooming, paning
		UpdateCamera(&camera, CAMERA_FREE);

		BeginDrawing();

		ClearBackground(SKYBLUE);

		BeginMode3D(camera);

		DrawGrid(16, 1.0f);

		// Drawing Voxels
		int hx = WORLD_X / 2;
		int hy = WORLD_Y / 2;
		int hz = WORLD_Z / 2;

		for (int x = 0; x < WORLD_X; x++) {
		    for (int y = 0; y < WORLD_Y; y++) {
		        for (int z = 0; z < WORLD_Z; z++) {

		            if (!voxels[x][y][z]) continue;

		            Vector3 pos = VoxelToWorld(x, y, z);

		            DrawModel(CubeModel, pos, 1.0f, LIME);
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
