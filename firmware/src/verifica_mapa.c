#include "hardware/regs/addressmap.h"
#include "hardware/regs/adc.h"
#include "hardware/regs/resets.h"
#include "hardware/regs/pads_bank0.h"
#include "hardware/regs/io_bank0.h"

#include "plataforma.h"

_Static_assert(ADC_BASE_ADDR == ADC_BASE, "ADC_BASE");
_Static_assert(RESETS_BASE_ADDR == RESETS_BASE, "RESETS_BASE");
_Static_assert(IO_BANK0_BASE_ADDR == IO_BANK0_BASE, "IO_BANK0_BASE");
_Static_assert(PADS_BANK0_BASE_ADDR == PADS_BANK0_BASE, "PADS_BANK0_BASE");

_Static_assert(RESETS_RESET_OFS == RESETS_RESET_OFFSET, "RESET offset");
_Static_assert(RESETS_RESET_DONE_OFS == RESETS_RESET_DONE_OFFSET, "RESET_DONE offset");
_Static_assert(RESETS_ADC_BIT == RESETS_RESET_ADC_BITS, "ADC reset bit");

_Static_assert(IO_GPIO_CTRL_OFS(26) == IO_BANK0_GPIO26_CTRL_OFFSET, "GPIO26 ctrl offset");
_Static_assert(IO_GPIO_FUNCSEL_MASK == IO_BANK0_GPIO26_CTRL_FUNCSEL_BITS, "GPIO26 FUNCSEL mask");
_Static_assert(PADS_GPIO_OFS(26) == PADS_BANK0_GPIO26_OFFSET, "GPIO26 pad offset");
_Static_assert(PAD_OD_BIT == PADS_BANK0_GPIO26_OD_BITS, "GPIO26 OD");
_Static_assert(PAD_IE_BIT == PADS_BANK0_GPIO26_IE_BITS, "GPIO26 IE");
_Static_assert(PAD_PUE_BIT == PADS_BANK0_GPIO26_PUE_BITS, "GPIO26 PUE");
_Static_assert(PAD_PDE_BIT == PADS_BANK0_GPIO26_PDE_BITS, "GPIO26 PDE");
#if PADS_TIENE_ISO
_Static_assert(PAD_ISO_BIT == PADS_BANK0_GPIO26_ISO_BITS, "GPIO26 ISO");
#endif

_Static_assert(ADC_CS_OFS == ADC_CS_OFFSET, "ADC CS offset");
_Static_assert(ADC_RESULT_OFS == ADC_RESULT_OFFSET, "ADC RESULT offset");
_Static_assert(ADC_FCS_OFS == ADC_FCS_OFFSET, "ADC FCS offset");
_Static_assert(ADC_DIV_OFS == ADC_DIV_OFFSET, "ADC DIV offset");
_Static_assert(ADC_CS_EN == ADC_CS_EN_BITS, "ADC CS.EN");
_Static_assert(ADC_CS_START_ONCE == ADC_CS_START_ONCE_BITS, "ADC START_ONCE");
_Static_assert(ADC_CS_READY == ADC_CS_READY_BITS, "ADC READY");
_Static_assert(ADC_AINSEL_MASK == ADC_CS_AINSEL_BITS, "ADC AINSEL mask");
_Static_assert(ADC_RROBIN_MASK == ADC_CS_RROBIN_BITS, "ADC RROBIN mask");
_Static_assert(ADC_RESULT_MASK == ADC_RESULT_BITS, "ADC result bits");
