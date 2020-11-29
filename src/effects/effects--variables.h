#ifndef _effect_variables_h
#define _effect_variables_h
#include <stdint.h>

// ----- EFFECTS CONSTANTS ----- last mod v1.7.21b
static const uint8_t pwr_mode_size = sizeof(pmode_t);
static const uint8_t effects_list_on_size = sizeof(effects_list_on) / sizeof(eff_item_t) - 1U; // количество в цифровом исчислении, где 0 - это 1 (необходимо для работы циклов)
static const uint8_t effects_list_off_size = sizeof(effects_list_off) / sizeof(eff_item_t) - 1U; // количество в цифровом исчислении, где 0 - это 1 (необходимо для работы циклов)
static const uint8_t effects_list_lamp_size = sizeof(effects_list_lamp) / sizeof(eff_item_t) - 1U; // количество в цифровом исчислении, где 0 - это 1 (необходимо для работы циклов)
static const uint8_t effects_list_demo_size = sizeof(effects_list_demo) / sizeof(eff_item_t) - 1U; // количество в цифровом исчислении, где 0 - это 1 (необходимо для работы циклов)
static const uint8_t effects_list_paint_size = sizeof(effects_list_paint) / sizeof(eff_item_t) - 1U; // количество в цифровом исчислении, где 0 - это 1 (необходимо для работы циклов)

extern const effects_list_t effects_lists[pwr_last] = {
		{nullptr, 0},
		{effects_list_off, sizeof(effects_list_off) / sizeof(eff_item_t)},
		{effects_list_on, sizeof(effects_list_on) / sizeof(eff_item_t)},
		{effects_list_lamp, sizeof(effects_list_lamp) / sizeof(eff_item_t)},
		{effects_list_demo, sizeof(effects_list_demo) / sizeof(eff_item_t)},
		{effects_list_paint, sizeof(effects_list_paint) / sizeof(eff_item_t)}};

#endif