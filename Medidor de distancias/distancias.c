#include <math.h>
#include <GL/glut.h>
#include <AR/gsub.h>
#include <AR/video.h>
#include <AR/param.h>
#include <AR/ar.h>
#include <math.h>
#include <string.h>

// ==== Definicion de constantes y variables globales ===============


size_t camera_size = 640 * 3 * 480; // Explicacion en main LOOP


GLfloat vertexLine[3];
char distString[100];

// ==== Definicion de estructuras ===================================
struct TObject {
  int id;                      // Identificador del patron
  int visible;                 // Es visible el objeto?
  double width;                // Ancho del patron
  double center[2];            // Centro del patron
  double patt_trans[3][4];     // Matriz asociada al patron
  void (* drawme)(void);       // Puntero a funcion drawme
};

struct TObject *objects = NULL;
int nobjects = 0;
double dist01;                 // Distancia entre el objeto 0 y 1

void print_error (char *error) {  printf(error); exit(0); }

// ==== addObject (Anade objeto a la lista de objetos) ==============
void addObject(char *p, double w, double c[2], void (*drawme)(void))
{
  int pattid;

  if ((pattid = arLoadPatt(p)) < 0)
    print_error ("Error en carga de patron\n");

  nobjects++;
  objects = (struct TObject *)
            realloc(objects, sizeof(struct TObject) * nobjects);

  objects[nobjects - 1].id = pattid;
  objects[nobjects - 1].width = w;
  objects[nobjects - 1].center[0] = c[0];
  objects[nobjects - 1].center[1] = c[1];
  objects[nobjects - 1].drawme = drawme;
}

// ==== draw****** (Dibujado especifico de cada objeto) =============

void drawString(void *font, char *string , GLfloat x, GLfloat y, GLfloat z)
{
  glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);
  glDisable(GL_LIGHTING);  // deshabilitamos lighting para el color
  glDisable(GL_TEXTURE_2D);

  glColor3f(0.0, 1.0, 0.0);   // color
  glRasterPos3f(x / 4, y, z);  // posicion del texto

  // loop string
  while (*string) {

    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *string++);
  }

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_LIGHTING);
  glPopAttrib();


}


void renderDist(void) {
  GLfloat material[]     = {0.0, 0.0, 0.0, 1.0};
  float value = 0.0;            // Intensidad del gris a dibujar
  float color [] = {0.0, 0.0, 0.0};
  // La intensidad se asigna en funcion de la distancia "dist01"
  // Mapear valor intensidad linealmente entre 160 y 320 -> (1..0)
  value = (320 - dist01) / 160.0;

  if (value < 0) color[0] = 1.0, color[1] = 0.0, color[2] = 0.0;
  if (value > 1) color[0] = 0.0, color[1] = 1.0, color[2] = 0.0;
  if (value < 1 && value >= 0) color[0] = 1.0, color[1] = value, color[2] = 0.0;

  material[0] = color[0];
  material[1] = color[1];
  material[2] = color[2];

  glMaterialfv(GL_FRONT, GL_SPECULAR, material);
  glLineWidth(10);
  glBegin(GL_LINES);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(vertexLine[0], vertexLine[1], vertexLine[2]);
  glEnd();

  //dibujamos string de distancia
  drawString(GLUT_BITMAP_TIMES_ROMAN_24, distString, vertexLine[0], vertexLine[1], vertexLine[2]);
  glutSwapBuffers();
}

// ======== cleanup =================================================
static void cleanup(void) {   // Libera recursos al salir ...
  arVideoCapStop();  arVideoClose();  argCleanup();  free(objects);
  exit(0);
}

// ======== keyboard ================================================
static void keyboard(unsigned char key, int x, int y) {
  switch (key) {
  case 0x1B: case 'Q': case 'q':
    cleanup(); break;
  }
}


// ======== draw ====================================================
void draw( void ) {
  double  gl_para[16];   // Esta matriz 4x4 es la usada por OpenGL
  GLfloat light_position[]  = {100.0, -200.0, 200.0, 0.0};
  double m[3][4], m2[3][4];
  int i;

  argDrawMode3D();              // Cambiamos el contexto a 3D
  argDraw3dCamera(0, 0);        // Y la vista de la camara a 3D
  glClear(GL_DEPTH_BUFFER_BIT); // Limpiamos buffer de profundidad
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  if (objects[0].visible && objects[1].visible) {
    arUtilMatInv(objects[0].patt_trans, m);
    arUtilMatMul(m, objects[1].patt_trans, m2);
    dist01 = sqrt(pow(m2[0][3], 2) + pow(m2[1][3], 2) + pow(m2[2][3], 2));
    printf ("Distancia objects[0] y objects[1]= %G\n", dist01);
    vertexLine[0] = m2[0][3];
    vertexLine[1] = m2[1][3];
    vertexLine[2] = m2[2][3];
    double dist01cm = dist01 / 10;
    sprintf(distString, "Distancia = %.2f cm", dist01cm);
  } else {
    vertexLine[0] = 0;
    vertexLine[1] = 0;
    vertexLine[2] = 0;
    sprintf(distString, "");


  }

  for (i = 0; i < nobjects; i++) {
    if ((objects[i].visible) && (objects[i].drawme != NULL)) {
      argConvGlpara(objects[i].patt_trans, gl_para);
      glMatrixMode(GL_MODELVIEW);
      glLoadMatrixd(gl_para);   // Cargamos su matriz de transf.

      glEnable(GL_LIGHTING);  glEnable(GL_LIGHT0);
      glLightfv(GL_LIGHT0, GL_POSITION, light_position);
      objects[i].drawme();      // Llamamos a su función de dibujar
    }
  }
  glDisable(GL_DEPTH_TEST);
}

// ======== init ====================================================
static void init( void ) {
  ARParam  wparam, cparam;   // Parametros intrinsecos de la camara
  int xsize, ysize;          // Tamano del video de camara (pixels)
  double c[2] = {0.0, 0.0};  // Centro de patron (por defecto)

  // Abrimos dispositivo de video
  if (arVideoOpen("-dev=/dev/video0") < 0) exit(0);
  if (arVideoInqSize(&xsize, &ysize) < 0) exit(0);

  // Cargamos los parametros intrinsecos de la camara
  if (arParamLoad("data/camera_para.dat", 1, &wparam) < 0)
    print_error ("Error en carga de parametros de camara\n");

  arParamChangeSize(&wparam, xsize, ysize, &cparam);
  arInitCparam(&cparam);   // Inicializamos la camara con "cparam"

  // Inicializamos la lista de objetos
  addObject("data/identic.patt", 120.0, c, renderDist);
  addObject("data/simple.patt", 90.0, c, NULL);


  argInit(&cparam, 1.0, 0, 0, 0, 0);   // Abrimos la ventana
}

// ======== mainLoop ================================================
static void mainLoop(void) {
  ARUint8 *dataPtr;
  //reservamos espacio en memoria para nuevo puntero
  ARUint8 *dataPtr2 = malloc(sizeof(ARUint8) * 640 * 3 * 480);
  ARMarkerInfo *marker_info;
  int marker_num, i, j, k, a;

  // Capturamos un frame de la camara de video
  if ((dataPtr = (ARUint8 *)arVideoGetImage()) == NULL) {
    // Si devuelve NULL es porque no hay un nuevo frame listo
    arUtilSleep(2);  return;  // Dormimos el hilo 2ms y salimos
  }

  //** Estas líneas de código son para dar la vuelta a mi cámara que en Ubuntu debe de fun *//
  //*  funcionar mal el driver. Cojo el puntero dataptr y los datos de la imagen los volteo junto con los */
  /** pixeles rgb**/

  for (a = 0; a < camera_size; a = a + 3) {
    dataPtr2[camera_size - 1 - a] = dataPtr[a + 2];
    dataPtr2[camera_size - 2 - a] = dataPtr[a + 1];
    dataPtr2[camera_size - 3 - a] = dataPtr[a];
  }

  argDrawMode2D();

  // Dibujo el nuevo dataptr2
  argDispImage(dataPtr2, 0, 0);   // Dibujamos lo que ve la camara

  // Detectamos la marca en el frame capturado (return -1 si error)
  if (arDetectMarker(dataPtr2, 100, &marker_info, &marker_num) < 0) {
    cleanup(); exit(0);   // Si devolvio -1, salimos del programa!
  }

  arVideoCapNext();      // Frame pintado y analizado... A por otro!

  // Vemos donde detecta el patron con mayor fiabilidad
  for (i = 0; i < nobjects; i++) {
    for (j = 0, k = -1; j < marker_num; j++) {
      if (objects[i].id == marker_info[j].id) {
        if (k == -1) k = j;
        else if (marker_info[k].cf < marker_info[j].cf) k = j;
      }
    }

    if (k != -1) {  // Si ha detectado el patron en algun sitio...
      objects[i].visible = 1;
      arGetTransMat(&marker_info[k], objects[i].center,
                    objects[i].width, objects[i].patt_trans);
    } else { objects[i].visible = 0; }  // El objeto no es visible
  }

  draw();           // Dibujamos los objetos de la escena
  free(dataPtr2);
  argSwapBuffers(); // Cambiamos el buffer con lo que tenga dibujado
}

// ======== Main ====================================================
int main(int argc, char **argv) {
  glutInit(&argc, argv);    // Creamos la ventana OpenGL con Glut
  init();                   // Llamada a nuestra funcion de inicio

  arVideoCapStart();        // Creamos un hilo para captura de video
  argMainLoop( NULL, keyboard, mainLoop );    // Asociamos callbacks
  return (0);
}