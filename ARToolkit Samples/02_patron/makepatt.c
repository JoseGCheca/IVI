#include <string.h>
#include <GL/glut.h>    
#include <AR/gsub.h>    
#include <AR/video.h>   
#include <AR/param.h>   
#include <AR/ar.h>

// ==== Definicion de constantes y variables globales ===============
ARMarkerInfo *gPatt;          // Patron a guardar 
char         gPattName[128];  // Nombre del fichero (patron)
ARUint8      *gImage=NULL;    // Ultima imagen capturada
int          imgSize=0;       // Tamano en bytes de la imagen
int          xsize, ysize;    // Tamano del video de camara (pixels)

void print_error (char *error) {  printf(error); exit(0); }
void draw_line (double x1, double y1, double x2, double y2) {
  glBegin(GL_LINES);  
  glVertex2f(x1, ysize - y1);
  glVertex2f(x2, ysize - y2);  
  glEnd();
}
// ======== cleanup =================================================
static void cleanup(void) {   
  arVideoCapStop();            // Libera recursos al salir ...
  arVideoClose();
  argCleanup();
  free(gImage);
  exit(0);
}

// ======== draw ====================================================
static void draw( void ) {
  glLineWidth(5);
  glColor3d(0, 1, 0);
  draw_line(gPatt->vertex[0][0], gPatt->vertex[0][1],
	    gPatt->vertex[1][0], gPatt->vertex[1][1]);
  draw_line(gPatt->vertex[1][0], gPatt->vertex[1][1],
	    gPatt->vertex[2][0], gPatt->vertex[2][1]);
  draw_line(gPatt->vertex[2][0], gPatt->vertex[2][1],
	    gPatt->vertex[3][0], gPatt->vertex[3][1]);
  draw_line(gPatt->vertex[3][0], gPatt->vertex[3][1],
	    gPatt->vertex[0][0], gPatt->vertex[0][1]);
}

// ======== keyboard ================================================
static void keyboard(unsigned char key, int x, int y) {
  switch (key) {
  case 0x1B: case 'Q': case 'q':
    cleanup(); break;
  case 's': case'S':
    if (gPatt != NULL) {
      if (arSavePatt(gImage, gPatt, gPattName)<0) 
	printf ("Error guardando patron %s\n", gPattName);
      else printf ("Patron %s guardado correctamente\n", gPattName);    
      cleanup(); 
    } else printf ("Patron actualmente no detectado\n");
    break;
  }
}

// ======== init ====================================================
static void init( void ) {
  ARParam  wparam, cparam;   // Parametros intrinsecos de la camara
  
  // Abrimos dispositivo de video
  if(arVideoOpen("") < 0) exit(0);  
  if(arVideoInqSize(&xsize, &ysize) < 0) exit(0);

  // Cargamos los parametros intrinsecos de la camara
  if(arParamLoad("data/camera_para.dat", 1, &wparam) < 0)   
    print_error ("Error en carga de parametros de camara\n");
  
  arParamChangeSize(&wparam, xsize, ysize, &cparam);
  arInitCparam(&cparam);   // Inicializamos la camara con "cparam"

  imgSize = cparam.xsize * cparam.ysize * AR_PIX_SIZE_DEFAULT;
  arMalloc(gImage, ARUint8, imgSize); // Reservamos memoria Imagen

  argInit(&cparam, 1.0, 0, 0, 0, 0);   // Abrimos la ventana 
}

// ======== mainLoop ================================================
static void mainLoop(void) {
  ARUint8 *dataPtr;
  ARMarkerInfo *marker_info;
  int marker_num, i, maxarea;

   // Capturamos un frame de la camara de video
  if((dataPtr = (ARUint8 *)arVideoGetImage()) == NULL) {
    // Si devuelve NULL es porque no hay un nuevo frame listo
    arUtilSleep(2);  return;  // Dormimos el hilo 2ms y salimos
  }

  argDrawMode2D();
  argDispImage(dataPtr, 0,0);    // Dibujamos lo que ve la camara 

  // Detectamos la marca en el frame capturado (return -1 si error)
  if(arDetectMarker(dataPtr, 100, &marker_info, &marker_num) < 0) {
    cleanup(); exit(0);   // Si devolvio -1, salimos del programa!
  }

  // Nos quedamos con el patron detectado de mayor tamano
  for(i = 0, maxarea=0, gPatt=NULL; i < marker_num; i++) {
    if(marker_info[i].area > maxarea){
      maxarea = marker_info[i].area;
      gPatt = &(marker_info[i]);
      memcpy(gImage, dataPtr, imgSize);
    }
  }

  if(gPatt != NULL) draw();      

  argSwapBuffers(); // Cambiamos el buffer con lo que tenga dibujado
  arVideoCapNext(); // Frame pintado y analizado... A por otro!
}

// ======== Main ====================================================
int main(int argc, char **argv) {
  glutInit(&argc, argv);    // Creamos la ventana OpenGL con Glut

  if (argc == 2)                   // El primer argumento es el 
    strcpy (gPattName, argv[1]);   //  nombre del patron
  else {printf ("Llamada %s <nombre_patron>\n", argv[0]); exit(0);}

  init();                   // Llamada a nuestra funcion de inicio
  
  arVideoCapStart();        // Creamos un hilo para captura de video
  argMainLoop( NULL, keyboard, mainLoop );  // Asociamos callbacks...
  return (0);
}
