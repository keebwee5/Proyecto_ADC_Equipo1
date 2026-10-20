# 1. Investigación, antecedentes y aplicaciones

## ¿Qué es el ADC?

Un **convertidor analógico-digital (ADC)** transforma una magnitud eléctrica continua, normalmente un voltaje, en un número que la CPU puede procesar. Resuelve el problema de conectar el mundo físico, que produce señales analógicas, con sistemas digitales que trabajan con valores binarios.

En RP2040 y RP2350 el ADC es de tipo **SAR (Successive Approximation Register)**. El Pico SDK documenta un ADC de **12 bits**, hasta **500 kS/s**, con FIFO de 8 muestras, interrupción y soporte para DMA.

La información manejada sigue esta cadena:

```text
magnitud física -> sensor -> voltaje -> ADC -> código de 12 bits -> CPU
```

Para este proyecto:

```text
luz -> R_LDR -> divisor de voltaje -> GP26/ADC0 -> 0...4095 -> voltaje y escala ADC
```

## Relación con CPU, buses y GPIO

El ADC es un periférico mapeado en memoria. La CPU accede a sus registros por el bus interno mediante direcciones de memoria. La entrada analógica seleccionada se conecta al multiplexor del ADC; en las placas usadas, GPIO26 corresponde a ADC0.

Para usar un GPIO como entrada analógica se deben deshabilitar las funciones digitales del pad. El SDK lo hace mediante `adc_gpio_init()`; en la implementación directa se selecciona `GPIO_FUNC_NULL` en `IO_BANK0.GPIO26_CTRL` y se modifican los bits del pad en `PADS_BANK0`.

## Problema tecnológico que resuelve

Una CPU no puede interpretar directamente una tensión continua. El ADC cuantifica ese voltaje y lo representa con un número. Con 12 bits existen 4096 códigos posibles.

## Antecedentes y evolución

La conversión analógica-digital surgió de la necesidad de representar señales físicas con números para transmisión, medición y control. Las arquitecturas fundamentales de ADC se desarrollaron antes de que existieran los microcontroladores modernos. En la arquitectura SAR, la conversión se realiza como una búsqueda binaria comparando la entrada contra valores generados internamente.

Los primeros SAR fueron equipos externos de gran tamaño. Con la integración de circuitos digitales y analógicos, los ADC pasaron a implementarse como circuitos integrados y posteriormente como periféricos dentro de microcontroladores. Esto redujo tamaño, consumo, costo y complejidad del sistema.

En sistemas embebidos actuales, el ADC suele compartir pines con GPIO y añade multiplexores, FIFO, DMA, interrupciones y control por software. RP2040 y RP2350 representan esta integración: un ADC SAR de 12 bits controlado mediante registros o mediante Pico SDK.

## Funcionamiento básico de un SAR

1. Se toma una muestra del voltaje de entrada.
2. El SAR prueba primero el bit más significativo.
3. Un DAC interno genera un voltaje de comparación.
4. Un comparador decide si el bit se conserva o se elimina.
5. El proceso se repite bit por bit.
6. El resultado final es el código digital de la muestra.

## Aplicaciones actuales

1. **Sensores analógicos:** temperatura, luz, presión, posición y fuerza.
2. **Monitoreo de baterías y fuentes:** medición de voltajes mediante divisores adecuados.
3. **Control industrial y adquisición de datos:** lectura de señales de procesos y realimentación.
4. **Audio e instrumentación:** digitalización de señales cuando la velocidad y resolución son suficientes para la aplicación.

## Sensor seleccionado: fotoresistencia LDR

La resistencia de la LDR disminuye con la luz. Junto con 10 kΩ forma el divisor **3V3 → LDR → nodo GP26/ADC0 → 10 kΩ → GND**. El ADC mide el voltaje del nodo, no la resistencia ni la luz directamente.

```text
Vnodo = V3V3 × 10000 / (R_LDR + 10000)
Vestimado = raw × 3.3 / 4096
Escala ADC (%) = raw × 100 / 4095
```

Se usa 3.3 V nominal. Ejemplo teórico: si R_LDR = 10 kΩ, Vnodo = 1.65 V y raw ideal ≈ 2048. Esto no implica un nivel de lux conocido. Al iluminar, suben voltaje y código; al cubrir, bajan. No se calculan lux porque no se conoce el modelo ni se dispone de calibración.

## Fuentes

- RP2040 Datasheet, sección ADC and Temperature Sensor.
- RP2350 Datasheet y headers oficiales del Pico SDK.
- Raspberry Pi Pico SDK, `hardware_adc`.
- Adafruit, Photocells: https://learn.adafruit.com/photocells/using-a-photocell (divisor adaptado a 3.3 V).
- Analog Devices, *Which ADC Architecture Is Right for Your Application?*.
