#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <windowsx.h>
#include "../batallas/guardado/guardado.h"

// ... definitions ...
#define ANCHO_CONSOLA 80
#define ALTO_CONSOLA 25

#define RUTA_FONDO "..\\assets\\menu\\menu_bg.bmp"
#define RUTA_FONDO_INSTRUC "..\\assets\\menu\\menu_bg_instruc.bmp"
#define RUTA_FONDO_ISLAS "..\\assets\\menu\\menu_bg_islas.bmp"
#define RUTA_MAPA_COMPLETO "..\\assets\\mapa_islas_guerra.bmp"
#define RUTA_MAPA2 "..\\assets\\mapa2.bmp"
#define RUTA_ISLA1 "..\\assets\\islas\\isla1.bmp"
#define RUTA_ISLA2 "..\\assets\\islas\\isla2.bmp"
#define RUTA_ISLA3 "..\\assets\\islas\\isla3.bmp"
#define RUTA_ISLA4 "..\\assets\\islas\\isla4.bmp"
#define RUTA_ISLA5 "..\\assets\\islas\\isla5.bmp"

static HBITMAP fondoBmp = NULL;
static BITMAP infoFondo;
static bool fondoListo = false;
static HBITMAP fondoInstrucBmp = NULL;
static BITMAP infoFondoInstruc;
static bool fondoInstrucListo = false;
static HBITMAP fondoIslasBmp = NULL;
static BITMAP infoFondoIslas;
static bool fondoIslasListo = false;

static int gSeleccion = 0;
static HFONT gFontTitulo = NULL;
static HFONT gFontOpciones = NULL;
static const char *OPCIONES_CON_CARGAR[] = { "Iniciar partida", "Cargar partida", "Instrucciones", "Salir" };
static const char *OPCIONES_SIN_CARGAR[] = { "Iniciar partida", "Instrucciones", "Salir" };
static const char **OPCIONES_MENU = NULL;
static int OPCIONES_TOTAL = 0;
static bool gHayPartidasGuardadas = false;
static bool gMostrandoInstrucciones = false;
static HWND gMenuHwnd = NULL;
static int gMenuAccion = 0;
static bool gMostrandoSeleccionIsla = false;
static int gSeleccionIsla = 0;
static int gIslaSeleccionada = 1;
static const char *OPCIONES_ISLAS[] = { 
    "Mapa 3 Islas", "Isla 1", "Isla 2", "Isla 3", 
    "Mapa 2 Islas", "Isla 4 [Bloqueada]", "Isla 5 [Bloqueada]", "Volver" 
};
static const int OPCIONES_ISLAS_TOTAL = 8;
static const bool ISLAS_BLOQUEADAS[] = { false, false, false, false, false, true, true, false };

static HBITMAP hIslaBmp[7] = {NULL};
static BITMAP infoIsla[7];
static bool islasCargadas = false;

static bool gMostrandoCargaPartidas = false;
static InfoPartida gPartidasDisponibles[MAX_PARTIDAS];
static int gNumPartidasDisponibles = 0;
static int gSeleccionPartida = 0;
static char gNombrePartidaSeleccionada[MAX_NOMBRE_JUGADOR] = {0};
static HFONT gFontPequena = NULL;

#define MAX_LINEAS_PAGINA 18
typedef struct { const char *lineas[MAX_LINEAS_PAGINA]; int count; } PaginaInstrucciones;
static const PaginaInstrucciones gPaginas[] = {
    {{"--- ISLAS EN GUERRA ---", "OBJETIVO: Conquistar las 5 islas.", "", "PASOS:", "1. Recolectar recursos (Comida, Madera).", "2. Entrenar ejercito en Ayuntamiento/Cuartel.", "3. Construir/Usar Barco para viajar.", "4. Eliminar enemigos en cada isla."}, 8},
    {{"CONTROLES DE CAMARA", "- Arrastrar con Click Izquierdo: Mover", "- Rueda del Mouse: Zoom In/Out", "- Tecla 'C': Centrar en Ayuntamiento", "- Tecla 'M': Ver mapa de colisiones (Debug)"}, 5},
    {{"CONTROLES DE UNIDADES", "- Click Izq: Seleccionar unidad", "- Arrastrar caja: Seleccion multiple", "- Click Der (Terreno): Mover", "- Click Der (Enemigo/Vaca/Arbol): Accion", "- Tecla 'H': Curar seleccion (100 Comida)"}, 6},
    {{"ESTRUCTURAS", "- Ayuntamiento: Crea Obreros. Almacen.", "- Cuartel: Entrena Caballeros y Guerreros.", "- Mina: Genera Oro, Piedra, Hierro (automatico).", "- Barco: Necesario para viajar entre islas."}, 5}
};
static const int gTotalPaginas = 4;
static int gPaginaActual = 0;

void mostrarInstrucciones();
static void dibujarPreviewIsla(HDC hdc, RECT rc);

static HBITMAP loadBmp(const char *path, BITMAP *bm) {
    HBITMAP h = (HBITMAP)LoadImageA(NULL, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    if (h && bm) GetObject(h, sizeof(BITMAP), bm);
    return h;
}

static void drawBmp(HDC hdc, RECT rc, HBITMAP h, BITMAP *bm) {
    if (!h) { HBRUSH b = CreateSolidBrush(0); FillRect(hdc, &rc, b); DeleteObject(b); return; }
    HDC mem = CreateCompatibleDC(hdc); HBITMAP old = (HBITMAP)SelectObject(mem, h);
    StretchBlt(hdc, 0, 0, rc.right-rc.left, rc.bottom-rc.top, mem, 0, 0, bm->bmWidth, bm->bmHeight, SRCCOPY);
    SelectObject(mem, old); DeleteDC(mem);
}

static bool cargarFondo(void) { if(fondoListo) return true; fondoBmp=loadBmp(RUTA_FONDO, &infoFondo); return (fondoListo=(fondoBmp!=NULL)); }
static bool cargarFondoInstruc(void) { if(fondoInstrucListo) return true; fondoInstrucBmp=loadBmp(RUTA_FONDO_INSTRUC, &infoFondoInstruc); return (fondoInstrucListo=(fondoInstrucBmp!=NULL)); }
static bool cargarFondoIslas(void) { if(fondoIslasListo) return true; fondoIslasBmp=loadBmp(RUTA_FONDO_ISLAS, &infoFondoIslas); return (fondoIslasListo=(fondoIslasBmp!=NULL)); }

static bool cargarIslas(void) {
    if (islasCargadas) return true;
    const char *rutas[] = { RUTA_MAPA_COMPLETO, RUTA_ISLA1, RUTA_ISLA2, RUTA_ISLA3, RUTA_MAPA2, RUTA_ISLA4, RUTA_ISLA5 };
    for (int i = 0; i < 7; i++) hIslaBmp[i] = loadBmp(rutas[i], &infoIsla[i]);
    return (islasCargadas = (hIslaBmp[0] != NULL));
}

static void dibujarFondoBmp(HDC hdc, RECT rc) { cargarFondo(); drawBmp(hdc, rc, fondoBmp, &infoFondo); }
static void dibujarFondoBmpIslas(HDC hdc, RECT rc) { cargarFondoIslas(); drawBmp(hdc, rc, fondoIslasBmp, &infoFondoIslas); }
static void dibujarFondoBmpInstrucciones(HDC hdc, RECT rc) { cargarFondoInstruc(); drawBmp(hdc, rc, fondoInstrucBmp, &infoFondoInstruc); }

static SIZE medirTexto(HDC hdc, HFONT font, const char *texto) {
    SIZE size = {0, 0};
    HFONT old = (HFONT)SelectObject(hdc, font);
    GetTextExtentPoint32A(hdc, texto, (int)strlen(texto), &size);
    SelectObject(hdc, old);
    return size;
}

static void dibujarTextoCentrado(HDC hdc, HFONT font, const char *texto, int xCentro, int y, COLORREF color) {
    HFONT oldFont = (HFONT)SelectObject(hdc, font);
    SIZE size = {0};
    GetTextExtentPoint32A(hdc, texto, (int)strlen(texto), &size);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, color);
    TextOutA(hdc, xCentro - size.cx / 2, y, texto, (int)strlen(texto));
    SelectObject(hdc, oldFont);
}

static void dibujarOpciones(HDC hdc, HFONT font, const char *opciones[], int total, int seleccion, RECT rc) {
    int ancho = rc.right - rc.left;
    int alto = rc.bottom - rc.top;
    int centroX = ancho / 2;
    SIZE size = medirTexto(hdc, font, "Ay");
    int alturaLinea = size.cy + 12;
    int bloqueAlto = alturaLinea * total;
    int inicioY = (alto - bloqueAlto) / 2;

    for (int i = 0; i < total; i++) {
        SIZE medida = medirTexto(hdc, font, opciones[i]);
        COLORREF color = (i == seleccion) ? RGB(120, 255, 150) : RGB(255, 255, 255);
        dibujarTextoCentrado(hdc, font, opciones[i], centroX, inicioY + i * alturaLinea, color);

        if (i == seleccion) {
            RECT highlight = {
                centroX - medida.cx / 2 - 18,
                inicioY + i * alturaLinea - 4,
                centroX + medida.cx / 2 + 18,
                inicioY + i * alturaLinea + medida.cy + 6};
            HBRUSH brush = CreateSolidBrush(RGB(20, 80, 30));
            FrameRect(hdc, &highlight, brush);
            DeleteObject(brush);
        }
    }
}

static void dibujarOpcionesIslas(HDC hdc, HFONT font, RECT rc) {
    int ancho = rc.right - rc.left;
    int alto = rc.bottom - rc.top;
    int centroX = ancho / 4;
    SIZE size = medirTexto(hdc, font, "Ay");
    int alturaLinea = size.cy + 12;
    int bloqueAlto = alturaLinea * OPCIONES_ISLAS_TOTAL;
    int inicioY = (alto - bloqueAlto) / 2;

    for (int i = 0; i < OPCIONES_ISLAS_TOTAL; i++) {
        SIZE medida = medirTexto(hdc, font, OPCIONES_ISLAS[i]);
        COLORREF color = ISLAS_BLOQUEADAS[i] ? RGB(120, 120, 120) : (i == gSeleccionIsla ? RGB(120, 255, 150) : RGB(255, 255, 255));
        dibujarTextoCentrado(hdc, font, OPCIONES_ISLAS[i], centroX, inicioY + i * alturaLinea, color);

        if (i == gSeleccionIsla) {
            RECT highlight = {
                centroX - medida.cx / 2 - 18,
                inicioY + i * alturaLinea - 4,
                centroX + medida.cx / 2 + 18,
                inicioY + i * alturaLinea + medida.cy + 6};
            HBRUSH brush = CreateSolidBrush(ISLAS_BLOQUEADAS[i] ? RGB(100, 50, 50) : RGB(20, 80, 30));
            FrameRect(hdc, &highlight, brush);
            DeleteObject(brush);
        }
    }
}

static void renderMenu(HWND hwnd) {
    RECT rc;
    GetClientRect(hwnd, &rc);
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    int ancho = rc.right - rc.left;
    int alto = rc.bottom - rc.top;
    int centroX = ancho / 2;
    int tituloY = alto / 5;

    if (gMostrandoInstrucciones) {
        dibujarFondoBmpInstrucciones(hdc, rc);
        char buf[64];
        snprintf(buf, sizeof(buf), "INSTRUCCIONES (%d/%d)", gPaginaActual + 1, gTotalPaginas);
        dibujarTextoCentrado(hdc, gFontTitulo, buf, centroX, tituloY, RGB(240, 240, 240));
        
        SIZE m = medirTexto(hdc, gFontOpciones, "Ay");
        int hLines = m.cy + 8;
        if (gTotalPaginas > 0) {
            const PaginaInstrucciones *pg = &gPaginas[gPaginaActual];
            int startY = (alto - pg->count * hLines) / 2 + 20;
            for(int i=0; i<pg->count; i++) 
                dibujarTextoCentrado(hdc, gFontOpciones, pg->lineas[i], centroX, startY + i*hLines, RGB(255,255,255));
        }
        int pieY = alto - 50;
        dibujarTextoCentrado(hdc, gFontPequena, "ARROWS: Navegar | ESC: Volver", centroX, pieY, RGB(100, 200, 255));
    } else if (gMostrandoSeleccionIsla) {
        dibujarFondoBmpIslas(hdc, rc);
        RECT rcOpc = rc;
        rcOpc.right = rc.left + ancho / 2;
        dibujarOpcionesIslas(hdc, gFontOpciones, rcOpc);
        dibujarPreviewIsla(hdc, rc);
    } else if (gMostrandoCargaPartidas) {
        dibujarFondoBmp(hdc, rc);
        dibujarTextoCentrado(hdc, gFontTitulo, "CARGAR PARTIDA", centroX, tituloY, RGB(240, 240, 240));
        if (gNumPartidasDisponibles == 0) {
            dibujarTextoCentrado(hdc, gFontOpciones, "Sin partidas guardadas", centroX, alto / 2, RGB(200, 200, 200));
        } else {
            SIZE m = medirTexto(hdc, gFontOpciones, "Ay");
            int hLines = m.cy + 20;
            int startY = (alto - gNumPartidasDisponibles * hLines) / 2;
            for (int i=0; i<gNumPartidasDisponibles; i++) {
                char buf[256];
                snprintf(buf, sizeof(buf), "%s - Isla %d", gPartidasDisponibles[i].nombreJugador, gPartidasDisponibles[i].islaActual);
                COLORREF col = (i==gSeleccionPartida)?RGB(120,255,150):RGB(255,255,255);
                dibujarTextoCentrado(hdc, gFontOpciones, buf, centroX, startY + i*hLines, col);
            }
        }
        dibujarTextoCentrado(hdc, gFontPequena, "ENTER: Cargar | ESC: Volver", centroX, alto - 80, RGB(255, 230, 120));
    } else {
        dibujarFondoBmp(hdc, rc);
        dibujarOpciones(hdc, gFontOpciones, OPCIONES_MENU, OPCIONES_TOTAL, gSeleccion, rc);
    }
    EndPaint(hwnd, &ps);
}

static LRESULT CALLBACK MenuWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            gFontTitulo = CreateFontA(48, 0, 0, 0, FW_HEAVY, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FF_DONTCARE, "Segoe UI");
            gFontOpciones = CreateFontA(28, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FF_DONTCARE, "Segoe UI");
            gFontPequena = CreateFontA(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FF_DONTCARE, "Segoe UI");
            return 0;
        case WM_PAINT:
            renderMenu(hwnd);
            return 0;
        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                 if (gMostrandoInstrucciones || gMostrandoCargaPartidas || gMostrandoSeleccionIsla) {
                     gMostrandoInstrucciones = false; gMostrandoCargaPartidas = false; gMostrandoSeleccionIsla = false;
                     InvalidateRect(hwnd, NULL, FALSE);
                 } else PostMessage(hwnd, WM_CLOSE, 0, 0);
            } else if (wParam == VK_UP || wParam == 'W') {
                if (gMostrandoCargaPartidas && gNumPartidasDisponibles > 0) gSeleccionPartida = (gSeleccionPartida - 1 + gNumPartidasDisponibles) % gNumPartidasDisponibles;
                else if (gMostrandoSeleccionIsla) gSeleccionIsla = (gSeleccionIsla - 1 + OPCIONES_ISLAS_TOTAL) % OPCIONES_ISLAS_TOTAL;
                else if (!gMostrandoInstrucciones) gSeleccion = (gSeleccion - 1 + OPCIONES_TOTAL) % OPCIONES_TOTAL;
                InvalidateRect(hwnd, NULL, FALSE);
            } else if (wParam == VK_DOWN || wParam == 'S') {
                if (gMostrandoCargaPartidas && gNumPartidasDisponibles > 0) gSeleccionPartida = (gSeleccionPartida + 1) % gNumPartidasDisponibles;
                else if (gMostrandoSeleccionIsla) gSeleccionIsla = (gSeleccionIsla + 1) % OPCIONES_ISLAS_TOTAL;
                else if (!gMostrandoInstrucciones) gSeleccion = (gSeleccion + 1) % OPCIONES_TOTAL;
                InvalidateRect(hwnd, NULL, FALSE);
            } else if (wParam == VK_RETURN) {
                if (gMostrandoCargaPartidas && gNumPartidasDisponibles > 0) {
                     strncpy(gNombrePartidaSeleccionada, gPartidasDisponibles[gSeleccionPartida].nombreJugador, MAX_NOMBRE_JUGADOR - 1);
                     gMenuAccion = 1; PostMessage(hwnd, WM_CLOSE, 0, 0);
                } else if (gMostrandoSeleccionIsla) {
                     if (gSeleccionIsla >= 1 && gSeleccionIsla <= 3) {
                         gIslaSeleccionada = gSeleccionIsla; gMenuAccion = 0; PostMessage(hwnd, WM_CLOSE, 0, 0);
                     } else if (gSeleccionIsla == OPCIONES_ISLAS_TOTAL - 1) {
                         gMostrandoSeleccionIsla = false; InvalidateRect(hwnd, NULL, FALSE);
                     }
                } else if (!gMostrandoInstrucciones) {
                     if (gSeleccion == 0) { gMostrandoSeleccionIsla = true; gSeleccionIsla = gIslaSeleccionada; InvalidateRect(hwnd, NULL, FALSE); }
                     else if (gHayPartidasGuardadas && gSeleccion == 1) { gMostrandoCargaPartidas = true; gSeleccionPartida = 0; InvalidateRect(hwnd, NULL, FALSE); }
                     else if ((gHayPartidasGuardadas && gSeleccion == 2) || (!gHayPartidasGuardadas && gSeleccion == 1)) mostrarInstrucciones();
                     else ExitProcess(0);
                }
            } else if (wParam == VK_LEFT || wParam == VK_BACK) {
                if (gMostrandoInstrucciones) { gPaginaActual = (gPaginaActual > 0) ? gPaginaActual - 1 : gTotalPaginas - 1; InvalidateRect(hwnd, NULL, FALSE); }
            } else if (wParam == VK_RIGHT || wParam == VK_SPACE) {
                if (gMostrandoInstrucciones) { gPaginaActual = (gPaginaActual + 1) % gTotalPaginas; InvalidateRect(hwnd, NULL, FALSE); }
            }
            return 0;
        case WM_DESTROY:
            if (gFontTitulo) DeleteObject(gFontTitulo);
            if (gFontOpciones) DeleteObject(gFontOpciones);
            if (gFontPequena) DeleteObject(gFontPequena);
            if (fondoIslasBmp) DeleteObject(fondoIslasBmp); fondoIslasListo = false;
            for(int i=0; i<7; i++) if(hIslaBmp[i]) DeleteObject(hIslaBmp[i]); islasCargadas = false;
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void ocultarCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) return;
    cursorInfo.dwSize = 1; cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}


void mostrarInstrucciones() { gMostrandoInstrucciones = true; if (gMenuHwnd) InvalidateRect(gMenuHwnd, NULL, TRUE); }
int menuObtenerAccion() { return gMenuAccion; }
int menuObtenerIsla() { return gIslaSeleccionada; }
const char* menuObtenerNombrePartida() { return gNombrePartidaSeleccionada; }

void mostrarMenu() {
    ocultarCursor();
    gNumPartidasDisponibles = obtenerPartidasGuardadas(gPartidasDisponibles);
    gHayPartidasGuardadas = (gNumPartidasDisponibles > 0);
    if (gHayPartidasGuardadas) { OPCIONES_MENU = OPCIONES_CON_CARGAR; OPCIONES_TOTAL = 4; }
    else { OPCIONES_MENU = OPCIONES_SIN_CARGAR; OPCIONES_TOTAL = 3; }

    gSeleccion = 0; gMostrandoInstrucciones = false; gMostrandoSeleccionIsla = false; gMostrandoCargaPartidas = false;
    gSeleccionIsla = 0; gSeleccionPartida = 0; gMenuAccion = 0; gNombrePartidaSeleccionada[0] = '\0';

    HINSTANCE hInst = GetModuleHandle(NULL);
    WNDCLASSA wc = {0};
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MenuWndProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = "MenuWndClass";
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    RegisterClassA(&wc);

    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);
    HWND hConsole = GetConsoleWindow();
    if (hConsole) ShowWindow(hConsole, SW_HIDE);

    HWND hwnd = CreateWindowExA(WS_EX_TOPMOST, wc.lpszClassName, "Islas en guerra - Menú", WS_POPUP, 0, 0, screenW, screenH, NULL, NULL, hInst, NULL);
    if (!hwnd) { MessageBoxA(NULL, "Error al crear menu", "Error", MB_OK); return; }

    gMenuHwnd = hwnd;
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    if (hConsole) ShowWindow(hConsole, SW_SHOW);
}

static void dibujarPreviewIsla(HDC hdc, RECT rc) {
    if (!cargarIslas()) return;
    if (gSeleccionIsla == OPCIONES_ISLAS_TOTAL - 1) return;
    int idx = gSeleccionIsla;
    if (idx < 0 || idx > 6 || !hIslaBmp[idx]) return;

    HDC mem = CreateCompatibleDC(hdc); HBITMAP old = (HBITMAP)SelectObject(mem, hIslaBmp[idx]);
    int w = rc.right - rc.left, h = rc.bottom - rc.top;
    int pw = w / 3, ph = h / 2;
    // Simple scaling
    if (infoIsla[idx].bmWidth) {
         double ratio = (double)infoIsla[idx].bmWidth / infoIsla[idx].bmHeight;
         if (ratio > (double)pw/ph) ph = pw/ratio; else pw = ph*ratio;
    }
    int dx = rc.left + (w/2) - (pw/2), dy = rc.top + (h/2) - (ph/2);
    StretchBlt(hdc, dx, dy, pw, ph, mem, 0, 0, infoIsla[idx].bmWidth, infoIsla[idx].bmHeight, SRCCOPY);
    
    if (ISLAS_BLOQUEADAS[idx]) {
        SetBkMode(hdc, TRANSPARENT); SetTextColor(hdc, RGB(255, 80, 80));
        HFONT oldF = (HFONT)SelectObject(hdc, gFontTitulo);
        const char *txt = "BLOQUEADA";
        SIZE sz; GetTextExtentPoint32A(hdc, txt, strlen(txt), &sz);
        TextOutA(hdc, dx + (pw-sz.cx)/2, dy + (ph-sz.cy)/2, txt, strlen(txt));
        SelectObject(hdc, oldF);
    }
    SelectObject(mem, old); DeleteDC(mem);
}