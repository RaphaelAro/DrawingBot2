//Enables uses of Math constants
#define _USE_MATH_DEFINES

#define WIDTH 512
#define HEIGHT 512

#define NUM_LINES 3000

//Include GLEW  
#include <glew.h>  
//Include GLFW  
#include <glfw3.h>  


#include <iostream>

#include <vector>
#include <math.h>
#include <ctime>

#include "main.h"
#include "GLImage.h"
#include "loss.h"


struct Line {
	Line() : midpoint_x(((float)rand() / (RAND_MAX)-0.5) * 2), midpoint_y(((float)rand() / (RAND_MAX)-0.5) * 2),
		angle((float)rand() / (RAND_MAX)* M_PI), len((float)rand() / (RAND_MAX) * 0.1) {}
	Line(float midpoint_x, float midpoint_y, float angle, float len) : midpoint_x(midpoint_x),
		midpoint_y(midpoint_y), angle(angle), len(len) {};
	float midpoint_x;
	float midpoint_y;
	float angle;
	float len;
};

std::vector<float> getVertices(std::vector<Line> lines) {
	int numLines = lines.size();
	std::vector<float> vertices(numLines * 4);
	int v_counter = 0;
	for (int i = 0; i < numLines; i++) {
		Line line = lines[i];
		float dir_x = cos((double)line.angle);
		float dir_y = sin((double)line.angle);
		vertices[v_counter] = line.midpoint_x - dir_x * line.len / 2; //x1
		v_counter++;
		vertices[v_counter] = line.midpoint_y - dir_y * line.len / 2; //y1
		v_counter++;
		vertices[v_counter] = line.midpoint_x + dir_x * line.len / 2; //x2
		v_counter++;
		vertices[v_counter] = line.midpoint_y + dir_y * line.len / 2; //y2
		v_counter++;
	}
	return vertices;
}

float randomFloat() {
	return (float)rand() / (RAND_MAX);
}

//Define an error callback  
static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
	_fgetchar();
}

//Define the key input callback  
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

int main(int argc, char* argv[]) {

	//Set the error callback  
	glfwSetErrorCallback(error_callback);

	//Load input Image
	Image img = loadImage("blume.bmp");

	//Set random seed
	std::srand(std::time(nullptr));

	//Initialize GLFW  
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	//Set the GLFW window creation hints - these are optional  
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Request a specific OpenGL version  
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //Request a specific OpenGL version  
	//glfwWindowHint(GLFW_SAMPLES, 4); //Request 4x antialiasing  
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);   


	//Create a window and create its OpenGL context  
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "DrawingBot", NULL, NULL);

	//If the window couldn't be created  
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//This function makes the context of the specified window current on the calling thread.   
	glfwMakeContextCurrent(window);
	//Sets the key callback  
	glfwSetKeyCallback(window, key_callback);

	//Initialize GLEW  
	GLenum err = glewInit();
	//If GLEW hasn't initialized  
	if (err != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return -1;
	}

	// Set a background color  
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	float loss = img.height * img.width + 0.f;

	//draw random lines
	std::vector<Line> currentPicture(NUM_LINES);
	int count = 0;

	//Main Loop 
	do
	{
		std::vector<Line> lines = currentPicture;
		// change one line randomly
		lines[(int)(randomFloat() * NUM_LINES)] = Line();
		//Clear color buffer  
		glClear(GL_COLOR_BUFFER_BIT);

		glLineWidth(2.0);
		glColor4f(0.0, 0.0, 0.1, 0.5);

		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		std::vector<float> vertices = getVertices(lines);
		// save as array
		float *line_vertex = &vertices[0];


		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, line_vertex);
		glDrawArrays(GL_LINES, 0, (int)vertices.size() / 2);

		glfwSwapBuffers(window);

		//Get image in array shape
		float pixels[IMSIZE];
		readLowResulutionImage(pixels);

		//Swap buffers  
		//glfwSwapBuffers(window);
		//Get and organize events, like keyboard and mouse input, window resizing, etc...  
		glfwPollEvents();

		// Calculate Loss:
		float newLoss = pixelLoss(pixels, img.pixels, img.height * img.width);

		// Print the results
		printf("Current loss: %f \t New loss: %f\n", loss, newLoss);

		//Check if new picture was better
		if (loss > newLoss) {
			currentPicture = lines;
			loss = newLoss;
		}
		count++;
	} //Check if the ESC key had been pressed or if the window had been closed  
	while (!glfwWindowShouldClose(window) && loss > 3000);


	printf("Done %d iterations. Saving picture...", count);

	saveAsBMP("image.bmp");

	//Close OpenGL window and terminate GLFW  
	glfwDestroyWindow(window);
	//Finalize and clean up GLFW  
	glfwTerminate();

	exit(EXIT_SUCCESS);

}