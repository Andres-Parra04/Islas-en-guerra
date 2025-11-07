#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "mapa.h"
#include "menu.h"
#include "../recursos/recursos.h" // ¡¡Añadir esto!!

int main() {
    char mapa[MAPA_F][MAPA_C];
    int px = 0, py = 0;
    int tecla;
    DWORD tInicio, tActual;
    
    // --- CREAR JUGADOR ---
    struct Jugador jugador1;
    // Definir dónde mostrar los stats (p.ej., Columna 85, Fila 1)
    // ¡Ajusta este valor si tu mapa es más ancho o estrecho!
    int stats_x = 85; 
    int stats_y = 1;

    mostrarMenu();  /* Menú de inicio con animación */

    // --- INICIALIZAR JUGADOR Y MAPA ---
    IniciacionRecursos(&jugador1, "Jugador 1");

    dibujarPanelFondo();

    inicializarMapa(mapa);
    srand((unsigned int)time(NULL));

    // Busca una posición inicial válida para el jugador
    do {
        px = rand() % MAPA_F;
        py = rand() % MAPA_C;
    } while (mapa[px][py] != '.');

    // Inicializar offset
    offset_f = px - FILAS / 2;
    offset_c = py - COLUMNAS / 2;
    if (offset_f < 0) offset_f = 0;
    if (offset_f > MAPA_F - FILAS) offset_f = MAPA_F - FILAS;
    if (offset_c < 0) offset_c = 0;
    if (offset_c > MAPA_C - COLUMNAS) offset_c = MAPA_C - COLUMNAS;

    mostrarMapa(mapa);

    // --- MOSTRAR STATS INICIALES ---
    mostrarStats(jugador1, STATS_X, STATS_Y);

    // Dibujar jugador
    moverCursor((py - offset_c) * 2 + 2, px - offset_f + 1); // +2 y +1 para estar dentro del marco
    setColor(0, 10);
    printf("P ");
    setColor(0, 15);

    moverCursor(0, FILAS + 2);
    printf("Usa W, A, S, D para moverte. ESC para salir.\n");

    tInicio = GetTickCount();

    while (1) {
        /* Animar el agua */
        tActual = GetTickCount();
        if (tActual - tInicio >= 300) {
            animarAgua(mapa);
            tInicio = tActual;
        }

        if (_kbhit()) {
            tecla = _getch();
            if (tecla == 27) break;
            if (tecla == 'w' || tecla == 'W' || tecla == 'a' || tecla == 'A' ||
                tecla == 's' || tecla == 'S' || tecla == 'd' || tecla == 'D') {
                
                // --- ¡¡LÓGICA DE MOVIMIENTO Y ACTUALIZACIÓN DE STATS!! ---
                // 1. Mover al jugador
                // 2. Comprobar si la función devolvió 1 (recurso recolectado)
                if (moverJugador(&jugador1, mapa, &px, &py, (char)tecla)) {
                    // 3. Si se recolectó, redibujar los stats
                    mostrarStats(jugador1, STATS_X, STATS_Y);
                }
            }
        }
    }

    moverCursor(0, FILAS + 3);
    setColor(0, 15);
    printf("Gracias por jugar ISLAS EN GUERRA ⚔️\n");
    return 0;
}