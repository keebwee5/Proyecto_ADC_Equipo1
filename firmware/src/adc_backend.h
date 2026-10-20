#ifndef ADC_BACKEND_H
#define ADC_BACKEND_H

#include <stdint.h>

typedef struct {
    const char *nombre;
    void (*init)(void);
    uint16_t (*leer)(void);
} adc_backend_t;

extern const adc_backend_t adc_backend_regs;
extern const adc_backend_t adc_backend_sdk;

#endif
