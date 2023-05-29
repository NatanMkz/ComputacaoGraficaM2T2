#include "../CGM2F/glew-2.1.0/include/GL/glew.h"
#include "../CGM2F/freeglut/include/GL/freeglut.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <chrono>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

bool example = false;

GLuint v, f;
GLuint p;

GLuint VAO, VBO;

// Vari�veis globais
unsigned int object;
vector<vector<float>> vertices_obj;
vector<vector<int>> faces;
vector<vector<float>> normals;
vector<vector<float>> textureBounds;
float angle_x = 0;
float angle_y = 0;
float angle_z = 0;
float pos_x = 0;
float pos_y = 0;
float pos_z = 0;
float translateX, translateY, translateZ;
float rotationX, rotationY, rotationZ;
float rotVel = 3.5;
bool light1, light2, light3;
float scale = 1;
float ang_passo = 0.1;


float* vertices;
vector<float> vertices_vector;
int bufferArraySize;

static unsigned int texturas[2];

float ambientLight = 0.1;
float difuseLight = 0.9;
float colorLight[3] = { 1, 1, 1 };
float lightPos[3] = { 0, 1, 0.25 };


struct BitMapFile {
	int sizeX;
	int sizeY;
	unsigned char* data;
};

std::vector<std::string> helperSplit(std::string, char);

// Carregando arquivo OBJ
void objLoad(string fname) {
	int read;
	float x, y, z;
	ifstream file(fname);
	if (!file.is_open()) {
		cout << "arquivo nao encontrado";
		exit(1);
	}
	else {
		string tipo;
		while (file >> tipo) {

			if (tipo == "v") {
				vector<float> vertice;
				float x, y, z;
				file >> x >> y >> z;
				vertice.push_back(x);
				vertice.push_back(y);
				vertice.push_back(z);
				vertices_obj.push_back(vertice);
			}

			if (tipo == "vt") {
				vector<float> textura;
				float x, y;
				file >> x >> y;
				textura.push_back(x);
				textura.push_back(y);
				textureBounds.push_back(textura);
			}

			if (tipo == "vn") {
				vector<float> normal;
				float x, y, z;
				file >> x >> y >> z;
				normal.push_back(x);
				normal.push_back(y);
				normal.push_back(z);
				normals.push_back(normal);
			}

			if (tipo == "f") {
				vector<int> face;
				string x, y, z;
				file >> x >> y >> z;
				vector<vector<string>> data;
				data.push_back(helperSplit(x, '/'));
				data.push_back(helperSplit(y, '/'));
				data.push_back(helperSplit(z, '/'));
				for (auto d : data) {
					for (auto e : d) {
						if (e.empty()) e = "0";
						int val = stoi(e);
						if (val > 0) val--;
						face.push_back(val);
					}
				}
				faces.push_back(face);
			}
		}
	}

	file.close();

	for (int i = 0; i < faces.size(); i++) {
		vector<int> face = faces[i];

		// vertice 1
		vertices_vector.push_back(vertices_obj[face[0]][0]);
		vertices_vector.push_back(vertices_obj[face[0]][1]);
		vertices_vector.push_back(vertices_obj[face[0]][2]);

		vertices_vector.push_back(textureBounds[face[1]][0]);
		vertices_vector.push_back(textureBounds[face[1]][1]);

		vertices_vector.push_back(normals[face[2]][0]);
		vertices_vector.push_back(normals[face[2]][1]);
		vertices_vector.push_back(normals[face[2]][2]);

		// vertice 2
		vertices_vector.push_back(vertices_obj[face[3]][0]);
		vertices_vector.push_back(vertices_obj[face[3]][1]);
		vertices_vector.push_back(vertices_obj[face[3]][2]);

		vertices_vector.push_back(textureBounds[face[4]][0]);
		vertices_vector.push_back(textureBounds[face[4]][1]);

		vertices_vector.push_back(normals[face[5]][0]);
		vertices_vector.push_back(normals[face[5]][1]);
		vertices_vector.push_back(normals[face[5]][2]);

		// vertice 3
		vertices_vector.push_back(vertices_obj[face[6]][0]);
		vertices_vector.push_back(vertices_obj[face[6]][1]);
		vertices_vector.push_back(vertices_obj[face[6]][2]);

		vertices_vector.push_back(textureBounds[face[7]][0]);
		vertices_vector.push_back(textureBounds[face[7]][1]);

		vertices_vector.push_back(normals[face[8]][0]);
		vertices_vector.push_back(normals[face[8]][1]);
		vertices_vector.push_back(normals[face[8]][2]);
	}
	bufferArraySize = vertices_vector.size();
	vertices = &vertices_vector[0];

}

void dataLoad() {

	bufferArraySize = 32;
	vertices = new float[bufferArraySize] {
		0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
			-0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	};
}


BitMapFile* bmpDataRetrieval(string filename) {
	BitMapFile* bmp = new BitMapFile;
	unsigned int size, offset, headerSize;


	ifstream infile(filename.c_str(), ios::binary);


	infile.seekg(10);
	infile.read((char*)&offset, 4);

	
	infile.read((char*)&headerSize, 4);

	
	infile.seekg(18);
	infile.read((char*)&bmp->sizeX, 4);
	infile.read((char*)&bmp->sizeY, 4);

	
	size = bmp->sizeX * bmp->sizeY * 24;
	bmp->data = new unsigned char[size];

	
	infile.seekg(offset);
	infile.read((char*)bmp->data, size);

	
	int temp;
	for (int i = 0; i < size; i += 3) {
		temp = bmp->data[i];
		bmp->data[i] = bmp->data[i + 2];
		bmp->data[i + 2] = temp;
	}

	return bmp;
}

void loadExternalTextures(std::string nome_arquivo, int id_textura) {
	BitMapFile* image[1];

	image[0] = bmpDataRetrieval(nome_arquivo);

	glBindTexture(GL_TEXTURE_2D, texturas[id_textura]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[0]->sizeX, image[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image[0]->data);
}

char* readStringFromFile(char* fn) {
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

void setShaders() {
	char* vs = NULL, * fs = NULL, * fs2 = NULL;

	glewInit();

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);

	char vertex_shader[] = "simples.vert";
	char fragment_shader[] = "simples.frag";
	vs = readStringFromFile(vertex_shader);
	fs = readStringFromFile(fragment_shader);

	const char* vv = vs;
	const char* ff = fs;

	glShaderSource(v, 1, &vv, NULL);
	glShaderSource(f, 1, &ff, NULL);

	free(vs); free(fs);

	glCompileShader(v);
	glCompileShader(f);

	int result;
	glGetShaderiv(v, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int size;
		glGetShaderiv(v, GL_INFO_LOG_LENGTH, &size);
		char* msg = new char[size];
		glGetShaderInfoLog(v, size, &size, msg);
		std::cout << msg << std::endl;
	}

	glGetShaderiv(f, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int size;
		glGetShaderiv(f, GL_INFO_LOG_LENGTH, &size);
		char* msg = new char[size];
		glGetShaderInfoLog(f, size, &size, msg);
		std::cout << msg << std::endl;
	}

	p = glCreateProgram();
	glAttachShader(p, v);
	glAttachShader(p, f);

	glLinkProgram(p);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, bufferArraySize * sizeof(float), vertices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glUseProgram(p);

}

void display(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	glPushMatrix();

	int id_pos_x = glGetUniformLocation(p, "pos_x");
	glUniform1f(id_pos_x, pos_x);
	int id_pos_y = glGetUniformLocation(p, "pos_y");
	glUniform1f(id_pos_y, pos_y);
	int id_pos_z = glGetUniformLocation(p, "pos_z");
	glUniform1f(id_pos_z, pos_z);
	int id_angulo_x = glGetUniformLocation(p, "angulo_x");
	glUniform1f(id_angulo_x, angle_x);
	int id_angulo_y = glGetUniformLocation(p, "angulo_y");
	glUniform1f(id_angulo_y, angle_y);
	int id_angulo_z = glGetUniformLocation(p, "angulo_z");
	glUniform1f(id_angulo_z, angle_z);
	int id_scale = glGetUniformLocation(p, "scale");
	glUniform1f(id_scale, scale);
	int id_int_luz_amb = glGetUniformLocation(p, "luz_int_amb");
	glUniform1f(id_int_luz_amb, ambientLight);
	int id_int_luz_dif = glGetUniformLocation(p, "luz_int_dif");
	glUniform1f(id_int_luz_dif, difuseLight);
	int id_cor_luz = glGetUniformLocation(p, "cor_luz");
	glUniform3f(id_cor_luz, colorLight[0], colorLight[1], colorLight[2]);
	int id_pos_luz = glGetUniformLocation(p, "pos_luz");
	glUniform3f(id_pos_luz, lightPos[0], lightPos[1], lightPos[2]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texturas[0]);
	int sp_texture = glGetUniformLocation(p, "textura");
	glUniform1i(sp_texture, 0);

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, texturas[1]);
	int sp_texture_normal = glGetUniformLocation(p, "textura_normal");
	glUniform1i(sp_texture_normal, 1);


	glBindTexture(GL_TEXTURE_2D, texturas[1]);
	glBindVertexArray(VAO);
	// obj normal vai ser gl_triangle e vai de 0 at� a quantidade de vertices
	if (example) {
		glDrawArrays(GL_QUADS, 0, 4);
	}
	else {
		glDrawArrays(GL_TRIANGLES, 0, faces.size() * 3);
	}

	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	glutPostRedisplay();
}

void key(unsigned char c, int x, int y) {
	if (c == 'z')
		ambientLight -= 0.05;
	else if (c == 'x')
		ambientLight += 0.05;

	if (c == 'c')
		lightPos[0] -= 0.05;
	else if (c == 'v')
		lightPos[0] += 0.05;

	if (c == 'b')
		lightPos[1] += 0.05;
	else if (c == 'n')
		lightPos[1] -= 0.05;

	if (c == 'a')
		pos_z -= 0.05;
	else if (c == 's')
		pos_z += 0.05;

	if (c == 'd')
		pos_y -= 0.05;
	else if (c == 'f')
		pos_y += 0.05;

	if (c == 'g')
		pos_x += 0.05;
	else if (c == 'h')
		pos_x -= 0.05;

	if (c == 55) {//7
		angle_x += ang_passo;

	}if (c == 57) {//9
		angle_x -= ang_passo;
	}
	if (c == 52) {//4
		angle_y += ang_passo;
	}
	if (c == 54) {//6
		angle_y -= ang_passo;
	}
	if (c == 49) {//1
		angle_z += ang_passo;
	}
	if (c == 51) {//3
		angle_z -= ang_passo;
	}
	if (c == 53) {//5
		angle_x += rand() % 30 - 15;
		angle_y += rand() % 30 - 15;
		angle_z += rand() % 20 - 10;
	}
	if (c == 32) {//space
		scale += 0.1;
	}
	if (c == 127) {//del
		scale -= 0.1;
	}
}



void reshape(int width, int height) {
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (float)640 / (float)480, 0.1f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void setupRC() {
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLfloat diffuseLight[] = { 0.7, 0.7, 0.7, 1.0 };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glEnable(GL_LIGHT0);

	GLfloat ambientLight[] = { 0.05, 0.05, 0.05, 1.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);

	glEnable(GL_LIGHTING);
}

void init() {
	glClearColor(0, 0, 0, 1);
	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	setupRC();

	glGenTextures(2, texturas);
	loadExternalTextures("Textures/brickwall.bmp", 0);
	loadExternalTextures("Textures/brickwall_normal.bmp", 1);
	glEnable(GL_TEXTURE_2D);

	setShaders();
}

std::vector<std::string> helperSplit(std::string string, char splitChar) {
	auto strings = std::vector<std::string>();
	std::string currentString = "";
	for (char currentChar : string) {
		if (currentChar == splitChar) {
			strings.push_back(currentString);
			currentString = "";
			continue;
		}
		currentString += currentChar;
	}
	if (!currentString.empty()) {
		strings.push_back(currentString);
	}
	return strings;
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(840, 680);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Computacao Grafica M2.3");

	if (example) {
		dataLoad();
	}
	else {
		objLoad("objs/cubo.obj");
	}

	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(key);
	

	glutMainLoop();

	return 0;
}