#include "stm32f4xx.h"

#define pin_INPUT	0x0U
#define pin_OUTPUT	0x1U
#define pin_ALT		0x2U
#define pin_ANALOG	0x3U
#define pinMODE(PORT, PIN, MODE) MODIFY_REG((PORT)->MODER, 0x3U << (uint32_t)((PIN)*2U), (MODE) << (uint32_t)((PIN)*2U))

#define spd_LOW		0x0U
#define spd_MEDIUM	0x1U
#define spd_HIGH	0x2U
#define spd_VHIGH	0x3U
#define pinSPD(PORT, PIN, SPEED) MODIFY_REG((PORT)->OSPEEDR, 0x3U << (uint32_t)((PIN)*2U), (SPEED) << (uint32_t)((PIN)*2U))

#define pin_NOPULL	0x0U
#define pin_PULLUP	0x1U
#define pin_PULLDOWN	0x2U
#define pinPULL(PORT, PIN, PULL) MODIFY_REG((PORT)->PUPDR, 0x3U << (uint32_t)((PIN)*2U), (PULL) << (uint32_t)((PIN)*2U))

#define HIGH	0x1U
#define LOW 	0x0U

#define READ_PIN(PORT, PIN) (((PORT)->IDR & (uint32_t)(1U << (PIN))) >> (PIN))
#define WRITE_PIN(PORT, PIN, VAL) MODIFY_REG((PORT)->ODR,  0x1U << (uint32_t)(PIN), (VAL) << (uint32_t)(PIN))
#define TOGGLE_PIN(PORT, PIN) WRITE_PIN((PORT), (PIN), ~(READ_PIN((PORT), (PIN)))


