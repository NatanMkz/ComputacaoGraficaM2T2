#ifndef READER_H
#define READER_H

#define EDITING_VERTEX 0
#define EDITING_TEXTURE 1
#define EDITING_NORMAL 2

#include "objectsStructures.h"



BitMapFile* getBMPData(string);

int materialTextures(string);
void loadTextures(string, int);

char* readFromFile(char* fn);

void getVertexCount(string, Obj&);

void getLines(string, Obj&);

int getElementsCount(string, string);

void getElementsCount(string, Obj&, string);

void getFaceELCount(string, Obj&);

void loadObj(Obj&, string);

#endif