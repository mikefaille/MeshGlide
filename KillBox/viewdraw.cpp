// Copyright (C) 2014-2017 Alexandre-Xavier Labont�-Lamoureux
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// viewdraw.cpp
// OpenGL camera functions for screen drawing

#include "viewdraw.h"
#include "things.h"
#include "vecmath.h" // Float3

#include <GLFW/glfw3.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <cstdlib>	/* EXIT_FAILURE */

#include <iostream>
#include <string>
#include <cmath>
using namespace std;

void Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void WindowResize_Callback(GLFWwindow* window, int width, int height)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	float ratio = (float)width / height;
	float fov = 90;

	// Wide horizontal screen. People with a large screen should not
	// see more than others. The action is mostly horizontal.
	if (ratio > 1)
	{
		fov = fov / ratio;
	}

	glViewport(0.0f, 0.0f, width, height);
	gluPerspective(fov, ratio, 0.1f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Error_Callback(int error, const char* description)
{
	cerr << description << endl;
}

void SetWindowTitle(GLFWwindow* window, string Title)
{
	glfwSetWindowTitle(window, Title.c_str());
}

GLFWwindow* Init_OpenGL()
{
	// Create the window
	GLFWwindow* window;

	glfwSetErrorCallback(Error_Callback);

	// Initialise the glfw library
	if (!glfwInit())
		exit(EXIT_FAILURE);

	// Create a windowed mode window and its OpenGL context
	window = glfwCreateWindow(640, 480, WindowTitle.c_str(), NULL, NULL);
	//window = glfwCreateWindow(1366, 768, "Full screen window", glfwGetPrimaryMonitor(), NULL);
	if (!window)
	{
		// If it didn't work
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// Make its context current
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);	// Vsync?
	glfwSetKeyCallback(window, Key_Callback);

	InitProjection(window);

	// Set callback
	glfwSetWindowSizeCallback(window, WindowResize_Callback);

	// Make the background gray
	glClearColor(0.0, 0.0, 0.5, 0.0);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);		// Draw objects at the appropriate Z
	glEnable(GL_CULL_FACE);		// Don't draw faces behind polygons

	return window;
}

void InitProjection(GLFWwindow* window)
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	// Tell GL how to show us the world
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// This sets the matrices for the window
	WindowResize_Callback(window, width, height);
}

void DrawScreen(GLFWwindow* window, Player* play, Level* lvl)
{
	// Reset colors and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Load identity matrix
	glLoadIdentity();

	// Enable textures
	glEnable(GL_TEXTURE_2D);

	gluLookAt(
		play->PosY, play->PosZ, play->PosX,		// Camera's position
		play->PosY + sin(play->GetRadianAngle(play->Angle)), 	// What the camera will look at
		play->PosZ, play->PosX + cos(play->GetRadianAngle(play->Angle)),	// What the camera will look at
		0.0, 1.0, 0.0);	// This is for the camera's frame rotation

	//glRotatef( xrot, 1.0f, 0.0f, 0.0f); /* Rotate On The X Axis */
	//glRotatef( yrot, 0.0f, 1.0f, 0.0f); /* Rotate On The Y Axis */
	//glRotatef( zrot, 0.0f, 0.0f, 1.0f); /* Rotate On The Z Axis */

	// Check if level is not a null pointer to avoid errors and draw its content
	if (lvl != nullptr)
	{
		// Draw walls
		for (int i = 0; i < lvl->ptrWalls.size(); i++)
		{
			lvl->UseTexture(lvl->ptrWalls[i].Texture);

			if (lvl->ptrWalls[i].TwoSided)
				glDisable(GL_CULL_FACE);
			else
				glEnable(GL_CULL_FACE);

			glPushMatrix();
			{
				//glTranslatef(0, 0, 0);
				glColor3f(lvl->ptrWalls[i].Light, lvl->ptrWalls[i].Light, lvl->ptrWalls[i].Light);	// "Light": Could be made tint (RGB) later

				if (lvl->ptrWalls[i].Vertices.size() == 4)
				{
					// Polygons that are square
					glBegin(GL_QUADS);
					{
						glTexCoord2f(0, 1 * lvl->ptrWalls[i].Yscale);
						glVertex3f(lvl->ptrWalls[i].Vertices[0].x, lvl->ptrWalls[i].Vertices[0].y, lvl->ptrWalls[i].Vertices[0].z);
						glTexCoord2f(1 * lvl->ptrWalls[i].Xscale, 1 * lvl->ptrWalls[i].Yscale);
						glVertex3f(lvl->ptrWalls[i].Vertices[1].x, lvl->ptrWalls[i].Vertices[1].y, lvl->ptrWalls[i].Vertices[1].z);
						glTexCoord2f(1 * lvl->ptrWalls[i].Xscale, 0);
						glVertex3f(lvl->ptrWalls[i].Vertices[2].x, lvl->ptrWalls[i].Vertices[2].y, lvl->ptrWalls[i].Vertices[2].z);
						glTexCoord2f(0, 0);
						glVertex3f(lvl->ptrWalls[i].Vertices[3].x, lvl->ptrWalls[i].Vertices[3].y, lvl->ptrWalls[i].Vertices[3].z);
					}
					glEnd();
				}
				else if (lvl->ptrWalls[i].Vertices.size() == 3)
				{
					// Polygons that have a triangular shape
					glBegin(GL_TRIANGLES);
					{
						glTexCoord2f(0, 1 * lvl->ptrWalls[i].Yscale);
						glVertex3f(lvl->ptrWalls[i].Vertices[0].x, lvl->ptrWalls[i].Vertices[0].y, lvl->ptrWalls[i].Vertices[0].z);
						glTexCoord2f(1 * lvl->ptrWalls[i].Xscale, 1 * lvl->ptrWalls[i].Yscale);
						glVertex3f(lvl->ptrWalls[i].Vertices[1].x, lvl->ptrWalls[i].Vertices[1].y, lvl->ptrWalls[i].Vertices[1].z);
						glTexCoord2f(1 * lvl->ptrWalls[i].Xscale, 0);
						glVertex3f(lvl->ptrWalls[i].Vertices[2].x, lvl->ptrWalls[i].Vertices[2].y, lvl->ptrWalls[i].Vertices[2].z);
					}
					glEnd();
				}
			}
			glPopMatrix();

		}
	}

	glEnable(GL_CULL_FACE);

	// Draw flat polygons from now
	glDisable(GL_TEXTURE_2D);

	for (int i = -3; i <= 3; i++)
	{
		for (int j = -3; j <= 3; j++)
		{
			float distance = pow(play->PosX - j*10, 2) + pow(play->PosY - i*10, 2);
			if (distance <= 500)
			{
				glColor3f(1.0f, 1.0f, 1.0f);
				glPushMatrix();
				glTranslatef(i * 10.0f, 0.0f, j * 10.0f);
				// Big body snow ball
				glTranslatef(0.0f, 0.75f, 0.0f);
				glutSolidSphere(0.75f, 20, 20);
				// Small head ball
				glTranslatef(0.0f, 1.0f, 0.0f);
				glutSolidSphere(0.25f, 20, 20);
				// Black Eyes
				glPushMatrix();
				glColor3f(0.0f, 0.0f, 0.0f);
				glTranslatef(0.05f, 0.10f, 0.18f);
				glutSolidSphere(0.05f, 10, 10);
				glTranslatef(-0.1f, 0.0f, 0.0f);
				glutSolidSphere(0.05f, 10, 10);
				glPopMatrix();
				// Carrot
				glColor3f(235.0f/256.0f, 95.0f/256.0f, 0.0f);
				glutSolidCone(0.08f, 0.5f, 10, 2);
				glPopMatrix();
			}
			else	// Far, so lower poly count
			{
				glColor3f(1.0f, 1.0f, 1.0f);
				glPushMatrix();
				glTranslatef(i * 10.0f, 0.0f, j * 10.0f);
				// Big body snow ball
				glTranslatef(0.0f, 0.75f, 0.0f);
				glutSolidSphere(0.75f, 10, 5);
				// Small head ball
				glTranslatef(0.0f, 1.0f, 0.0f);
				glutSolidSphere(0.25f, 10, 5);
				// Black Eyes
				glPushMatrix();
				glColor3f(0.0f, 0.0f, 0.0f);
				glTranslatef(0.05f, 0.10f, 0.18f);
				glutSolidSphere(0.05f, 6, 5);
				glTranslatef(-0.1f, 0.0f, 0.0f);
				glutSolidSphere(0.05f, 6, 5);
				glPopMatrix();
				// Carrot
				glColor3f(235.0f/256.0f, 95.0f/256.0f, 0.0f);
				glutSolidCone(0.08f, 0.5f, 5, 2);
				glPopMatrix();
			}
		}
	}

	// Draw sky (relative to player)
	glPushMatrix();
		glColor3f(153.0f / 256.0f, 217.0f / 256.0f, 234.0f / 256.0f);
		glBegin(GL_QUADS);
			// (Xpos, Zpos, Ypos)
			// TODO: Use this for sky coords: glTranslatef(play->PosY, play->PosZ, play->PosX);
			glVertex3f(play->PosY + 100.0f, 10.0f, play->PosX - 100.0f);
			glVertex3f(play->PosY + 100.0f, 10.0f, play->PosX + 100.0f);
			glVertex3f(play->PosY - 100.0f, 10.0f, play->PosX + 100.0f);
			glVertex3f(play->PosY - 100.0f, 10.0f, play->PosX - 100.0f);
		glEnd();
	glPopMatrix();

	// Swap the front and back buffers
	glfwSwapBuffers(window);
}

void Close_OpenGL(GLFWwindow* window)
{
	glfwDestroyWindow(window);
	glfwTerminate();
}
