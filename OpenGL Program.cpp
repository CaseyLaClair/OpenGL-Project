//Header Inclusions
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

//GLM math header inclusions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std; //Standard namespace

#define WINDOW_TITLE "Modern OpenGL"

//Shader program Macro
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif

//Variable declaration for shader, window size, buffer and array objects
GLint shaderProgram, WindowWidth = 800, WindowHeight = 600;
GLuint VBO, VAO;

//Adjusts camera speed
GLfloat cameraSpeed = 0.05f; 

//locks mouse cursor 
GLfloat lastMouseX = 400, lastMouseY = 300; 

//mouse offset, yaw, and pitch variables
GLfloat mouseXOffset, mouseYOffset, yaw = 0.0f, pitch = 0.0f;
 
//mouse sensitivity
GLfloat sensitivity = 0.05f;

//initially false
bool mouseDetected = false; 

//Global vector declarations
glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 5.0f); //Init camera position
glm::vec3 CameraUpY = glm::vec3(0.0f, 1.0f, 0.0f); //Temp Y unit vector
glm::vec3 CameraForwardZ = glm::vec3(0.0f, 0.0f, -1.0f); //Temp Z unit vector
glm::vec3 front;


//Function prototypes
void UResizeWindow(int, int);
void URenderGraphics(void);
void UCreateShader(void);
void UCreateBuffers(void);
void UPressedMouseMove(int x, int y);
void UMouseClick(int button, int state, int x, int y);

//Vertex Shader Source code
const GLchar * vertexShaderSource = GLSL(330,
	layout (location = 0) in vec3 position;
	layout (location = 1) in vec3 color;

	out vec3 mobileColor;

	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;

//set main position
void main(){
		gl_Position = projection * view * model * vec4(position, 1.0f);
		mobileColor = color;
	}
);

//Fragment Shader Source code
const GLchar * fragmentShaderSource = GLSL(330,

		in vec3 mobileColor;

		out vec4 gpuColor;

	void main(){

		gpuColor = vec4(mobileColor, 1.0);
	}

);


//Main Program
int main(int argc, char* argv[]){

	//init variables and window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(WindowWidth, WindowHeight);
	glutCreateWindow(WINDOW_TITLE);

	glutReshapeFunc(UResizeWindow);

	glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK){
			std::cout << "Failed to initialize GLEW" << std::endl;
			return -1;
		}

	//Create shader
	UCreateShader();

	UCreateBuffers();

	//Use the shader program
	glUseProgram(shaderProgram);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //Background color

	glutDisplayFunc(URenderGraphics);

	glutMouseFunc(UMouseClick);

	glutMainLoop();

	//Destroys buffer objects after use
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	return 0;
}

//Resize the window
void UResizeWindow(int w, int h){
	WindowWidth = w;
	WindowHeight = h;
	glViewport(0, 0, WindowWidth, WindowHeight);
}

//Renders graphics
void URenderGraphics(void){

	glEnable(GL_DEPTH_TEST); //enable z-depth

	//Clears the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(VAO);

	CameraForwardZ = front;

	//Transforms the object
	glm::mat4 model;

	//place the object at the center of viewport
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); 

	//rotate the object 45 degrees on the x axis
	model = glm::rotate(model, 45.0f, glm::vec3(0.0f, 1.0f, 0.0f)); 

	//Increase the object size
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f)); 

	//Transforms the camera
	glm::mat4 view;
	view = glm::lookAt(CameraForwardZ, cameraPosition, CameraUpY);

	//Creates a perspective projection
	glm::mat4 projection;
	projection = glm::perspective(45.0f, (GLfloat)WindowWidth / (GLfloat)WindowHeight, 0.1f, 100.0f);

	//Retrieves and passes transform matrices to the Shader Program
	GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
	GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
	GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glutPostRedisplay();

	glDrawArrays(GL_TRIANGLES, 0, 66);

	glBindVertexArray(0);

	//Flips the back buffer with the front buffer every frame
	glutSwapBuffers(); 

}

//Creates Shader program
void UCreateShader(){

	//Vertex Shader
	GLint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	//Fragment Shader
	GLint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	//Shader program
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	//Delete the Vertex and Fragment shaders after linking
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}

void UCreateBuffers(){

	GLfloat vertices[] = {
			 //Positions		  //Color
			 //Right side
			 0.15f,  0.8f,  0.15f,  1.0f,  0.0f,  0.0f,
			 0.15f,  0.8f, -0.15f,  1.0f,  0.0f,  0.0f,
			 0.15f, -0.8f,  0.15f,  1.0f,  0.0f,  0.0f,
			 0.15f, -0.8f,  0.15f,  1.0f,  0.0f,  0.0f,
			 0.15f, -0.8f, -0.15f,  1.0f,  0.0f,  0.0f,
			 0.15f,  0.8f, -0.15f,  1.0f,  0.0f,  0.0f,

			 //Back side
			-0.15f, -0.8f, -0.15f,  0.0f,  1.0f,  0.0f,
			 0.15f, -0.8f, -0.15f,  0.0f,  1.0f,  0.0f,
			 0.15f,  0.8f, -0.15f,  0.0f,  1.0f,  0.0f,
			 0.15f,  0.8f, -0.15f,  0.0f,  1.0f,  0.0f,
			-0.15f,  0.8f, -0.15f,  0.0f,  1.0f,  0.0f,
			-0.15f, -0.8f, -0.15f,  0.0f,  1.0f,  0.0f,

			 //Left side
			-0.15f,  0.8f,  0.15f,  0.0f,  0.0f,  1.0f,
		    -0.15f,  0.8f, -0.15f,  0.0f,  0.0f,  1.0f,
			-0.15f, -0.8f, -0.15f,  0.0f,  0.0f,  1.0f,
			-0.15f, -0.8f, -0.15f,  0.0f,  0.0f,  1.0f,
			-0.15f, -0.8f,  0.15f,  0.0f,  0.0f,  1.0f,
			-0.15f,  0.8f,  0.15f,  0.0f,  0.0f,  1.0f,

			 //Front side
			-0.15f,  0.8f,  0.15f,  1.0f,  1.0f,  0.0f,
			 0.15f,  0.8f,  0.15f,  1.0f,  1.0f,  0.0f,
			 0.15f, -0.8f,  0.15f,  1.0f,  1.0f,  0.0f,
			 0.15f, -0.8f,  0.15f,  1.0f,  1.0f,  0.0f,
			-0.15f, -0.8f,  0.15f,  1.0f,  1.0f,  0.0f,
			-0.15f,  0.8f,  0.15f,  1.0f,  1.0f,  0.0f,

			 //Top-left
			-0.15f,  0.95f,  0.15f,  0.0f,  1.0f,  1.0f,
			-0.15f,  0.80f,  0.15f,  0.0f,  1.0f,  1.0f,
			-0.15f,  0.80f, -0.15f,  0.0f,  1.0f,  1.0f,

			 //Top-right
			 0.15f,  0.80f,  0.15f,  0.0f,  1.0f,  1.0f,
			 0.15f,  0.80f, -0.15f,  0.0f,  1.0f,  1.0f,
			 0.15f,  0.95f,  0.15f,  0.0f,  1.0f,  1.0f,

			 //Top-back
			 0.15f,  0.95f,  0.15f,  1.0f,  0.0f,  1.0f,
			 0.15f,  0.80f, -0.15f,  1.0f,  0.0f,  1.0f,
		    -0.15f,  0.95f,  0.15f,  1.0f,  0.0f,  1.0f,
			-0.15f,  0.95f,  0.15f,  1.0f,  0.0f,  1.0f,
			-0.15f,  0.80f, -0.15f,  1.0f,  0.0f,  1.0f,
			 0.15f,  0.80f, -0.15f,  1.0f,  0.0f,  1.0f,

			 //Cone-top
			-0.25f,  0.90f,  0.55f,  1.0f,  0.0f,  0.0f,
			-0.15f,  0.95f,  0.15f,  1.0f,  0.0f,  0.0f,
			 0.25f,  0.90f,  0.55f,  1.0f,  0.0f,  0.0f,
			 0.25f,  0.90f,  0.55f,  1.0f,  0.0f,  0.0f,
			 0.15f,  0.95f,  0.15f,  1.0f,  0.0f,  0.0f,
		    -0.15f,  0.95f,  0.15f,  1.0f,  0.0f,  0.0f,

			 //Cone-bottom
			-0.25f,  0.60f,  0.40f,  1.0f,  1.0f,  0.0f,
			-0.15f,  0.80f,  0.15f,  1.0f,  0.0f,  0.0f,
			 0.15f,  0.80f,  0.15f,  1.0f,  0.0f,  0.0f,
			 0.15f,  0.80f,  0.15f,  1.0f,  0.0f,  0.0f,
			 0.25f,  0.60f,  0.40f,  1.0f,  0.0f,  0.0f,
			-0.25f,  0.60f,  0.40f,  1.0f,  0.0f,  0.0f,

		     //Cone-left
			-0.15f,  0.95f,  0.15f,  0.0f,  0.0f,  1.0f,
			-0.25f,  0.90f,  0.55f,  0.0f,  0.0f,  1.0f,
			-0.25f,  0.60f,  0.40f,  0.0f,  0.0f,  1.0f,
			-0.25f,  0.60f,  0.40f,  0.0f,  0.0f,  1.0f,
			-0.15f,  0.80f,  0.15f,  0.0f,  0.0f,  1.0f,
			-0.15f,  0.95f,  0.15f,  0.0f,  0.0f,  1.0f,

			 //Cone-right
			 0.25f,  0.90f,  0.55f,  0.0f,  1.0f,  0.0f,
			 0.15f,  0.95f,  0.15f,  0.0f,  1.0f,  0.0f,
			 0.15f,  0.80f,  0.15f,  0.0f,  1.0f,  0.0f,
			 0.15f,  0.80f,  0.15f,  0.0f,  1.0f,  0.0f,
			 0.25f,  0.60f,  0.40f,  0.0f,  1.0f,  0.0f,
			 0.25f,  0.90f,  0.55f,  0.0f,  1.0f,  0.0f,

			 //Base
			 -0.40f, -0.80f,  0.40f,  1.0f,  0.0f,  0.0f,
			 -0.40f, -0.80f, -0.40f,  1.0f,  0.0f,  0.0f,
			  0.40f, -0.80f, -0.40f,  1.0f,  0.0f,  0.0f,
			  0.40f, -0.80f, -0.40f,  1.0f,  0.0f,  0.0f,
			  0.40f, -0.80f,  0.40f,  1.0f,  0.0f,  0.0f,
			 -0.40f, -0.80f,  0.40f,  1.0f,  0.0f,  0.0f
	};

	//generate buffer ids
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	//Activate the VAO before binding and setting VBOs
	glBindVertexArray(VAO);

	//Activate the VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Set attribute pointer 0 to hold position data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Set attribute pointer 1 to hold color data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

}

void UMouseZoom(int x, int y){

	if(mouseDetected){
		lastMouseY = y;
		mouseDetected = false;
	}

	//Gets the direction the mouse was moved in x and y
	mouseYOffset = lastMouseY - y;

	//Updates with new mouse coordinates
	lastMouseY = y;

	mouseYOffset *= sensitivity;

	if(mouseYOffset > lastMouseY){
		cameraPosition += cameraSpeed * CameraForwardZ;
	}

	if(mouseYOffset < lastMouseY){
		cameraPosition -= cameraSpeed * CameraForwardZ;
	}
}

void UMouseClick(int button, int state, int x, int y){

	if((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)){
		glutMotionFunc(UPressedMouseMove);
	}

	if((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN)){
		glutMotionFunc(UMouseZoom);
	}

}

//Implements the mouse move function
void UPressedMouseMove(int x, int y){

	//Immediately replaces center locked coordinates with new mouse coordinates
	if(mouseDetected){
		lastMouseX = x;
		lastMouseY = y;
		mouseDetected = false;
	}

	//Gets the direction the mouse was moved in x and y
	mouseXOffset = x - lastMouseX;
	mouseYOffset = lastMouseY - y;

	//Updates with new mouse coordinates
	lastMouseX = x;
	lastMouseY = y;

	//Applies sensitivity to mouse direction
	mouseXOffset *= sensitivity;
	mouseYOffset *= sensitivity;

	//Accumulates the yaw and pitch variables
	yaw += mouseXOffset;
	pitch += mouseYOffset;

	//Orbits around the center
	front.x = 10.0f * cos(yaw);
	front.y = 10.0f * sin(pitch);
	front.z = sin(yaw) * cos(pitch) * 10.0f;

}


