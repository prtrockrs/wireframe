// reads .obj files and parses into a usable state for wireframe.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"

#define MAX_READ 256

const char* OBJ_FOLDER = "./objects/";
const char* FILE_EXTENSION = ".obj";

// mallocs space for the mesh object, must be freed by caller
ObjMesh parse(const char* filename) {
	char filepath[256];
	snprintf(filepath, sizeof(filepath),
			"%s%s%s",
			OBJ_FOLDER,
			filename,
			FILE_EXTENSION);

	FILE *file = fopen(filepath, "r");

	if (file == NULL) {
		perror("Could not open file\n");
		return (ObjMesh){0};
	}

	ObjMesh mesh = { 0 };
	char buf[MAX_READ];
	int vertexCount = 0;
	int edgeCount = 0;

	// 1st pass, counting vertices and faces
	while (fgets(buf, MAX_READ, file)) {
		if (strncmp(buf, "v ", 2) == 0) {
			vertexCount++;
		} else if (strncmp(buf, "f ", 2) == 0) {
			int verts = 0;
			char* token = strtok(buf + 2, " \t\n"); // starts after the "f "
			
			while (token) {
				verts++;
				token = strtok(NULL, " \t\n");
			}

			edgeCount += verts;
		}
	}

	// allocate appropriate space
	mesh.edgeCount = edgeCount;
	mesh.vertexCount = vertexCount;
	mesh.edges = malloc(sizeof(int[2]) * edgeCount);
	mesh.vertices = malloc(sizeof(Vertex3D) * vertexCount);

	// 2nd pass, populate values
	rewind(file);

	int parsedVertCount = 0;
	int parsedEdgeCount = 0;

	while(fgets(buf, MAX_READ, file)) {
		if (strncmp(buf, "v ", 2) == 0) {
			Vertex3D v;
			sscanf(buf + 2, "%f %f %f", &v.x, &v.y, &v.z);
			mesh.vertices[parsedVertCount++] = v;
		} else if (strncmp(buf, "f ", 2) == 0) {
			int indices[64];
			int count = 0;

			char* token = strtok(buf + 2, " \t\n");

			while (token) {
				indices[count++] = atoi(token) - 1; // cuz in obj files its 1 based
				token = strtok(NULL, " \t\n");
			}

			for (int i = 0; i < count; i++) {
				mesh.edges[parsedEdgeCount][0] = indices[i];
				mesh.edges[parsedEdgeCount][1] = indices[(i + 1) % count];
				parsedEdgeCount++;
			}
		}
	}

	fclose(file);
	return mesh;
}

void normalizeMesh(ObjMesh *mesh) {
	if (mesh->vertexCount == 0) return;

	Vertex3D min = mesh->vertices[0];
    Vertex3D max = mesh->vertices[0];

    for (int i = 1; i < mesh->vertexCount; i++) {
        Vertex3D v = mesh->vertices[i];

        if (v.x < min.x) min.x = v.x;
        if (v.y < min.y) min.y = v.y;
        if (v.z < min.z) min.z = v.z;

        if (v.x > max.x) max.x = v.x;
        if (v.y > max.y) max.y = v.y;
        if (v.z > max.z) max.z = v.z;
    }

    Vertex3D center = {
        (min.x + max.x) * 0.5f,
        (min.y + max.y) * 0.5f,
        (min.z + max.z) * 0.5f
    };

    float width  = max.x - min.x;
    float height = max.y - min.y;
    float depth  = max.z - min.z;

    float largest = width;
    if (height > largest) largest = height;
    if (depth > largest) largest = depth;

    if (largest == 0.0f)
        return;

    for (int i = 0; i < mesh->vertexCount; i++) {
        mesh->vertices[i].x = (mesh->vertices[i].x - center.x);
        mesh->vertices[i].y = (mesh->vertices[i].y - center.y);
        mesh->vertices[i].z = (mesh->vertices[i].z - center.z);
    }
}