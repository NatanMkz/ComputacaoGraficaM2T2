#ifndef STRUCTS_H
#define STRUCTS_H

#include <iostream>
#include <GL/glew.h>
#include <GL/GLU.h>
#include <GL/freeglut_std.h>
#include <Windows.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

extern unsigned int textures[2];



struct Face {
	unsigned short int vertexPerFace;
	unsigned short int* vertex;
	unsigned short int* texture;
	unsigned short int* normal;
};

struct Point3D {
	double x;
	double y;
	double z;

	Point3D() {
		this->x = 0;
		this->y = 0;
		this->z = 0;
	}

	Point3D(double x, double y, double z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
};
struct Point2D {
	double x;
	double y;

	Point2D() {
		this->x = 0;
		this->y = 0;
	}

	Point2D(double x, double y) {
		this->x = x;
		this->y = y;
	}
};


struct Texture {
	Point2D coordenates;
	string material;
};


struct Obj {
	Point3D angle;
	Point3D position;
	Point3D size;
	Point3D offset;
	Point3D* vertex;
	Point3D* normals;
	Texture* textures;
	Face* faces;

	double scale;
	int vertexNum;
	int normalNum;
	int facesNum;
	int texturesCount;
	int materialCount;
	unsigned int* textureId;
};

struct BitMapFile {
	int sizeX;
	int sizeY;
	unsigned char* data;
};

#endif