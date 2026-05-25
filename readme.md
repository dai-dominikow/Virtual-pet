## Photosensitive virtual pet!.
Dai Dominikow - 2026

### Intro
Inicialmente lo pense para un ESP32-C3, pero finalmente utilicé un Arduino Nano. Dejo ambas versiones.

### Componentes necesarios

- 1x ESP32-C3 Super Mini o Arduino Nano 
- 1x OLED I2C 128x64
- 1x LDR / fotoresistor
- 1x resistencia 10kΩ
- cables dupont
- protoboard (opcional)


### Comportamiento
La mascota cambia de estado dependiendo de la iluminación ambiente:

- Oscuro → duerme (`- -` + `zZz`)
- Luz normal → idle feliz + corazones flotantes
- Mucha luz → doble blink tipo `> <`

También incluye:
- breathing animation
- blinking aleatorio
- corazón flotante animado
- comportamiento reactivo a la luz

### Pins 
| OLED | ESP32-C3 | Arduino Nano |
|---|---|---|
|  VCC | 3.3V | 5V |
|  GND | GND | GND |
|  SDA | GPIO 4 |  A4 |
|  SCL | GPIO 5 |  A5 |


### Divisor de tensión para LDR

```text
VCC
 |
[LDR]
 |
 +---- GPIO 0
 |
[10kΩ]
 |
GND