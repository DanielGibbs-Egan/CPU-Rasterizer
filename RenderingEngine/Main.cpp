

#include "Main.h"
#include <chrono>

/* RGB Color */

Color3::Color3() 
{
	// default the RGB value to black
	R = G = B = 0;
	// set the alpha value to max (opaque)
	this->A = 255;
}

Color3::Color3(char value) {
	// assign the given value to RGB
	R = G = B = value;
	// set the alpha value to max (opaque)
	this->A = 255;
}

Color3::Color3(char value, float alpha) {
	// assign the given value to RGB and alpha to A
	R = G = B = value;
	this->A = alpha;
}

Color3::Color3(char R, char G, char B) {
	// assign the given RGB values
	this->R = R;
	this->G = G;
	this->B = B;
	// set the alpha value to max (opaque)
	this->A = 255;
}

Color3 Color3::operator * (float mult) {
	// create a copy of the current color object scaling along each axis by mult
	Color3 color = Color3((int)(R * mult), (int)(G * mult), (int)(B * mult));
	// copy the original colors alpha
	color.A = this->A;
	// return the new color object
	return color;
}

/* BitMap */

BitMap::BitMap() {}

BitMap::BitMap(std::string fileDirectory)
{
	// instantiate a file object
	std::ifstream file;

	// attempt to access the bmp file
	file.open(fileDirectory, std::ios::binary);

	// if the file was not opened
	if (!file.is_open())
	{
		// print a failure message and return an empty bitmap
		std::cout << "failure";
		return;
	}

	// create an array to store read bytes
	char* read_bytes = new char[4];

	// read the first two bytes and check the file type
	file.read(read_bytes, 2);
	bool BMheader = read_bytes[0] == 'B' && read_bytes[1] == 'M';

	// if the file is not a bitmap
	if (!BMheader)
	{
		// free memory
		delete[] read_bytes;
		// print a failure message and return an empty bitmap
		std::cout << "file is not a bitmap";
		return;
	}

	// read the total size of the bitmap file
	file.read(read_bytes, 4);
	uint32_t total_size = read_bytes[0] | read_bytes[1] << 8 | read_bytes[2] << 16 | read_bytes[3] << 24;

	// skip: file creation information 
	file.ignore(4);

	// find the location of the bitmap data
	file.read(read_bytes, 4);
	uint32_t data_offset = read_bytes[0] | read_bytes[1] << 8 | read_bytes[2] << 16 | read_bytes[3] << 24;


	// read the header size
	file.read(read_bytes, 4);
	uint32_t header_size = read_bytes[0] | read_bytes[1] << 8 | read_bytes[2] << 16 | read_bytes[3] << 24;

	// if the file's header is not BITMAPINFOHEADER
	if (header_size != 40)
	{
		// free memory
		delete[] read_bytes;
		// print a failure message and return an empty bitmap
		std::cout << "file's header BITMAPINFOHEADER";
		return;
	}

	// read the file width
	file.read(read_bytes, 4);
	width = read_bytes[0] | read_bytes[1] << 8 | read_bytes[2] << 16 | read_bytes[3] << 24;

	// read the file height
	file.read(read_bytes, 4);
	height = read_bytes[0] | read_bytes[1] << 8 | read_bytes[2] << 16 | read_bytes[3] << 24;

	// skip: number of color planes
	file.ignore(2);

	// read the number of bits per pixel
	file.read(read_bytes, 2);
	bpp = read_bytes[0] | read_bytes[1] << 8;

	// read the compression method
	file.read(read_bytes, 4);
	uint32_t compression_method = read_bytes[0] | read_bytes[1] << 8 | read_bytes[2] << 16 | read_bytes[3] << 24;;

	// if the file has a different compression method
	if (compression_method != 0)
	{
		// free memory
		delete[] read_bytes;
		// print a failure message and return an empty bitmap
		std::cout << "file is not: uncompressed rgb bitmap";
		return;
	}

	// 34 bytes in

	// skip to the start of the image data frpm 
	file.ignore(data_offset - 34);

	byte_count = width * height * bpp / 8;

	char* pixel_data = new char[byte_count];

	bytes = new uint8_t[byte_count];

	file.read(pixel_data, byte_count);

	memcpy(bytes, pixel_data, byte_count);

	// free memory
	delete[] pixel_data;
	delete[] read_bytes;

	// end
	return;
}

BitMap::~BitMap() 
{
	// free memory
	delete [] bytes;
}

BitMap& BitMap::operator=(const BitMap& rhs)
{
	// copy data
	width = rhs.width;
	height = rhs.height;
	bpp = rhs.bpp;
	byte_count = rhs.byte_count;

	// reserve memory for data
	bytes = new uint8_t[byte_count];

	// copy data from original
	memcpy(bytes, rhs.bytes, byte_count);

	// return the bitmap
	return *this;
}

/* RenderPane */

// check bounds and sets the pixel value 
void RenderPane::setLPBit(int x, int y, float z, Color3 color) {

	// check if the pixel is contained in the pane
	if (x < 0 || y < 0 || x >= width || y >= height)
		return;

	// calculate the pixel ID
	int pxID = (y * width + x);

	// check if the pixel is infront of the current pixel or it doesn't exist
	if (lpDistance[pxID] >= z || lpDistance[pxID] == 0)
	{
		// set the distance value
		lpDistance[pxID] = (float)z;

		// find the color array index
		pxID *= 4;

		// set the color value
		lpBits[pxID + 0] = color.B;
		lpBits[pxID + 1] = color.G;
		lpBits[pxID + 2] = color.R;
	}
}

// draw the pixels from a bitmap onto the renderpane
void RenderPane::drawBitmap(BitMap& bitmap)
{
	// loop through each pixel
	for (int x = 0; x < bitmap.width; x++)
		for (int y = 0; y < bitmap.height; y++)
		{
			// find the location of the pixel in the array
			int location = bitmap.bpp * (y * bitmap.width + x) / 8;

			// colect the color values
			uint8_t B = bitmap.bytes[location    ];
			uint8_t G = bitmap.bytes[location + 1];
			uint8_t R = bitmap.bytes[location + 2];

			setLPBit(x, bitmap.height - 1 - y, 1, Color3(R, G, B));
		}
}

// sets the values at each pixel to zero
void RenderPane::clear() 
{
	// reset each element in the color and distance arrays
	std::memset(lpBits, 0, width * height * 4);
	std::memset(lpDistance, 0, width * height * sizeof(float));
}

// create a render pane object
RenderPane::RenderPane(int width, int height)
{
	// update the width and height
	this->width  =  width;
	this->height = height;

	// reserve memory to store the color and distance for each pixel
	lpBits = new BYTE[4 * width * height]();
	lpDistance = new float[width * height]();
}

RenderPane::~RenderPane()
{
	// free memory
	delete[] lpBits;
	delete[] lpDistance;
}

/* Face */

Vector3D calculateNormal(Vector3D& v1, Vector3D& v2, Vector3D& v3)
{
	// the unit surface normal is the unit of the cross product of the edge vectors
	return (v2 - v1).cross(v3 - v1).unit();
}

/* Mesh */

Mesh::Mesh() {} // default defined inside class definition

Mesh::Mesh(std::string file_path)
{
	// attempt to open the .obj file
	std::ifstream file(file_path);

	// return empty mesh if file doesn't exist
	if (!file.is_open())
		return;

	// store hold the current line
	std::string line;
	// store extracted information
	std::string* information = nullptr;
	// store the numerical values of extracted information
	void* tmp;
	Face face;
	Vector3D v3;
	Vector2D v2;

	// create stacks to hold the mesh information
	std::stack<Vector3D> vertices;
	std::stack<Vector3D> normals;
	std::stack<Vector2D> textureCoords;
	std::stack<Face>    faces;

	// read the each line
	while (std::getline(file, line))
	{
		// read the first character to determine the type
		switch (line[0])
		{
		case 'v':
			// read the second character to determine the subtype
			switch (line[1])
			{
			case ' ': // vertex

				// extract the data starting at the index 2 and collect 3 number strings
				information = numberExtract(line, 2, 3);

				// convert data and free memory
				v3 = Vector3D(stod(information[0]), stod(information[1]), stod(information[2]));
				delete[] information;

				// add the vertex to the list of mesh vertices
				vertices.push(v3);

				break;
			case 'n': // vertex normal

				// extract the data starting at the index 3 and collect 3 number strings
				information = numberExtract(line, 3, 3);

				// convert data and free memory
				v3 = Vector3D(stod(information[0]), stod(information[1]), stod(information[2]));
				delete[] information;

				// add the normal to the list of mesh normals
				normals.push(v3);

				break;
			case 't': // vertex texture coordinates

				// extract the data starting at the index 3 and collect 2 number strings
				information = numberExtract(line, 3, 2);

				// convert data and free memory
				v2 = Vector2D(stod(information[0]), stod(information[1]));
				//tmp = new double[2] {stod(information[0]), stod(information[1])};

				// add the coordinate to the list of mesh texture coordinates
				textureCoords.push(v2);

				delete[] information;
				information = nullptr;
				/*delete[] tmp;
				tmp = nullptr;*/

				break;
			}
			break;
		case 'f': // face

			// extract the data starting at the index 3 and collect 9 number strings
			information = numberExtract(line, 2, 9);

			// create a new face to store data
			face = Face();
			// convert data
			tmp = new int[9] {
				stoi(information[0]) - 1, stoi(information[3]) - 1, stoi(information[6]) - 1,
					stoi(information[1]) - 1, stoi(information[4]) - 1, stoi(information[7]) - 1,
					stoi(information[2]) - 1, stoi(information[5]) - 1, stoi(information[8]) - 1
				};
			//copy data into the new face
			memcpy(face.info, tmp, sizeof(face.info));

			// free memory
			delete[] information;
			delete[] tmp;
			information = nullptr;
			tmp = nullptr;

			// add the face to the list of mesh faces
			faces.push(face);

			break;
		}
	}

	// create a new mesh with space for all of the mesh data collected from the obj
	sizes[0] = vertices.size();
	sizes[1] = normals.size();
	sizes[2] = textureCoords.size();
	sizes[3] = faces.size();

	// create arrays to hold mesh data 
	this->vertices = new Vector3D[sizes[0]];
	this->normals = new Vector3D[sizes[1]];
	this->textureCoords = new Vector2D[sizes[2]];
	this->faces = new Face[sizes[3]];

	// copy the vertex data from the stacks into the mesh
	while (!vertices.empty())
	{
		this->vertices[vertices.size() - 1] = vertices.top();
		vertices.pop();
	}

	// copy the normal data from the stacks into the mesh
	while (!normals.empty())
	{
		this->normals[normals.size() - 1] = normals.top();
		normals.pop();
	}

	// copy the texture coordinate data from the stacks into the mesh
	while (!textureCoords.empty())
	{
		this->textureCoords[textureCoords.size() - 1] = textureCoords.top();
		textureCoords.pop();
	}

	// copy the face data from the stacks into the mesh
	while (!faces.empty())
	{
		this->faces[faces.size() - 1] = faces.top();
		faces.pop();
	}

	// return the mesh
	return;

}

Mesh::Mesh(const Mesh& orig)
{
	// copy original size data to the new mesh
	memcpy(sizes, orig.sizes, sizeof(orig.sizes));

	// create arrays to hold the mesh data using copied size data
	vertices = new Vector3D[sizes[0]];
	normals = new Vector3D[sizes[1]];
	textureCoords = new Vector2D[sizes[2]];
	faces = new Face[sizes[3]];

	// copy original mesh data to the new mesh
	memcpy(vertices, orig.vertices, sizeof(Vector3D) * sizes[0]);
	memcpy(normals, orig.normals, sizeof(Vector3D) * sizes[1]);
	memcpy(textureCoords, orig.textureCoords, sizeof(Vector2D) * sizes[2]);
	memcpy(faces, orig.faces, sizeof(Face) * sizes[3]);
}

Mesh& Mesh::operator = (const Mesh& orig)
{
	// if self assign don't change anything
	if (this == &orig) return *this;

	// free old data
	delete[] vertices;
	delete[] normals;
	delete[] textureCoords;
	delete[] faces;

	// copy original size data to the new mesh
	memcpy(sizes, orig.sizes, sizeof(orig.sizes));

	// create arrays to hold the mesh data using copied size data
	vertices = new Vector3D[sizes[0]];
	normals = new Vector3D[sizes[1]];
	textureCoords = new Vector2D[sizes[2]];
	faces = new Face[sizes[3]];

	// copy original mesh data to the new mesh
	memcpy(vertices,	  orig.vertices,	  sizeof(Vector3D) * sizes[0]	);
	memcpy(normals,		  orig.normals,		  sizeof(Vector3D) * sizes[1]	);
	memcpy(textureCoords, orig.textureCoords, sizeof(Vector2D) * sizes[2]	);
	memcpy(faces,		  orig.faces,		  sizeof(Face) * sizes[3]		);

	// return the mesh
	return *this;
}

Mesh::~Mesh() 
{
	// free memory
	delete [] vertices;
	delete [] normals;
	delete [] textureCoords;
	delete [] faces;
}

/* Helper Functions */

// get values seperated by space characters
std::string* numberExtract(std::string str, int initialIndex, int nElements)
{
	// create an array to store strings
	std::string* stringArray = new std::string[nElements]();

	// track the amount of sequences seperated 
	int separations = 0;

	// track the current character index
	int i = initialIndex;

	// loop until str has no more characters or we have a triplet
	while (i < str.length() && separations < nElements)
	{
		// if the character is not a number or a decimal point
		if ((str[i] < '0' || str[i] > '9') && str[i] != '.' && str[i] != '-')
		{
			separations++;
		}
		// otherwise addend the character to the current sequence
		else
			stringArray[separations] += str[i];

		// increment the index
		i++;
	}

	// return the triplet
	return stringArray;
}

// sorts an array of size 3
static void sort3(double* array)
{
	// sort the first two elements
	if (array[0] > array[1])
		std::swap(array[0], array[1]);

	// end if the array is sorted
	if (array[2] > array[1])
		return;

	// sort the last two elements
	std::swap(array[1], array[2]);

	// end if the array is sorted
	if (array[0] < array[1])
		return;

	// sort the first two elements
	std::swap(array[0], array[1]);

	// the array is sorted
	return;
}

// finds the rectangle containing a triangle
static void getBounds(int* output, Triangle& vertices)
{
	// create arrays to store the distance along each axis
	double X[3] = { vertices.a.x, vertices.b.x, vertices.c.x };
	double Y[3] = { vertices.a.y, vertices.b.y, vertices.c.y };

	// sort the positions along each axis
	sort3(X);
	sort3(Y);

	// set the bounds to the min and max along each axis and bind to the screen.
	output[0] = max(min((int)floor(X[0]), width - 1), 0);
	output[1] = max(min((int)floor(Y[0]), height - 1), 0);
	output[2] = max(min((int)ceil(X[2]), width - 1), 0);
	output[3] = max(min((int)ceil(Y[2]), height - 1), 0);
}

long long int frames = 0;
std::chrono::time_point<std::chrono::high_resolution_clock> startT, endT;
std::chrono::nanoseconds clear_time(0);

// draw a triangle to the screen using its vertex, texture and normal data
static void drawFace(Triangle& vertices, int faceId)
{
	// triangle edge vectors 
	Vector3D left = (vertices.b - vertices.a);
	Vector3D right = (vertices.c - vertices.a);

	// the normal values compared to the sun direction at each vertex
	double normalSun = ((1 - model.mesh->normals[model.mesh->faces[faceId].info[6]].dot(sunDirection)) * 0.5);
	double leftNormalSun = ((1 - model.mesh->normals[model.mesh->faces[faceId].info[7]].dot(sunDirection)) * 0.5) - normalSun;
	double rightNormalSun = ((1 - model.mesh->normals[model.mesh->faces[faceId].info[8]].dot(sunDirection)) * 0.5) - normalSun;

	// texture coordinates
	Vector2D& textureOrigin = model.mesh->textureCoords[model.mesh->faces[faceId].info[3]];
	Vector2D textureLeft = model.mesh->textureCoords[model.mesh->faces[faceId].info[4]] - textureOrigin;
	Vector2D textureRight = model.mesh->textureCoords[model.mesh->faces[faceId].info[5]] - textureOrigin;

	// collect texture bitmap data
	int spacing = model.bitmap->bpp / 8;
	int& end = model.bitmap->byte_count;

	// find the screenspace bounds of the face.
	int bounds[4] = { 0,0,0,0 };
	getBounds(bounds, vertices);

	// calculate the inverse of the scalar for the edge vectors
	double inv_scalar = (left.y * right.x - left.x * right.y);

	// scale the edge vectors prior to entering the loop
	left.x /= inv_scalar;
	left.y /= inv_scalar;
	right.x /= inv_scalar;
	right.y /= inv_scalar;

	// store the mapping of (x, y) onto (u, v)
	double u, v;

	// instantiate variables
	int mlt_x = (bounds[1] - 1) * width + (bounds[0] - 1);
	int mlt_xy;

	int pxID_x = 4 * mlt_x;
	int pxID_xy;

	// locate the initial point mapping (x,y) onto (u,v) from the top left corner of the bounding rect
	double	u_i = ((vertices.a.y - bounds[1] + 1) * left.x + (bounds[0] - 1 - vertices.a.x) * left.y),
			v_i = ((vertices.a.x - bounds[0] + 1) * right.y + (bounds[1] - 1 - vertices.a.y) * right.x);

	// loop horizontally
	for (int x = bounds[0]; x <= bounds[2]; x++)
	{

		// increment variables for horizontal adjustments
		mlt_x += 1;
		pxID_x += 4;

		u_i += left.y;
		v_i -= right.y;
		
		u = u_i;
		v = v_i;

		// update duplicate variables for vertical adjustments
		mlt_xy = mlt_x;
		pxID_xy = pxID_x;

		// loop vertically
		for (int y = bounds[1]; y <= bounds[3]; y++)
		{

			// increment variables for vertical adjustments
			mlt_xy += width;
			pxID_xy += 4 * width;

			u -= left.x;
			v += right.x;

			// skip the pixel if it is outside the triangle
			if (u < 0 || v < 0 || u + v > 1)
				continue;

			/* check distance */

			// calculate the pixel's distance from the camera
			double distance = v * left.z + u * right.z + vertices.a.z;

			// check if the pixel is either empty or closer to the camera
			if (0 != renderPane.lpDistance[mlt_xy] && renderPane.lpDistance[mlt_xy] > distance) continue;

			// update the pixel's distance from the camera
			renderPane.lpDistance[mlt_xy] = (float)distance;

			/* find pixel color and shade */

			// locate the texture index
			// interpolate vertex coordinates to find pixel coordinates 
			int colorIndex = (
				(int)((
					(int)((
						textureLeft.y * v + textureRight.y * u + textureOrigin.y
					) * (
						model.bitmap->height
					)) + (
						textureLeft.x * v + textureRight.x * u + textureOrigin.x
					)) * (
						model.bitmap->width
				))
			) * spacing;

			// interpolate the brightness multipliers for shading
			double sunShade = leftNormalSun * v + rightNormalSun * u + normalSun;

			/* update the pixel */

			// check if the color index is located within the bitmap array
			if (colorIndex >= 0 && colorIndex + 3 < end)
			{
				// set the pixel color value and adjust brightness based on sun direction
				renderPane.lpBits[pxID_xy    ] = (BYTE)(model.bitmap->bytes[colorIndex    ] * sunShade);
				renderPane.lpBits[pxID_xy + 1] = (BYTE)(model.bitmap->bytes[colorIndex + 1] * sunShade);
				renderPane.lpBits[pxID_xy + 2] = (BYTE)(model.bitmap->bytes[colorIndex + 2] * sunShade);
				
			}
		}
	}

}

static void drawObjectParallel(Camera& camera, Vector3D* projectedVertices, int start, int end)
{

	for (int faceId = start; faceId < end; faceId++)
	{
		// Check if any point on face is visible to the camera
		// convert from base 1 index to base 0
		Vector3D normal = calculateNormal(
			projectedVertices[model.mesh->faces[faceId].info[0]],
			projectedVertices[model.mesh->faces[faceId].info[1]],
			projectedVertices[model.mesh->faces[faceId].info[2]]
		);

		// check if the face is visible to the camera
		if (normal.z > 0)
			continue;

		// get the vertices of the face from the projected vertex vector array
		Triangle vertices = Triangle(
			projectedVertices[model.mesh->faces[faceId].info[0]],
			projectedVertices[model.mesh->faces[faceId].info[1]],
			projectedVertices[model.mesh->faces[faceId].info[2]]
		);

		drawFace(vertices, faceId);
	}

}

static void drawObject(Camera& camera, Model& model) 
{
	// check if the model has faces
	if (model.mesh->sizes[3] == 0) return;

	// create an array to hold the projected vertices
	Vector3D* projectedVertices = new Vector3D[model.mesh->sizes[0]];

	// project each vertex into screen space
	for (int i = 0; i < model.mesh->sizes[0]; i++)
		projectedVertices[i] = (midpoint + camera.getProjected(model.mesh->vertices[i]));

	// section out the faces and handle the rendering on seperate threads
	for (int i = 0; i < amtThreads; i++) 
		threads[i] = std::thread(
			drawObjectParallel, 
			std::ref(camera), 
			projectedVertices,
			(i * (int)model.mesh->sizes[3]) / amtThreads,
			((i + 1) * (int)model.mesh->sizes[3]) / amtThreads
		);

	// rejoin the threads
	for (auto& t : threads) t.join();

	// free the projections from memory
	delete [] projectedVertices;
}

static void renderScene(int width, int height)
{

	// access the camera and update its variables
	Camera& camera = *CAMERA_PTR;

	camera.yawTheta = yaw * 2 * M_PI;
	camera.pitchTheta = pitch * 2 * M_PI;
	camera.update();

	camera.position = camera.look * (-cameraDistance);

	// draw object
	drawObject(camera, model);

}

/* Rendering */

DWORD WINAPI threadLoop(LPVOID lpParam) {
	while (true) {
		if (hwndGlobal != nullptr) {
			InvalidateRect(hwndGlobal, nullptr, false);
		}
		Sleep(20);
	}
	return 0;
}

int WINAPI main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	Mesh mesh("Text.obj");
	BitMap bmp("monkeytexture.bmp");

	// link mesh and color data to model
	model.mesh = &mesh;
	model.bitmap = &bmp;

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

LRESULT CALLBACK WindowProcessMessages(HWND hwnd, UINT msg, WPARAM param, LPARAM lparam) {

    HDC hdcMem;

	static HBITMAP hBitmap;

	int xPosNew, yPosNew;

	auto ms = (const MOUSEHOOKSTRUCT*)lparam;

	static BYTE* lpBits = renderPane.lpBits;

	switch (msg) {

	case WM_DESTROY:

		std::cout << "average time: " << clear_time << " / " << frames - 200 << '\n';
		
		delete CAMERA_PTR;
		PostQuitMessage(0);
		break;

	case WM_CREATE:

		break;

	case WM_MOUSEMOVE:

		xPosNew = GET_X_LPARAM(lparam);
		yPosNew = GET_Y_LPARAM(lparam);

		if (leftMouseDown) 
		{
			yaw = ((int)(yaw * 2000 + xPos - xPosNew) % 2000) / 2000.0f;
			pitch = ((int)(pitch * 2000 - yPos + yPosNew) % 2000) / 2000.0f;
		}

		xPos = xPosNew;
		yPos = yPosNew;

		break;

	case WM_MOUSEWHEEL:
		
		cameraDistance -= GET_WHEEL_DELTA_WPARAM(param) / 100.0f;

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

		yaw = ((int)(yaw * 1000 + deltaX + yawDelta) % 1000) / 1000.0f;
		pitch = ((int)(pitch * 1000 - deltaY + pitchDelta) % 1000) / 1000.0f;

		startT = std::chrono::high_resolution_clock::now();

		renderScene(width, height);

		endT = std::chrono::high_resolution_clock::now();

		if (frames > 200)
			clear_time += endT - startT;

		frames += 1;

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
