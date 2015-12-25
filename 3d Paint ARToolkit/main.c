#include <GL/glut.h>    
#include <AR/gsub.h>    
#include <AR/video.h>   
#include <AR/param.h>   
#include <AR/ar.h>
#include <AR/arMulti.h>

#define MAX_PAINTINGS 1000

size_t camera_size = 640 * 3 * 480; // Tamaño pixeles camara 

void print_error (char *error) {  printf("%s\n",error); exit(0); }

struct TPainter {            // Puntero dibujador"
  float r, g, b;  
  float size;              // Representa el tamaño del puntero                
};

enum TColor {                   // Colores disponibles para pintar
  red = 0, green, blue, black, white, last=white 
} color;                //last es para saber cual es el ultimo valor

struct TLine{
  float x1, y1, z1;             // Punto inicial
  float x2, y2, z2;             // Punto final
  struct TPainter painter; // dibujador painter
};

// --**** Definicion de constantes y variables globales ****---

ARMultiMarkerInfoT *mMarker;              // Estructura global Multimarca
int simple_patt_id;                       // Identificador unico de la marca simple
double simple_patt_trans[3][4];           // Matriz de transformacion de la marca simple
struct TPainter painter;                  // Valor del pincel
struct TLine lines[MAX_PAINTINGS];        // Almacena todas las lineas guardadas con el painter
int bLine;                                // Determina si dibujar o no
float lastMarkPoint[3];                   // Ultimo punto donde se detecto la marca 
int numLines;

// Cmabiar color del painter
void changeColor(enum TColor color) {
  switch(color) {
  case red:    painter.r = 1.0; painter.g = 0.0; painter.b = 0.0; break;
  case green:  painter.r = 0.0; painter.g = 1.0; painter.b = 0.0; break;
  case blue:   painter.r = 0.0; painter.g = 0.0; painter.b = 1.0; break;
  case black:  painter.r = 0.0; painter.g = 0.0; painter.b = 0.0; break;
  case white:  painter.r = 1.0; painter.g = 1.0; painter.b = 1.0; break;
  }
}

// ======== cleanup =================================================
static void cleanup(void) {   // Libera recursos al salir ...
  arVideoCapStop();   
  arVideoClose();   
  argCleanup();   
  exit(0);
}

// Manejar teclado

static void keyboard(unsigned char key, int x, int y) {
  switch (key) {
   case'c':
      if(color + 1 > last) 
        color = 0;
      else 
        color = color + 1;
      changeColor(color);
      break;
    case '+': 
      if (painter.size + 1 < 10)
        painter.size ++;
      break;
    case '-': 
      if (painter.size - 1 > 2)
        painter.size --;
      break;
    case 'P': case 'p': bLine = !bLine; break;

  }
}

void drawline (float w, float r, float g, float b, float x1, float y1, float z1, float x2, float y2, float z2) {
  glLineWidth(w); glColor3f(r, g, b);
  glBegin(GL_LINES);
  glVertex3f(x1, y1, z1);
  glVertex3f(x2, y2, z2);
  glEnd();
}

void drawReference () {
  glColor3f(1, 1, 1);
  glBegin(GL_QUADS);
  glVertex3f(0, 0, 0);
  glVertex3f(0, -210, 0);
  glVertex3f(297, -210, 0);
  glVertex3f(297, 0, 0);
  glEnd();

  drawline(5, 1, 0, 0, 0, 0, 0, 297, 0, 0);
  drawline(5, 0, 1, 0, 0, 0, 0, 0, -210, 0);
  drawline(5, 0, 0, 1, 0, 0, 0, 0, 0, 210);
}

// draw
void drawPointer(double m2[3][4]) {
  // Se dibuja el puntero
  glPushMatrix();
  glTranslatef(m2[0][3], m2[1][3], m2[2][3]);
  glColor3f(painter.r, painter.g, painter.b);
  glutSolidSphere(painter.size, 20, 20);
  glPopMatrix();
  
  // Dibujar proyecciones. Dibuja linea hasta borde, trasladas y dibujas
  drawline(0.5, 1.0, 0.0, 0.0, m2[0][3], m2[1][3], m2[2][3], m2[0][3], 0.0, m2[2][3]);
  glPushMatrix();
  glTranslatef(m2[0][3], 0.0, m2[2][3]);
  glColor3f(1.0, 0.0, 0.0);
  glutSolidSphere(2.0, 20, 20);
  glPopMatrix();
    
  drawline(0.5, 0.0, 1.0, 0.0, m2[0][3], m2[1][3], m2[2][3], m2[0][3], m2[1][3], 0.0);
  glPushMatrix();
  glTranslatef(m2[0][3], m2[1][3], 0.0);
  glColor3f(0.0, 1.0, 0.0);
  glutSolidSphere(2.0, 20, 20);
  glPopMatrix();

  drawline(0.5, 0.0, 0.0, 1.0, m2[0][3], m2[1][3], m2[2][3], 0.0, m2[1][3], m2[2][3]);
  glPushMatrix();
  glTranslatef(0.0, m2[1][3], m2[2][3]);
  glColor3f(0.0, 0.0, 1.0);
  glutSolidSphere(2.0, 20, 20);
  glPopMatrix();
}

void saveLine(double m2[3][4]) {
  if (numLines < MAX_PAINTINGS) {

    lines[numLines].x1 = lastMarkPoint[0];
    lines[numLines].y1 = lastMarkPoint[1];
    lines[numLines].z1 = lastMarkPoint[2];

    lines[numLines].x2 = m2[0][3];
    lines[numLines].y2 = m2[1][3];
    lines[numLines].z2 = m2[2][3];

    lines[numLines].painter.size = painter.size;
    lines[numLines].painter.r = painter.r;
    lines[numLines].painter.g = painter.g;
    lines[numLines].painter.b = painter.b;
    
    numLines ++;
  }
}

void paintLine() {
  for (int i = 0; i < numLines; ++i) {
    // Dibujar linea guardada
    drawline(lines[i].painter.size, lines[i].painter.r, lines[i].painter.g, lines[i].painter.b,
        lines[i].x1, lines[i].y1, lines[i].z1, lines[i].x2, lines[i].y2, lines[i].z2);
    
    // Dibujar proyeccion
    drawline(lines[i].painter.size, 0.5, 0.5, 0.5,
        lines[i].x1, lines[i].y1, 0, lines[i].x2, lines[i].y2, 0);
  }
}

// Pintar todo
void drawAll(int k) {
  double gl_para[16];        
  double m[3][4], m2[3][4];   

  if(k != -1) {
    arUtilMatInv(mMarker->trans, m);
    arUtilMatMul(m, simple_patt_trans, m2);
  }
  
  argDrawMode3D();              // Se cambia el contexto a 3D
  argDraw3dCamera(0, 0);        // Y la vista de la camara a 3D
  glClear(GL_DEPTH_BUFFER_BIT); // Se limpia buffer de profundidad
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  
  // Se dibuja el lienzo
  argConvGlpara(mMarker->trans, gl_para);
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixd(gl_para);
  
  drawReference();
  
  // Se dibuja el puntero si se ha detectado la marca
  if(k != -1) {
    drawPointer(m2);

    if (bLine)
      saveLine(m2);

    lastMarkPoint[0] = m2[0][3];
    lastMarkPoint[1] = m2[1][3];
    lastMarkPoint[2] = m2[2][3];
  }

  paintLine(); //Se dibujan las líneas guardadas
  
  glDisable(GL_DEPTH_TEST);
}

// ======== init ====================================================
static void init( void ) {
  ARParam  wparam, cparam;   // Parametros intrinsecos de la camara
  int xsize, ysize;          // Tamano del video de camara (pixels)

  // Abrimos dispositivo de video
  if(arVideoOpen("-dev=/dev/video0") < 0) exit(0);  
  if(arVideoInqSize(&xsize, &ysize) < 0) exit(0);

  // Cargamos los parametros intrinsecos de la camara
  if(arParamLoad("data/camera_para.dat", 1, &wparam) < 0)   
    print_error ("Error en carga de parametros de camara\n");
  
  arParamChangeSize(&wparam, xsize, ysize, &cparam);
  arInitCparam(&cparam);   // Inicializamos la camara con "cparam"

  // Cargamos el fichero de especificacion multimarca
  if( (mMarker = arMultiReadConfigFile("data/marker.dat")) == NULL )
    print_error("Error en fichero marker.dat\n");

  // Cargamos el fichero de la marca simple
  if((simple_patt_id = arLoadPatt("data/simple.patt")) < 0) 
    print_error ("Error en carga de patron\n");


  argInit(&cparam, 1.0, 0, 0, 0, 0);   // Abrimos la ventana 

  painter.size = 3.0;
  color = red;
  bLine = 0;
  numLines = 0;
  changeColor(red);
}

// ======== mainLoop ================================================
static void mainLoop(void) {
 ARUint8 *dataPtr;

  //reservamos espacio en memoria para nuevo puntero
  ARUint8 *dataPtr2 = malloc(sizeof(ARUint8) * 640 * 3 * 480);

  ARMarkerInfo *marker_info;
  int marker_num, j, k, a;

  double p_width = 120.0;        // Ancho del patron (marca)
  double p_center[2] = {0.0, 0.0};   // Centro del patron (marca)

  // Capturamos un frame de la camara de video
  if((dataPtr = (ARUint8 *)arVideoGetImage()) == NULL) {
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
  argDispImage(dataPtr2, 0,0);    // Dibujamos lo que ve la camara 

  // Detectamos la marca en el frame capturado (return -1 si error)
  if(arDetectMarker(dataPtr2, 100, &marker_info, &marker_num) < 0) {
    cleanup(); exit(0);   // Si devolvio -1, salimos del programa!
  }

  arVideoCapNext();      // Frame pintado y analizado... A por otro!
  
  // Si se detecta la multimarca (lienzo) se puede detectar el puntero
  if(mMarker != NULL) {
    // Se ve donde detecta el patron con mayor fiabilidad
    for(j = 0, k = -1; j < marker_num; j++) {
      if(simple_patt_id == marker_info[j].id) {
        if(k == -1) k = j;
        else if(marker_info[k].cf < marker_info[j].cf) k = j;
      }
    }
  }

  if(k != -1)   // Si ha detectado el patron en algun sitio...
    // Obtener transformacion relativa entre la marca y la camara real
    arGetTransMat(&marker_info[k], p_center, p_width, simple_patt_trans);

  else if (bLine) bLine = !bLine;  //Si deja de detectar la marca deja de almacenar lineas

  if(arMultiGetTransMat(marker_info, marker_num, mMarker) > 0)
    drawAll(k);      // Se dibujan todos los objetos
    free(dataPtr2);
  argSwapBuffers();  // Se cambia el buffer con lo que tenga dibujado
}


// ======== Main ====================================================
int main(int argc, char **argv) {
  glutInit(&argc, argv);    // Creamos la ventana OpenGL con Glut
  init();                   // Llamada a nuestra funcion de inicio
  
  arVideoCapStart();        // Creamos un hilo para captura de video
  argMainLoop( NULL, keyboard, mainLoop );   // Asociamos callbacks...
  return (0);
}
