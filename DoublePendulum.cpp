#include <iostream>
#include <cmath>
#include <vector>
#include "include/GL/freeglut.h"

#pragma comment(lib, "freeglut.lib")
#pragma comment(lib, "glew32.lib")

#define STABLE(a, t) while ((a) > (t)) (a) -= D_PI; while ((a) < -(t)) (a) += D_PI

using namespace std;

static double PI = 3.14159265;
static double D_PI = 2 * PI;
static float G = 0.98f;
double firstAngle, secondAngle;
double firstAngleVelocity, secondAngleVelocity;
double airRegistance = 0.9;
float lastTime;
GLfloat centerX, centerY;
GLfloat firstLine, secondLine;
GLfloat firstCircleRadius, secondCircleRadius;
vector<pair<GLfloat, GLfloat>> points;

void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;

	float ratio = w * 1.0 / h;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	//gluPerspective(45.0f, ratio, 0.1f, 100.0f);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}

void drawCircle(GLfloat x, GLfloat y, GLfloat radius) {
	int i;
	int triangleAmount = 50; //# of triangles used to draw circle

	//GLfloat radius = 0.8f; //radius
	GLfloat twicePi = 2.0f * PI;

	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(x, y); // center of circle
	for (i = 0; i <= triangleAmount; i++) {
		glVertex2f(
			x + (radius * cos(i *  twicePi / triangleAmount)),
			y + (radius * sin(i * twicePi / triangleAmount))
		);
	}
	glEnd();
}

void drawBitmapText(char *str, float x, float y) {
	glRasterPos2f(x, y); //문자열이 그려질 위치 지정

	while (*str) {
		//GLUT_BITMAP_TIMES_ROMAN_24 폰트를 사용하여 문자열을 그린다.
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *str);
		str++;
	}
}

void drawLine(GLfloat x, GLfloat y, GLfloat length, float angle) {
	glLineWidth(2);
	glBegin(GL_LINES);
	glVertex2f(x, y);
	glVertex2f(x + length * cos(angle), y + length * sin(angle));
	glEnd();
}

void renderScene(void) {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); //glClear에서 컬러 버퍼 지운 후 윈도우를 채울 색을 지정, 검은색
	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT);

	// Reset transformations
	glLoadIdentity();
	// Set the camera
	/*gluLookAt(0.0f, 0.0f, 10.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);*/

	glColor3f(1.0f, 1.0f, 1.0f);

	int currentTime = glutGet(GLUT_ELAPSED_TIME);
	float time = (currentTime - lastTime) / 1000;
	lastTime = currentTime;

	char frame[20];
	if (time == 0) {
		time = 1;
	}
	snprintf(frame, 21, "%.2f fps", 1 / time);
	//printf("time : %s\n", frame);
	drawBitmapText(frame, -1.0f, 0.8f);

	// Calculate angle's acceleration and velocity
	double firstAngleAcceleration, secondAngleAcceleration;
	// Calculate first
	double denominator = firstLine * (2 * firstCircleRadius + secondCircleRadius - secondCircleRadius * cos(2 * firstAngle - 2 * secondAngle));
	double tmp1 = -G * (2 * firstCircleRadius + secondCircleRadius) * sin(firstAngle);
	double tmp2 = -secondCircleRadius * G * sin(firstAngle - 2 * secondAngle);
	double tmp3 = -2 * sin(firstAngle - secondAngle) * secondCircleRadius * (secondAngleVelocity * secondAngleVelocity * secondLine + firstAngleVelocity * firstAngleVelocity * firstLine * cos(firstAngle - secondAngle));
	double numerator = tmp1 + tmp2 + tmp3;
	firstAngleAcceleration = numerator / denominator;
	// Calculate second
	denominator = secondLine * (2 * firstCircleRadius + secondCircleRadius - secondCircleRadius * cos(2 * firstAngle - 2 * secondAngle));
	tmp1 = firstAngleVelocity * firstAngleVelocity * firstLine * (firstCircleRadius + secondCircleRadius);
	tmp2 = G * (firstCircleRadius + secondCircleRadius) * cos(firstAngle);
	tmp3 = secondAngleVelocity * secondAngleVelocity * secondLine * secondCircleRadius * cos(firstAngle - secondAngle);
	numerator = 2 * sin(firstAngle - secondAngle) * (tmp1 + tmp2 + tmp3);
	secondAngleAcceleration = numerator / denominator;

	firstAngleVelocity += firstAngleAcceleration * time;
	secondAngleVelocity += secondAngleAcceleration * time;

	firstAngle += firstAngleVelocity * time;
	secondAngle += secondAngleVelocity * time;

	STABLE(firstAngle, D_PI);
	//STABLE(firstAngleVelocity, 2 * D_PI);
	//STABLE(firstAngleAcceleration, 2 * D_PI);
	STABLE(secondAngle, D_PI);
	//STABLE(secondAngleVelocity, 2 * D_PI);
	//STABLE(secondAngleAcceleration, 2 * D_PI);

	char buf[21];
	snprintf(buf, 20, "angle1 = %.5f", firstAngle);
	drawBitmapText(buf, -1.0f, 0.75f);
	snprintf(buf, 20, "angle2 = %.5f", secondAngle);
	drawBitmapText(buf, -1.0f, 0.7f);
	snprintf(buf, 20, "v1 = %.5f", firstAngleVelocity);
	drawBitmapText(buf, -1.0f, 0.65f);
	snprintf(buf, 20, "v2 = %.5f", secondAngleVelocity);
	drawBitmapText(buf, -1.0f, 0.6f);
	snprintf(buf, 20, "a1 = %.5f", firstAngleAcceleration);
	drawBitmapText(buf, -1.0f, 0.55f);
	snprintf(buf, 20, "a2 = %.5f", secondAngleAcceleration);
	drawBitmapText(buf, -1.0f, 0.5f);

	snprintf(buf, 20, "center x = %.2f", centerX);
	drawBitmapText(buf, 0.6f, 0.8f);
	snprintf(buf, 20, "center y = %.2f", centerY);
	drawBitmapText(buf, 0.6f, 0.75f);
	snprintf(buf, 20, "m1 = %.2f", firstCircleRadius);
	drawBitmapText(buf, 0.6f, 0.7f);
	snprintf(buf, 20, "m2 = %.2f", secondCircleRadius);
	drawBitmapText(buf, 0.6f, 0.65f);
	snprintf(buf, 20, "L1 = %.2f", firstLine);
	drawBitmapText(buf, 0.6f, 0.6f);
	snprintf(buf, 20, "L2 = %.2f", secondLine);
	drawBitmapText(buf, 0.6f, 0.55f);
	
	GLfloat firstCircleX = centerX + firstLine * cos(firstAngle - PI / 2), firstCircleY = centerY + firstLine * sin(firstAngle - PI / 2);
	GLfloat secondCircleX = firstCircleX + secondLine * cos(secondAngle - PI / 2), secondCircleY = firstCircleY + secondLine * sin(secondAngle - PI / 2);

	points.push_back(pair<GLfloat, GLfloat>(secondCircleX, secondCircleY));
	// #00ddea
	glColor3f(0.0f, 0.87f, 0.91f);
	glBegin(GL_LINE_STRIP);
	for (auto point : points) {
		glVertex2f(point.first, point.second);
	}
	glEnd();
	
	// Draw all circles and lines
	glColor3f(1.0f, 1.0f, 1.0f);
	drawLine(centerX, centerY, firstLine, firstAngle - PI / 2);
	glColor3f(1.0f, 1.0f, 1.0f);
	drawLine(firstCircleX, firstCircleY, secondLine, secondAngle - PI / 2);
	glColor3f(abs(firstAngle) / D_PI, abs(firstAngleVelocity) / D_PI, abs(firstAngleAcceleration) / D_PI);
	drawCircle(firstCircleX, firstCircleY, firstCircleRadius);
	glColor3f(abs(secondAngle) / D_PI, abs(secondAngleVelocity) / D_PI, abs(secondAngleAcceleration) / D_PI);
	drawCircle(secondCircleX, secondCircleY, secondCircleRadius);

	glutSwapBuffers();
}

void setup() {
	lastTime = glutGet(GLUT_ELAPSED_TIME);
	firstAngle = PI / 2;
	secondAngle = firstAngle + PI / 9;
	firstAngleVelocity = 0;
	secondAngleVelocity = 0;
	centerX = 0.0f, centerY = 0.05f;
	firstLine = 0.5f, secondLine = 0.5f;
	firstCircleRadius = 0.06f, secondCircleRadius = 0.06f;
}

int main(int argc, char **argv)
{
	// init GLUT and create window
	glutInit(&argc, argv);
	//glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Double Pendulum");

	// register callbacks
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);

	setup();

	// enter GLUT event processing cycle
	glutMainLoop();
}