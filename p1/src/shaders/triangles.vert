/* Pass-through shader */

/* Versión de GLSL a usar - GLSL 3.3 */
#version 330 core

/* vPosition mantiene información posicional del vértice ***/
/* Tipo vec4 para vector de 4 elementos en punto flotante **/
/* Modificador in - Datos de entrada al shader *************/
/* layout para proporcionar metadatos **********************/
layout( location = 0 ) in vec4 vPosition;

/* Copia la posición del vértice de entrada en gl_Position */
void main() 
{
	gl_Position = vPosition;
}