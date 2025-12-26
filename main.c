#include <windows.h>
#include <windowsx.h>
#include "mapa/mapa.h"
#include "mapa/menu.h"
#include <stdbool.h>

// --- CONFIGURACIÓN DE LÍMITES Y CONSTANTES ---
#define ZOOM_MAXIMO 6.0f  // Limite para evitar que los píxeles se vean demasiado mal
#define MAPA_SIZE 2048    // Dimensiones fijas del mapa .bmp

Camara camara = {0, 0, 1.0f};
bool arrastrando = false;
POINT mouseUltimo;

// --- MOTOR DE VALIDACIÓN DE CÁMARA (Anti-Bordes Negros y Anti-Pixelación) ---
void corregirLimitesCamara(RECT rect) {
    int anchoV = rect.right - rect.left;
    int altoV = rect.bottom - rect.top;
    
    // 1. Zoom Mínimo: Basado en el tamaño actual de la ventana
    // Calculamos qué escala cubre el ancho y cuál el alto
    float scaleX = (float)anchoV / (float)MAPA_SIZE;
    float scaleY = (float)altoV / (float)MAPA_SIZE;
    
    // Elegimos el mayor para que el mapa "desborde" y no deje ver fondo negro
    float zMinimo = (scaleX > scaleY) ? scaleX : scaleY;

    if (camara.zoom < zMinimo) camara.zoom = zMinimo;

    // 2. Zoom Máximo: Bloqueo de pixelación
    if (camara.zoom > ZOOM_MAXIMO) camara.zoom = ZOOM_MAXIMO;

    // 3. Clamp de Posición: No permitir que la vista salga del mapa de 1024x1024
    int maxW = MAPA_SIZE - (int)(anchoV / camara.zoom);
    int maxH = MAPA_SIZE - (int)(altoV / camara.zoom);

    if (camara.x < 0) camara.x = 0;
    if (camara.y < 0) camara.y = 0;
    if (camara.x > maxW) camara.x = maxW;
    if (camara.y > maxH) camara.y = maxH;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static RECT rect;

    switch (uMsg) {
        case WM_SIZE: {
            // Se activa al maximizar o cambiar el tamaño de la ventana
            GetClientRect(hwnd, &rect);
            corregirLimitesCamara(rect);
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }
        case WM_MOUSEWHEEL: {
            int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            ScreenToClient(hwnd, &pt);
            
            float zoomViejo = camara.zoom;
            camara.zoom *= (delta > 0) ? 1.1f : 0.9f;

            // Aplicar corrección de límites antes de re-centrar
            corregirLimitesCamara(rect);

            // Ajuste matemático para que el zoom siga al mouse:
            // MundoX = CamaraX + (MouseX / Zoom)
            camara.x = (int)((camara.x + (pt.x / zoomViejo)) - (pt.x / camara.zoom));
            camara.y = (int)((camara.y + (pt.y / zoomViejo)) - (pt.y / camara.zoom));
            
            corregirLimitesCamara(rect); // Verificación final
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }
        case WM_MOUSEMOVE:
            if (arrastrando) {
                int curX = GET_X_LPARAM(lParam);
                int curY = GET_Y_LPARAM(lParam);
                
                // Desplazamiento relativo al zoom actual
                camara.x -= (int)((curX - mouseUltimo.x) / camara.zoom);
                camara.y -= (int)((curY - mouseUltimo.y) / camara.zoom);
                
                corregirLimitesCamara(rect);

                mouseUltimo.x = curX;
                mouseUltimo.y = curY;
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        case WM_LBUTTONDOWN:
            arrastrando = true;
            mouseUltimo.x = GET_X_LPARAM(lParam);
            mouseUltimo.y = GET_Y_LPARAM(lParam);
            SetCapture(hwnd);
            return 0;
        case WM_LBUTTONUP:
            arrastrando = false;
            ReleaseCapture();
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            // El dibujo utiliza doble buffer para evitar parpadeos
            dibujarMundo(hdc, rect, camara); 
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int main() {
    // 1. Mostrar Menú Principal GDI
    mostrarMenu(); 
    
    // Si la acción es salir (o el usuario cerró el menú), terminar
    if (menuObtenerAccion() == 3) return 0;

    // 2. Ocultar consola y preparar ventana de juego
    // ShowWindow(GetConsoleWindow(), SW_HIDE);

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "ClaseGuerraIslas";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    RegisterClass(&wc);

    // 3. Crear ventana con bordes estándar (Maximizar, Minimizar, Cerrar)
    HWND hwnd = CreateWindowEx(0, wc.lpszClassName, "Islas en Guerra - Motor GDI", 
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, NULL, NULL, wc.hInstance, NULL);

    cargarRecursosGraficos();

    // 4. Iniciar maximizada para cubrir el escritorio pero permitir acceso a la barra de tareas
    ShowWindow(hwnd, SW_SHOWMAXIMIZED);
    UpdateWindow(hwnd);

    // 5. Bucle de mensajes de alta prioridad
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}