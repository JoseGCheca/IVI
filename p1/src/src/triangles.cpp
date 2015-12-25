#include <iostream>
using namespace std;

#include "vgl.h"
#include "LoadShaders.h"

/* Variables globales */
enum VAO_IDs    { Triangles, NumVAOs };
enum Buffer_IDs { ArrayBuffer, NumBuffers };
/* Para manipular info a nivel de vertex */
enum Attrib_IDs { vPosition = 0 };

/* Buffers */
GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];

const GLuint NumVertices = 6;

/* Inicialización de OpenGL */ 
void init () {
  /* Reserva NumVAOs objetos vertex-array en VAOs */
  glGenVertexArrays(NumVAOs, VAOs);
  /* Activa los objetos vertex-array */
  glBindVertexArray(VAOs[Triangles]);

  /* Geometría */
  GLfloat vertices[NumVertices][2] = {
    { -0.90, -0.90 },  /* Triángulo 1 */
    {  0.85, -0.90 },
    { -0.90,  0.85 },
    {  0.90, -0.85 },  /* Triángulo 2 */
    {  0.90,  0.90 },
    { -0.85,  0.90 }
  };

  /* Devuelve NumBuffers nombres para los objetos en Buffers */
  glGenBuffers(NumBuffers, Buffers);
  /* Activa los nombres devueltos */
  glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
  /* Asigna memoria para datos y los copia en la memoria de OpenGL */
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
	       vertices, GL_STATIC_DRAW);

  /* Shaders a utilizar */
  ShaderInfo  shaders[] = {
    { GL_VERTEX_SHADER,   "./shaders/triangles.vert" },
    { GL_FRAGMENT_SHADER, "./shaders/triangles.frag" },
    { GL_NONE, NULL }
  };

  /* Carga de shaders */ 
  GLuint program = LoadShaders(shaders);
  /* Uso del 'programa' asociado a los shaders cargados */
  glUseProgram(program);

  /* Especifica dónde se puede acceder a los valores para el atributo vertex */
  glVertexAttribPointer(vPosition, 2, GL_FLOAT,
			GL_FALSE, 0, BUFFER_OFFSET(0));
  /* Activa array de atributos */
  glEnableVertexAttribArray(vPosition);

  /* Establece el color de fondo *******/ 
  /* Más eficiente aquí que en display */
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

/* Retrollamada de rendering */
void display () {
  /* Limpia el framebuffer*/
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* Selecciona el array de vértices a utilizar */ 
  glBindVertexArray(VAOs[Triangles]);
  /* Envío de la geometría al pipeline de OpenGL*/
  glDrawArrays(GL_TRIANGLES, 0, NumVertices);

  /* Intercambio de buffers */
  glutSwapBuffers();
}

int main (int argc, char** argv) {   
  /* Inicializa glut */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE| GLUT_RGBA);
  glutInitWindowSize(512, 512);
  glutInitWindowPosition(200, 200);

  /* Especifica el 'contexto' de OpenGL - Versión 3.3 */
  glutInitContextVersion(3, 3);
  glutInitContextProfile(GLUT_CORE_PROFILE);
  glutCreateWindow("IVI - P2 GLSL");

  /* Información de la tarjeta gráfica */
  const GLubyte* renderer = glGetString (GL_RENDERER); 
  const GLubyte* version = glGetString (GL_VERSION); 
  cout << "Renderer: " << renderer << endl;
  cout << "OpenGL - Versión soportada " << version << endl;
  
  /* Inicializa la biblioteca GLEW - OpenGL Extension Wrangler */
  glewExperimental = GL_TRUE; 
  if (glewInit()) {
    cerr << "Imposible inicializar GLEW... saliendo." << endl;
    exit(EXIT_FAILURE);
  }
  
  /* Inicializa OpenGL */
  init();
  
  /* Retrollamada de rendering */
  glutDisplayFunc(display);
  
  /* Bucle de glut */
  glutMainLoop();
}
