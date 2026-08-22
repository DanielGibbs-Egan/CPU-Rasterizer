#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))

#include <iostream>
#include <fstream>
#include <stack>
#include <mutex>
#include <chrono>
#include <condition_variable>

#define _USE_MATH_DEFINES
//#include "SDL2/SDL.h"
#include <windows.h>
#include <cmath>
#include <winuser.h>
#include <wingdi.h>
#include <windef.h>
#include <chrono>
#include <thread>
#include <algorithm>
#include <stdio.h>

#include "Camera.cpp"

HBITMAP hBitmap = NULL;

LRESULT CALLBACK WindowProcessMessages(HWND hwnd, UINT msg, WPARAM param, LPARAM lparam);

HWND hwndGlobal = nullptr;

class Color3 {
public:
	float opacity = 1;
	int R, G, B;

	Color3();
	Color3(int value);
	Color3(int value, float opacity);
	Color3(int R, int G, int B);
	Color3 operator * (float mult);
};

class BitMap {
public:
	uint32_t width = 0;
	uint32_t height = 0;
	uint16_t bpp = 0;
	uint8_t* bytes = new uint8_t[0];

	int byte_count = 0;

	BitMap();
};

class RenderPane {
public:
	int width, height;
	float* lpDistance;
	BYTE* lpBits;

	void setLPBit(int x, int y, Color3 color, float z);

	void drawBitmap(BitMap bitmap);

	void clear();

	RenderPane(int width, int height);
};

class Face {
public:
	int* info;

	Face();
};

class Mesh
{
public:
	std::vector<Vector3D> vertices;
	std::vector<Vector3D> normals;
	std::vector<Vector3D> textureCoords;
	std::vector<Face> faces;

	BitMap bitmap;
	Mesh();
	Mesh(size_t vSize, size_t nSize, size_t tCSize, size_t fSize);

};

class Model
{
public:
	Mesh* mesh = nullptr;
	std::vector<Vector3D> projectedVertices;

	Vector3D position;
	Vector3D scale;
	Vector3D rot;
};

DWORD WINAPI threadLoop(LPVOID lpParam);

BitMap readBMP(std::string fileDirectory);

std::string* spaceSeperatedValues(std::string str, int initialIndex);

Mesh readOBJ(std::string file_path);

Model model;

static Vector3D sunDirection = Vector3D(0, -1, 0).unit();

const int width = 1920, height = 1080;

static Vector3D midpoint = Vector3D(width / 2, height / 2, 0);

static Camera* const CAMERA_PTR = new Camera();

struct Triangle
{
	Vector3D a, b, c;
};

float
cameraDistance = 15,
yaw = 0,
pitch = 0;
int
yawDelta = 0,
pitchDelta = 0;

int
xPos = -1,
yPos = -1,
deltaX = 0,
deltaY = 0;

bool keyDown[4] = { false };
bool leftMouseDown = false;

int WINAPI main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

int lerp(int a, int b, double ratio);

RenderPane renderPane = RenderPane(width, height);

static void sort3(double* array);

static void getBounds(int* bounds, Triangle vertices);

static void drawFace(Triangle& vertices, int faceId);

const int amtThreads = 4;
std::thread threads[amtThreads];

static void drawObjectParallel(Camera& camera, std::vector<Vector3D>& projectedVertices, int start, int end);

static void drawObject(Camera& camera, Model& model);

double M_PI = 3.1415926535;

static void drawObjects(int width, int height);