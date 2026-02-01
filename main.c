#include "raylib.h"
#include "raymath.h"
#include <float.h>

#define WORLD_SIZE 8

typedef struct {
    bool hit;
    int x, y, z;
    Vector3 position;
    Vector3 normal;
} VoxelHit;

VoxelHit RaycastVoxelDDA(
    Ray ray,
    bool voxels[WORLD_SIZE][WORLD_SIZE][WORLD_SIZE],
    float maxDistance
) {
    VoxelHit result = {0};

    Vector3 origin = ray.position;
    Vector3 dir = Vector3Normalize(ray.direction);

    // Starting voxel
    int x = (int)floorf(origin.x);
    int y = (int)floorf(origin.y);
    int z = (int)floorf(origin.z);

    // Step direction
    int stepX = (dir.x > 0) ? 1 : -1;
    int stepY = (dir.y > 0) ? 1 : -1;
    int stepZ = (dir.z > 0) ? 1 : -1;

    // Avoid division by zero
    float tDeltaX = (dir.x != 0) ? fabsf(1.0f / dir.x) : FLT_MAX;
    float tDeltaY = (dir.y != 0) ? fabsf(1.0f / dir.y) : FLT_MAX;
    float tDeltaZ = (dir.z != 0) ? fabsf(1.0f / dir.z) : FLT_MAX;

    // Distance to first voxel boundary
    float nextX = (stepX > 0) ? (x + 1.0f) : x;
    float nextY = (stepY > 0) ? (y + 1.0f) : y;
    float nextZ = (stepZ > 0) ? (z + 1.0f) : z;

    float tMaxX = (dir.x != 0) ? (nextX - origin.x) / dir.x : FLT_MAX;
    float tMaxY = (dir.y != 0) ? (nextY - origin.y) / dir.y : FLT_MAX;
    float tMaxZ = (dir.z != 0) ? (nextZ - origin.z) / dir.z : FLT_MAX;

    float dist = 0.0f;
    Vector3 hitNormal = {0};

    while (dist <= maxDistance) {

        // Bounds check
        if (x >= 0 && y >= 0 && z >= 0 &&
            x < WORLD_SIZE && y < WORLD_SIZE && z < WORLD_SIZE) {

            if (voxels[x][y][z]) {
                result.hit = true;
                result.x = x;
                result.y = y;
                result.z = z;
                result.position = Vector3Add(origin, Vector3Scale(dir, dist));
                result.normal = hitNormal;
                return result;
            }
        }

        // Step to next voxel
        if (tMaxX < tMaxY && tMaxX < tMaxZ) {
            x += stepX;
            dist = tMaxX;
            tMaxX += tDeltaX;
            hitNormal = (Vector3){ -stepX, 0, 0 };
        }
        else if (tMaxY < tMaxZ) {
            y += stepY;
            dist = tMaxY;
            tMaxY += tDeltaY;
            hitNormal = (Vector3){ 0, -stepY, 0 };
        }
        else {
            z += stepZ;
            dist = tMaxZ;
            tMaxZ += tDeltaZ;
            hitNormal = (Vector3){ 0, 0, -stepZ };
        }
    }

    return result;
}


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

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			Vector2 screenCenter = {screenWidth/2.0f, screenHeight/2.0f};
			Ray ray = GetMouseRay(screenCenter, camera);

			bool voxelRemoved = false;
			for (int x = 0; (x < WORLD_SIZE) && !voxelRemoved; x++) {
				for (int y = 0; (y < WORLD_SIZE) && !voxelRemoved; y++) {
					for (int z = 0; (z < WORLD_SIZE) && !voxelRemoved; z++) {
						if (!voxels[x][y][z]) { continue; }

						Vector3 position = { (float)x, (float)y, (float)z };
						// Bounding box for the selected voxel
						BoundingBox box = {
							(Vector3){ position.x - 0.5f, position.y - 0.5f, position.z - 0.5f },
                            (Vector3){ position.x + 0.5f, position.y + 0.5f, position.z + 0.5f }
						};

						// ray-box collision
						RayCollision collision = GetRayCollisionBox(ray, box);
						if (collision.hit) {
							voxels[x][y][z] = false;
							voxelRemoved = true;
						}
					}
				}
			}

		}

		BeginDrawing();

		ClearBackground(RAYWHITE);

		BeginMode3D(camera);

		// DrawCube(cubePos, 2.0f, 2.0f, 2.0f, RED);
		// DrawCubeWires(cubePos, 2.0f, 2.0f, 2.0f, MAROON);
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
