#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h> //Necesario para poder posicionar en cualqueir lado de la pantalla los datos de los jugadores.
#include "../mapa/menu.h"
#include "../mapa/mapa.h"
#include "recursos.h"


void IniciacionRecursos (struct Jugador *j ,const char *Nombre){
	strcpy(j->Nombre,Nombre);
	j->Comida=200;
	j->Oro=100;
	j->Madera=150;
	j->Piedra=100;	
	j->Ejercito= NULL;
	j->NumeroTropas=0;
	j->Capacidad=0;
	j->CantidadCaballeria=0;
	j->CantidadArqueros=0;
	j->CantidadPicas=0;
	j->CantidadEspadas=0;
}
void IniciacionTropa (struct Tropa *t, const char *Nombre, int Oro , int Comida, int Madera, int Piedra, int Vida, int Fuerza , int VelocidadAtaque,int DistanciaAtaque){
	strcpy (t->Nombre,Nombre);
	t->CostoComida= Comida;
	t->CostoOro= Oro;
	t->CostoMadera=Madera;
	t->CostoPiedra=Piedra;
	t->Vida=Vida;
	t->Fuerza=Fuerza;
	t->VelocidadAtaque=VelocidadAtaque;
	t->DistanciaAtaque=DistanciaAtaque;
	
	
}
void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
void mostrarStats(struct Jugador j, int x, int y) {
    // Usamos gotoxy para posicionar el cursor
    // El 'x' debe ser un valor a la derecha de tu mapa
    // (p.ej., si tu mapa usa 80 columnas, usa x = 85)
    
    // Limpiar área por si acaso (opcional)
    
    setColor(0, 15); // Blanco
  	gotoxy(x, y + 0);
   	printf("--- %s ---     ", j.Nombre); // Espacios para borrar texto antiguo
  	gotoxy(x, y + 1);
    setColor(0, 14); // Amarillo
  	printf("Oro   : %d     ", j.Oro);
  	gotoxy(x, y + 2);
    setColor(0, 12); // Rojo
  	printf("Comida: %d     ", j.Comida);
  	gotoxy(x, y + 3);
    setColor(0, 6); // Marrón/Naranja
  	printf("Madera: %d     ", j.Madera);
  	gotoxy(x, y + 4);
    setColor(0, 8); // Gris
  	printf("Piedra: %d     ", j.Piedra);
    
    // Puedes añadir más stats si quieres
  	gotoxy(x, y + 6);
    setColor(0, 7); // Gris claro
  	printf("Tropas: %d     ", j.NumeroTropas);
    
    setColor(0, 15); // Restaurar color
}

void dibujarPanelFondo() {
    int i, j;
    // Usamos el color 0 (Negro) y el texto blanco (15)
    setColor(0, 15); 

    // --- PARTE 1: DIBUJAR EL PANEL DE STATS (Como antes) ---
    for (i = 0; i < PANEL_HEIGHT; i++) {
        gotoxy(STATS_X, STATS_Y + i);
        
        if (i == 0 || i == PANEL_HEIGHT - 1) {
             for (j = 0; j < PANEL_WIDTH; j++) {
                printf("#");
             }
        } else {
             printf("#"); // Borde izquierdo
             for (j = 1; j < PANEL_WIDTH - 1; j++) {
                printf(" "); // Espacios internos
             }
             printf("#"); // Borde derecho
        }
    }
    
    // --- ¡¡PARTE 2: LIMPIAR EL ÁREA VERDE DE ABAJO!! ---
    // Poner el fondo negro (0) y texto negro (0)
    setColor(0, 0); 
    
    // Empezar *debajo* del panel de stats (STATS_Y + PANEL_HEIGHT)
    // y continuar hasta el final de la ventana del mapa (FILAS + 1)
    for (i = STATS_Y + PANEL_HEIGHT; i <= FILAS + 1; i++) { 
        gotoxy(STATS_X, i);
        // Dibujar espacios en negro para cubrir el ancho del panel
        for (j = 0; j < PANEL_WIDTH; j++) {
            printf(" ");
        }
    }

    setColor(0, 15); // Restaurar color
}