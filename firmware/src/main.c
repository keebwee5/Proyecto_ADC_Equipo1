#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/structs/sio.h"

#include "plataforma.h"
#include "adc_backend.h"

#define LED_PIN 15
#define BUTTON_PIN 13
#define LED_MASK (1u << LED_PIN)
#define BUTTON_MASK (1u << BUTTON_PIN)

#define LINE_BUFFER_SIZE 64
#define MAX_ARGS 4
#define PROMEDIO_MUESTRAS 16u

typedef int (*command_handler_t)(int argc, char **argv);
typedef struct {
    const char *name;
    const char *help;
    command_handler_t handler;
} command_t;

static int cmd_help(int argc, char **argv);
static int cmd_on(int argc, char **argv);
static int cmd_off(int argc, char **argv);
static int cmd_toggle(int argc, char **argv);
static int cmd_read(int argc, char **argv);
static int cmd_sio(int argc, char **argv);
static int cmd_mask(int argc, char **argv);
static int cmd_status(int argc, char **argv);
static int cmd_luz(int argc, char **argv);

static const command_t commands[] = {
    {"help", "Muestra la lista de comandos", cmd_help},
    {"on", "Enciende LED externo GPIO15", cmd_on},
    {"off", "Apaga LED externo GPIO15", cmd_off},
    {"toggle", "Invierte LED GPIO15", cmd_toggle},
    {"read", "Lee boton GPIO13", cmd_read},
    {"sio", "Muestra GPIO_IN, GPIO_OUT y GPIO_OE", cmd_sio},
    {"mask", "Muestra mascara GPIO15", cmd_mask},
    {"status", "Estado LED y boton", cmd_status},
    {"luz", "luz <regs|sdk>", cmd_luz},
};
#define COMMAND_COUNT (sizeof(commands) / sizeof(commands[0]))

static void read_line(char *buffer, size_t size) {
    size_t pos = 0;
    while (true) {
        int c = getchar();
        if (c == '\r' || c == '\n') {
            putchar('\n');
            buffer[pos] = '\0';
            return;
        }
        if (c == '\b' || c == 127) {
            if (pos > 0) {
                pos--;
                printf("\b \b");
            }
            continue;
        }
        if (pos < size - 1) {
            buffer[pos++] = (char)c;
            putchar(c);
        }
    }
}

static int split_line(char *line, char **argv, int max_args) {
    int argc = 0;
    char *token = strtok(line, " \t");
    while (token != NULL && argc < max_args) {
        argv[argc++] = token;
        token = strtok(NULL, " \t");
    }
    return argc;
}

static void execute_command(int argc, char **argv) {
    if (argc == 0) return;
    for (size_t i = 0; i < COMMAND_COUNT; ++i) {
        if (strcmp(argv[0], commands[i].name) == 0) {
            commands[i].handler(argc, argv);
            return;
        }
    }
    printf("Comando desconocido: %s\n", argv[0]);
    printf("Escribe 'help'.\n");
}

static int cmd_help(int argc, char **argv) {
    (void)argc; (void)argv;
    printf("\nComandos disponibles:\n");
    for (size_t i = 0; i < COMMAND_COUNT; ++i)
        printf("  %-12s - %s\n", commands[i].name, commands[i].help);
    return 0;
}

static int cmd_on(int argc, char **argv) {
    (void)argc; (void)argv;
    sio_hw->gpio_set = LED_MASK;
    printf("LED GPIO%d = 1\n", LED_PIN);
    return 0;
}

static int cmd_off(int argc, char **argv) {
    (void)argc; (void)argv;
    sio_hw->gpio_clr = LED_MASK;
    printf("LED GPIO%d = 0\n", LED_PIN);
    return 0;
}

static int cmd_toggle(int argc, char **argv) {
    (void)argc; (void)argv;
    sio_hw->gpio_togl = LED_MASK;
    printf("GPIO%d invertido\n", LED_PIN);
    return 0;
}

static int cmd_read(int argc, char **argv) {
    (void)argc; (void)argv;
    uint32_t entradas = sio_hw->gpio_in;
    printf("GPIO%d = %d\n", BUTTON_PIN, (entradas & BUTTON_MASK) ? 1 : 0);
    return 0;
}

static int cmd_sio(int argc, char **argv) {
    (void)argc; (void)argv;
    printf("GPIO_IN  = 0x%08lx\n", (unsigned long)sio_hw->gpio_in);
    printf("GPIO_OUT = 0x%08lx\n", (unsigned long)sio_hw->gpio_out);
    printf("GPIO_OE  = 0x%08lx\n", (unsigned long)sio_hw->gpio_oe);
    return 0;
}

static int cmd_mask(int argc, char **argv) {
    (void)argc; (void)argv;
    printf("1u << %d = 0x%08lx\n", LED_PIN, (unsigned long)LED_MASK);
    return 0;
}

static int cmd_status(int argc, char **argv) {
    (void)argc; (void)argv;
    printf("GPIO%d LED   : %lu\n", LED_PIN,
           (unsigned long)((sio_hw->gpio_out & LED_MASK) >> LED_PIN));
    printf("GPIO%d BOTON : %lu\n", BUTTON_PIN,
           (unsigned long)((sio_hw->gpio_in & BUTTON_MASK) >> BUTTON_PIN));
    return 0;
}

static int cmd_luz(int argc, char **argv) {
    if (argc != 2) {
        printf("Uso: luz <regs|sdk>\n");
        return -1;
    }

    const adc_backend_t *backend;
    if (strcmp(argv[1], "regs") == 0) {
        backend = &adc_backend_regs;
    } else if (strcmp(argv[1], "sdk") == 0) {
        backend = &adc_backend_sdk;
    } else {
        printf("Error: backend invalido '%s' (usa regs o sdk)\n", argv[1]);
        return -1;
    }

    backend->init();

    uint32_t suma = 0;
    for (uint32_t i = 0; i < PROMEDIO_MUESTRAS; ++i) {
        suma += backend->leer();
        sleep_ms(2);
    }
    uint32_t raw = (suma + PROMEDIO_MUESTRAS / 2u) / PROMEDIO_MUESTRAS;

    /* Pico SDK usa la relacion 3.3 / 4096 para un ADC de 12 bits.
     * Se trabaja en microvoltios para evitar printf con float. */
    /* El producto supera 32 bits para codigos altos: multiplicar en 64 bits. */
    uint32_t microvoltios = (uint32_t)(((uint64_t)raw * 3300000u + 2048u) / 4096u);
    /* Porcentaje del codigo ADC, NO lux ni porcentaje fisico de iluminacion. */
    uint32_t escala_decimas = (raw * 1000u + 2047u) / 4095u;

    printf("argv[1]    : %s\n", argv[1]);
    printf("Tarjeta    : %s (%s)\n", TARJETA_NOMBRE, CHIP_NOMBRE);
    printf("Backend    : %s\n", backend->nombre);
    printf("ADC bruto  : %lu / 4095\n", (unsigned long)raw);
    printf("Voltaje    : %lu.%03lu V\n",
           (unsigned long)(microvoltios / 1000000u),
           (unsigned long)((microvoltios % 1000000u) / 1000u));
    printf("Escala ADC : %lu.%01lu %% (no lux)\n",
           (unsigned long)(escala_decimas / 10u),
           (unsigned long)(escala_decimas % 10u));
    return 0;
}

int main(void) {
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_down(BUTTON_PIN);

    sleep_ms(1500);
    printf("\nADC + LDR | Equipo 1 | %s (%s)\n", TARJETA_NOMBRE, CHIP_NOMBRE);
    printf("Escribe 'help'.\n");

    char line[LINE_BUFFER_SIZE];
    char *argv[MAX_ARGS];

    while (true) {
        printf("%s> ", CHIP_NOMBRE);
        read_line(line, sizeof(line));
        int argc = split_line(line, argv, MAX_ARGS);
        execute_command(argc, argv);
    }
}
