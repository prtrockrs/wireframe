#pragma once

typedef struct {
	float x, y;
} Vertex2D;

typedef struct {
	float x, y, z;
} Vertex3D;

typedef struct {
	Vertex3D *vertices;
	int vertexCount;
	int (*edges)[2];
	int edgeCount;
} ObjMesh;