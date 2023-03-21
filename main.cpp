//#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <iostream>
#include <GL/glut.h>
#include <cstdlib>
#include <time.h>
#include <vector>
#include <omp.h>

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
float n = 100, m = 100;
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

void display()
{
    // tiempo de inicio
    double t_inicial = omp_get_wtime();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(thetaX,1,0,0);
    glRotatef(thetaY,0,1,0);
    float norm[3];
    double xGap=(xNear-xFar)/n;
    double zGap=(zNear-zFar)/m;
    float values[100][100][12];
    #pragma omp parallel for collapse(2) num_threads(10) schedule(static, 1)
    for (int i = 0; i < (int)n; i++)
    {
        for (int j = 0; j < (int)m; j++)
        {
            float x = xFar + i*xGap;
            values[i][j][0] = x;
            float z = zFar + j*zGap;
            values[i][j][1] = z;
            float y = f(moveFunX, x,z);
            values[i][j][2] = y;
            float x2 = xFar + i*xGap;
            values[i][j][3] = x2;
            float z2 = zFar + (j+1)*zGap;
            values[i][j][4] = z2;
            float y2 = f(moveFunX, x2,z2);
            values[i][j][5] = y2;
            float x3 = xFar + (i+1)*xGap;
            values[i][j][6] = x3;
            float z3 = zFar + (j+1)*zGap;
            values[i][j][7] = z3;
            float y3 = f(moveFunX, x3,z3);
            values[i][j][8] = y3;
            //  light direction
            float light[3] = {0,0,1};
            float dot = norm[0]*light[0] + norm[1]*light[1] + norm[2]*light[2];
            float r = ((x + 6) / 12)*0.8 + 0.5*dot;
            values[i][j][9] = r;
            float g = ((z + 6) / 12)*0.8 + 0.5*dot;
            values[i][j][10] = g;
            float b = ((z + 6) / 12)*0.8 + 0.5*dot;
            values[i][j][11] = b;
        }
    }
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            glBegin(GL_LINE_LOOP);
                glVertex3f(values[i][j][0],values[i][j][1],values[i][j][2]);
                normalVector(moveFunX,values[i][j][0],values[i][j][1],values[i][j][2],norm);
                glNormal3fv(norm);
                glVertex3f(values[i][j][3],values[i][j][4],values[i][j][5]);
                normalVector(moveFunX,values[i][j][3],values[i][j][4],values[i][j][5],norm);
                glNormal3fv(norm);
                glVertex3f(values[i][j][6],values[i][j][7],values[i][j][8]);
                normalVector(moveFunX,values[i][j][6],values[i][j][7],values[i][j][8],norm);
                glNormal3fv(norm);
                glColor3f(values[i][j][9],values[i][j][10],values[i][j][11]);
            glEnd();
        }
    }
    glutSwapBuffers();
    frames++;
    t_end = time(NULL);
    if (t_end - t_start >= 1) {
        string fps = "FPS: " + to_string(frames / (t_end - t_start));
        cout << fps << endl;
        frames = 0;
        t_start = time(NULL);
    }
    // tiempo final
    double t_final = omp_get_wtime();
    // tiempo transcurrido
    double t_total = t_final - t_inicial;
    //cout << "Tiempo transcurrido: " << t_total << endl;
}

void idle()
{
    /*thetaY += increment;
    if(thetaY > 360.0)
        thetaY -= 360.0;*/
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