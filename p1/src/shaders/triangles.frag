/* Versión de GLSL a usar - GLSL 3.3 */
#version 330 core

/* fColor representa el color del fragment *****************/
/* Tipo vec4 para vector de 4 elementos en punto flotante **/
/* Calificador out para enviar valores de color ************/
out vec4 fColor;

/* Asigna color en base a esquema RGB-alpha */
/* Color azul totalmente opaco */
void main()
{
	fColor = vec4(0.0, 0.0, 1.0, 1.0);
}