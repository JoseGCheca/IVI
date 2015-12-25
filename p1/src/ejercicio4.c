#include <GL/glut.h>

static GLint rotate = 0;

void render () { 
  /* Limpieza de buffers */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /* Carga de la matriz identidad */
  glLoadIdentity();
  /* Posición de la cámara virtual (position, look, up) */
  gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);


  /* En color blanco */
  glColor3f( 1.0, 1.0, 1.0 ); 

  glRotatef(rotate, 1.0, 0.0, 0.0); 
  /* Renderiza la tetera */
  glutWireTeapot(1.5);
  /* Intercambio de buffers... Representation ---> Window */
 /* Transformaciones de la tierra */
  glutSwapBuffers();      
} 
void update_rotation(){
  rotate++;
  glutPostRedisplay();
}
void update_state(unsigned char key, int x, int y){
  switch (key){
    case 'x':
      glutIdleFunc(update_rotation);
    break;
    case 's':
      glutIdleFunc(NULL);
    break;
  }
}



void resize (int w, int h) { 
  /* Definición del viewport */
  glViewport(0, 0, w, h);

  /* Cambio a transform. vista */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  /* Actualiza el ratio ancho/alto */
  gluPerspective(50., w/(double)h, 1., 10.);

  /* Vuelta a transform. modelo */
  glMatrixMode(GL_MODELVIEW);
}

int main (int argc, char* argv[]) { 
  glutInit( &argc, argv ); 
  glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE ); 
  glutInitWindowSize(640, 480); 
  glutCreateWindow("Practica 2 - Ejercicio 4");
  glEnable (GL_DEPTH_TEST);
  
  /* Registro de funciones de retrollamada */
  glutDisplayFunc(render); 
  glutReshapeFunc(resize);
  glutKeyboardFunc(update_state);
  
  /* Bucle de renderizado */
  glutMainLoop();  
  
  return 0; 
} 
