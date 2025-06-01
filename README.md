# Sistema de monitorización de clima

## Índice

* [Introducción](#introducción)
* [Objetivo del Proyecto](#objetivo-del-proyecto)
* [Hardware Utilizado](#hardware-utilizado)
* [Diagrama de Conexiones](#diagrama-de-conexiones)
* [Descripción de los Sensores](#descripción-de-los-sensores)
* [Explicación del Código Fuente](#explicación-del-código-fuente)
* [Funcionamiento del Sistema](#funcionamiento-del-sistema)
* [Análisis Detallado de Funciones](#análisis-detallado-de-funciones)
* [Uso del Sistema](#uso-del-sistema)
* [Limitaciones y Mejoras Futuras](#limitaciones-y-mejoras-futuras)
* [Conclusión](#conclusión)

---

## Introducción

Este proyecto implementa un sistema de monitoreo climático utilizando una tarjeta comercial que lleva un microcontrolador ESP32. Emplea múltiples sensores analógicoos y digitales para medir variables ambientales como temperatura, humedad, intensidad lumínica, calidad del aire, dirección e intensidad del viento. Los resultados se visualizan en tiempo real a través de una pantalla LCD I2C 16x2 y se opera con un simple pulsador para cambiar secuencialmente entre las distintas pantallas de visualización.

## Objetivo del Proyecto

Desarrollar una estación de monitorización ambiental modular capaz de visualizar en tiempo real información climática desde distintos sensores utilizando un microcontrolador ESP32 y una interfaz LCD. El sistema debe ser interactivo, eficiente y fácil de extender o adaptar. Además deben implementar sistemas de control para el mantenimiento de la temperatura de las baterias en condiciones idoneas para su correcto funcionamiento.

## Hardware Utilizado

* ESP32 DevKit
* Pantalla LCD 16x2 con interfaz I2C
* Sensor de temperatura y humedad DHT22
* Termistor NTC
* Sensor de luz LDR
* Sensor de calidad del aire
* Potenciómetros para la simulación de la intensidad del viento y su dirección
* Botón pulsador para navegación de modo
* Barra de LEDs indicadora de luz 
* Leds indicadores de funcionamiento de calefactor/enfriador
* Resistencias y conexiones necesarias

## Diagrama de Conexiones


<a href="https://github.com/sergiov2020/EIE_actividad_2/blob/1dedea211f7b8844a6fb23307607c5f3c0e5c49d/imagenes/imagen_general.png">
    <img src="https://github.com/sergiov2020/EIE_actividad_2/blob/1dedea211f7b8844a6fb23307607c5f3c0e5c49d/imagenes/imagen_general.png" width="600" />
</a>


| Componente        | Pin ESP32                                |
| ----------------- | ---------------------------------------- |
| DHT22             | GPIO 21                                  |
| LCD SDA           | GPIO 23                                  |
| LCD SCL           | GPIO 22                                  |
| NTC (temperatura) | GPIO 2                                   |
| LDR               | GPIO 4                                   |
| Sensor Gas        | GPIO 34                                  |
| Viento Vertical   | GPIO 35                                  |
| Viento Horizontal | GPIO 32                                  |
| Viento Intensidad | GPIO 33                                  |
| Calefactor        | GPIO 0                                   |
| Enfriador         | GPIO 15                                  |
| Botón             | GPIO 26                                  |
| LEDs indicadores  | GPIOs 27, 14, 12, 13, 9, 7, 8, 17, 18, 5 |

> Simulado en [Wokwi.com](https://wokwi.com/projects/431310772084770817)

## Descripción de los Sensores

### DHT22

Sensor digital que mide temperatura y humedad relativa. Alta precisión y estabilidad. Interfaz de un solo pin digital.

<a href="https://github.com/sergiov2020/EIE_actividad_2/blob/1d6c2b44f4d170b0b2ed779770202c63bb014292/imagenes/Captura%20desde%202025-06-01%2021-46-28.png">
    <img src="https://github.com/sergiov2020/EIE_actividad_2/blob/1d6c2b44f4d170b0b2ed779770202c63bb014292/imagenes/Captura%20desde%202025-06-01%2021-46-28.png" width="300" />
</a>

### NTC (Negative Temperature Coefficient)

Termistor que reduce su resistencia a mayor temperatura. Conectado con divisor resistivo para lectura por entrada analógica.

<a href="https://github.com/sergiov2020/EIE_actividad_2/blob/1d6c2b44f4d170b0b2ed779770202c63bb014292/imagenes/sensor_ntc.png">
    <img src="https://github.com/sergiov2020/EIE_actividad_2/blob/1d6c2b44f4d170b0b2ed779770202c63bb014292/imagenes/sensor_ntc.png" width="300" />
</a>

### LDR

Sensor resistivo que disminuye su resistencia cuando aumenta la intensidad de luz. Se usa con un divisor de tensión y se mide por una entrada analógica del microcontrolador.

<a href="https://github.com/sergiov2020/EIE_actividad_2/blob/1d6c2b44f4d170b0b2ed779770202c63bb014292/imagenes/sensor_ldr.png">
    <img src="https://github.com/sergiov2020/EIE_actividad_2/blob/1d6c2b44f4d170b0b2ed779770202c63bb014292/imagenes/sensor_ldr.png" width="300" />
</a>

### Sensor de Gas

Sensor de GAS que analiza el aire y devuelve un resultado en ppm (partes por millon). Representa la presencia de contaminantes en el aire.

<a href="https://github.com/sergiov2020/EIE_actividad_2/blob/1d6c2b44f4d170b0b2ed779770202c63bb014292/imagenes/sensor_gas.png">
    <img src="https://github.com/sergiov2020/EIE_actividad_2/blob/1d6c2b44f4d170b0b2ed779770202c63bb014292/imagenes/sensor_gas.png" width="600" />
</a>

### Potenciómetros

* **Dirección del viento:** dos ejes (vertical y horizontal) a traves de un joystick que simula la dirección del viento.

<a href="https://github.com/sergiov2020/EIE_actividad_2/blob/1d6c2b44f4d170b0b2ed779770202c63bb014292/imagenes/simulador_dir.png">
    <img src="https://github.com/sergiov2020/EIE_actividad_2/blob/1d6c2b44f4d170b0b2ed779770202c63bb014292/imagenes/simulador_dir.png" width="300" />
</a>

* **Intensidad del viento:** valor analógico proporcional a traves de un potenciometro lineal

<a href="https://github.com/sergiov2020/EIE_actividad_2/blob/1d6c2b44f4d170b0b2ed779770202c63bb014292/imagenes/pot_simul_int_viento.png">
    <img src="https://github.com/sergiov2020/EIE_actividad_2/blob/1d6c2b44f4d170b0b2ed779770202c63bb014292/imagenes/pot_simul_int_viento.png" width="300" />
</a>


# Bill of Materials

| Cantidad | Componente                          | Descripción                                          |
|----------|-------------------------------------|------------------------------------------------------|
| 1        | ESP32 DevKit-C V4                   | Placa de desarrollo con WiFi y Bluetooth             |
| 1        | LCD 16x2 con interfaz I2C           | Display LCD con interfaz I2C                         |
| 1        | Sensor de temperatura NTC           | Sensor de temepratura NTC                            |
| 1        | Fotoresistor                        | Sensor de luz (LDR)                                  |
| 1        | Sensor DHT22                        | Sensor de temperatura y humedad                      |
| 1        | Sensor de gas                       | Sensor de calidad de aire                            |
| 2        | Módulo relay                        | Módulo de relé para control de calefactor/enfriador  |
| 2        | LED (rojo)                          | Diodo emisor de luz color rojo                       |
| 2        | Resistencia 470Ω                    | Para los LEDs conectados al relé                     |
| 1        | Joystick analógico                  | Control tipo palanca con eje X/Y                     |
| 1        | Potenciómetro deslizante            | Control de entrada analógica (slide pot)             |
| 2        | Resistencia 2.2kΩ                   | Resistencias pull-up para I2C                        |
| 1        | Pulsador (verde)                    | Botón con rebote activado                            |
| 1        | Gráfico de barra LED (10 segmentos) | Módulo con 10 LEDs de colores                        |
| 10       | Resistencias 330Ω                   | Para el gráfico de barra LED                         |



## Explicación del Código Fuente

El código está escrito en C++ y se ha utilizado Wokwi para su simulación. Incluye bibliotecas estándar como `Wire`, `LiquidCrystal_I2C` y `DHT`.

* **setup()** inicializa todos los pines, sensores y la pantalla LCD.
* **loop()** actualiza los datos de la intensidad luminosa constantemente y cambia el modo de visualización con el botón.
* Se usan **interrupciones** para leer el botón de modo sin bloquear el ciclo principal.
* Cada sensor tiene su propia función que procesa y muestra la información relevante.

## Funcionamiento del Sistema

1. Al arrancar, se muestra un mensaje de inicio en el LCD.
2. Los LEDs indican la intensidad de luz ambiental medida por el LDR.
3. Un botón permite cambiar entre distintos modos de visualización:

   * Temperatura de batería (NTC)
   * Intensidad de luz (LDR)
   * Temperatura y humedad (DHT22)
   * Calidad del aire (sensor de gas)
   * Intensidad del viento
   * Dirección del viento
4. Según la temperatura de la batería, se activa el calefactor o el enfriador con control de zona muerta.
5. La información se presenta en la pantalla LCD y en el monitor serial.

## Análisis Detallado de Funciones

### debounceCount()

Evita rebotes en la lectura del botón utilizando un umbral de tiempo. Incrementa `ISRCounter` para cambiar de modo.

### luxMonitoring()

Lee el valor del LDR, calcula lux aproximado y enciende una cantidad proporcional de LEDs.

### batteryTempMonitoring()

Lee temperatura con un NTC, convierte el valor de la entrada analógica en grados Celsius usando fórmula de Steinhart-Hart. Controla el calefactor o enfriador.

### ldrMonitoring()

Convierte la lectura del LDR en resistencia y luego lo convierte a lux. Muestra el valor en pantalla.

### dht22Monitoring()

Obtiene la temperatura y humedad usando la librería `DHT`. Los datos se muestran en pantalla.

### airQualityMonitoring()

Lee un valor ADC del sensor de gas y lo muestra como "ppm" en el LCD. No es un valor real, sino una estimación simulada.

### windIntensityMonitoring()

Muestra la intensidad del viento a partir de un valor analógico que se simula con un potenciómetro.

### dirMonitoring()

Evalúa los valores de los ejes vertical y horizontal del viento, simulados a través de un joystick analógico, para determinar su dirección cardinal. 

## Uso del Sistema

1. Abrir el proyecto en Wokwi.
2. Observar los LEDs y la pantalla LCD.
3. Usar el botón para cambiar entre los 6 modos de visualización.
4. Usar potenciómetros para simular cambios en sensores.
5. Visualizar los resultados en el LCD y monitor serie.

## Limitaciones y Mejoras Futuras

* El sistema no posee almacenamiento de datos.
* No implementa comunicación remota (Wi-Fi o Bluetooth).
* La estimación de dirección del viento es básica y sin precisión angular.
* Agregar pantalla táctil o interfaz web.
* Añadir base de datos para estudio de historico de datos y sacar valores relevantes.

## Conclusión

El sistema de monitorización climática propuesto permite observar múltiples variables ambientales en tiempo real. Su diseño modular facilita su comprensión, extensión y adaptación a escenarios reales. Además, su integración en Wokwi permite probarlo sin necesidad de hardware físico.
