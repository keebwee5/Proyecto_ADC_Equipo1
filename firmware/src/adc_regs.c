#include <stdint.h>

#include "plataforma.h"
#include "adc_backend.h"

static volatile uint32_t *const resets_reset =
    (volatile uint32_t *)(uintptr_t)(RESETS_BASE_ADDR + RESETS_RESET_OFS);
static volatile uint32_t *const resets_done =
    (volatile uint32_t *)(uintptr_t)(RESETS_BASE_ADDR + RESETS_RESET_DONE_OFS);
static volatile uint32_t *const gpio26_ctrl =
    (volatile uint32_t *)(uintptr_t)(IO_BANK0_BASE_ADDR + IO_GPIO_CTRL_OFS(ADC_GPIO));
static volatile uint32_t *const pad_gpio26 =
    (volatile uint32_t *)(uintptr_t)(PADS_BANK0_BASE_ADDR + PADS_GPIO_OFS(ADC_GPIO));
static volatile uint32_t *const adc_cs =
    (volatile uint32_t *)(uintptr_t)(ADC_BASE_ADDR + ADC_CS_OFS);
static volatile uint32_t *const adc_result =
    (volatile uint32_t *)(uintptr_t)(ADC_BASE_ADDR + ADC_RESULT_OFS);

static void regs_init(void) {
    /* 1) Reiniciar y liberar el ADC. Bit 0 controla ADC en ambos chips. */
    *resets_reset |= RESETS_ADC_BIT;
    *resets_reset &= ~RESETS_ADC_BIT;
    while (((*resets_done) & RESETS_ADC_BIT) == 0u) { }

    /* 2) Desconectar GP26 de las funciones digitales de IO_BANK0.
     * FUNCSEL=31 (NULL) deja el driver de perifericos en alta impedancia. */
    uint32_t ctrl = *gpio26_ctrl;
    ctrl = (ctrl & ~IO_GPIO_FUNCSEL_MASK) | IO_GPIO_FUNC_NULL;
    *gpio26_ctrl = ctrl;

    /* 3) Preparar el pad como entrada analogica: sin receptor digital,
     * sin pulls y con salida deshabilitada. RP2350 agrega ISO. */
    uint32_t pad = *pad_gpio26;
    pad |= PAD_OD_BIT;
    pad &= ~(PAD_IE_BIT | PAD_PUE_BIT | PAD_PDE_BIT);
#if PADS_TIENE_ISO
    pad &= ~PAD_ISO_BIT;
#endif
    *pad_gpio26 = pad;

    /* 4) Encender ADC y esperar a que el hardware este listo. */
    *adc_cs = ADC_CS_EN;
    while (((*adc_cs) & ADC_CS_READY) == 0u) { }

    /* 5) ADC0 / GP26. */
    uint32_t cs = *adc_cs;
    cs &= ~ADC_AINSEL_MASK;
    cs |= (ADC_INPUT << ADC_CS_AINSEL_LSB) & ADC_AINSEL_MASK;
    *adc_cs = cs;
}

static uint16_t regs_leer(void) {
    *adc_cs |= ADC_CS_START_ONCE;
    while (((*adc_cs) & ADC_CS_READY) == 0u) { }
    return (uint16_t)((*adc_result) & ADC_RESULT_MASK);
}

const adc_backend_t adc_backend_regs = {
    .nombre = "registros y punteros",
    .init = regs_init,
    .leer = regs_leer,
};
