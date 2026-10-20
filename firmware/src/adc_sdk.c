#include "hardware/adc.h"

#include "plataforma.h"
#include "adc_backend.h"

static void sdk_init(void) {
    adc_init();
    adc_gpio_init(ADC_GPIO);
    adc_select_input(ADC_INPUT);
}

static uint16_t sdk_leer(void) {
    return adc_read();
}

const adc_backend_t adc_backend_sdk = {
    .nombre = "Pico SDK",
    .init = sdk_init,
    .leer = sdk_leer,
};
