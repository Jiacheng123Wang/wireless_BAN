#ifndef NRF_LIBRARY_UPDATE_H__
#define NRF_LIBRARY_UPDATE_H__

#include "nrf_gpio.h"
#include "define_config.h"

#if (ARGET_CHIP == NRF52)

/* input/ouput PIN configuration, used for UICC interface IO */

__STATIC_INLINE void nrf_gpio_cfg_output_S0D1(uint32_t pin_number)
{
    nrf_gpio_cfg(
            pin_number,
            NRF_GPIO_PIN_DIR_OUTPUT,
            NRF_GPIO_PIN_INPUT_CONNECT,
            NRF_GPIO_PIN_PULLUP,
            NRF_GPIO_PIN_S0D1,
            NRF_GPIO_PIN_NOSENSE);
}

__STATIC_INLINE void nrf_gpio_cfg_output_H0H1(uint32_t pin_number)
{
    nrf_gpio_cfg(
            pin_number,
            NRF_GPIO_PIN_DIR_OUTPUT,
            NRF_GPIO_PIN_INPUT_DISCONNECT,
            NRF_GPIO_PIN_NOPULL,
            NRF_GPIO_PIN_H0H1,
            NRF_GPIO_PIN_NOSENSE);
}

__STATIC_INLINE void nrf_gpio_cfg_input_H0H1(uint32_t pin_number, nrf_gpio_pin_pull_t pull_config)
{
    nrf_gpio_cfg(
            pin_number,
            NRF_GPIO_PIN_DIR_INPUT,
            NRF_GPIO_PIN_INPUT_CONNECT,
            pull_config,
            NRF_GPIO_PIN_H0H1,
            NRF_GPIO_PIN_NOSENSE);
}


/* the minimal unit of delay function */
static void __INLINE delay_tick(uint32_t volatile number_of_tick) __attribute__((always_inline));
static void __INLINE delay_tick(uint32_t volatile number_of_tick)
{
register uint32_t delay asm ("r0") = number_of_tick;
__ASM volatile (
    "1:\n"
    " SUBS %0, %0, #1\n"
    " BNE 1b\n"
    : "+r" (delay));
}

#else
static __INLINE void nrf_gpio_cfg_output_S0D1(uint32_t pin_number)
{
    /*lint -e{845} // A zero has been given as right argument to operator '|'" */
    NRF_GPIO->PIN_CNF[pin_number] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                            | (GPIO_PIN_CNF_DRIVE_H0D1 << GPIO_PIN_CNF_DRIVE_Pos)
                                            | (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos)
                                            | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                            | (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
}

static __INLINE void nrf_gpio_cfg_output_H0H1(uint32_t pin_number)
{
    /*lint -e{845} // A zero has been given as right argument to operator '|'" */
    NRF_GPIO->PIN_CNF[pin_number] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                            | (GPIO_PIN_CNF_DRIVE_H0H1 << GPIO_PIN_CNF_DRIVE_Pos)
                                            | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
                                            | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                            | (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
}

extern void inline
__attribute__((__gnu_inline__,__always_inline__))
delay_tick(uint32_t volatile number_of_tick)
{
  __ASM volatile (
      "1:\tSUB %0, %0, #1\n\t"
      "BNE 1b\n\t"
      : "+l"(number_of_tick)
  );
}

extern void inline
__attribute__((__gnu_inline__,__always_inline__))
delay_us(uint32_t volatile number_of_us)
{
  __ASM volatile (
    "1:\tSUB %0, %0, #1\n\t"
    " NOP\n"
    " NOP\n"
    " NOP\n"
    " NOP\n"
    " NOP\n"
    " NOP\n"   
    " NOP\n"  
    " NOP\n"
    " NOP\n"
    " NOP\n"
    " NOP\n"
    " NOP\n"
    "BNE 1b\n\t"
    : "+l"(number_of_us)
  );
}

__INLINE void delay_ms(uint32_t volatile number_of_ms)
{
  while(number_of_ms != 0)
  {
    number_of_ms--;
    delay_tick(4058);
  }
}

#endif

#endif 