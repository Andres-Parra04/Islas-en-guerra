#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "mapa/mapa.h"
#include "mapa/menu.h"
#include <signal.h>
#include "recursos/recursos.h"

int main() {
    // Ignorar señales de interrupción
    signal(SIGINT, SIG_IGN); 
    
    #if defined(SIGBREAK)
        signal(SIGBREAK, SIG_IGN);
    #endif
    
    #if defined(SIGTSTP)
        signal(SIGTSTP, SIG_IGN); 
    #endif
    
    #if defined(SIGQUIT)
        signal(SIGQUIT, SIG_IGN);
    #endif
    
    char mapa[MAPA_F][MAPA_C];
    int px = 0, py = 0;
    int tecla;
    DWORD tInicio, tActual;
    struct Jugador jugador1;
    
    // Mostrar menú de inicio
    mostrarMenu();
    
    // Inicializar jugador
    IniciacionRecursos(&jugador1, "Jugador 1");
    
    // CRÍTICO: Inicializar sistema de minas ANTES de crear el mapa
    inicializarSistemaMinas(); 
    
    // Inicializar mapa (esto registrará las minas en el sistema)
    inicializarMapa(mapa);
    
    // Inicializar generador de números aleatorios
    srand((unsigned int)time(NULL));
    
    // Encontrar posición inicial válida para el jugador
    do {
        px = rand() % MAPA_F;
        py = rand() % MAPA_C;
    } while (mapa[px][py] != '.');
    
    // Calcular offset inicial de la cámara
    offset_f = px - FILAS / 2;
    offset_c = py - COLUMNAS / 2;
    
    // Ajustar límites del offset
    if (offset_f < 0) offset_f = 0;
    if (offset_f > MAPA_F - FILAS) offset_f = MAPA_F - FILAS;
    if (offset_c < 0) offset_c = 0;
    if (offset_c > MAPA_C - COLUMNAS) offset_c = MAPA_C - COLUMNAS;
    
    // Mostrar mapa inicial
    mostrarMapa(mapa, px, py);
    dibujarPanelEnMapa(jugador1);
    
    // Mostrar instrucciones
    moverCursor(0, FILAS + 2);
    printf("Usa WASD para moverte. ENTER para interactuar. ESC para salir.\n");
    
    // Inicializar temporizador para animación de agua
    tInicio = GetTickCount();
    
    // ============================================================================
    // BUCLE PRINCIPAL DEL JUEGO
    // ============================================================================
    while (1) {
        // Actualizar sistema de minas (genera recursos cada 60 segundos)
        actualizarMinas();
        
        // Animar el agua cada 300 milisegundos
        tActual = GetTickCount();
        if (tActual - tInicio >= 300) {
            animarAgua(mapa);
            forzarRedibujoPanelEnMapa(jugador1);
            tInicio = tActual;
        }
        
        // Detectar entrada del teclado
        if (_kbhit()) {
            tecla = _getch();
            
            // ESC para salir
            if (tecla == 27) break;
            
            // Procesar movimiento e interacciones
            if (tecla == 'w' || tecla == 'W' || tecla == 'a' || tecla == 'A' ||
                tecla == 's' || tecla == 'S' || tecla == 'd' || tecla == 'D' ||
                tecla == 13) {
                
                // Ejecutar acción del jugador
                explorarMapa(&jugador1, mapa, &px, &py, (char)tecla);
                
                // Actualizar panel de recursos
                forzarRedibujoPanelEnMapa(jugador1);
            }
        }
        
        // Pequeña pausa para no consumir 100% del CPU
        Sleep(10);
    }
    
    // Mensaje de despedida
    moverCursor(0, FILAS + 3);
    setColor(0, 15);
    printf("Gracias por jugar ISLAS EN GUERRA\n");
    
    return 0;
}
