#ifndef PLATAFORMA_H
#define PLATAFORMA_H

#include <stdint.h>

/* Mapa escrito a mano para demostrar acceso directo a registros. */
#if PICO_RP2040
#define CHIP_NOMBRE             "RP2040"
#define TARJETA_NOMBRE          "Raspberry Pi Pico"
#define RESETS_BASE_ADDR        0x4000C000u
#define IO_BANK0_BASE_ADDR      0x40014000u
#define PADS_BANK0_BASE_ADDR    0x4001C000u
#define ADC_BASE_ADDR           0x4004C000u
#define ADC_AINSEL_MASK         0x00007000u
#define ADC_RROBIN_MASK         0x001F0000u
#define PADS_TIENE_ISO          0
#elif PICO_RP2350
#define CHIP_NOMBRE             "RP2350"
#define TARJETA_NOMBRE          "Raspberry Pi Pico 2 W"
#define RESETS_BASE_ADDR        0x40020000u
#define IO_BANK0_BASE_ADDR      0x40028000u
#define PADS_BANK0_BASE_ADDR    0x40038000u
#define ADC_BASE_ADDR           0x400A0000u
#define ADC_AINSEL_MASK         0x0000F000u
#define ADC_RROBIN_MASK         0x01FF0000u
#define PADS_TIENE_ISO          1
#else
#error "Plataforma no soportada: usar PICO_BOARD=pico o pico2_w"
#endif

#define ADC_GPIO                26u
#define ADC_INPUT               0u

/* RESETS */
#define RESETS_RESET_OFS        0x000u
#define RESETS_RESET_DONE_OFS   0x008u
#define RESETS_ADC_BIT          (1u << 0)

/* IO_BANK0: por GPIO hay STATUS y CTRL; GPIOx_CTRL = 0x04 + 8*x */
#define IO_GPIO_CTRL_OFS(x)     (0x004u + 0x008u * (x))
#define IO_GPIO_FUNCSEL_MASK    0x0000001Fu
#define IO_GPIO_FUNC_NULL       0x0000001Fu

/* PADS_BANK0: VOLTAGE_SELECT en 0x00; GPIOx en 0x04 + 4*x */
#define PADS_GPIO_OFS(x)        (0x004u + 0x004u * (x))
#define PAD_ISO_BIT             (1u << 8)
#define PAD_OD_BIT              (1u << 7)
#define PAD_IE_BIT              (1u << 6)
#define PAD_PUE_BIT             (1u << 3)
#define PAD_PDE_BIT             (1u << 2)

/* ADC */
#define ADC_CS_OFS              0x000u
#define ADC_RESULT_OFS          0x004u
#define ADC_FCS_OFS             0x008u
#define ADC_FIFO_OFS            0x00Cu
#define ADC_DIV_OFS             0x010u
#define ADC_INTR_OFS            0x014u
#define ADC_INTE_OFS            0x018u
#define ADC_INTF_OFS            0x01Cu
#define ADC_INTS_OFS            0x020u

#define ADC_CS_EN               (1u << 0)
#define ADC_CS_TS_EN            (1u << 1)
#define ADC_CS_START_ONCE       (1u << 2)
#define ADC_CS_START_MANY       (1u << 3)
#define ADC_CS_READY            (1u << 8)
#define ADC_CS_ERR              (1u << 9)
#define ADC_CS_ERR_STICKY       (1u << 10)
#define ADC_CS_AINSEL_LSB       12u
#define ADC_RESULT_MASK         0x00000FFFu

#define REG32(addr) (*(volatile uint32_t *)(uintptr_t)(addr))

#endif
