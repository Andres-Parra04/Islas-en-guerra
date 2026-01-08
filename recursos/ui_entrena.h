// recursos/ui_entrena.h
#ifndef UI_ENTRENA_H
#define UI_ENTRENA_H

#include "recursos.h"
#include <stdbool.h>
#include <windows.h>

// --- CONSTANTES DE ENTRENAMIENTO ---
#define COSTO_OBRERO_ORO 30
#define COSTO_OBRERO_COMIDA 20

#define COSTO_CABALLERO_ORO 35
#define COSTO_CABALLERO_COMIDA 30
#define COSTO_CABALLERO_MADERA 25
#define COSTO_CABALLERO_HIERRO 30

#define COSTO_CABALLERO_SIN_ESCUDO_ORO 25
#define COSTO_CABALLERO_SIN_ESCUDO_COMIDA 25
#define COSTO_CABALLERO_SIN_ESCUDO_MADERA 15
#define COSTO_CABALLERO_SIN_ESCUDO_HIERRO 0

#define COSTO_GUERRERO_ORO 30
#define COSTO_GUERRERO_COMIDA 25
#define COSTO_GUERRERO_MADERA 15
#define COSTO_GUERRERO_HIERRO 15

// Costos para mejorar el barco por nivel (muy altos)
#define COSTO_MEJORA_BARCO_2_ORO 250
#define COSTO_MEJORA_BARCO_2_MADERA 200
#define COSTO_MEJORA_BARCO_2_PIEDRA 150
#define COSTO_MEJORA_BARCO_2_HIERRO 100

#define COSTO_MEJORA_BARCO_3_ORO 500
#define COSTO_MEJORA_BARCO_3_MADERA 450
#define COSTO_MEJORA_BARCO_3_PIEDRA 350
#define COSTO_MEJORA_BARCO_3_HIERRO 250

#define COSTO_MEJORA_BARCO_4_ORO 1000
#define COSTO_MEJORA_BARCO_4_MADERA 800
#define COSTO_MEJORA_BARCO_4_PIEDRA 600
#define COSTO_MEJORA_BARCO_4_HIERRO 500

// --- ESTRUCTURA DEL MENÚ DE ENTRENAMIENTO ---
typedef struct MenuEntrenamiento {
  bool abierto;
  int pantallaX, pantallaY;     // Posición en pantalla del menú
  int ancho, alto;              // Dimensiones del menú
  RECT botonObrero;             // Botón para entrenar obrero
  RECT botonCaballero;          // Botón para entrenar caballero
  RECT botonCaballeroSinEscudo; // Botón para entrenar caballero sin escudo
  RECT botonGuerrero;           // Botón para entrenar guerrero
  RECT botonMejorarBarco;       // Botón para mejorar capacidad del barco
  RECT botonCerrar;             // Rectángulo del botón cerrar
  char mensajeError[100];       // Mensaje de error si recursos insuficientes
  int tiempoError;              // Contador para mostrar error temporalmente
} MenuEntrenamiento;

// --- FUNCIONES DEL MENÚ ---

// Inicializa el menú de entrenamiento
void menuEntrenamientoInicializar(MenuEntrenamiento *menu);

// Abre el menú centrado en la pantalla
void menuEntrenamientoAbrir(MenuEntrenamiento *menu, int anchoPantalla,
                            int altoPantalla);

// Cierra el menú
void menuEntrenamientoCerrar(MenuEntrenamiento *menu);

// Dibuja el menú en pantalla
void menuEntrenamientoDibujar(HDC hdc, MenuEntrenamiento *menu,
                              struct Jugador *jugador);

// Maneja clicks del mouse en el menú, retorna true si se procesó el click
bool menuEntrenamientoClick(MenuEntrenamiento *menu, struct Jugador *jugador,
                            int pantallaX, int pantallaY);

// Actualiza el estado del menú (para animaciones, timers, etc)
void menuEntrenamientoActualizar(MenuEntrenamiento *menu);

#endif
