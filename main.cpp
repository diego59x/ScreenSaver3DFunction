//#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <iostream>
#include <GL/glut.h>
#include <cstdlib>
#include <time.h>

using namespace std;

void init();
void display();
void idle();
int main(int argc, char** argv);
float f(float moveFunX, double x, double z);
void normalVector(float moveFunX, float x, float y, float z, float *norm);

float thetaY = 0;
float thetaX = 20;
float increment = 0.5;
float xNear = -6, xFar = 6, yNear = -6, yFar = 6, zNear = -6, zFar = 6;
float n = 50, m = 50;
float moveFunX = 0.0;
int t_start = time(NULL), t_end, frames = 0;

void init()
{
    glClearColor(1.0, 1.0, 1.0, 0);
    glMatrixMode(GL_PROJECTION);
    glOrtho(xNear, xFar, yNear, yFar, zNear, zFar);

}

float f(float moveFunX, double x, double z)
{
    //return moveFunX*(x*x)-(z*z);
    return moveFunX*cos(sqrt(x*x+z*z)*moveFunX*0.25);
}

void normalVector(float moveFunX, float x, float y, float z, float *norm)
{
    norm[0] = moveFunX*sin(sqrt(x*x+z*z))*x/sqrt(x*x+z*z);
    norm[1] = 1;
    norm[2] = moveFunX*sin(sqrt(x*x+z*z))*z/sqrt(x*x+z*z);
    float d = norm[0]*norm[0] + norm[1]*norm[1] + norm[2]*norm[2];
    if(d > 0)
        for (int k = 0; k < 3; k++)
            norm[k]/=sqrt(d);
}

void displayFPS(string fps) {
    glColor3f(0.0, 0.0, 0.0);
    glRasterPos2f(10, 10);
    for (int i = 0; i < fps.length(); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, fps[i]);
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(thetaX,1,0,0);
    glRotatef(thetaY,0,1,0);
    float norm[3];
    double xGap=(xNear-xFar)/n;
    double zGap=(zNear-zFar)/m;
    for (int i=0; i<n; i++)
    {
        for (int j=0; j<m; j++)
        {
            glBegin(GL_LINE_LOOP);
                float x = xFar + i*xGap;
                float z = zFar + j*zGap;
                float y = f(moveFunX, x,z);
                glVertex3f(x,y,z);
                normalVector(moveFunX,x,y,z,norm);
                glNormal3fv(norm);
                x = xFar + i*xGap;
                z = zFar + (j+1)*zGap;
                y = f(moveFunX, x,z);
                glVertex3f(x,y,z);
                normalVector(moveFunX,x,y,z,norm);
                glNormal3fv(norm);
                x = xFar + (i+1)*xGap;
                z = zFar + (j+1)*zGap;
                y = f(moveFunX, x,z);
                glVertex3f(x,y,z);
                normalVector(moveFunX,x,y,z,norm);
                glNormal3fv(norm);
                //  light direction
                float light[3] = {0,0,1};
                float dot = norm[0]*light[0] + norm[1]*light[1] + norm[2]*light[2];
                float r = ((x + 6) / 12)*0.8 + 0.5*dot;
                float g = ((z + 6) / 12)*0.8 + 0.5*dot;
                float b = ((z + 6) / 12)*0.8 + 0.5*dot;
                glColor3f(r,g,b);
            glEnd();
        }
    }
    glutSwapBuffers();
    frames++;
    t_end = time(NULL);
    if (t_end - t_start >= 1) {
        string fps = "FPS: " + to_string(frames / (t_end - t_start));
        cout << fps << endl;
        displayFPS(fps);
        frames = 0;
        t_start = time(NULL);
    }
}

void idle()
{
    thetaY += increment;
    if(thetaY > 360.0)
        thetaY -= 360.0;
    moveFunX = 5* cos((float)glutGet(GLUT_ELAPSED_TIME)/1000);
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(800,800);
    glutInitWindowPosition(10,10);
    glutCreateWindow("Screen saver function math :P");
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    init();
    glutMainLoop();
    return 0;
}