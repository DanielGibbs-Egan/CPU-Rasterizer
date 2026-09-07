#pragma once

#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))

const long double M_PI = 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679;

#include <iostream>
#include <fstream>
#include <stack>
#include <thread>

#include <windows.h>

#include "Camera.h"
#include "Vector3D.h"
#include "Vector2D.h"

#ifndef MAIN_H_   /* Include guard */
#define MAIN_H_

HBITMAP hBitmap = NULL;

LRESULT CALLBACK WindowProcessMessages(HWND hwnd, UINT msg, WPARAM param, LPARAM lparam);

HWND hwndGlobal = nullptr;

struct Color3 
{
	// red, green, blue, alpha
	char R, G, B, A = 255; // default to opaque white

	Color3(); // default
	Color3(char value); // brightness
	Color3(char value, float alpha); // brightness, opacity
	Color3(char R, char G, char B); // RGB color
	Color3 operator * (float mult); // adjust color brightness
};

struct BitMap 
{
	// image dimensions
	uint32_t width = 0, height = 0;
	// color accuracy
	uint16_t bpp = 0;
	// image data and size
	int byte_count = 0;
	uint8_t* bytes = nullptr;

	BitMap(); // default
	BitMap(std::string fileDirectory); // import a bitmap file
	~BitMap(); // free memory

	BitMap& operator=(const BitMap& rhs); // copy bitmap
};

struct RenderPane 
{
	// dimensions
	int width, height;
	// render data
	// distance
	float* lpDistance;
	// color
	BYTE* lpBits;

	// assign distance and color data to a pixel in the render pane
	void setLPBit(int x, int y, float z, Color3 color);

	// draw a bitmap object to the screen
	void drawBitmap(BitMap& bitmap);

	// reset distance and color data
	void clear();

	// create a new render pane with given dimensions
	RenderPane(int width, int height);

	// free memory
	~RenderPane();
};

struct Face 
{
	// store face data
	int info[9];
};

struct Triangle
{
	// store references to each vertex
	Vector3D& a, & b, & c;
	// initialize references
	Triangle(Vector3D& u, Vector3D& v, Vector3D& w) : a(u), b(v), c(w) {}
};

struct Mesh
{
	// store dynamic array sizes 
	// (vertices, normals, texture coordinates, faces)
	size_t sizes[4] = {0,0,0,0};

	// store dynamic arrays
	Vector3D* vertices = nullptr; // size: 3
	Vector3D* normals = nullptr; // size: 3
	Vector2D* textureCoords = nullptr; // size: 2
	Face* faces = nullptr; // size: 9

	Mesh(); // default
	Mesh(std::string file_path); // import mesh file
	Mesh(const Mesh& orig); // copy mesh
	~Mesh(); // free memory

	Mesh& operator= (const Mesh& orig); // copy mesh

};

struct Model
{
	// store shape and texture information
	Mesh* mesh = nullptr;
	BitMap* bitmap = nullptr;

	// store vertices as they are transformed
	std::vector<Vector3D> projectedVertices;

	// store transformation: unimplemented
	Vector3D position;
	Vector3D scale;
	Vector3D rot;
};

DWORD WINAPI threadLoop(LPVOID lpParam);

std::string* numberExtract(std::string str, int initialIndex, int nElements);

Model model;

// output dimensions
const int width = 1920, height = 1080;

// create output
RenderPane renderPane = RenderPane(width, height);

// middle of output with zero depth
static Vector3D midpoint = Vector3D(width / 2, height / 2, 0);

// direction the scene lighting is pointing
static Vector3D sunDirection = Vector3D(0, -1, 0).unit();

// viewpoint for scene
static Camera* const CAMERA_PTR = new Camera();

/* camera information */
float
cameraDistance = 5,
yaw = 0,
pitch = 0;
int
yawDelta = 0,
pitchDelta = 0;

/* user input information */

// mouse
int
xPos = -1,
yPos = -1,
deltaX = 0,
deltaY = 0;
bool leftMouseDown = false;

// keyboard
bool keyDown[4] = { false };

// Change to WinMain, and adjust Linker > System > SubSystem to /SUBSYSTEM:WINDOWS when done debugging
int WINAPI main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

// draw an object to the screen
static void drawObject(Camera& camera, Model& model);
// run in parallel to draw a set of faces to the render pane
static void drawObjectParallel(Camera& camera, Vector3D* projectedVertices, int start, int end);

// draw a face to the screen
static void drawFace(Triangle& vertices, int faceId);
// find the rectangle bounding a triangle
static void getBounds(int* output, Triangle& vertices);
// sort an array of size three
static void sort3(double* array);

// set number of threads used
const int amtThreads = 4;
// instantiate thread array
std::thread threads[amtThreads];

static void renderScene(int width, int height);

#endif