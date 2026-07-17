#include <raylib.h>
#include <stdio.h>
#include <math.h>

#include "types.h"
#include "objparser.h"

const int WIDTH = 600;
const int HEIGHT = 600;
const char* TITLE = "wireframe";
const float POINT_RADIUS = 1.5f;
const float ROTATE_SPEED = 1.0f;
const int TARGET_FPS = 60;
const float ZOOM_SPEED = 2.0f;

float camDist = 2.5f;

int NDCToWindowWidth(float NDCval) {
	return ((NDCval + 1.f) / 2.f) * WIDTH;
}

int NDCToWindowHeight(float NDCval) {
	return ((NDCval*(-1.f) + 1.f) / 2.f) * HEIGHT;
}

Vertex2D project(Vertex3D coords) {
	Vertex2D out;
	float z = coords.z + camDist;

	if (z <= 0.001f) z = 0.001f;

	out.x = (coords.x / z);
	out.y = (coords.y / z);
	return out;
}

Vertex3D rotateXZ(Vertex3D coord, float angle) {
	Vertex3D out;
	out.x = coord.x * cosf(angle) - coord.z * sinf(angle);
	out.y = coord.y;
	out.z = coord.x * sinf(angle) + coord.z * cosf(angle);
	return out;
}

void draw3D(Vertex3D *vertices,
			int vertexCount,
			int edges[][2],
			int edgeCount,
			float dt,
			bool drawVerts,
			bool drawEdges,
			bool drawFaces
		) {
	// vertices
	if (drawVerts) {
		for (int i = 0; i < vertexCount; i++) {
			Vertex3D vertex = vertices[i];
			Vertex2D projectedCoords = project(rotateXZ(vertex, dt));
			DrawCircle(NDCToWindowWidth(projectedCoords.x),
					NDCToWindowHeight(projectedCoords.y),
					POINT_RADIUS, GREEN);
		}
	}

	// edges
	if (drawEdges) {
		for (int i = 0; i < edgeCount; i++) {
			Vertex3D start = vertices[edges[i][0]];
			Vertex3D end = vertices[edges[i][1]];
			Vertex2D projectedStart = project(rotateXZ(start, dt));
			Vertex2D projectedEnd = project(rotateXZ(end, dt));
			DrawLine(NDCToWindowWidth(projectedStart.x), NDCToWindowHeight(projectedStart.y),
					 NDCToWindowWidth(projectedEnd.x), NDCToWindowHeight(projectedEnd.y),
					 GREEN);
		}
	}

	if (drawFaces) {
		// TODO
	}
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		perror("Filename not provided");
		return 1;
	}

	InitWindow(WIDTH, HEIGHT, TITLE);
	int monitor = GetCurrentMonitor();
	SetWindowPosition((GetMonitorWidth(monitor) - WIDTH) / 2,
					  (GetMonitorHeight(monitor) - HEIGHT) / 2);
	SetTargetFPS(TARGET_FPS);
	float dt = 0;
	char* filename = argv[1];
	ObjMesh mesh = parse(filename);
	normalizeMesh(&mesh);
	
	while (!WindowShouldClose()) {
		dt += GetFrameTime() * ROTATE_SPEED;

		if (IsKeyDown(KEY_UP)) {
			camDist -= ZOOM_SPEED * GetFrameTime();
		} else if (IsKeyDown(KEY_DOWN)) {
			camDist += ZOOM_SPEED * GetFrameTime();
		}

		BeginDrawing();
		ClearBackground(BLACK);
		draw3D(mesh.vertices, mesh.vertexCount, mesh.edges, mesh.edgeCount, dt, false, true, false);
		EndDrawing();
	}

	free(mesh.edges);
	free(mesh.vertices);
	CloseWindow();
	return 0;
}