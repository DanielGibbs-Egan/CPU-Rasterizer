# CPU Rasterizer
Displays a .obj mesh and .bmp texture map using a windows application

## Completed:
- Extracts data from .obj and .bmp files
- Creates an application with a 1920x1080 pixel rendering window
- Projects the objects vertices to the camera view
- Handles user input for camera zoom and rotation
- Uses back-face culling to reduce render time
- Renders a triangle using its bounding box and edge equations 
- Uses interpolation to reduce per pixel cost
- Shades objects using normal and sun direction
- Implements basic parallelism to reduce render time

## Planned:
- Camera and object translation
- Cast shadows
