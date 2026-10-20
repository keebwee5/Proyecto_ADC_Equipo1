# 3. Mapa de registros utilizado

La implementación directa usa únicamente los registros necesarios para preparar GP26, sacar el ADC de reset, seleccionar el canal y obtener una conversión.

## RP2040 - Raspberry Pi Pico

### Direcciones base

| Bloque | Base |
|---|---:|
| RESETS | `0x4000C000` |
| IO_BANK0 | `0x40014000` |
| PADS_BANK0 | `0x4001C000` |
| ADC | `0x4004C000` |

### Registros utilizados

| Registro | Offset | Dirección | Acceso | Uso |
|---|---:|---:|---|---|
| `RESETS.RESET` | `0x000` | `0x4000C000` | R/W | Bit 0 controla reset del ADC |
| `RESETS.RESET_DONE` | `0x008` | `0x4000C008` | R | Bit 0 indica ADC fuera de reset |
| `IO_BANK0.GPIO26_CTRL` | `0x0D4` | `0x400140D4` | R/W | `FUNCSEL=31` desconecta funciones digitales |
| `PADS_BANK0.GPIO26` | `0x06C` | `0x4001C06C` | R/W | Deshabilita funciones digitales del pad |
| `ADC.CS` | `0x000` | `0x4004C000` | R/W | Control y estado |
| `ADC.RESULT` | `0x004` | `0x4004C004` | R | Resultado de 12 bits |

## RP2350 - Raspberry Pi Pico 2 W

### Direcciones base

| Bloque | Base |
|---|---:|
| RESETS | `0x40020000` |
| IO_BANK0 | `0x40028000` |
| PADS_BANK0 | `0x40038000` |
| ADC | `0x400A0000` |

### Registros utilizados

| Registro | Offset | Dirección | Acceso | Uso |
|---|---:|---:|---|---|
| `RESETS.RESET` | `0x000` | `0x40020000` | R/W | Bit 0 controla reset del ADC |
| `RESETS.RESET_DONE` | `0x008` | `0x40020008` | R | Bit 0 indica ADC fuera de reset |
| `IO_BANK0.GPIO26_CTRL` | `0x0D4` | `0x400280D4` | R/W | `FUNCSEL=31` desconecta funciones digitales |
| `PADS_BANK0.GPIO26` | `0x06C` | `0x4003806C` | R/W | Configura el pad analógico; incluye `ISO` |
| `ADC.CS` | `0x000` | `0x400A0000` | R/W | Control y estado |
| `ADC.RESULT` | `0x004` | `0x400A0004` | R | Resultado de 12 bits |

## Campos utilizados

### `RESETS.RESET` y `RESET_DONE`

| Bit | Campo | Valor usado |
|---:|---|---|
| 0 | ADC | `0` en RESET para liberar el periférico; esperar `1` en RESET_DONE |


### `IO_BANK0.GPIO26_CTRL`

| Bits | Campo | Valor usado | Función |
|---:|---|---:|---|
| [4:0] | `FUNCSEL` | 31 (`NULL`) | Desconecta GP26 de las funciones digitales/periféricas |

### `PADS_BANK0.GPIO26`

| Bit | Campo | RP2040 | RP2350 | Valor usado |
|---:|---|---|---|---:|
| 8 | `ISO` | No existe | Sí | 0 |
| 7 | `OD` | Sí | Sí | 1 |
| 6 | `IE` | Sí | Sí | 0 |
| 3 | `PUE` | Sí | Sí | 0 |
| 2 | `PDE` | Sí | Sí | 0 |

`OD=1` evita que el pad sea manejado como salida digital. `IE=0` apaga el receptor digital. Las resistencias internas pull-up/pull-down se deshabilitan. En RP2350 `ISO=0` retira el aislamiento del pad.

### `ADC.CS`

| Campo | RP2040 | RP2350 | Valor usado | Función |
|---|---|---|---:|---|
| `EN` | bit 0 | bit 0 | 1 | Enciende el ADC |
| `START_ONCE` | bit 2 | bit 2 | pulso a 1 | Inicia una conversión |
| `READY` | bit 8 | bit 8 | esperar 1 | Conversión terminada / listo |
| `AINSEL` | [14:12] | [15:12] | 0 | Selecciona ADC0 / GP26 |

Otros campos importantes aunque no se usan en la prueba:

- `START_MANY`: conversión continua.
- `RROBIN`: secuencia automática entre canales.
- `TS_EN`: sensor interno de temperatura.
- `ERR` y `ERR_STICKY`: errores de conversión.

### `ADC.RESULT`

| Bits | Acceso | Contenido |
|---|---|---|
| [11:0] | R | Resultado de la conversión, `0...4095` |

## Secuencia de acceso

Ejemplo RP2040:

```text
ADC_BASE          = 0x4004C000
CS offset         = 0x000
ADC_CS dirección  = 0x4004C000
puntero           = (volatile uint32_t *)0x4004C000
valor             = EN / AINSEL / START_ONCE
resultado         = *(0x4004C004) & 0xFFF
```

La misma secuencia se usa en RP2350 sustituyendo las direcciones base correspondientes.
