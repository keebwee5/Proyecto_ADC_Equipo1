# 4. Acceso directo mediante punteros y configuración con SDK

## Implementación mediante punteros

Archivo: `firmware/src/adc_regs.c`.

### Dirección y puntero

```c
static volatile uint32_t *const adc_cs =
    (volatile uint32_t *)(uintptr_t)(ADC_BASE_ADDR + ADC_CS_OFS);
```

- `ADC_BASE_ADDR` cambia entre RP2040 y RP2350.
- `ADC_CS_OFS` vale `0x00`.
- La suma produce la dirección absoluta.
- El cast convierte la dirección numérica en un puntero a un registro de 32 bits.

### Desreferenciación

```c
*adc_cs = ADC_CS_EN;
```

El operador `*` accede al contenido de la dirección. En este caso no es RAM normal: es un registro del periférico.

### `volatile`

`volatile` indica al compilador que el valor puede cambiar por acción del hardware y que las lecturas/escrituras no deben eliminarse ni conservarse únicamente en registros de CPU.

### Máscaras

```c
#define ADC_CS_EN         (1u << 0)
#define ADC_CS_START_ONCE (1u << 2)
#define ADC_CS_READY      (1u << 8)
```

Una máscara permite modificar o consultar únicamente los bits relevantes.

Ejemplo:

```c
*adc_cs |= ADC_CS_START_ONCE;
while (((*adc_cs) & ADC_CS_READY) == 0u) { }
```

### Tamaño del registro

Los registros utilizados son de **32 bits**, por eso se usa `uint32_t`. `RESULT` contiene un valor útil de 12 bits.

## Implementación equivalente con Pico SDK

Archivo: `firmware/src/adc_sdk.c`.

```c
adc_init();
adc_gpio_init(26);
adc_select_input(0);
uint16_t raw = adc_read();
```

## Relación SDK -> hardware

| SDK | Operación que abstrae |
|---|---|
| `adc_init()` | Reset del ADC, habilitación de `CS.EN` y espera de `CS.READY` |
| `adc_gpio_init(26)` | `IO_BANK0.GPIO26_CTRL.FUNCSEL=NULL`, pulls desactivados y receptor digital deshabilitado en el pad |
| `adc_select_input(0)` | Escritura de `CS.AINSEL` |
| `adc_read()` | `START_ONCE`, espera de `READY` y lectura de `RESULT` |

## Ventajas y limitaciones

### Punteros y registros

**Ventajas**
- Muestra exactamente qué dirección y bit controla el hardware.
- Permite comprender el mapa del periférico.
- Da control total sobre la secuencia.

**Limitaciones**
- El código depende más del chip.
- Es más fácil equivocarse en direcciones o máscaras.
- Hay que manejar diferencias como el bit `ISO` del RP2350.

### Pico SDK

**Ventajas**
- Código más corto y legible.
- El SDK selecciona el mapa correcto para cada plataforma.
- Reduce errores al configurar GPIO y registros.

**Limitaciones**
- Oculta parte del funcionamiento interno.
- Para explicar arquitectura y registros es necesario consultar el código y los headers del SDK.

## Equivalencia funcional

Las dos implementaciones realizan exactamente la misma tarea: obtener una muestra de ADC0. El comando REPL permite elegir el backend y comparar los resultados sin cambiar el circuito.
