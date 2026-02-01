#include "raylib.h"
#include "raymath.h"

#define WORLD_X 8
#define WORLD_Y 4
#define WORLD_Z 8

bool voxels[WORLD_X][WORLD_Y][WORLD_Z] = {false};

Vector3 VoxelToWorld(int x, int y, int z) {
    return (Vector3){
        x - WORLD_X/2.0f,
        y - WORLD_Y/2.0f,
        z - WORLD_Z/2.0f
    };
}

static inline bool IsSolid(int x, int y, int z) {
    if (x < 0 || x >= WORLD_X ||
        y < 0 || y >= WORLD_Y ||
        z < 0 || z >= WORLD_Z)
        return false;

    return voxels[x][y][z];
}

void DrawFace(Vector3 pos, int face) {
    float s = 0.5f; // half cube size

    Vector3 v0, v1, v2, v3;

    switch (face) {
        case 0: // +X
            v0 = (Vector3){pos.x + s, pos.y - s, pos.z - s};
            v1 = (Vector3){pos.x + s, pos.y - s, pos.z + s};
            v2 = (Vector3){pos.x + s, pos.y + s, pos.z + s};
            v3 = (Vector3){pos.x + s, pos.y + s, pos.z - s};
            break;

        case 1: // -X
            v0 = (Vector3){pos.x - s, pos.y - s, pos.z + s};
            v1 = (Vector3){pos.x - s, pos.y - s, pos.z - s};
            v2 = (Vector3){pos.x - s, pos.y + s, pos.z - s};
            v3 = (Vector3){pos.x - s, pos.y + s, pos.z + s};
            break;

        case 2: // +Y
            v0 = (Vector3){pos.x - s, pos.y + s, pos.z - s};
            v1 = (Vector3){pos.x + s, pos.y + s, pos.z - s};
            v2 = (Vector3){pos.x + s, pos.y + s, pos.z + s};
            v3 = (Vector3){pos.x - s, pos.y + s, pos.z + s};
            break;

        case 3: // -Y
            v0 = (Vector3){pos.x - s, pos.y - s, pos.z + s};
            v1 = (Vector3){pos.x + s, pos.y - s, pos.z + s};
            v2 = (Vector3){pos.x + s, pos.y - s, pos.z - s};
            v3 = (Vector3){pos.x - s, pos.y - s, pos.z - s};
            break;

        case 4: // +Z
            v0 = (Vector3){pos.x - s, pos.y - s, pos.z + s};
            v1 = (Vector3){pos.x - s, pos.y + s, pos.z + s};
            v2 = (Vector3){pos.x + s, pos.y + s, pos.z + s};
            v3 = (Vector3){pos.x + s, pos.y - s, pos.z + s};
            break;

        case 5: // -Z
            v0 = (Vector3){pos.x + s, pos.y - s, pos.z - s};
            v1 = (Vector3){pos.x + s, pos.y + s, pos.z - s};
            v2 = (Vector3){pos.x - s, pos.y + s, pos.z - s};
            v3 = (Vector3){pos.x - s, pos.y - s, pos.z - s};
            break;
    }

    // Two triangles per face
    DrawTriangle3D(v0, v1, v2, LIME);
    DrawTriangle3D(v0, v2, v3, LIME);
}

void DrawWorld() {
	int hx = WORLD_X / 2;
	int hy = WORLD_Y / 2;
	int hz = WORLD_Z / 2;

	for (int x = 0; x < WORLD_X; x++) {
	    for (int y = 0; y < WORLD_Y; y++) {
	        for (int z = 0; z < WORLD_Z; z++) {

	            if (!voxels[x][y][z]) continue;

	            Vector3 pos = {
	                x - hx,
	                y - hy,
	                z - hz
	            };

	            // +X
	            if (!IsSolid(x + 1, y, z)) DrawFace(pos, 0);
	            // -X
	            if (!IsSolid(x - 1, y, z)) DrawFace(pos, 1);
	            // +Y
	            if (!IsSolid(x, y + 1, z)) DrawFace(pos, 2);
	            // -Y
	            if (!IsSolid(x, y - 1, z)) DrawFace(pos, 3);
	            // +Z
	            if (!IsSolid(x, y, z + 1)) DrawFace(pos, 4);
	            // -Z
	            if (!IsSolid(x, y, z - 1)) DrawFace(pos, 5);
	        }
	    }
	}
}


int main(void) {
	const int screenWidth = 960;
	const int screenHeight = 540;

	InitWindow(screenWidth, screenHeight, "My Voxel Engine");

	for (int x = 0; x < WORLD_X; x++) {
	    for (int y = 0; y < WORLD_Y; y++) {
	        for (int z = 0; z < WORLD_Z; z++) {
	            voxels[x][y][z] = true; // solid cube
	        }
	    }
	}

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
		DrawWorld();

		EndMode3D();

		DrawFPS(2, 2);

		EndDrawing();
	}

	UnloadModel(CubeModel);
	CloseWindow();

	return 0;
}
