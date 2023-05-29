#define EDITING_VERTEX 0
#define EDITING_TEXTURE 1
#define EDITING_NORMAL 2

#include "objectsStructures.h"

char* readFromFile(char* fn) {
	FILE* fp;
	char* content = NULL;
	int count = 0;

	if (fn != NULL) {
		fopen_s(&fp, fn, "rt");

		if (fp != NULL) {

			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);

			if (count > 0) {
				content = (char*)malloc(sizeof(char) * (count + 1));
				count = fread(content, sizeof(char), count, fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	return content;
}

// Funciona somente com bmp de 24 bits
BitMapFile* getBMPData(string filename) {
	BitMapFile* bmp = new BitMapFile;
	unsigned int size, offset, headerSize;

	// Ler o arquivo de entrada
	ifstream infile(filename.c_str(), ios::binary);

	// Pegar o ponto inicial de leitura
	infile.seekg(10);
	infile.read((char*)&offset, 4);

	// Pegar o tamanho do cabe�alho do bmp.
	infile.read((char*)&headerSize, 4);

	// Pegar a altura e largura da imagem no cabe�alho do bmp.
	infile.seekg(18);
	infile.read((char*)&bmp->sizeX, 4);
	infile.read((char*)&bmp->sizeY, 4);

	// Alocar o buffer para a imagem.
	size = bmp->sizeX * bmp->sizeY * 24;
	bmp->data = new unsigned char[size];

	// Ler a informa��o da imagem.
	infile.seekg(offset);
	infile.read((char*)bmp->data, size);

	// Reverte a cor de bgr para rgb
	int temp;
	for (int i = 0; i < size; i += 3)
	{
		temp = bmp->data[i];
		bmp->data[i] = bmp->data[i + 2];
		bmp->data[i + 2] = temp;
	}

	return bmp;
}

void loadTextures(string nome_arquivo, int id_textura) {
	BitMapFile* image[1];

	image[0] = getBMPData(nome_arquivo);

	glBindTexture(GL_TEXTURE_2D, textures[id_textura]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[0]->sizeX, image[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image[0]->data);
}


void getLines(string filename, Obj& obj) {
	obj.facesNum = 0; obj.vertexNum = 0; obj.normalNum = 0; obj.texturesCount = 0;
	ifstream file(filename);

	if (file.is_open()) {
		string id;

		while (file >> id) {
			if (id == "v") {
				obj.vertexNum += 1;
			}
			else if (id == "vn") {
				obj.normalNum += 1;
			}
			else if (id == "f") {
				obj.facesNum += 1;
			}
			else if (id == "vt") {
				obj.texturesCount += 1;
			}
		}

		file.close();
	}
}

int getElementsCount(string filename, string identifier) {
	ifstream file(filename);
	string id = "", line;
	int counter = 0;

	while (id != identifier) {
		file >> id;
		getline(file, line);
	}
	for (int i = 0; line[i + 1] != 0; i++) {
		if (line[i] == ' ' && line[i + 1] != ' ') {
			counter++;
		}
	}

	file.close();

	return counter;
}

void getVertexCount(string filename, Obj& obj) {
	ifstream file(filename);
	string id = "";
	string line;
	int currentFace = 0;

	while (file >> id) {
		getline(file, line);
		if (id == "f") {
			int counter = 0;
			for (int i = 0; line[i + 1] != 0; i++) {
				if (line[i] == ' ' && line[i + 1] != ' ') {
					counter++;
				}
			}

			obj.faces[currentFace].vertexPerFace = counter;
			obj.faces[currentFace].vertex = new unsigned short int[counter];
			obj.faces[currentFace].texture = new unsigned short int[counter];
			obj.faces[currentFace].normal = new unsigned short int[counter];
			currentFace++;
		}
	}

	file.close();
}

void getElementsCount(string filename, Obj& obj, string identifier) {
	if (obj.normalNum == 0 && identifier == "vn") return;

	int numElements = getElementsCount(filename, identifier);
	double maxX = -10000, minX = 10000;
	double maxY = -10000, minY = 10000;
	double maxZ = -10000, minZ = 10000;
	string id, x, y, z, w, currentMaterial;
	int current = 0;
	ifstream file(filename);

	while (file >> id) {
		if (id == identifier) {
			if (identifier == "usemtl") {
				file >> x;
			}
			else if (numElements == 2) {
				file >> x >> y;
			}
			else if (numElements == 3) {
				file >> x >> y >> z;
			}
			else if (numElements == 4) {
				file >> x >> y >> z >> w;
			}

			if (identifier == "v") {
				//---x---//
				obj.vertex[current].x = stod(x);
				if (stod(x) > maxX) { maxX = stod(x); }
				if (stod(x) < minX) { minX = stod(x); }
				//---y---//
				obj.vertex[current].y = stod(y);
				if (stod(y) > maxY) { maxY = stod(y); }
				if (stod(y) < minY) { minY = stod(y); }
				//---z---//
				obj.vertex[current].z = stod(z);
				if (stod(z) > maxZ) { maxZ = stod(z); }
				if (stod(z) < minZ) { minZ = stod(z); }
			}
			else if (identifier == "vn") {
				obj.normals[current].x = stod(x);
				obj.normals[current].y = stod(y);
				obj.normals[current].z = stod(z);
			}
			else if (identifier == "usemtl") {
				currentMaterial = x;
			}
			else if (identifier == "vt") {
				obj.textures[current].coordenates.x = stod(x);
				obj.textures[current].coordenates.y = stod(y);
				obj.textures[current].material = currentMaterial;
			}

			current++;
		}
	}

	if (identifier == "v") {
		obj.offset.x = minX + ((maxX - minX) / 2);
		obj.offset.y = minY + ((maxY - minY) / 2);
		obj.offset.z = minZ + ((maxZ - minZ) / 2);
		obj.size.x = maxX - minX;
		obj.size.y = maxY - minY;
		obj.size.z = maxZ - minZ;
	}

	file.close();
}

void getFaceELCount(string filename, Obj& obj) {
	ifstream file(filename);
	string line, id = "", temp;
	int currentVertex = -1, currentFace = 0, numVertexPassed = 0, numNormalsPassed = 0, numTexturesPassed = 0;

	while (file >> id) {
		getline(file, line);
		int editing = EDITING_NORMAL;

		if (id == "v") numVertexPassed++;
		if (id == "vn") numNormalsPassed++;
		if (id == "vt") numTexturesPassed++;

		// face identifier
		if (id == "f") {
			for (int i = 1; line[i] != 0; i++) {
				// currentFace incrementer and currentVertex resetter
				if (currentVertex == obj.faces[currentFace].vertexPerFace) {
					currentFace++;
					currentVertex = 0;
				}

				// element identifier
				if (line[i - 1] == ' ' && line[i] != ' ') {
					editing = EDITING_VERTEX;
					temp = "";
					currentVertex++;
				}

				else if (editing == EDITING_VERTEX && line[i - 1] == '/') {
					editing = EDITING_TEXTURE;
					temp = "";
				}
				else if (editing == EDITING_TEXTURE && line[i - 1] == '/') {
					editing = EDITING_NORMAL;
					temp = "";
				}

				// element "builder" (if isn't '/' or ' ')
				if (!(line[i] == '/' || line[i] == ' ')) {
					temp += line[i];
					// element applier (if is '/' or ' ' and has something in temp)
				}
				else if (temp != "") {
					if (editing == EDITING_VERTEX) {
						obj.faces[currentFace].vertex[currentVertex] = (stoi(temp) > 0) ? abs(stoi(temp)) - 1 : numVertexPassed - abs(stoi(temp));
					}
					else if (editing == EDITING_TEXTURE) {
						obj.faces[currentFace].texture[currentVertex] = (stoi(temp) > 0) ? abs(stoi(temp)) - 1 : numTexturesPassed - abs(stoi(temp));
					}
					else if (editing == EDITING_NORMAL) {
						obj.faces[currentFace].normal[currentVertex] = (stoi(temp) > 0) ? abs(stoi(temp)) - 1 : numNormalsPassed - abs(stoi(temp));
					}
				}
			}

			// ---last normal from current line---
			obj.faces[currentFace].normal[currentVertex] = (stoi(temp) > 0) ? abs(stoi(temp)) - 1 : numNormalsPassed - abs(stoi(temp));
		}
	}
}

int materialTextures(string filename) {
	ifstream file(filename);
	string id = "", line;
	int count = 0;

	while (file >> id) {
		if (id == "newmtl") {
			count++;
		}
	}

	return count;
}

void loadObj(Obj& obj, string filename) {
	getLines(filename, obj);

	if (obj.vertexNum) {
		obj.vertex = new Point3D[obj.vertexNum];
		obj.normals = new Point3D[obj.normalNum];
		obj.faces = new Face[obj.facesNum];
		obj.textures = new Texture[obj.texturesCount];

	
		getElementsCount(filename, obj, "v");
		cout << "Vertex...Counting... " + to_string(obj.vertexNum) << endl;
		getElementsCount(filename, obj, "vn");
		cout << "Normals...Counting... " + to_string(obj.normalNum) << endl;
		getVertexCount(filename, obj);
		getFaceELCount(filename, obj);
		cout << "Faces...Counting... " + to_string(obj.facesNum) << endl;
		getElementsCount(filename, obj, "vt");
		cout << "Textures...Counting... " + to_string(obj.texturesCount) << endl;
	}
}