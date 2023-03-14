//#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "GL/glut.h"
#include <cstdlib>
#include <time.h>

void init();
void display();
void myIdle();
int main(int argc, char** argv);
float f(double x, double z);
void normalVector(float x, float y, float z, float *norm);

float thetaY = 0;
float thetaX = 20;
float increment = 0.5;
float xNear = -6, xFar = 6, yNear = -6, yFar = 6, zNear = -6, zFar = 6;
float n = 50, m = 50;

void init()
{
    glClearColor(1.0, 1.0, 1.0, 0);
    glMatrixMode(GL_PROJECTION);
    glOrtho(xNear, xFar, yNear, yFar, zNear, zFar);
    glColor3f(0.0,0.0,1.0);

}

float f(int moveFunX, int moveFunZ, double x, double z)
{
    return moveFunX*(x*x)- moveFunZ*(z*z);
}

void normalVector(int moveFunX, int moveFunZ, float x, float y, float z, float *norm)
{

    norm[0] = moveFunX*sin(sqrt(x*x+z*z))*x/sqrt(x*x+z*z);
    norm[1] = 1;
    norm[2] = moveFunZ*sin(sqrt(x*x+z*z))*z/sqrt(x*x+z*z);
    float d = norm[0]*norm[0] + norm[1]*norm[1] + norm[2]*norm[2];
    if(d > 0)
        for (int k = 0; k < 3; k++)
            norm[k]/=sqrt(d);
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

    int moveFunX = (rand() % 6) - 3;
    int moveFunZ = (rand() % 6) - 3;


    for (int i=0; i<n; i++)
    {
        for (int j=0; j<m; j++)
        {
            glBegin(GL_LINE_LOOP);
                float x = xFar + i*xGap;
                float z = zFar + j*zGap;
                float y = f(moveFunX, moveFunZ, x,z);
                glVertex3f(x,y,z);
                normalVector(moveFunX, moveFunZ, x, y, z, norm);
                glNormal3fv(norm);
                x = xFar + i*xGap;
                z = zFar + (j+1)*zGap;
                y = f(moveFunX, moveFunZ, x,z);
                glVertex3f(x,y,z);
                normalVector(moveFunX, moveFunZ, x, y, z, norm);
                glNormal3fv(norm);
                x = xFar + (i+1)*xGap;
                z = zFar + (j+1)*zGap;
                y = f(moveFunX, moveFunZ, x,z);
                glVertex3f(x,y,z);
                normalVector(moveFunX, moveFunZ, x, y, z, norm);
                glNormal3fv(norm);
            glEnd();
            glBegin(GL_LINE_LOOP);
                x = xFar + i*xGap;
                z = zFar + j*zGap;
                y = f(moveFunX, moveFunZ, x,z);
                glVertex3f(x,y,z);
                normalVector(moveFunX, moveFunZ, x, y, z, norm);
                glNormal3fv(norm);
                x = xFar + (i+1)*xGap;
                z = zFar + (j+1)*zGap;
                y = f(moveFunX, moveFunZ, x,z);
                glVertex3f(x,y,z);
                normalVector(moveFunX, moveFunZ, x, y, z, norm);
                glNormal3fv(norm);
                x = xFar + (i+1)*xGap;
                z = zFar + (j)*zGap;
                y = f(moveFunX, moveFunZ, x,z);
                glVertex3f(x,y,z);
                normalVector(moveFunX, moveFunZ, x, y, z, norm);
                glNormal3fv(norm);
            glEnd();
        }
    }
    glutSwapBuffers();
}

void myIdle()
{
    double zzz=0;
    thetaY += increment;
    if(thetaY > 360.0)
        thetaY -= 360.0;
    for (int i=0; i<1000000; i++)
        zzz = sqrt((double)i);
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    // Seed of random
    srand(time(0));

    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(800,800);
    glutInitWindowPosition(10,10);
    glutCreateWindow("Screen saver function math :P");
    glutDisplayFunc(display);
    glutIdleFunc(myIdle);
    init();
    glutMainLoop();
    return 0;
}