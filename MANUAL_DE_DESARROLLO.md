# 📚 MANUAL DE DESARROLLO - Islas en Guerra

## Guía de Aprendizaje para Dominar el Motor del Juego

**Versión:** 1.0  
**Fecha:** Enero 2026  
**Motor:** C con Win32 API (GDI)

---

## 📑 ÍNDICE

1. [El "Mapa Mental" del Proyecto](#1-el-mapa-mental-del-proyecto)
2. [Dominando el Mapa y las Matrices](#2-dominando-el-mapa-y-las-matrices)
3. [Sistema de Entidades (Cómo agregar personajes)](#3-sistema-de-entidades-cómo-agregar-personajes)
4. [Lógica de Movimiento y Colisiones](#4-lógica-de-movimiento-y-colisiones)
5. [Gestión de Recursos (BMP y Memoria)](#5-gestión-de-recursos-bmp-y-memoria)
6. [Retos de Aprendizaje](#6-retos-de-aprendizaje)

---

## 1. EL "MAPA MENTAL" DEL PROYECTO

### 1.1 Flujo de Ejecución: Del `main()` al Primer Sprite

Imagina el juego como una orquesta: el `main()` es el director que coordina cuándo entra cada músico (módulo).

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                          FLUJO DE ARRANQUE                                  │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   main()                                                                    │
│     │                                                                       │
│     ├──► RegisterClassA(&wc)  ─────► Registra la clase de ventana Win32    │
│     │         └──► ProcedimientoVentana  (callback para eventos)           │
│     │                                                                       │
│     ├──► mostrarMenu()  ─────► Muestra menú principal en consola           │
│     │         └──► menuObtenerAccion() → Nueva partida o Cargar            │
│     │                                                                       │
│     ├──► mapaSeleccionarIsla(isla)  ─────► Selecciona BMP de isla          │
│     │                                                                       │
│     ├──► cargarRecursosGraficos()  ─────► ¡AQUÍ SE CARGAN LOS SPRITES!     │
│     │         ├──► LoadImageA() para mapa (isla1.bmp, etc)                 │
│     │         ├──► loadBmp() para árboles, unidades, barco                 │
│     │         └──► generarBosqueAutomatico()  → Coloca árboles/vacas       │
│     │                                                                       │
│     ├──► edificiosCargarSprites()  ─────► Carga ayuntamiento, mina, etc.   │
│     │                                                                       │
│     ├──► CreateWindowEx()  ─────► Crea la ventana del juego                │
│     │         └──► Dispara WM_CREATE                                       │
│     │                                                                       │
│     └──► GetMessage() loop  ─────► Bucle de mensajes Windows               │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 1.2 El Evento `WM_CREATE`: Donde Nace el Juego

Cuando se crea la ventana, `ProcedimientoVentana` recibe `WM_CREATE`. Es como el "constructor" del juego:

```c
case WM_CREATE:
    // 1. Reiniciar sistema de navegación
    navegacionReiniciarEstado();
    
    // 2. Inicializar recursos del jugador (obreros, comida, etc.)
    IniciacionRecursos(&jugador1, "Jugador 1");
    
    // 3. Guardar isla inicial
    jugador1.islaActual = menuObtenerIsla();
    
    // 4. Crear edificios en posiciones fijas
    edificioInicializar(&ayuntamiento, EDIFICIO_AYUNTAMIENTO, 1024.0f-64.0f, 1024.0f-64.0f);
    edificioInicializar(&mina, EDIFICIO_MINA, 1024.0f-64.0f, 450.0f);
    edificioInicializar(&cuartel, EDIFICIO_CUARTEL, 1024.0f-100.0f, 1400.0f);
    
    // 5. Marcar edificios en mapa de colisiones
    mapaMarcarEdificio(ayuntamiento.x, ayuntamiento.y, ...);
    
    // 6. Inicializar menús UI
    menuCompraInicializar(&menuCompra);
    menuEmbarqueInicializar(&menuEmbarque);
    
    // 7. Crear timer para 60 FPS (16ms)
    SetTimer(hwnd, IDT_TIMER_JUEGO, 16, NULL);
```

### 1.3 El Bucle de Renderizado (`WM_PAINT`)

Cada vez que la pantalla necesita actualizarse, Windows envía `WM_PAINT`:

```c
case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    
    // dibujarMundo() hace TODO el trabajo de renderizado
    dibujarMundo(hdc, rect, camara, &jugador1, &menuCompra, ...);
    
    EndPaint(hwnd, &ps);
}
```

### 1.4 Diagrama de Interacción de Archivos

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    ARQUITECTURA DE ARCHIVOS                                 │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│                           ┌──────────────┐                                  │
│                           │   main.c     │                                  │
│                           │  (Director)  │                                  │
│                           └──────┬───────┘                                  │
│                                  │                                          │
│          ┌───────────────────────┼───────────────────────┐                  │
│          │                       │                       │                  │
│          ▼                       ▼                       ▼                  │
│   ┌─────────────┐         ┌─────────────┐         ┌─────────────┐          │
│   │  mapa/      │         │  recursos/  │         │  batallas/  │          │
│   │  ├─mapa.h   │◄───────►│  ├─recursos.h│◄───────►│  ├─batallas.h│         │
│   │  └─mapa.c   │         │  └─recursos.c│         │  └─batallas.c│         │
│   └─────────────┘         └──────┬──────┘         └─────────────┘          │
│          │                       │                                          │
│          │                ┌──────┴──────┐                                   │
│          │                │  edificios/ │                                   │
│          │                │  ├─edificios.h                                  │
│          │                │  └─edificios.c                                  │
│          │                └─────────────┘                                   │
│          │                                                                  │
│          ▼                                                                  │
│   ┌─────────────┐         ┌─────────────┐         ┌─────────────┐          │
│   │  menu.h     │         │  navegacion.h│        │  guardado/  │          │
│   │  menu.c     │         │  navegacion.c│        │  guardado.h │          │
│   │(Menú inicio)│         │(Viaje islas)│         │  guardado.c │          │
│   └─────────────┘         └─────────────┘         └─────────────┘          │
│                                                                             │
│                           ┌─────────────┐                                   │
│                           │   assets/   │                                   │
│                           │ (Imágenes)  │                                   │
│                           │ ├─islas/    │                                   │
│                           │ ├─obrero/   │                                   │
│                           │ ├─caballero/│                                   │
│                           │ └─...       │                                   │
│                           └─────────────┘                                   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

**Explicación de dependencias:**

| Archivo | Responsabilidad Principal |
|---------|--------------------------|
| `main.c` | Punto de entrada, gestión de ventana Win32, bucle de eventos |
| `mapa/mapa.h/.c` | Renderizado del mundo, matrices de colisión, detección de agua |
| `recursos/recursos.h/.c` | Estructuras de jugador, unidades, movimiento, pathfinding |
| `recursos/edificios/` | Lógica y sprites de edificios (ayuntamiento, mina, cuartel) |
| `recursos/navegacion.h/.c` | Sistema de viaje entre islas, gestión de enemigos por isla |
| `batallas/batallas.h/.c` | Lógica de combate entre unidades |
| `batallas/guardado/` | Sistema de guardado/carga de partidas |

---

## 2. DOMINANDO EL MAPA Y LAS MATRICES

### 2.1 La Matriz como Representación del Mundo

Piensa en la matriz como un tablero de ajedrez gigante donde cada casilla puede contener algo diferente.

**Dimensiones del mundo:**

```c
#define MAPA_SIZE 2048      // El mapa mide 2048x2048 píxeles
#define TILE_SIZE 64        // Cada celda mide 64x64 píxeles
#define GRID_SIZE (MAPA_SIZE / TILE_SIZE)  // 2048/64 = 32x32 celdas
```

**Visualización:**

```
                        32 columnas
          ┌─────────────────────────────────────┐
          │  0   1   2   3   4 ... 30  31       │
        0 │ [~] [~] [~] [~] [~]     [~] [~]     │  '~' = Agua
        1 │ [~] [.] [A] [.] [.]     [~] [~]     │  '.' = Tierra vacía
        2 │ [~] [.] [.] [V] [.]     [A] [~]     │  'A' = Árbol
    32    3 │ [~] [O] [.] [.] [E]     [.] [~]     │  'O' = Obrero
   filas  4 │ [~] [.] [.] [.] [.]     [.] [~]     │  'E' = Edificio
        ..│                                     │  'V' = Vaca
       31 │ [~] [~] [~] [~] [~]     [~] [~]     │  'B' = Barco
          └─────────────────────────────────────┘
```

### 2.2 La Matriz de Objetos (`mapaObjetos`)

Esta matriz de **caracteres** almacena qué hay en cada celda:

```c
// Declaración global en mapa.c
char mapaObjetos[GRID_SIZE][GRID_SIZE];  // 32x32 = 1024 celdas

// Símbolos definidos en mapa.h
#define SIMBOLO_AGUA '~'      // Agua (no transitable)
#define SIMBOLO_VACIO '.'     // Terreno vacío
#define SIMBOLO_ARBOL 'A'     // Árbol (obstáculo)
#define SIMBOLO_OBRERO 'O'    // Obrero
#define SIMBOLO_CABALLERO 'C' // Caballero
#define SIMBOLO_GUERRERO 'G'  // Guerrero
#define SIMBOLO_VACA 'V'      // Vaca
#define SIMBOLO_BARCO 'B'     // Barco
#define SIMBOLO_EDIFICIO 'E'  // Edificio
#define SIMBOLO_MINA 'M'      // Mina
#define SIMBOLO_CUARTEL 'Q'   // Cuartel
```

### 2.3 Conversión: Píxeles ↔ Celdas

**¡Concepto clave!** El mundo del juego existe en dos espacios:

1. **Espacio de píxeles** (0-2048): Donde se posicionan los sprites
2. **Espacio de celdas** (0-31): Donde se almacena la lógica

```c
// De píxeles a celda
static int pixelACelda(float px) {
    int c = (int)(px / (float)TILE_SIZE);  // Dividir por 64
    return clampInt(c, 0, GRID_SIZE - 1);  // Asegurar rango 0-31
}

// De celda a píxel (centro de la celda)
static float celdaCentroPixel(int celda) {
    return (float)(celda * TILE_SIZE) + (float)(TILE_SIZE / 2);
    // celda * 64 + 32 = centro de la celda en píxeles
}
```

**Ejemplo práctico:**

```
Un obrero en posición (450.0f, 320.0f):
- Celda X = 450 / 64 = 7 (columna 7)
- Celda Y = 320 / 64 = 5 (fila 5)
- El obrero está en mapaObjetos[5][7]
```

### 2.4 Tutorial: Cómo Mover un Objeto con Aritmética de Punteros

La especificación académica requiere usar **aritmética de punteros**. Veamos cómo funciona:

```c
// FUNCIÓN: Mueve un objeto de una celda a otra en mapaObjetos
void mapaMoverObjeto(float viejoX, float viejoY, 
                     float nuevoX, float nuevoY, char simbolo) {
    
    // 1. CONVERTIR COORDENADAS DE PÍXELES A CELDAS
    int viejaFila = (int)(viejoY / TILE_SIZE);  // Fila = Y / 64
    int viejaCol  = (int)(viejoX / TILE_SIZE);  // Columna = X / 64
    int nuevaFila = (int)(nuevoY / TILE_SIZE);
    int nuevaCol  = (int)(nuevoX / TILE_SIZE);
    
    // 2. LIMPIAR CELDA VIEJA USANDO PUNTEROS
    // *(mapaObjetos + fila) obtiene el puntero a la fila
    // *(puntero_fila + columna) accede al carácter específico
    if (viejaFila >= 0 && viejaFila < GRID_SIZE && 
        viejaCol >= 0 && viejaCol < GRID_SIZE) {
        
        // Forma tradicional: mapaObjetos[viejaFila][viejaCol] = SIMBOLO_VACIO;
        // Forma con punteros:
        char *ptrFilaVieja = *(mapaObjetos + viejaFila);  // Fila como puntero
        *(ptrFilaVieja + viejaCol) = SIMBOLO_VACIO;        // Limpiar celda
    }
    
    // 3. MARCAR CELDA NUEVA
    if (nuevaFila >= 0 && nuevaFila < GRID_SIZE && 
        nuevaCol >= 0 && nuevaCol < GRID_SIZE) {
        
        char *ptrFilaNueva = *(mapaObjetos + nuevaFila);
        *(ptrFilaNueva + nuevaCol) = simbolo;
    }
}
```

**Analogía de punteros:**

Imagina que `mapaObjetos` es un archivador con 32 cajones (filas). Cada cajón tiene 32 carpetas (columnas).

- `mapaObjetos + fila` = "Abre el cajón número `fila`"
- `*(mapaObjetos + fila)` = "Dame el contenido del cajón" (el puntero a las carpetas)
- `*(ptrFila + columna)` = "Dame la carpeta número `columna`"

### 2.5 La Matriz de Colisiones (`gCollisionMap`)

Mientras `mapaObjetos` guarda **qué** hay, `gCollisionMap` guarda **si se puede pasar**:

```c
// Matriz dinámica de enteros (int**)
static int **gCollisionMap = NULL;

// Valores posibles:
// 0 = Celda libre (transitable)
// 1 = Obstáculo permanente (agua, árbol)
// 2 = Edificio (impasable)
// 3 = Ocupado temporalmente (unidad, vaca)
```

**Asignación de memoria (requerimiento académico):**

```c
static void collisionMapAllocIfNeeded(void) {
    if (gCollisionMap) return;  // Ya existe
    
    // 1. Asignar array de punteros (filas)
    gCollisionMap = (int **)malloc(GRID_SIZE * sizeof(int *));
    
    // 2. Por cada fila, asignar array de columnas
    for (int i = 0; i < GRID_SIZE; i++) {
        // *(gCollisionMap + i) es equivalente a gCollisionMap[i]
        *(gCollisionMap + i) = (int *)calloc(GRID_SIZE, sizeof(int));
    }
}
```

### 2.6 Relación Matriz-Pantalla

El renderizado transforma las coordenadas del mundo a coordenadas de pantalla:

```c
// En dibujarMundo():
// 1. Obtener posición del sprite en el mundo
float mundoX = unidad->x;
float mundoY = unidad->y;

// 2. Aplicar cámara y zoom
int pantallaX = (int)((mundoX - camara.x) * camara.zoom);
int pantallaY = (int)((mundoY - camara.y) * camara.zoom);

// 3. Calcular tamaño escalado del sprite
int tamano = (int)(64 * camara.zoom);

// 4. Dibujar solo si está visible
if (pantallaX + tamano > 0 && pantallaX < anchoPantalla &&
    pantallaY + tamano > 0 && pantallaY < altoPantalla) {
    StretchBlt(hdc, pantallaX, pantallaY, tamano, tamano, ...);
}
```

---

## 3. SISTEMA DE ENTIDADES (Cómo agregar personajes)

### 3.1 La Estructura `Unidad`

Todas las unidades del juego (obreros, caballeros, guerreros) comparten la misma estructura:

```c
typedef struct {
    // === POSICIÓN Y MOVIMIENTO ===
    float x, y;               // Posición actual en píxeles (0-2048)
    float destinoX, destinoY; // Destino del movimiento
    bool moviendose;          // ¿Está en movimiento?
    Direccion dir;            // DIR_FRONT, DIR_BACK, DIR_LEFT, DIR_RIGHT
    int frame;                // Frame de animación actual
    
    // === PATHFINDING ===
    int objetivoFila, objetivoCol;  // Celda destino
    int *rutaCeldas;                // Array de celdas a recorrer
    int rutaLen;                    // Longitud de la ruta
    int rutaIdx;                    // Índice actual en la ruta
    
    // === SINCRONIZACIÓN CON MATRIZ ===
    int celdaFila, celdaCol;  // Celda actual ocupada
    
    // === ANIMACIÓN ===
    const Animacion *animActual;  // Puntero a estado de animación
    int animTick;                 // Contador de ticks
    
    // === TIPO Y ESTADÍSTICAS ===
    TipoUnidad tipo;  // TIPO_OBRERO, TIPO_CABALLERO, etc.
    float vida, vidaMax;
    float dano, critico, defensa, alcance;
    
    // === ESTADO DE COMBATE ===
    bool seleccionado;
    bool recibiendoAtaque;
    ULONGLONG tiempoMuerteMs;  // Para animación de muerte
    int frameMuerte;
} Unidad;
```

### 3.2 Tipos de Unidades

```c
typedef enum {
    TIPO_OBRERO,              // Trabajador (recolecta, construye)
    TIPO_CABALLERO,           // Soldado con escudo (tanque)
    TIPO_CABALLERO_SIN_ESCUDO,// Soldado rápido (DPS)
    TIPO_GUERRERO,            // Soldado cuerpo a cuerpo
    TIPO_BARCO                // Transporte marítimo
} TipoUnidad;
```

### 3.3 Guía Paso a Paso: Crear una Nueva Unidad

**Ejemplo: Agregar un "Aldeano"**

#### Paso 1: Definir el Tipo (en `recursos.h`)

```c
// Añadir al enum TipoUnidad:
typedef enum {
    TIPO_OBRERO,
    TIPO_CABALLERO,
    TIPO_CABALLERO_SIN_ESCUDO,
    TIPO_GUERRERO,
    TIPO_BARCO,
    TIPO_ALDEANO  // ← NUEVO
} TipoUnidad;

// Añadir constantes de stats:
#define ALDEANO_VIDA 80
#define ALDEANO_VELOCIDAD 3.0f
#define MAX_ALDEANOS 50

// Añadir al struct Jugador:
struct Jugador {
    // ... otros campos ...
    Unidad aldeanos[MAX_ALDEANOS];  // ← NUEVO
};
```

#### Paso 2: Definir el Símbolo (en `mapa.h`)

```c
#define SIMBOLO_ALDEANO 'D'  // 'D' de aldeano (evitar letras usadas)
```

#### Paso 3: Preparar los Sprites (en carpeta `assets/`)

Crear carpeta `assets/aldeano/` con:
- `aldeano_front.bmp` (64x64 píxeles)
- `aldeano_back.bmp`
- `aldeano_left.bmp`
- `aldeano_right.bmp`

#### Paso 4: Declarar Variables de Sprites (en `mapa.c`)

```c
// Al inicio de mapa.c, junto a los otros sprites:
static HBITMAP hAldeanoBmp[4] = {NULL};  // Front, Back, Left, Right

// Definir rutas:
#define ALDEANO_FRONT "../assets/aldeano/aldeano_front.bmp"
#define ALDEANO_BACK "../assets/aldeano/aldeano_back.bmp"
#define ALDEANO_LEFT "../assets/aldeano/aldeano_left.bmp"
#define ALDEANO_RIGHT "../assets/aldeano/aldeano_right.bmp"
```

#### Paso 5: Cargar Sprites (en `cargarRecursosGraficos()`)

```c
void cargarRecursosGraficos() {
    // ... código existente ...
    
    // Cargar sprites del aldeano
    hAldeanoBmp[0] = loadBmp(ALDEANO_FRONT, 64, 64);
    hAldeanoBmp[1] = loadBmp(ALDEANO_BACK, 64, 64);
    hAldeanoBmp[2] = loadBmp(ALDEANO_LEFT, 64, 64);
    hAldeanoBmp[3] = loadBmp(ALDEANO_RIGHT, 64, 64);
}
```

#### Paso 6: Inicializar Unidades (en `recursos.c`)

```c
void IniciacionRecursos(struct Jugador *j, const char *Nombre) {
    // ... código existente ...
    
    // Inicializar aldeanos (similar a obreros)
    for (int i = 0; i < MAX_ALDEANOS; i++) {
        Unidad *a = &j->aldeanos[i];
        a->x = -1000.0f;  // Inactivo por defecto
        a->y = -1000.0f;
        a->tipo = TIPO_ALDEANO;
        a->vidaMax = ALDEANO_VIDA;
        a->vida = ALDEANO_VIDA;
        a->moviendose = false;
        a->seleccionado = false;
        a->celdaFila = -1;
        a->celdaCol = -1;
        a->rutaCeldas = NULL;
        a->animActual = animPorDireccion(DIR_FRONT);
    }
}
```

#### Paso 7: Función para Entrenar/Crear Aldeano

```c
bool entrenarAldeano(struct Jugador *j, float x, float y) {
    // Buscar slot libre
    for (int i = 0; i < MAX_ALDEANOS; i++) {
        Unidad *a = &j->aldeanos[i];
        if (a->x < 0) {  // Slot inactivo
            a->x = x;
            a->y = y;
            a->vida = ALDEANO_VIDA;
            a->vidaMax = ALDEANO_VIDA;
            a->moviendose = false;
            a->seleccionado = false;
            a->dir = DIR_FRONT;
            a->tipo = TIPO_ALDEANO;
            
            // Registrar en matriz
            mapaRegistrarObjeto(x, y, SIMBOLO_ALDEANO);
            
            return true;
        }
    }
    return false;  // No hay slots libres
}
```

#### Paso 8: Actualizar Movimiento (en `actualizarPersonajes()`)

```c
void actualizarPersonajes(struct Jugador *j) {
    int **col = mapaObtenerCollisionMap();
    if (!col) return;
    
    actualizarGrupoUnidades(j->obreros, MAX_OBREROS, col);
    actualizarGrupoUnidades(j->caballeros, MAX_CABALLEROS, col);
    // ... otros grupos ...
    actualizarGrupoUnidades(j->aldeanos, MAX_ALDEANOS, col);  // ← NUEVO
}
```

#### Paso 9: Renderizar (en `dibujarMundo()` de `mapa.c`)

```c
// Función auxiliar para dibujar aldeanos
static void dibujarAldeanos(HDC hdcBuffer, struct Jugador *j, 
                            Camara cam, int anchoP, int altoP) {
    for (int i = 0; i < MAX_ALDEANOS; i++) {
        Unidad *a = &j->aldeanos[i];
        if (a->x < 0 || a->vida <= 0) continue;
        
        int pantX = (int)((a->x - cam.x) * cam.zoom);
        int pantY = (int)((a->y - cam.y) * cam.zoom);
        int tam = (int)(64 * cam.zoom);
        
        // Culling: no dibujar si está fuera de pantalla
        if (pantX + tam <= 0 || pantX >= anchoP ||
            pantY + tam <= 0 || pantY >= altoP) continue;
        
        // Seleccionar sprite según dirección
        HBITMAP sprite = hAldeanoBmp[a->dir];
        
        // Dibujar con transparencia
        HDC hdcSprites = CreateCompatibleDC(hdcBuffer);
        SelectObject(hdcSprites, sprite);
        TransparentBlt(hdcBuffer, pantX, pantY, tam, tam,
                       hdcSprites, 0, 0, 64, 64,
                       RGB(255, 0, 255));  // Magenta = transparente
        DeleteDC(hdcSprites);
    }
}

// En dibujarMundo(), añadir la llamada:
void dibujarMundo(...) {
    // ... código existente ...
    dibujarObreros(hdcBuffer, pJugador, cam, anchoP, altoP);
    dibujarAldeanos(hdcBuffer, pJugador, cam, anchoP, altoP);  // ← NUEVO
}
```

### 3.4 Resumen de Archivos a Modificar

| Archivo | Cambios |
|---------|---------|
| `recursos/recursos.h` | Añadir `TIPO_ALDEANO`, constantes, campo en `Jugador` |
| `mapa/mapa.h` | Añadir `SIMBOLO_ALDEANO` |
| `mapa/mapa.c` | Variables de sprites, carga en `cargarRecursosGraficos()`, función de dibujo |
| `recursos/recursos.c` | Inicialización en `IniciacionRecursos()`, función `entrenarAldeano()` |
| `assets/aldeano/` | Crear carpeta con 4 sprites BMP |

---

## 4. LÓGICA DE MOVIMIENTO Y COLISIONES

### 4.1 El Sistema de Movimiento

El movimiento usa un sistema de **pathfinding BFS** (Breadth-First Search) que encuentra la ruta más corta evitando obstáculos.

**Flujo de movimiento:**

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                      SISTEMA DE MOVIMIENTO                                  │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   1. CLICK DERECHO                                                          │
│         │                                                                   │
│         ▼                                                                   │
│   2. rtsComandarMovimiento(jugador, mundoX, mundoY)                         │
│         │                                                                   │
│         ├──► Convertir (mundoX, mundoY) a celda destino (gF, gC)            │
│         │                                                                   │
│         └──► Para cada unidad seleccionada:                                 │
│                   │                                                         │
│                   ▼                                                         │
│         3. pathfindSimple(startF, startC, goalF, goalC, collision, ...)     │
│                   │                                                         │
│                   ├──► BFS encuentra ruta evitando colisiones               │
│                   │                                                         │
│                   └──► Guarda ruta en unidad->rutaCeldas[]                  │
│                                                                             │
│   4. CADA FRAME (WM_TIMER)                                                  │
│         │                                                                   │
│         ▼                                                                   │
│   5. actualizarGrupoUnidades(grupo, count, collisionMap)                    │
│         │                                                                   │
│         ├──► Lee siguiente celda de la ruta                                 │
│         │                                                                   │
│         ├──► Calcula dirección y velocidad                                  │
│         │                                                                   │
│         ├──► Actualiza (x, y) acercándose al centro de celda                │
│         │                                                                   │
│         └──► Cuando llega: actualiza celdaFila/Col, avanza rutaIdx          │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 4.2 Algoritmo de Movimiento (Código Explicado)

```c
static void actualizarGrupoUnidades(Unidad *grupo, int count, int **col) {
    const float vel = 5.0f;           // Píxeles por frame
    const float umbralLlegada = 8.0f; // Distancia para considerar "llegó"
    
    for (int i = 0; i < count; i++) {
        Unidad *u = &grupo[i];
        
        // Saltar unidades inactivas
        if (u->x < 0) continue;
        
        // Si no tiene celda asignada, registrarla
        if (u->celdaFila == -1) {
            int fila = obreroFilaActual(u);
            int col = obreroColActual(u);
            ocupacionActualizarUnidad(col, u, fila, col);
        }
        
        // Si no se está moviendo, saltar
        if (!u->moviendose) continue;
        
        // Obtener siguiente celda de la ruta
        if (u->rutaCeldas && u->rutaIdx < u->rutaLen) {
            int targetCelda = u->rutaCeldas[u->rutaIdx];
            int nextF = targetCelda / GRID_SIZE;  // Fila
            int nextC = targetCelda % GRID_SIZE;  // Columna
            
            // Verificar si la celda está bloqueada
            int valor = *(*(col + nextF) + nextC);
            if (valor == 1 || valor == 2) {
                // Obstáculo permanente: cancelar movimiento
                u->moviendose = false;
                obreroLiberarRuta(u);
                continue;
            }
            
            // Calcular posición objetivo (centro de la celda)
            float tx = celdaCentroPixel(nextC);
            float ty = celdaCentroPixel(nextF);
            
            // Calcular vector hacia el objetivo
            float cx = u->x + (TILE_SIZE / 2);  // Centro de la unidad
            float cy = u->y + (TILE_SIZE / 2);
            float vx = tx - cx;
            float vy = ty - cy;
            float dist = sqrtf(vx*vx + vy*vy);
            
            // Actualizar dirección visual
            if (fabsf(vx) > fabsf(vy)) {
                u->dir = (vx > 0) ? DIR_RIGHT : DIR_LEFT;
            } else {
                u->dir = (vy > 0) ? DIR_FRONT : DIR_BACK;
            }
            
            // ¿Llegamos a la celda?
            if (dist <= umbralLlegada) {
                // Snap a la posición exacta
                u->x = tx - (TILE_SIZE / 2);
                u->y = ty - (TILE_SIZE / 2);
                u->rutaIdx++;  // Avanzar a siguiente celda
                
                // Actualizar ocupación en matriz
                ocupacionActualizarUnidad(col, u, nextF, nextC);
                
                // ¿Terminamos la ruta?
                if (u->rutaIdx >= u->rutaLen) {
                    u->moviendose = false;
                    obreroLiberarRuta(u);
                }
            } else {
                // Mover hacia el objetivo
                float newX = u->x + (vx / dist) * vel;
                float newY = u->y + (vy / dist) * vel;
                
                // Clamp dentro del mapa
                if (newX < 0) newX = 0;
                if (newY < 0) newY = 0;
                if (newX > MAPA_SIZE - 64) newX = MAPA_SIZE - 64;
                if (newY > MAPA_SIZE - 64) newY = MAPA_SIZE - 64;
                
                u->x = newX;
                u->y = newY;
            }
        }
    }
}
```

### 4.3 Sistema de Colisiones

#### Detección de Agua (Color del BMP)

El juego detecta agua analizando los **colores del mapa BMP**:

```c
// Función que determina si un color RGB es "agua"
static inline bool esColorAgua(BYTE r, BYTE g, BYTE b, int tema) {
    if (tema == 4) {  // Isla de hielo
        return (r < 20 && g < 80 && b > 100);
    }
    if (tema == 5) {  // Isla de fuego
        return (r < 50 && b > 80 && b > g + 40);
    }
    // Tema clásico: azul predominante
    return ((b > r + 20 && b > g + 20 && b > 60) || 
            (b > r && b > g && b > 100));
}

// Proceso completo de detección de agua
static void detectarAguaEnMapa(void) {
    // 1. Obtener los píxeles del BMP del mapa
    BYTE *pixelData = ...; // Se extrae con GetDIBits()
    
    // 2. Recorrer cada celda
    for (int f = 0; f < GRID_SIZE; f++) {
        for (int c = 0; c < GRID_SIZE; c++) {
            // 3. Obtener color del centro de la celda
            int px = (c * TILE_SIZE) + 16;  // Offset al centro
            int py = (f * TILE_SIZE) + 16;
            
            int offset = py * rowSize + px * 3;
            BYTE b = pixelData[offset];      // Formato BGR
            BYTE g = pixelData[offset + 1];
            BYTE r = pixelData[offset + 2];
            
            // 4. Si es agua, marcar como impasable
            if (esColorAgua(r, g, b, tema)) {
                gCollisionMap[f][c] = 1;      // Bloquear
                mapaObjetos[f][c] = SIMBOLO_AGUA;
            }
        }
    }
}
```

#### Verificación de Colisión en Movimiento

```c
// Verificar si una celda es transitable
bool mapaCeldaEsTierra(int fila, int col) {
    // Validar límites
    if (fila < 0 || fila >= GRID_SIZE || col < 0 || col >= GRID_SIZE) {
        return false;
    }
    
    // Verificar collision map usando punteros
    int **colMap = mapaObtenerCollisionMap();
    if (!colMap) return false;
    
    int *filaPtr = *(colMap + fila);
    int valor = *(filaPtr + col);
    
    // 0 = libre, cualquier otro valor = bloqueado
    return (valor == 0);
}
```

### 4.4 Valores del Collision Map

| Valor | Significado | Ejemplos |
|-------|-------------|----------|
| `0` | Celda libre, transitable | Hierba, tierra |
| `1` | Obstáculo permanente | Agua, árboles |
| `2` | Edificio | Ayuntamiento, mina, cuartel |
| `3` | Ocupación temporal | Unidad, vaca |

---

## 5. GESTIÓN DE RECURSOS (BMP y Memoria)

### 5.1 Tutorial: Cómo Añadir un Nuevo BMP

#### Requisitos del Archivo BMP

- **Formato:** Windows Bitmap (.bmp)
- **Profundidad de color:** 24 bits (RGB)
- **Tamaño estándar:**
  - Unidades: 64x64 píxeles
  - Edificios: 128x128 píxeles
  - Barco: 192x192 píxeles
  - Árboles: 128x128 píxeles
- **Color de transparencia:** Magenta `RGB(255, 0, 255)`

#### Paso 1: Crear el Archivo

Usa cualquier editor (GIMP, Paint, Photoshop) y guarda como BMP 24-bit.

#### Paso 2: Ubicar en `assets/`

Organiza por categoría:
```
assets/
├── tu_nuevo_sprite/
│   ├── sprite_front.bmp
│   ├── sprite_back.bmp
│   ├── sprite_left.bmp
│   └── sprite_right.bmp
```

#### Paso 3: Definir Rutas (en `mapa.c`)

```c
#define MI_SPRITE_FRONT "../assets/tu_nuevo_sprite/sprite_front.bmp"
#define MI_SPRITE_BACK  "../assets/tu_nuevo_sprite/sprite_back.bmp"
#define MI_SPRITE_LEFT  "../assets/tu_nuevo_sprite/sprite_left.bmp"
#define MI_SPRITE_RIGHT "../assets/tu_nuevo_sprite/sprite_right.bmp"
```

#### Paso 4: Declarar Variable Global

```c
static HBITMAP hMiSprite[4] = {NULL};  // Front, Back, Left, Right
```

#### Paso 5: Cargar en `cargarRecursosGraficos()`

```c
void cargarRecursosGraficos() {
    // ... código existente ...
    
    // Cargar tu nuevo sprite
    hMiSprite[0] = loadBmp(MI_SPRITE_FRONT, 64, 64);
    hMiSprite[1] = loadBmp(MI_SPRITE_BACK, 64, 64);
    hMiSprite[2] = loadBmp(MI_SPRITE_LEFT, 64, 64);
    hMiSprite[3] = loadBmp(MI_SPRITE_RIGHT, 64, 64);
    
    // Verificación de error (opcional pero recomendado)
    if (!hMiSprite[0]) {
        MessageBoxA(NULL, "Error cargando sprite_front.bmp", "Error", MB_OK);
    }
}
```

#### Paso 6: Dibujar el Sprite

```c
// Ejemplo de función de dibujo
void dibujarMiSprite(HDC hdcBuffer, float x, float y, Direccion dir,
                     Camara cam, int anchoP, int altoP) {
    // 1. Calcular posición en pantalla
    int pantX = (int)((x - cam.x) * cam.zoom);
    int pantY = (int)((y - cam.y) * cam.zoom);
    int tam = (int)(64 * cam.zoom);
    
    // 2. Culling: no dibujar si está fuera
    if (pantX + tam <= 0 || pantX >= anchoP ||
        pantY + tam <= 0 || pantY >= altoP) {
        return;
    }
    
    // 3. Seleccionar sprite según dirección
    HBITMAP sprite = hMiSprite[dir];  // DIR_FRONT=0, etc.
    if (!sprite) return;
    
    // 4. Crear DC temporal para el sprite
    HDC hdcSprites = CreateCompatibleDC(hdcBuffer);
    HBITMAP hOld = (HBITMAP)SelectObject(hdcSprites, sprite);
    
    // 5. Dibujar con transparencia (magenta = invisible)
    TransparentBlt(
        hdcBuffer,          // Destino
        pantX, pantY,       // Posición destino
        tam, tam,           // Tamaño destino (escalado)
        hdcSprites,         // Origen
        0, 0,               // Posición origen
        64, 64,             // Tamaño origen
        RGB(255, 0, 255)    // Color transparente
    );
    
    // 6. Limpiar
    SelectObject(hdcSprites, hOld);
    DeleteDC(hdcSprites);
}
```

### 5.2 Limpieza de Memoria (Memory Management)

#### ¿Por qué es Importante?

En C no hay "garbage collector". Si no liberas memoria, tienes **memory leaks** que eventualmente crashean el programa.

#### Cuándo Liberar Memoria

1. **Al cerrar la ventana (`WM_DESTROY`)**
2. **Al cambiar de isla** (si se recargan sprites)
3. **Al cancelar movimiento** (liberar ruta)

#### Ejemplo: Liberar Collision Map

```c
void mapaLiberarCollisionMap(void) {
    if (!gCollisionMap) return;  // Ya está libre
    
    // Liberar cada fila primero
    for (int i = 0; i < GRID_SIZE; i++) {
        free(*(gCollisionMap + i));  // free(gCollisionMap[i])
    }
    
    // Luego liberar el array de punteros
    free(gCollisionMap);
    gCollisionMap = NULL;  // ¡Importante! Evita double-free
}
```

#### Ejemplo: Liberar Rutas de Pathfinding

```c
static void obreroLiberarRuta(Unidad *o) {
    if (o->rutaCeldas) {
        free(o->rutaCeldas);
        o->rutaCeldas = NULL;
    }
    o->rutaLen = 0;
    o->rutaIdx = 0;
}

// Liberar rutas de TODO un grupo
void rtsLiberarMovimientoJugador(struct Jugador *j) {
    // Liberar rutas de obreros
    for (int i = 0; i < MAX_OBREROS; i++) {
        obreroLiberarRuta(&j->obreros[i]);
    }
    
    // Liberar rutas de caballeros
    for (int i = 0; i < MAX_CABALLEROS; i++) {
        obreroLiberarRuta(&j->caballeros[i]);
    }
    
    // ... repetir para otros grupos ...
}
```

#### Ejemplo: Liberar Sprites

```c
void edificiosLiberarSprites() {
    // Liberar solo si existe
    if (g_spriteAyuntamiento) {
        DeleteObject(g_spriteAyuntamiento);
        g_spriteAyuntamiento = NULL;
    }
    
    if (g_spriteMina) {
        DeleteObject(g_spriteMina);
        g_spriteMina = NULL;
    }
    
    // ... etc ...
}
```

### 5.3 El Ciclo de Vida de un Recurso

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    CICLO DE VIDA DE RECURSOS                                │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   CARGA (Inicio del juego)                                                  │
│         │                                                                   │
│         ▼                                                                   │
│   cargarRecursosGraficos()                                                  │
│         │                                                                   │
│         ├──► LoadImageA() → Retorna HBITMAP                                 │
│         │                                                                   │
│         ├──► malloc() → Retorna puntero a memoria                           │
│         │                                                                   │
│         └──► calloc() → Retorna puntero inicializado a 0                    │
│                                                                             │
│   ─────────────────────────────────────────────────────────────────         │
│                                                                             │
│   USO (Durante el juego)                                                    │
│         │                                                                   │
│         ├──► SelectObject() → Usa el HBITMAP                                │
│         │                                                                   │
│         └──► Acceso a arrays → Lee/escribe memoria                          │
│                                                                             │
│   ─────────────────────────────────────────────────────────────────         │
│                                                                             │
│   LIBERACIÓN (WM_DESTROY)                                                   │
│         │                                                                   │
│         ├──► DeleteObject() → Para HBITMAP, HBRUSH, HFONT                   │
│         │                                                                   │
│         ├──► DeleteDC() → Para HDC creados                                  │
│         │                                                                   │
│         └──► free() → Para memoria de malloc/calloc                         │
│                   │                                                         │
│                   └──► ¡SIEMPRE poner puntero = NULL después!               │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 6. RETOS DE APRENDIZAJE

### Ejercicio 1: Cambiar la Velocidad de las Unidades

**Objetivo:** Hacer que todas las unidades se muevan 50% más rápido.

**Pistas:**
- Busca la constante `vel` en `recursos.c` dentro de `actualizarGrupoUnidades()`
- Actualmente es `5.0f` (píxeles por frame)
- Cambia a `7.5f` para 50% más rápido

**Desafío extra:** Haz que cada tipo de unidad tenga velocidad diferente:
- Obreros: 4.0f (lentos, cargan recursos)
- Caballeros: 5.0f (equilibrados)
- Guerreros: 6.0f (rápidos)

---

### Ejercicio 2: Mover la Isla de Fuego 5 Píxeles a la Derecha

**Objetivo:** En el mapa global (`mapaDemo2.bmp`), ajustar la posición de renderizado de la isla de fuego.

**Pistas:**
- Busca en `navegacion.c` o `mapa.c` donde se definen las coordenadas de las islas
- La función `navegacionObtenerPosicionBarcoIsla()` contiene coordenadas fijas por isla
- También revisa `dibujarMapaGlobal()` donde se renderizan las islas

**Código a modificar:**
```c
// Busca algo similar a:
case 5: // Isla de fuego
    *outX = 1200.0f + 5.0f;  // ← Añadir 5 píxeles
    *outY = 300.0f;
    break;
```

---

### Ejercicio 3: Añadir un Nuevo Recurso (Comida al Matar Vacas)

**Objetivo:** Cuando un obrero mata una vaca, el jugador gana +50 de comida.

**Pistas:**
1. Encuentra la función `recursosIntentarCazar()` en `recursos.c`
2. Cuando se confirma la caza exitosa, antes de eliminar la vaca:

```c
bool recursosIntentarCazar(struct Jugador *j, float mundoX, float mundoY) {
    // ... código existente de detección ...
    
    if (vacaEncontrada) {
        j->Comida += 50;  // ← AÑADIR ESTA LÍNEA
        
        // Eliminar vaca del mapa
        mapaEliminarVacaPorIndice(indiceVaca);
        return true;
    }
    return false;
}
```

---

### Ejercicio Avanzado: Crear un Sistema de Día/Noche

**Objetivo:** Implementar un overlay oscuro que simule la noche.

**Pasos sugeridos:**
1. Añadir un contador de tiempo global (`static int gTiempoMundo = 0;`)
2. Incrementarlo cada frame en `WM_TIMER`
3. Calcular "hora del día" (`hora = (gTiempoMundo / 3600) % 24`)
4. Si es de noche (hora > 20 || hora < 6), dibujar un rectángulo semitransparente oscuro sobre todo el buffer

**Código de inicio:**
```c
// En dibujarMundo(), al final:
void dibujarOverlayNoche(HDC hdcBuffer, RECT rect, int hora) {
    if (hora > 20 || hora < 6) {
        // Crear brush semitransparente
        HBRUSH hNoche = CreateSolidBrush(RGB(0, 0, 30));
        
        // Usar modo de mezcla
        SetBkMode(hdcBuffer, TRANSPARENT);
        // Nota: Para verdadera transparencia necesitarías AlphaBlend()
        
        // Por simplicidad, solo oscurecer los bordes:
        RECT borde = {0, 0, rect.right, 50};
        FillRect(hdcBuffer, &borde, hNoche);
        
        DeleteObject(hNoche);
    }
}
```

---

## 📝 APÉNDICE: Glosario de Términos

| Término | Definición |
|---------|------------|
| **BFS** | Breadth-First Search. Algoritmo que explora nivel por nivel para encontrar la ruta más corta. |
| **Culling** | Técnica de optimización que evita dibujar objetos fuera de la pantalla. |
| **Double Buffering** | Técnica que dibuja en un buffer oculto y luego lo copia a pantalla para evitar parpadeo. |
| **GDI** | Graphics Device Interface. API de Windows para dibujar gráficos 2D. |
| **Handle** | Identificador numérico que Windows usa para referirse a recursos (HWND, HDC, HBITMAP). |
| **Pathfinding** | Algoritmo que encuentra una ruta entre dos puntos evitando obstáculos. |
| **Sprite** | Imagen 2D que representa un objeto del juego (personaje, edificio, etc.). |
| **Tile** | Celda del mapa. En este juego, cada tile mide 64x64 píxeles. |
| **TransparentBlt** | Función de Windows que dibuja un bitmap ignorando un color específico (transparencia). |

---

## 🎓 CONCLUSIÓN

Este manual te ha proporcionado las herramientas conceptuales y técnicas para:

1. **Entender** cómo fluye la ejecución desde `main()` hasta el renderizado
2. **Manipular** las matrices de objetos y colisiones usando aritmética de punteros
3. **Crear** nuevas unidades siguiendo el patrón establecido
4. **Comprender** el sistema de movimiento con pathfinding BFS
5. **Gestionar** recursos gráficos (carga, uso, liberación)

**Recuerda:** La mejor forma de aprender es modificando el código. Empieza con los ejercicios propuestos y gradualmente implementa tus propias ideas.

¡Buena suerte, desarrollador! 🚀

---

*Manual creado para el proyecto "Islas en Guerra" - Motor de juego en C con Win32 API*
