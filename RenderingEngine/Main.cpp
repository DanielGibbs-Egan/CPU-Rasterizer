
#include "Main.h"


Color3::Color3() {
	R = G = B = 0;
}
Color3::Color3(int value) {
	R = G = B = value;
}
Color3::Color3(int value, float opacity) {
	this->opacity = opacity;
	R = G = B = value;
}
Color3::Color3(int R, int G, int B) {
	this->R = R;
	this->G = G;
	this->B = B;
}
Color3 Color3::operator * (float mult) {
	Color3 color = Color3((int)(R * mult), (int)(G * mult), (int)(B * mult));
	color.opacity = this->opacity;
	return color;
}

BitMap::BitMap() {}


void RenderPane::setLPBit(int x, int y, Color3 color, float z) {

	if (x < 0 || y < 0 || x >= width || y >= height) {
		return;
	}

	int pxID = (y * width + x);

	if (lpDistance[pxID] >= z || lpDistance[pxID] == 0)
	{
		lpDistance[pxID] = (float)z;

		pxID *= 4;

		lpBits[pxID + 0] = color.B;//lerp(lpBits[pxID + 0], color.B, 1 - color.opacity);
		lpBits[pxID + 1] = color.G;//lerp(lpBits[pxID + 1], color.G, 1 - color.opacity);
		lpBits[pxID + 2] = color.R;//lerp(lpBits[pxID + 2], color.R, 1 - color.opacity);
	}
}

void RenderPane::drawBitmap(BitMap bitmap)
{
	for (unsigned int x = 0; x < bitmap.width; x++)
		for (unsigned int y = 0; y < bitmap.height; y++)
		{
			unsigned int location = bitmap.bpp * (y * bitmap.width + x) / 8;
			uint8_t B = bitmap.bytes[location + 1];
			uint8_t G = bitmap.bytes[location + 2];
			uint8_t R = bitmap.bytes[location + 3];

			setLPBit(x, y, Color3(R, G, B), 1);
		}
}

void RenderPane::clear() {

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {

			int pxID = (y * width + x);
			lpDistance[pxID] = 0;

			pxID *= 4;

			lpBits[pxID + 0] = 0;
			lpBits[pxID + 1] = 0;
			lpBits[pxID + 2] = 0;

		}
	}
}

RenderPane::RenderPane(int width, int height)
{
	this->width  =  width;
	this->height = height;
	lpBits = new BYTE[4 * width * height]();
	lpDistance = new float[width * height]();
}


Face::Face() {
	info = new int[9] {0};
}
Mesh::Mesh() {}
Mesh::Mesh(size_t vSize, size_t nSize, size_t tCSize, size_t fSize)
{
	vertices.resize(vSize);
	normals.resize(nSize);
	textureCoords.resize(tCSize);

	faces.resize(fSize);
}

DWORD WINAPI threadLoop( LPVOID lpParam ) {
	while (true) {
		if (hwndGlobal != nullptr) {
			InvalidateRect(hwndGlobal, nullptr, false);
		}
		Sleep(20);
	}
	return 0;
}

BitMap readBMP(std::string fileDirectory)
{
	std::ifstream file;

	file.open(fileDirectory, std::ios::binary);

	char* bytes = new char[4];

	BitMap bmp = BitMap();

	if (file.is_open()) {
		
		file.read(bytes, 2);

		bool BMheader = bytes[0] == 'B' && bytes[1] == 'M';

		if (BMheader) 
		{
			file.read(bytes, 4);

			uint32_t total_size = bytes[0] | bytes[1] << 8 | bytes[2] << 16| bytes[3] << 24;

			file.ignore(4); // skip these bytes

			file.read(bytes, 4);

			uint32_t data_offset = bytes[0] | bytes[1] << 8 | bytes[2] << 16 | bytes[3] << 24;

			file.read(bytes, 4);

			uint32_t info_size = bytes[0] | bytes[1] << 8 | bytes[2] << 16 | bytes[3] << 24;

			file.read(bytes, 4);
			bmp.width = bytes[0] | bytes[1] << 8 | bytes[2] << 16 | bytes[3] << 24;

			file.read(bytes, 4);
			bmp.height = bytes[0] | bytes[1] << 8 | bytes[2] << 16 | bytes[3] << 24;

			file.ignore(2); // skip these bytes

			file.read(bytes, 2);

			bmp.bpp = bytes[0] | bytes[1] << 8;

			file.ignore(data_offset - 30 - 1); // skip to the start of the image data

			bmp.byte_count = bmp.width * bmp.height * bmp.bpp / 8;

			char* pixel_data = new char[bmp.byte_count];

			bmp.bytes = new uint8_t[bmp.byte_count];

			file.read(pixel_data, bmp.byte_count);

			for (unsigned int x = 0; x < bmp.width; x++)
				for (unsigned int y = 0; y < bmp.height; y++)
				{
					unsigned int position = (y * bmp.width + x) * bmp.bpp / 8;
					unsigned int inverted = ((bmp.height - y) * bmp.width + x) * bmp.bpp / 8;

					for (int i = 0; i < (int)(bmp.bpp * 0.125); i++)
					{
						bmp.bytes[position + i] = (uint8_t)pixel_data[position + i];
					}
				}

		}
	}
	else 
	{
		std::cout << "failure";
	}
	return bmp;
}

std::string* spaceSeperatedValues(std::string str, int initialIndex)
{
	std::string* stringArray = new std::string[3]{};
	int spaces = 0;

	for (int i = initialIndex; i < str.length(); i++)
	{
		if (str[i] == ' ')
		{
			spaces++;
			continue;
		}
		else
		{
			stringArray[spaces] += str[i];
		}
	}

	return stringArray;
}

Mesh readOBJ(std::string file_path)
{

	std::ifstream file(file_path);
	std::string str;

	std::getline(file, str);

	std::stack<Vector3D>  vertices;
	std::stack<Vector3D>  normals;
	std::stack<Vector3D>  textureCoords;
	std::stack<Face> faces;

	while (getline(file, str))
	{
		if (str[0] == 'v') {
			if (str[1] == ' ') 
			{
				std::string* stringArray = spaceSeperatedValues(str, 2);
				Vector3D v = Vector3D(
					stod(stringArray[0]),
					stod(stringArray[1]),
					stod(stringArray[2])
				);
				vertices.push(v);
			} 
			else if(str[1] == 'n') 
			{
				std::string* stringArray = spaceSeperatedValues(str, 3);
				Vector3D vn = Vector3D(
					stod(stringArray[0]),
					stod(stringArray[1]),
					stod(stringArray[2])
				);
				normals.push(vn);
			}
			else if (str[1] == 't')
			{
				std::string* stringArray = spaceSeperatedValues(str, 3);
				Vector3D vt = Vector3D(
					stod(stringArray[0]),
					stod(stringArray[1]),
					0
				);
				textureCoords.push(vt);
			}
		}
		else if (str[0] == 'f') 
		{
			std::string* stringArray = spaceSeperatedValues(str, 2);
			Face intArray = Face();
			for (int i = 0; i < 3; i++) 
			{
				std::string stringArray2[3] = {};
				int slashes = 0;

				for (int j = 0; j < stringArray[i].length(); j++)
				{
					if (stringArray[i][j] == '/')
					{
						slashes++;
						continue;
					}
					else
					{
						stringArray2[slashes] += stringArray[i][j];
					}
				}

				intArray.info[i] = stoi(stringArray2[0]) - 1;
				intArray.info[3 + i] = stoi(stringArray2[1]) - 1;
				intArray.info[6 + i] = stoi(stringArray2[2]) - 1;

			}
			faces.push(intArray);


		}
	}


	Mesh meshData = Mesh(vertices.size(), normals.size(), textureCoords.size(), faces.size());

	while (!vertices.empty()) 
	{
		meshData.vertices[vertices.size() - 1] = vertices.top();
		vertices.pop();
	}

	while (!normals.empty())
	{
		meshData.normals[normals.size() - 1] = normals.top();
		normals.pop();
	}

	while (!textureCoords.empty())
	{
		meshData.textureCoords[textureCoords.size() - 1] = textureCoords.top();
		textureCoords.pop();
	}

	while (!faces.empty())
	{
		meshData.faces[faces.size() - 1].info = faces.top().info;
		faces.pop();
	}

	return meshData;

}

// static Camera *const CAMERA_PTR = new Camera();

int WINAPI main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	Mesh mesh = readOBJ("MonkeyWood.txt"); // Text.txt

	model.mesh = &mesh;

	mesh.bitmap = readBMP("WoodColor.bmp"); //monkeytexture.bmp

    DWORD id;
	CreateThread(
			NULL,
			0,
			threadLoop,
			nullptr,
			0,
			&id
	);

	const char* CLASS_NAME = "myWin32WindowClass";
	WNDCLASS wc{};
	wc.hInstance = hInstance;
	wc.lpszClassName = TEXT("myWin32WindowClass");
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpfnWndProc = WindowProcessMessages;
	RegisterClass(&wc);

	HWND hwnd = CreateWindowA(
			CLASS_NAME,
			"Render Engine",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			500,
			500,
			nullptr,
			nullptr,
			hInstance,
			nullptr
		);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	MSG msg{};

	while (GetMessage(&msg, nullptr, 0, 0)) {

		TranslateMessage(&msg);
		DispatchMessage(&msg);

	}

	return 0;
}

int lerp(int a, int b, double ratio) {
	return (int)(ratio * (a - b) + b);
}

static void sort3(double* array)
{
	if (array[0] > array[1])
		std::swap(array[0], array[1]);

	if (array[2] > array[1])
		return;

	std::swap(array[1], array[2]);

	if (array[0] < array[1])
		return;

	std::swap(array[0], array[1]);
}

static void getBounds(int* bounds, Triangle vertices)
{
	double X[3] = { vertices.a.x, vertices.b.x, vertices.c.x };
	double Y[3] = { vertices.a.y, vertices.b.y, vertices.c.y };

	sort3(X);
	sort3(Y);

	bounds[0] = max((int)floor(X[0]), 0);
	bounds[1] = max((int)floor(Y[0]), 0);
	bounds[2] = min((int)ceil(X[2]), width - 1);
	bounds[3] = min((int)ceil(Y[2]), height - 1);
}

static void drawFace(Triangle& vertices, int faceId, bool isBack)
{
	// the center of the triangle
	Vector3D mid = (vertices.a + vertices.b + vertices.c) / 3;

	// triangle edge vectors 
	Vector3D left = (vertices.b - vertices.a);
	Vector3D right = (vertices.c - vertices.a);

	double normalSun = ((1 - model.mesh->normals[model.mesh->faces[faceId].info[6]].dot(sunDirection)) * 0.5);
	double leftNormalSun = ((1 - model.mesh->normals[model.mesh->faces[faceId].info[7]].dot(sunDirection)) * 0.5) - normalSun;
	double rightNormalSun = ((1 - model.mesh->normals[model.mesh->faces[faceId].info[8]].dot(sunDirection)) * 0.5) - normalSun;

	// texture coordinates.
	Vector3D& textureOrigin = model.mesh->textureCoords[model.mesh->faces[faceId].info[3]];
	Vector3D textureLeft = model.mesh->textureCoords[model.mesh->faces[faceId].info[4]] - textureOrigin;
	Vector3D textureRight = model.mesh->textureCoords[model.mesh->faces[faceId].info[5]] - textureOrigin;

	double n_multiplier = 1.0 / (left.y * right.x - left.x * right.y);

	int spacing = model.mesh->bitmap.bpp / 8;
	int end = model.mesh->bitmap.byte_count;

	// find the screenspace bounds of the face.
	int bounds[4] = { 0,0,0,0 };
	getBounds(bounds, vertices);

	int mlt;
	double m, n;
	int step = 200;

	for (int x = bounds[0]; x <= bounds[2]; x)
	{
		mlt = (bounds[1] - 1) * width;

		for (int y = bounds[1]; y <= bounds[3]; y)
		{
			for (int dx = 0; dx < step && x + dx <= bounds[2]; dx++)
			{
				int mlt2 = mlt;
				for (int dy = 0; dy < step && y + dy <= bounds[3]; dy++)
				{
					mlt2 += width;

					m = ((x + dx - vertices.a.x) * left.y - (y + dy - vertices.a.y) * left.x) * n_multiplier;

					if (m < 0)
						continue;

					n = ((y + dy - vertices.a.y) * right.x - (x + dx - vertices.a.x) * right.y) * n_multiplier;

					if (n < 0 || m + n > 1)
						continue;

					{

						float dist = n * left.z + m * right.z + vertices.a.z;

						int pxID = (mlt2 + x + dx);

						float& distance = renderPane.lpDistance[pxID];

						if (distance < dist && distance != 0) continue;

						distance = dist;

						pxID *= 4;

						double sunShade = leftNormalSun * n + rightNormalSun * m + normalSun;

						Vector3D colorCoord = textureLeft * n + textureRight * m + textureOrigin;

						int colorIndex = ((int)(colorCoord.y * model.mesh->bitmap.height) * model.mesh->bitmap.width + (int)(colorCoord.x * model.mesh->bitmap.width)) * spacing;

						if (isBack)
						{
							renderPane.lpBits[pxID] = 0;
							renderPane.lpBits[pxID + 1] = 0;
							renderPane.lpBits[pxID + 2] = 255 * sunShade;
						}
						else if (colorIndex >= 0 && colorIndex + 3 < end)
						{
							renderPane.lpBits[pxID] = model.mesh->bitmap.bytes[colorIndex + 1] * sunShade;
							renderPane.lpBits[pxID + 1] = model.mesh->bitmap.bytes[colorIndex + 2] * sunShade;
							renderPane.lpBits[pxID + 2] = model.mesh->bitmap.bytes[colorIndex] * sunShade;
						}
					}
				}
			}
			mlt += width * step;
			y += step;
		}
		x += step;
	}

}

Vector3D calculateNormal(Vector3D v1, Vector3D v2, Vector3D v3)
{
	return (v2 - v1).cross(v3 - v1).unit();
}

static void drawObjectParallel(Camera& camera, std::vector<Vector3D>& projectedVertices, int start, int end)
{
	Triangle vertices = Triangle();

	for (int faceId = start; faceId < end; faceId++)
	{

		// Check if any point on face is visible to the camera
		// convert from base 1 index to base 0
		Vector3D normal = calculateNormal(
			model.mesh->vertices[model.mesh->faces[faceId].info[0]],
			model.mesh->vertices[model.mesh->faces[faceId].info[1]],
			model.mesh->vertices[model.mesh->faces[faceId].info[2]]
		);

		bool isBack = (normal.dot(camera.look) < 0);
	/*	if (normal.dot(camera.look) < 0)
			continue;*/

		// get the vertices of the face from the projected vertex vector array
		{
			vertices.a = projectedVertices[model.mesh->faces[faceId].info[0]];
			vertices.b = projectedVertices[model.mesh->faces[faceId].info[1]];
			vertices.c = projectedVertices[model.mesh->faces[faceId].info[2]];
		}

		drawFace(vertices, faceId, isBack);
	}

}

static void drawObject(Camera& camera, Model& model) 
{
	if (model.mesh->faces.empty()) return;

	std::vector<Vector3D> projectedVertices;
	projectedVertices.resize(model.mesh->vertices.size());

	for (int i = 0; i < projectedVertices.size(); i++)
	{
		projectedVertices[i] = midpoint + camera.getProjected(model.mesh->vertices[i]);
	}

	for (int i = 0; i < amtThreads; i++) {
		threads[i] = std::thread(drawObjectParallel, std::ref(camera), std::ref(projectedVertices), (i * model.mesh->faces.size()) / amtThreads, ((i + 1) * model.mesh->faces.size()) / amtThreads);
	}

	for (auto& t : threads) t.join();
}

static void drawObjects(int width, int height)
{
	Camera& camera = *CAMERA_PTR;

	camera.yawTheta = yaw * 2 * M_PI;
	camera.pitchTheta = pitch * 2 * M_PI;
	camera.update();

	camera.position = camera.look * (-cameraDistance);

	drawObject(camera, model);

}

LRESULT CALLBACK WindowProcessMessages(HWND hwnd, UINT msg, WPARAM param, LPARAM lparam) {

    HDC hdcMem;

	static HBITMAP hBitmap;

	int xPosNew, yPosNew;

	auto ms = (const MOUSEHOOKSTRUCT*)lparam;

	static BYTE* lpBits = new BYTE[width * 4 * height]{0};

	renderPane.lpBits = lpBits;

	switch (msg) {

	case WM_DESTROY:

		PostQuitMessage(0);
		break;

	case WM_CREATE:

		break;

	case WM_MOUSEMOVE:

		xPosNew = GET_X_LPARAM(lparam);
		yPosNew = GET_Y_LPARAM(lparam);

		if (leftMouseDown) 
		{
			deltaX = xPos - xPosNew;
			deltaY = yPos - yPosNew;
		}

		xPos = xPosNew;
		yPos = yPosNew;

		break;

	case WM_MOUSEWHEEL:
		
		cameraDistance -= GET_WHEEL_DELTA_WPARAM(param) / 100.0;

		break;

	case WM_LBUTTONDOWN:

		xPos = GET_X_LPARAM(lparam);
		yPos = GET_Y_LPARAM(lparam);

		leftMouseDown = true;

		break;

	case WM_LBUTTONUP:

		leftMouseDown = false;

		break;

	case WM_KEYDOWN:

		if (param == 'A' && !keyDown[0])
		{
			keyDown[0] = true;
			yawDelta = !keyDown[1]? 10: 0;
		}
		else if (param == 'D' && !keyDown[1])
		{
			keyDown[1] = true;
			yawDelta = !keyDown[0] ? -10 : 0;
		}
		else if (param == 'W' && !keyDown[2])
		{
			keyDown[2] = true;
			pitchDelta = !keyDown[3] ? -10 : 0;
		}
		else if (param == 'S' && !keyDown[3])
		{ 
			keyDown[3] = true;
			pitchDelta = !keyDown[2] ? 10 : 0;
		}
		break;

	case WM_KEYUP:

		if (param == 'A')
		{
			keyDown[0] = false;
			yawDelta = !keyDown[1] ? 0 : -10;
		}
		else if (param == 'D')
		{
			keyDown[1] = false;
			yawDelta = !keyDown[0] ? 0 : 10;
		}
		else if (param == 'W')
		{
			keyDown[2] = false;
			pitchDelta = !keyDown[3] ? 0 : 10;
		}
		else if (param == 'S')
		{
			keyDown[3] = false;
			pitchDelta = !keyDown[2] ? 0 : -10;
		}

		break;

	case WM_PAINT:

		PAINTSTRUCT     ps;
		HDC             hdc;

		RECT rect;

		hdc = BeginPaint(hwnd, &ps);

		yaw = ((int)(yaw * 1000 + deltaX + yawDelta) % 1000) / 1000.0;
		pitch = ((int)(pitch * 1000 - deltaY + pitchDelta) % 1000) / 1000.0;

		drawObjects(width, height);

		deltaX = 0;
		deltaY = 0;

		if (!hBitmap) {
			hBitmap = CreateCompatibleBitmap(hdc, width, height);
		}
		SetBitmapBits(hBitmap, width * 4 * height, lpBits);

		GetClientRect(hwnd, &rect);

		hdcMem = CreateCompatibleDC(hdc);
		SelectObject(hdcMem, hBitmap);
        BitBlt(hdc, 0, 0, rect.right-rect.left, rect.bottom-rect.top, hdcMem, 0, 0, SRCCOPY);
        DeleteDC(hdcMem);
		EndPaint(hwnd, &ps);

		renderPane.clear();
		hwndGlobal = hwnd;

		break;

	default:
		return DefWindowProc(hwnd, msg, param, lparam);
	}

	return 0;
}
