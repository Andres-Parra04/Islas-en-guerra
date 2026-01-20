
#include "ui_entrena.h"
#include <stdio.h>
#include <string.h>

void menuEntrenamientoInicializar(MenuEntrenamiento *menu) {
  menu->abierto = false;
  menu->ancho = 500;
  menu->alto = 450;
  menu->mensajeError[0] = '\0';
  menu->tiempoError = 0;
}

void menuEntrenamientoAbrir(MenuEntrenamiento *menu, int anchoPantalla,
                            int altoPantalla) {
  menu->abierto = true;
  menu->ancho = 560;
  menu->alto = 720; // Más alto para 5 botones (4 tropas + 1 mejora barco)
  menu->pantallaX = (anchoPantalla - menu->ancho) / 2;
  menu->pantallaY = (altoPantalla - menu->alto) / 2;

  int xCentro = menu->pantallaX + (menu->ancho / 2);

  // Configurar botones
  int btnAncho = 500;
  int btnAlto = 85;
  int espacioEntreBotones = 12;

  // Botón Entrenar Obrero
  menu->botonObrero.left = xCentro - (btnAncho / 2);
  menu->botonObrero.top = menu->pantallaY + 110;
  menu->botonObrero.right = xCentro + (btnAncho / 2);
  menu->botonObrero.bottom = menu->botonObrero.top + btnAlto;

  // Botón Entrenar Caballero con Escudo
  menu->botonCaballero.left = xCentro - (btnAncho / 2);
  menu->botonCaballero.top = menu->botonObrero.bottom + espacioEntreBotones;
  menu->botonCaballero.right = xCentro + (btnAncho / 2);
  menu->botonCaballero.bottom = menu->botonCaballero.top + btnAlto;

  // Botón Entrenar Caballero sin Escudo
  menu->botonCaballeroSinEscudo.left = xCentro - (btnAncho / 2);
  menu->botonCaballeroSinEscudo.top =
      menu->botonCaballero.bottom + espacioEntreBotones;
  menu->botonCaballeroSinEscudo.right = xCentro + (btnAncho / 2);
  menu->botonCaballeroSinEscudo.bottom =
      menu->botonCaballeroSinEscudo.top + btnAlto;

  // Botón Entrenar Guerrero
  menu->botonGuerrero.left = xCentro - (btnAncho / 2);
  menu->botonGuerrero.top =
      menu->botonCaballeroSinEscudo.bottom + espacioEntreBotones;
  menu->botonGuerrero.right = xCentro + (btnAncho / 2);
  menu->botonGuerrero.bottom = menu->botonGuerrero.top + btnAlto;

  // Botón Mejorar Barco (NUEVO)
  menu->botonMejorarBarco.left = xCentro - (btnAncho / 2);
  menu->botonMejorarBarco.top =
      menu->botonGuerrero.bottom + espacioEntreBotones;
  menu->botonMejorarBarco.right = xCentro + (btnAncho / 2);
  menu->botonMejorarBarco.bottom = menu->botonMejorarBarco.top + btnAlto;

  // Botón Cerrar
  menu->botonCerrar.left = xCentro - 80;
  menu->botonCerrar.top = menu->pantallaY + menu->alto - 55;
  menu->botonCerrar.right = xCentro + 80;
  menu->botonCerrar.bottom = menu->botonCerrar.top + 45;

  menu->mensajeError[0] = '\0';
  menu->tiempoError = 0;
}

void menuEntrenamientoCerrar(MenuEntrenamiento *menu) {
  menu->abierto = false;
  menu->mensajeError[0] = '\0';
  menu->tiempoError = 0;
}

// Helper para dibujar botones de opción
void dibujarBotonEntrenamiento(HDC hdc, RECT rect, const char *titulo,
                               const char *costo, const char *stats) {
  HBRUSH brush = CreateSolidBrush(RGB(80, 40, 40)); // Fondo rojo oscuro
  SelectObject(hdc, brush);
  HPEN pen = CreatePen(PS_SOLID, 2, RGB(200, 100, 100));
  SelectObject(hdc, pen);
  RoundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, 8, 8);

  DeleteObject(brush);
  DeleteObject(pen);

  SetBkMode(hdc, TRANSPARENT);

  // Título (Arriba)
  SetTextColor(hdc, RGB(255, 200, 100));
  HFONT fontTitulo =
      CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                 OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                 DEFAULT_PITCH | FF_SWISS, "Arial");
  HFONT oldFont = (HFONT)SelectObject(hdc, fontTitulo);
  TextOut(hdc, rect.left + 15, rect.top + 8, titulo, strlen(titulo));
  SelectObject(hdc, oldFont);
  DeleteObject(fontTitulo);

  // Costo (segunda línea)
  SetTextColor(hdc, RGB(220, 220, 220));
  HFONT fontCosto =
      CreateFont(13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                 OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                 DEFAULT_PITCH | FF_SWISS, "Arial");
  SelectObject(hdc, fontCosto);
  TextOut(hdc, rect.left + 15, rect.top + 32, costo, strlen(costo));

  // Stats (si están presentes)
  if (stats != NULL) {
    SetTextColor(hdc, RGB(150, 255, 150));
    TextOut(hdc, rect.left + 15, rect.top + 52, stats, strlen(stats));
  }

  SelectObject(hdc, oldFont);
  DeleteObject(fontCosto);
}

void menuEntrenamientoDibujar(HDC hdc, MenuEntrenamiento *menu,
                              struct Jugador *jugador) {
  if (!menu->abierto)
    return;

  // Fondo del menú
  HBRUSH brushFondo = CreateSolidBrush(RGB(50, 30, 30));
  HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brushFondo);
  HPEN penBorde = CreatePen(PS_SOLID, 3, RGB(200, 50, 50));
  HPEN oldPen = (HPEN)SelectObject(hdc, penBorde);

  Rectangle(hdc, menu->pantallaX, menu->pantallaY,
            menu->pantallaX + menu->ancho, menu->pantallaY + menu->alto);

  SelectObject(hdc, oldPen);
  DeleteObject(penBorde);

  // Título
  SetBkMode(hdc, TRANSPARENT);
  SetTextColor(hdc, RGB(255, 220, 100));
  HFONT fontTitulo =
      CreateFont(28, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                 OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                 DEFAULT_PITCH | FF_SWISS, "Arial");
  HFONT oldFont = (HFONT)SelectObject(hdc, fontTitulo);

  RECT rectTitulo = {menu->pantallaX + 20, menu->pantallaY + 20,
                     menu->pantallaX + menu->ancho - 20, menu->pantallaY + 60};
  DrawText(hdc, "CUARTEL DE ENTRENAMIENTO", -1, &rectTitulo,
           DT_CENTER | DT_VCENTER | DT_SINGLELINE);

  SelectObject(hdc, oldFont);
  DeleteObject(fontTitulo);

  // Fuente normal
  HFONT fontNormal =
      CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                 OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                 DEFAULT_PITCH | FF_SWISS, "Arial");
  SelectObject(hdc, fontNormal);

  // Recursos
  char buffer[100];
  SetTextColor(hdc, RGB(255, 255, 255));
  sprintf(buffer, "Oro:%d | Com:%d | Mad:%d | Pie:%d | Hie:%d",
          jugador->Oro, jugador->Comida, jugador->Madera, jugador->Piedra,
          jugador->Hierro);
  TextOut(hdc, menu->pantallaX + 20, menu->pantallaY + 70, buffer,
          strlen(buffer));

  // Separador
  MoveToEx(hdc, menu->pantallaX + 20, menu->pantallaY + 110, NULL);
  LineTo(hdc, menu->pantallaX + menu->ancho - 20, menu->pantallaY + 110);

  // DIBUJAR BOTONES
  char costo[100];
  char stats[150];

  // Botón Entrenar Obrero
  sprintf(costo, "Costo: %d Oro, %d Comida", COSTO_OBRERO_ORO,
          COSTO_OBRERO_COMIDA);
  dibujarBotonEntrenamiento(
      hdc, menu->botonObrero, "Entrenar Obrero", costo,
      "Unidad b\xE1sica para recolecci\xF3n de recursos.");

  // Botón Entrenar Caballero con Escudo
  sprintf(costo, "Costo: %d Oro, %d Comida, %d Madera, %d Hierro",
          COSTO_CABALLERO_ORO, COSTO_CABALLERO_COMIDA, COSTO_CABALLERO_MADERA,
          COSTO_CABALLERO_HIERRO);
  sprintf(stats,
          "Vida:%d | Da\xF1o:%d | Cr\xEDtico:%d%% | Def:%d | Alcance:Cuerpo a "
          "cuerpo",
          CABALLERO_VIDA, CABALLERO_DANO, CABALLERO_CRITICO, CABALLERO_DEFENSA);
  dibujarBotonEntrenamiento(hdc, menu->botonCaballero,
                            "Entrenar Caballero con Escudo", costo, stats);

  // Botón Entrenar Caballero sin Escudo
  sprintf(costo, "Costo: %d Oro, %d Comida, %d Madera",
          COSTO_CABALLERO_SIN_ESCUDO_ORO, COSTO_CABALLERO_SIN_ESCUDO_COMIDA,
          COSTO_CABALLERO_SIN_ESCUDO_MADERA);
  sprintf(stats,
          "Vida:%d | Da\xF1o:%d | Cr\xEDtico:%d%% | Def:%d | Alcance:Cuerpo a "
          "cuerpo",
          CABALLERO_SIN_ESCUDO_VIDA, CABALLERO_SIN_ESCUDO_DANO,
          CABALLERO_SIN_ESCUDO_CRITICO, CABALLERO_SIN_ESCUDO_DEFENSA);
  dibujarBotonEntrenamiento(hdc, menu->botonCaballeroSinEscudo,
                            "Entrenar Caballero sin Escudo", costo, stats);

  // Botón Entrenar Guerrero
  sprintf(costo, "Costo: %d Oro, %d Comida, %d Madera, %d Hierro",
          COSTO_GUERRERO_ORO, COSTO_GUERRERO_COMIDA, COSTO_GUERRERO_MADERA,
          COSTO_GUERRERO_HIERRO);
  sprintf(stats,
          "Vida:%d | Da\xF1o:%d | Cr\xEDtico:%d%% | Def:%d | Alcance:Cuerpo a "
          "cuerpo",
          GUERRERO_VIDA, GUERRERO_DANO, GUERRERO_CRITICO, GUERRERO_DEFENSA);
  dibujarBotonEntrenamiento(hdc, menu->botonGuerrero, "Entrenar Guerrero",
                            costo, stats);

  // Botón Barco: Construir o Mejorar según estado
  if (!jugador->barco.construido) {
    // === BARCO NO CONSTRUIDO: Mostrar opción de CONSTRUIR ===
    char tituloConstruir[] = "Construir Barco";
    char costoConstruir[120];
    char statsConstruir[] = "Repara el barco para poder navegar.";

    sprintf(costoConstruir, "Costo: %d Oro, %d Madera, %d Piedra, %d Hierro",
            COSTO_CONSTRUIR_BARCO_ORO, COSTO_CONSTRUIR_BARCO_MADERA,
            COSTO_CONSTRUIR_BARCO_PIEDRA, COSTO_CONSTRUIR_BARCO_HIERRO);

    dibujarBotonEntrenamiento(hdc, menu->botonMejorarBarco, tituloConstruir,
                              costoConstruir, statsConstruir);
  } else {
    // Variables para la lógica de mejora del barco
    int nivelActual = jugador->barco.nivelMejora;
    int capacidadActual = jugador->barco.capacidadMaxima;
    
    if (nivelActual < 4) {
    // === BARCO CONSTRUIDO: Mostrar opción de MEJORAR ===
    char tituloBarco[80];
    char costoBarco[120];
    char statsBarco[100];

    int siguienteNivel = nivelActual + 1;
    int siguienteCapacidad = siguienteNivel * 3 + 3; // 9, 12, 15

    sprintf(tituloBarco, "Mejorar Barco (Niv %d -> %d)", nivelActual,
            siguienteNivel);
    sprintf(statsBarco, "Capacidad: %d -> %d tropas", capacidadActual,
            siguienteCapacidad);

    // Determinar costos según el siguiente nivel
    int oro, madera, piedra, hierro;
    if (siguienteNivel == 2) {
      oro = COSTO_MEJORA_BARCO_2_ORO;
      madera = COSTO_MEJORA_BARCO_2_MADERA;
      piedra = COSTO_MEJORA_BARCO_2_PIEDRA;
      hierro = COSTO_MEJORA_BARCO_2_HIERRO;
    } else if (siguienteNivel == 3) {
      oro = COSTO_MEJORA_BARCO_3_ORO;
      madera = COSTO_MEJORA_BARCO_3_MADERA;
      piedra = COSTO_MEJORA_BARCO_3_PIEDRA;
      hierro = COSTO_MEJORA_BARCO_3_HIERRO;
    } else { // siguienteNivel == 4
      oro = COSTO_MEJORA_BARCO_4_ORO;
      madera = COSTO_MEJORA_BARCO_4_MADERA;
      piedra = COSTO_MEJORA_BARCO_4_PIEDRA;
      hierro = COSTO_MEJORA_BARCO_4_HIERRO;
    }

    sprintf(costoBarco, "Costo: %d Oro, %d Madera, %d Piedra, %d Hierro", oro,
            madera, piedra, hierro);

    dibujarBotonEntrenamiento(hdc, menu->botonMejorarBarco, tituloBarco,
                              costoBarco, statsBarco);
  } else {
    // Nivel máximo alcanzado
    HBRUSH brushMaximo = CreateSolidBrush(RGB(50, 50, 50)); // Gris oscuro
    SelectObject(hdc, brushMaximo);
    HPEN penMaximo = CreatePen(PS_SOLID, 2, RGB(100, 100, 100));
    SelectObject(hdc, penMaximo);
    RoundRect(hdc, menu->botonMejorarBarco.left, menu->botonMejorarBarco.top,
              menu->botonMejorarBarco.right, menu->botonMejorarBarco.bottom, 8,
              8);
    DeleteObject(brushMaximo);
    DeleteObject(penMaximo);

    SetTextColor(hdc, RGB(150, 150, 150));
    RECT rectTexto = menu->botonMejorarBarco;
    DrawText(hdc, "Barco al Nivel Máximo (15 tropas)", -1, &rectTexto,
             DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
  }

  SelectObject(hdc, oldFont);
  DeleteObject(fontNormal);

  // Botón CERRAR
  HBRUSH brushCerrar = CreateSolidBrush(RGB(150, 50, 50));
  SelectObject(hdc, brushCerrar);
  RoundRect(hdc, menu->botonCerrar.left, menu->botonCerrar.top,
            menu->botonCerrar.right, menu->botonCerrar.bottom, 10, 10);
  SetTextColor(hdc, RGB(255, 255, 255));
  DrawText(hdc, "CERRAR", -1, &menu->botonCerrar,
           DT_CENTER | DT_VCENTER | DT_SINGLELINE);
  DeleteObject(brushCerrar);

  // Mensaje de Error
  if (menu->tiempoError > 0) {
    SetTextColor(hdc, RGB(255, 50, 50)); // Red brighter
    
    // Crear fuente para el error
    HFONT fontError =
        CreateFont(22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                   OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                   DEFAULT_PITCH | FF_SWISS, "Arial");
    HFONT prevFont = (HFONT)SelectObject(hdc, fontError);

    // Posición inferior (entre el ultimo boton y boton cerrar)
    RECT rErr = {menu->pantallaX, menu->pantallaY + 600,
                 menu->pantallaX + menu->ancho, menu->pantallaY + 640};
    DrawText(hdc, menu->mensajeError, -1, &rErr,
             DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    SelectObject(hdc, prevFont);
    DeleteObject(fontError);
  }

  SelectObject(hdc, oldBrush);
  DeleteObject(brushFondo);
}

// Helper para procesar el entrenamiento de cualquier unidad
typedef bool (*EntrenarFunc)(struct Jugador *, float, float);

static void procesarEntrenamientoUnidad(MenuEntrenamiento *menu, struct Jugador *jugador, 
                                      RECT btn, EntrenarFunc func,
                                      int oro, int comida, int madera, int hierro,
                                      const char *errorMax) {
    if (jugador->Oro >= oro && jugador->Comida >= comida && 
        jugador->Madera >= madera && jugador->Hierro >= hierro) {
        if (func(jugador, 0, 0)) {
            jugador->Oro -= oro;
            jugador->Comida -= comida;
            jugador->Madera -= madera;
            jugador->Hierro -= hierro;
        } else {
            strcpy(menu->mensajeError, errorMax);
            menu->tiempoError = 60;
        }
    } else {
        strcpy(menu->mensajeError, "Recursos insuficientes!");
        menu->tiempoError = 60;
    }
}

// Helper para verificar clic en botón
static bool isClicked(int x, int y, RECT r) {
    return (x >= r.left && x <= r.right && y >= r.top && y <= r.bottom);
}

bool menuEntrenamientoClick(MenuEntrenamiento *menu, struct Jugador *jugador,
                            int pantallaX, int pantallaY) {
  if (!menu->abierto)
    return false;

  // Fuera del menú?
  if (pantallaX < menu->pantallaX ||
      pantallaX > menu->pantallaX + menu->ancho ||
      pantallaY < menu->pantallaY || pantallaY > menu->pantallaY + menu->alto) {
    return false;
  }

  // Declaraciones extern para las funciones de recursos
  extern bool entrenarObrero(struct Jugador * j, float x, float y);
  extern bool entrenarCaballero(struct Jugador * j, float x, float y);
  extern bool entrenarCaballeroSinEscudo(struct Jugador * j, float x, float y);
  extern bool entrenarGuerrero(struct Jugador * j, float x, float y);

  // --- ENTRENAR OBRERO ---
  if (isClicked(pantallaX, pantallaY, menu->botonObrero)) {
      procesarEntrenamientoUnidad(menu, jugador, menu->botonObrero, entrenarObrero,
                                COSTO_OBRERO_ORO, COSTO_OBRERO_COMIDA, 0, 0,
                                "No hay espacio para mas obreros!");
      return true;
  }

  // --- ENTRENAR CABALLERO CON ESCUDO ---
  if (isClicked(pantallaX, pantallaY, menu->botonCaballero)) {
      procesarEntrenamientoUnidad(menu, jugador, menu->botonCaballero, entrenarCaballero,
                                COSTO_CABALLERO_ORO, COSTO_CABALLERO_COMIDA, 
                                COSTO_CABALLERO_MADERA, COSTO_CABALLERO_HIERRO,
                                "No hay espacio para mas caballeros!");
      return true;
  }

  // --- ENTRENAR CABALLERO SIN ESCUDO ---
  if (isClicked(pantallaX, pantallaY, menu->botonCaballeroSinEscudo)) {
      procesarEntrenamientoUnidad(menu, jugador, menu->botonCaballeroSinEscudo, entrenarCaballeroSinEscudo,
                                COSTO_CABALLERO_SIN_ESCUDO_ORO, COSTO_CABALLERO_SIN_ESCUDO_COMIDA, 
                                COSTO_CABALLERO_SIN_ESCUDO_MADERA, COSTO_CABALLERO_SIN_ESCUDO_HIERRO,
                                "No hay espacio para mas caballeros!");
      return true;
  }

  // --- ENTRENAR GUERRERO ---
  if (isClicked(pantallaX, pantallaY, menu->botonGuerrero)) {
      procesarEntrenamientoUnidad(menu, jugador, menu->botonGuerrero, entrenarGuerrero,
                                COSTO_GUERRERO_ORO, COSTO_GUERRERO_COMIDA, 
                                COSTO_GUERRERO_MADERA, COSTO_GUERRERO_HIERRO,
                                "No hay espacio para mas guerreros!");
      return true;
  }

  // --- BARCO (CONSTRUIR o MEJORAR) ---
  if (isClicked(pantallaX, pantallaY, menu->botonMejorarBarco)) {
    
    if (!jugador->barco.construido) {
      // Construir barco
      extern bool construirBarco(struct Jugador *j);
      if (construirBarco(jugador)) {
        strcpy(menu->mensajeError, "Barco construido con exito!");
        menu->tiempoError = 80;
      } else {
        strcpy(menu->mensajeError, "Recursos insuficientes!");
        menu->tiempoError = 60;
      }
    } else if (jugador->barco.nivelMejora < 4) {
      // Mejorar barco
      extern bool mejorarBarco(struct Jugador *j);
      if (mejorarBarco(jugador)) {
        strcpy(menu->mensajeError, "Barco mejorado con exito!");
        menu->tiempoError = 80;
      } else {
        strcpy(menu->mensajeError, "Recursos insuficientes!");
        menu->tiempoError = 60;
      }
    } else {
      strcpy(menu->mensajeError, "El barco ya esta al nivel maximo!");
      menu->tiempoError = 60;
    }
    return true;
  }

  // Cerrar
  if (isClicked(pantallaX, pantallaY, menu->botonCerrar)) {
    menuEntrenamientoCerrar(menu);
    return true;
  }

  return true; // Click dentro del menú
}

void menuEntrenamientoActualizar(MenuEntrenamiento *menu) {
  if (menu->tiempoError > 0) {
    menu->tiempoError--;
  }
}
