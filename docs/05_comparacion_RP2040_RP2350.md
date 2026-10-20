# 5. Comparación técnica RP2040 vs RP2350

La práctica usa Raspberry Pi Pico con RP2040 y Raspberry Pi Pico 2 W con RP2350A. El circuito externo no cambia: el divisor LDR + 10 kΩ permanece conectado a GP26/ADC0.

| Característica | RP2040 | RP2350 / Pico 2 W |
|---|---|---|
| Arquitectura ADC | SAR | SAR |
| Resolución | 12 bits | 12 bits |
| Velocidad máxima indicada por SDK | 500 kS/s | 500 kS/s |
| ENOB indicado por SDK | 8.7 | 9.2 |
| FIFO | 8 muestras | 8 muestras |
| DMA | Sí | Sí |
| Interrupción de FIFO | Sí | Sí |
| ADC externos en QFN-60 | GPIO26-29 | GPIO26-29 |
| Sensor interno | Sí | Sí |
| `ADC_BASE` | `0x4004C000` | `0x400A0000` |
| `RESETS_BASE` | `0x4000C000` | `0x40020000` |
| `IO_BANK0_BASE` | `0x40014000` | `0x40028000` |
| `PADS_BANK0_BASE` | `0x4001C000` | `0x40038000` |
| Offset `CS` | `0x00` | `0x00` |
| Offset `RESULT` | `0x04` | `0x04` |
| `CS.AINSEL` | [14:12] | [15:12] |
| `CS.RROBIN` | [20:16], 5 bits | [24:16], hasta 9 bits |
| `PADS.GPIO26.ISO` | No existe | Sí, bit 8 |

## Qué permanece igual en el código

- GP26 se usa como ADC0 en las dos placas del proyecto.
- `CS.EN`, `CS.START_ONCE` y `CS.READY` mantienen la misma posición.
- `RESULT` permanece en offset `0x04` y contiene 12 bits.
- El código de alto nivel con Pico SDK es el mismo.
- El divisor LDR + 10 kΩ y el cableado son los mismos.

## Qué cambia en acceso directo

1. Dirección base de `ADC`.
2. Dirección base de `RESETS`.
3. Dirección base de `IO_BANK0`.
4. Dirección base de `PADS_BANK0`.
5. El RP2350 incluye `ISO` en el pad y debe quedar desactivado.
6. El campo `AINSEL` es más ancho en RP2350 porque existen variantes con más entradas ADC.
7. `RROBIN` también es más ancho en RP2350.

## Efecto del SDK

La llamada:

```c
adc_init();
adc_gpio_init(26);
adc_select_input(0);
adc_read();
```

se mantiene igual para ambas tarjetas. Durante la compilación el Pico SDK selecciona los headers y estructuras adecuados a la plataforma, ocultando los cambios de dirección y campos.

## Observación sobre la referencia analógica

La Pico 2 W usa su alimentación analógica como referencia del ADC y la documentación de la placa advierte que existen offset y ruido asociados a esa alimentación. Para una demostración con LDR se puede usar el valor nominal de 3.3 V y promediar varias muestras; para medición de precisión se requiere caracterizar o mejorar la referencia.
