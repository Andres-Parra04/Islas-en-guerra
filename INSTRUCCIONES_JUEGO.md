# ⚔️ Islas en Guerra - Manual de Juego ⚔️

Bienvenido a **Islas en Guerra**, un juego de estrategia en tiempo real (RTS) y exploración donde tu objetivo es gestionar recursos, comandar un ejército y conquistar el archipiélago enemigo.

## 🎯 Objetivo del Juego

El objetivo principal es **conquistar las 5 islas** del archipiélago.
Para lograrlo, deberás:

1.  Recolectar recursos básicos para sobrevivir y crecer.
2.  Entrenar un ejército poderoso en el Cuartel.
3.  Reparar y mejorar tu Barco para viajar entre islas.
4.  Eliminar a todos los enemigos de cada isla para reclamarla.

¡Cuidado! Si todas tus unidades mueren en batalla, serás derrotado y forzado a retirarte.

---

## 🎮 Controles Básicos

### Mouse

- **Clic Izquierdo**:
  - **Seleccionar unidades**: Haz clic sobre un Obrero, Caballero o Guerrero.
  - **Interactuar con edificios**: Haz clic en el Ayuntamiento, Cuartel o Barco (si hay una unidad cerca) para abrir menús.
  - **Botones en menús**: Para comprar o entrenar.
- **Clic Derecho**:
  - **Mover**: Ordena a las unidades seleccionadas moverse al punto clicado.
  - **Acciones**: Contextual según el objetivo (Talar árbol, Cazar vaca, Entrar en mina).
- **Arrastrar Mouse (con Clic Izquierdo)**: Mueve la cámara por el mapa.
- **Rueda del Mouse**: Alejar o acercar el zoom de la cámara.

### Teclado

- **`C`**: Centrar la cámara en el Ayuntamiento.
- **`H`**: Curar a las unidades seleccionadas (Cuesta **100 Comida** por unidad herida).
- **`M`**: (Debug) Mostrar mapa en consola.
- **`ESC`**: Pausar el juego y abrir el menú de Guardar/Salir.

---

## 💎 Recursos y Economía

Gestionar tus recursos es vital. Los verás en el panel superior derecho.

| Recurso    | Icono/Fuente           | Uso Principal                                                          |
| :--------- | :--------------------- | :--------------------------------------------------------------------- |
| **Comida** | 🐄 **Vacas** (Cazar)   | Necesaria para **entrenar tropas** y **curar unidades**.               |
| **Madera** | 🌲 **Árboles** (Talar) | Construcción de barcos, mejoras y entrenamiento de arqueros/guerreros. |
| **Oro**    | ⛏️ **Mina**            | Moneda principal para comprar tropas y mejoras.                        |
| **Piedra** | ⛏️ **Mina**            | Material de construcción pesado (mejoras de barco).                    |
| **Hierro** | ⛏️ **Mina**            | Necesario para armas y armaduras de unidades avanzadas.                |

**¿Cómo recolectar?**

- **Mina**: Envía un Obrero al centro de la mina. Si hay recursos acumulados, aparecerá un mensaje para recogerlos.
- **Árboles**: Selecciona **Obreros** o **Guerreros** y haz clic derecho en un árbol. Tras 3 golpes, obtendrás madera.
- **Vacas**: Selecciona cualquier unidad ofensiva u obrero y haz clic derecho en una vaca para cazarla (+100 Comida).

---

## 💂 Unidades y Ejército

Puedes entrenar unidades en el **Cuartel** (requiere tener un Obrero cerca para abrir el menú).

### 1. Obrero (Trabajador)

- **Rol**: Recolección y Logística.
- **Habilidades**: Puede talar árboles, extraer de minas y construir/reparar el Barco.
- **Combate**: Muy débil. Mantenlos protegidos.

### 2. Caballero (Con Escudo)

- **Rol**: Tanque / Defensivo.
- **Stats**: Vida Alta (150), Defensa Alta (25), Daño Medio (35).
- **Uso**: Envíalos al frente para absorber el daño enemigo.

### 3. Caballero Sin Escudo (Ofensivo)

- **Rol**: Ataque Rápido (DPS).
- **Stats**: Vida Media (135), Crítico Alto (20%), Defensa Media (20).
- **Uso**: Ideal para flanquear mientras los tanques distraen.

### 4. Guerrero

- **Rol**: Fuerza Bruta y Utilidad.
- **Stats**: Daño Muy Alto (50), Crítico Devastador (30%), Vida Baja (120).
- **Habilidad Especial**: ¡También puede talar árboles!
- **Uso**: Elimina enemigos rápidamente, pero es frágil.

---

## ⛵ Navegación y Conquista

El mundo se divide en islas. Para ganar, debes viajar a todas.

1.  **El Barco**: Comienzas con el barco destruido o básico en la orilla.
2.  **Construcción**: Envía un obrero al barco y usa Clic Derecho (o interactúa) para repararlo. Cuesta: 50 Oro, 150 Madera, 150 Piedra, 160 Hierro.
3.  **Embarque**: Selecciona tus tropas y haz clic derecho en el barco para subirlas.
4.  **Viaje**: Haz clic izquierdo en el barco para abrir el menú de navegación y elegir tu destino.
5.  **Mejoras**: Puedes mejorar el nivel del barco para transportar más tropas (Capacidad inicial: 6 → Máxima: 15).

---

## 🏛️ Edificios Principales

- **Ayuntamiento (Castillo)**: Tu base central. Aquí puedes comprar recursos de emergencia si tienes Oro de sobra.
- **Mina**: Genera Oro, Piedra y Hierro pasivamente con el tiempo. ¡Recuerda enviar un obrero a recogerlo frecuentemente!
- **Cuartel**: Lugar de entrenamiento militar.

---

## 💡 Estrategia para Ganar

1.  **Inicio Rápido**: Al empezar, prioriza la **Comida** y la **Madera**. Caza vacas cercanas y tala árboles inmediatamente.
2.  **Economía**: No dejes que la Mina se llene. Recoge los recursos a menudo para financiar tu ejército.
3.  **Composición de Ejército**:
    - No hagas solo Caballeros. Combina **Caballeros con Escudo** al frente para resistir y **Guerreros** detrás para hacer daño masivo.
4.  **Curación**: Es más barato curar una unidad veterana (tecla `H`) que entrenar una nueva. ¡Cuida tu comida!
5.  **Expansión**: No te quedes en la primera isla. Repara el barco cuanto antes. Las islas avanzadas pueden tener enemigos más fuertes pero recompensas necesarias para la victoria.

¡Buena suerte, comandante! El destino del archipiélago está en tus manos.
