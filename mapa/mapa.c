#include "mapa.h"
#include <stdio.h>

#define rutaImagenMapa "..//assets//mapa_islas_guerra.bmp"
static HBITMAP hMapaBmp = NULL;

void cargarRecursosGraficos() {
    hMapaBmp = (HBITMAP)LoadImageA(NULL, rutaImagenMapa, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if (!hMapaBmp) {
        // Fallback: Intentar ruta local si falla la relativa
        hMapaBmp = (HBITMAP)LoadImageA(NULL, "mapa_islas_guerra.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    }
}

void dibujarMundo(HDC hdc, RECT rect, Camara cam) {
    if (!hMapaBmp) return;

    int anchoPantalla = rect.right - rect.left;
    int altoPantalla = rect.bottom - rect.top;

    // --- BLINDAJE CONTRA PARPADEO (Back-Buffering) ---
    HDC hdcBuffer = CreateCompatibleDC(hdc);
    HBITMAP hbmBuffer = CreateCompatibleBitmap(hdc, anchoPantalla, altoPantalla);
    HBITMAP hOldBuffer = (HBITMAP)SelectObject(hdcBuffer, hbmBuffer);

    HDC hdcMapa = CreateCompatibleDC(hdc);
    HBITMAP hOldMapa = (HBITMAP)SelectObject(hdcMapa, hMapaBmp);

    // 1. Limpiar buffer (Fondo negro)
    FillRect(hdcBuffer, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));

    // 2. Lógica de Proyección de Zoom
    // Calculamos cuánto del mapa original cabe en la pantalla según el zoom
    int anchoMundoVisible = (int)(anchoPantalla / cam.zoom);
    int altoMundoVisible = (int)(altoPantalla / cam.zoom);

    // 3. Renderizado Atómico
    // 3. Renderizado Atómico
    SetStretchBltMode(hdcBuffer, HALFTONE); 
    StretchBlt(
        hdcBuffer, 0, 0, anchoPantalla, altoPantalla, 
        hdcMapa, cam.x, cam.y, anchoMundoVisible, altoMundoVisible, 
        SRCCOPY // <--- Asegúrate que tenga doble 'C'
    );

    // 4. Volcado final
    BitBlt(hdc, 0, 0, anchoPantalla, altoPantalla, hdcBuffer, 0, 0, SRCCOPY); // <--- Aquí también

    // --- LIMPIEZA DE MEMORIA GDI ---
    SelectObject(hdcMapa, hOldMapa);
    SelectObject(hdcBuffer, hOldBuffer);
    DeleteDC(hdcMapa);
    DeleteDC(hdcBuffer);
    DeleteObject(hbmBuffer);
}