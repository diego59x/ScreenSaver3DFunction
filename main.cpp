//#include "stdafx.h"
#include <math.h>
#include <string>
#include <iostream>
#include <GL/glut.h>
#include <time.h>
#include <omp.h>

using namespace std;

void init();
void display();
void idle();
int main(int argc, char** argv);
float f(float moveFunA, double x, double z);
void normalVector(float moveFunA, float x, float y, float z, float *norm);

float thetaY = 0; // angulo de rotacion de la camara en el eje Y
float thetaX = 25; // angulo de rotacion de la camara en el eje X
float rotationIncrement = 0.5, ampIncrement = 5, velIncrement = 1000; // incrementos
float xNear = -15, xFar = 15, yNear = -15, yFar = 15, zNear = -15, zFar = 15; // limites de la camara
// Definicion en los ejes X e Y del plano
const int n = 80;
const int m = 80;
float moveFunX = 0.0; // parametro para mover la funcion (animacion)
int t_start = time(NULL), t_end, frames = 0; // variables para calcular el fps

/*
    Inicializa la ventana y los parametros de la camara antes de entrar en el bucle principal
*/
void init()
{
    glClearColor(1.0, 1.0, 1.0, 0); // color de fondo
    glMatrixMode(GL_PROJECTION);
    glOrtho(xNear, xFar, yNear, yFar, zNear, zFar);
}

/*
    Funcion que se dibuja, recibe el parametro moveFunX para animar la funcion
*/
float f(float moveFunX, double x, double z)
{
    return moveFunX*cos(sqrt(x*x+z*z)*moveFunX*0.25);
}

/*
    setea los valores de la normal para x, y, z
*/
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

/*
    Funcion que se llama cada vez que se redibuja la pantalla
*/
void display()
{
    // tiempo de inicio
    double t_inicial = omp_get_wtime();
    // limpiar la pantalla y el buffer de profundidad
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // resetear transformaciones
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // rotar la camara (solo thetaY cambia continuamente)
    glRotatef(thetaX,1,0,0);
    glRotatef(thetaY,0,1,0);
    // espacio entre cada punto del plano
    double xGap = (xNear-xFar)/n;
    double zGap = (zNear-zFar)/m;
    // static to avoid store in stack and avoid memory allocation stack overflow
    static float points[n][m][9];
    static float colors[n][m][9];
    static float norm[n][m][3][3];
    // hace los calculos en paralelo de los valores para pintar cada triangulo del plano
    #pragma omp parallel for collapse(2) num_threads(10) schedule(static, 1)
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            // posicion del primer punto del triangulo
            float x = xFar + i*xGap;
            points[i][j][0] = x;
            float z = zFar + j*zGap;
            points[i][j][1] = z;
            float y = f(moveFunX, x,z);
            points[i][j][2] = y;
            normalVector(moveFunX, x, z, y, norm[i][j][0]);
            // posicion del segundo punto del triangulo
            float x2 = xFar + i*xGap;
            points[i][j][3] = x2;
            float z2 = zFar + (j+1)*zGap;
            points[i][j][4] = z2;
            float y2 = f(moveFunX, x2,z2);
            points[i][j][5] = y2;
            normalVector(moveFunX, x2, z2, y2, norm[i][j][1]);
            // posicion del tercer punto del triangulo
            float x3 = xFar + (i+1)*xGap;
            points[i][j][6] = x3;
            float z3 = zFar + (j+1)*zGap;
            points[i][j][7] = z3;
            float y3 = f(moveFunX, x3,z3);
            points[i][j][8] = y3;
            normalVector(moveFunX, x3, z3, y3, norm[i][j][2]);
            // color de la luz a partir de la direccion para el primer punto
            float light[3] = {0,0,1};
            float dot = norm[i][j][0][0]*light[0] + norm[i][j][0][1]*light[1] + norm[i][j][0][2]*light[2];
            float r = ((x + 6) / 12)*0.8 + 0.5*dot;
            colors[i][j][0] = r;
            float g = ((z + 6) / 12)*0.8 + 0.5*dot;
            colors[i][j][1] = g;
            float b = ((z + 6) / 12)*0.8 + 0.5*dot;
            colors[i][j][2] = b;
            // direccion de la luz para el segundo punto
            float dot2 = norm[i][j][1][0]*light[0] + norm[i][j][1][1]*light[1] + norm[i][j][1][2]*light[2];
            float r2 = ((x2 + 6) / 12)*0.8 + 0.5*dot2;
            colors[i][j][3] = r2;
            float g2 = ((z2 + 6) / 12)*0.8 + 0.5*dot2;
            colors[i][j][4] = g2;
            float b2 = ((z2 + 6) / 12)*0.8 + 0.5*dot2;
            colors[i][j][5] = b2;
            // direccion de la luz para el tercer punto
            float dot3 = norm[i][j][2][0]*light[0] + norm[i][j][2][1]*light[1] + norm[i][j][2][2]*light[2];
            float r3 = ((x3 + 6) / 12)*0.8 + 0.5*dot3;
            colors[i][j][6] = r3;
            float g3 = ((z3 + 6) / 12)*0.8 + 0.5*dot3;
            colors[i][j][7] = g3;
            float b3 = ((z3 + 6) / 12)*0.8 + 0.5*dot3;
            colors[i][j][8] = b3;
        }
    }
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            // dibuja el triangulo (Loop para unir los 3 puntos)
            glBegin(GL_LINE_LOOP);
                // dibuja el primer punto del triangulo
                glColor3f(colors[i][j][0],colors[i][j][1],colors[i][j][2]);
                glVertex3f(points[i][j][0],points[i][j][2],points[i][j][1]);
                glNormal3fv(norm[i][j][0]);
                // dibuja el segundo punto del triangulo
                glColor3f(colors[i][j][3],colors[i][j][4],colors[i][j][5]);
                glVertex3f(points[i][j][3],points[i][j][5],points[i][j][4]);
                glNormal3fv(norm[i][j][1]);
                // dibuja el tercer punto del triangulo
                glColor3f(colors[i][j][6],colors[i][j][7],colors[i][j][8]);
                glVertex3f(points[i][j][6],points[i][j][8],points[i][j][7]);
                glNormal3fv(norm[i][j][2]);
            glEnd();
        }
    }
    glutSwapBuffers();
    // contador de frames (solo pone en pantalla cada segundo, no cada renderizado)
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
    // tiempo transcurrido en segundos de este renderizado
    double t_total = t_final - t_inicial;
    cout << "Tiempo transcurrido: " << t_total << endl;
}

/*
    Funcion que se hace en el background, cada renderizado de pantalla.
*/
void idle()
{
    // mueve la camara en 360, al rededor de la funcion
    thetaY += rotationIncrement;
    if(thetaY > 360.0)
        thetaY -= 360.0;
    // mueve la funcion en el eje x, la anima
    moveFunX = ampIncrement*cos((float)glutGet(GLUT_ELAPSED_TIME)/velIncrement);
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
        // escape
        case 27:
            exit(0);
            break;
        case 'q':
            if (rotationIncrement > -3)
                rotationIncrement -= 0.1;
            break;
        case 'w':
            if (rotationIncrement < 3)
                rotationIncrement += 0.1;
            break;
        case 'a':
            if (ampIncrement > 0.1)
                ampIncrement -= 0.1;
            break;
        case 's':
            if (ampIncrement < 6)
                ampIncrement += 0.5;
            break;
        case 'z':
            if (velIncrement > 300)
                velIncrement -= 100;
            break;
        case 'x':
            if (velIncrement < 2000)
                velIncrement += 100;
            break;
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(800,800);
    glutInitWindowPosition(10,10);
    glutCreateWindow("Screen saver function math :P");
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    init();
    glutMainLoop();
    return 0;
}