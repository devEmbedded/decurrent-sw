/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#ifndef BOARD_H
#define BOARD_H

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*
 * Setup for devEmbedded DECurrent board
 */

/*
 * Board identifier.
 */
#define BOARD_DECURRENT
#define BOARD_NAME                  "DECurrent"

/*
 * Board oscillators-related settings.
 */
#define STM32_LSECLK                0U
#define STM32_LSEDRV                0U
#define STM32_HSECLK                24000000U

/*
 * MCU type as defined in the ST header.
 */
#define STM32H743xx

// Note: STM32H7 has high-speed ULPI on OTG1 interface, but chibios calls it OTG2
#define BOARD_OTG2_USES_ULPI

/*
 * IO pins assignments.
 * 
 */

// PORT  PIN   NAME         MODE       STATE  OUTTYPE  OUTSPEED PULLMODE  ALTFUNC
#define GPIOA_PINS(X) \
X(GPIOA,  0, ADC1_INP0   , ANALOG     , LOW , PUSHPULL ,  LOW , FLOATING ,  0) \
X(GPIOA,  1, ADC2_INP1   , ANALOG     , LOW , PUSHPULL ,  LOW , FLOATING ,  0) \
X(GPIOA,  2, PA2         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOA,  3, ULPI_D0     , ALTERNATE  , LOW , PUSHPULL , HIGH , FLOATING , 10) \
X(GPIOA,  4, DAC1_OUT    , ANALOG     , LOW , PUSHPULL ,  LOW , FLOATING ,  0) \
X(GPIOA,  5, ULPI_CLK    , ALTERNATE  , LOW , PUSHPULL , HIGH , FLOATING , 10) \
X(GPIOA,  6, BUFFER_DIR  , OUTPUT     , LOW , PUSHPULL ,  LOW , FLOATING ,  0) \
X(GPIOA,  7, GNDREF      , ANALOG     , LOW , PUSHPULL ,  LOW , FLOATING ,  0) \
X(GPIOA,  8, PA8         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOA,  9, USART1_TX   , ALTERNATE  , LOW , PUSHPULL ,  LOW , FLOATING ,  7) \
X(GPIOA, 10, USART1_RX   , ALTERNATE  , LOW , PUSHPULL ,  LOW , PULLDOWN ,  7) \
X(GPIOA, 11, USB_DM      , INPUT      , LOW , PUSHPULL , HIGH , FLOATING ,  0) \
X(GPIOA, 12, USB_DP      , INPUT      , LOW , PUSHPULL , HIGH , FLOATING ,  0) \
X(GPIOA, 13, SWDIO       , ALTERNATE  , LOW , PUSHPULL , HIGH , FLOATING ,  0) \
X(GPIOA, 14, SWCLK       , ALTERNATE  , LOW , PUSHPULL , HIGH , FLOATING ,  0) \
X(GPIOA, 15, USB_PROBECLK, ALTERNATE  , LOW , PUSHPULL , HIGH , FLOATING ,  1) \

#define GPIOB_PINS(X) \
X(GPIOB,  0, ULPI_D1     , ALTERNATE  , LOW , PUSHPULL , HIGH , FLOATING , 10) \
X(GPIOB,  1, ULPI_D2     , ALTERNATE  , LOW , PUSHPULL , HIGH , FLOATING , 10) \
X(GPIOB,  2, PB2         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOB,  3, SWO         , ALTERNATE  , LOW , PUSHPULL , HIGH , FLOATING ,  0) \
X(GPIOB,  4, PB4         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOB,  5, ULPI_D7     , ALTERNATE  , LOW , PUSHPULL , HIGH , FLOATING , 10) \
X(GPIOB,  6, NC6         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOB,  7, BUTTON1     , INPUT      , LOW , PUSHPULL ,  LOW , FLOATING ,  0) \
X(GPIOB,  8, PB8         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOB,  9, PB9         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOB, 10, ULPI_D3     , ALTERNATE  , LOW , PUSHPULL , HIGH , FLOATING , 10) \
X(GPIOB, 11, ULPI_D4     , ALTERNATE  , LOW , PUSHPULL , HIGH , FLOATING , 10) \
X(GPIOB, 12, ULPI_D5     , ALTERNATE  , LOW , PUSHPULL , HIGH , FLOATING , 10) \
X(GPIOB, 13, ULPI_D6     , ALTERNATE  , LOW , PUSHPULL , HIGH , FLOATING , 10) \
X(GPIOB, 14, PB14        , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOB, 15, PB15        , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \

#define GPIOC_PINS(X) \
X(GPIOC,  0, ULPI_STP    , ALTERNATE  , LOW , PUSHPULL , HIGH , FLOATING , 10) \
X(GPIOC,  1, ADC3_INP11  , ANALOG     , LOW , PUSHPULL ,  LOW , FLOATING ,  0) \
X(GPIOC,  2, ULPI_DIR    , ALTERNATE  , LOW , PUSHPULL , HIGH , FLOATING , 10) \
X(GPIOC,  3, ULPI_NXT    , ALTERNATE  , LOW , PUSHPULL , HIGH , FLOATING , 10) \
X(GPIOC,  4, PC4         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOC,  5, PC5         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOC,  6, PC6         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOC,  7, PC7         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOC,  8, PC8         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOC,  9, USB_CLK     , ALTERNATE  , LOW , PUSHPULL , HIGH , FLOATING ,  0) \
X(GPIOC, 10, SPI3_SCK    , ALTERNATE  , LOW , PUSHPULL , HIGH , FLOATING ,  6) \
X(GPIOC, 11, PC11        , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOC, 12, PC12        , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOC, 13, PC13        , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOC, 14, PC14        , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOC, 15, PC15        , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \

#define GPIOD_PINS(X) \
X(GPIOD,  0, PD0         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOD,  1, PD1         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOD,  2, PD2         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOD,  3, PD3         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOD,  4, PD4         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOD,  5, PD5         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOD,  6, SPI3_MOSI   , ALTERNATE  , LOW , PUSHPULL , HIGH , FLOATING ,  5) \
X(GPIOD,  7, PD7         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOD,  8, BOARDREV0   , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOD,  9, BOARDREV1   , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOD, 10, BOARDREV2   , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOD, 11, BOARDREV3   , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOD, 12, PD12        , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOD, 13, PD13        , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOD, 14, PD14        , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOD, 15, PD15        , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \

#define GPIOE_PINS(X) \
X(GPIOE,  0, PE0         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOE,  1, PE1         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOE,  2, PE2         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOE,  3, PE3         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOE,  4, PE4         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOE,  5, VSW_PWM     , ALTERNATE  , LOW , PUSHPULL ,  LOW , PULLDOWN ,  4) \
X(GPIOE,  6, PE6         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOE,  7, QSPI_BK2_IO0, ALTERNATE  , LOW , PUSHPULL , HIGH , FLOATING , 10) \
X(GPIOE,  8, QSPI_BK2_IO1, ALTERNATE  , LOW , PUSHPULL , HIGH , FLOATING , 10) \
X(GPIOE,  9, QSPI_BK2_IO2, ALTERNATE  , LOW , PUSHPULL , HIGH , FLOATING , 10) \
X(GPIOE, 10, QSPI_BK2_IO3, ALTERNATE  , LOW , PUSHPULL , HIGH , FLOATING , 10) \
X(GPIOE, 11, LED_STATUS  , OUTPUT     , LOW , PUSHPULL ,  LOW , FLOATING ,  0) \
X(GPIOE, 12, PE12        , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOE, 13, PE13        , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOE, 14, USB_PHY_RST , OUTPUT     , HIGH, PUSHPULL ,  LOW , FLOATING ,  0) \
X(GPIOE, 15, PE15        , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \

#define GPIOF_PINS(X) \
X(GPIOF,  0, PF0         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOF,  1, PF1         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOF,  2, PF2         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOF,  3, PF3         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOF,  4, HUB_RST     , OUTPUT     , LOW , PUSHPULL ,  LOW , FLOATING ,  0) \
X(GPIOF,  5, PF5         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOF,  6, QSPI_BK1_IO3, ALTERNATE  , LOW , PUSHPULL , HIGH , FLOATING ,  9) \
X(GPIOF,  7, QSPI_BK1_IO2, ALTERNATE  , LOW , PUSHPULL , HIGH , FLOATING ,  9) \
X(GPIOF,  8, QSPI_BK1_IO0, ALTERNATE  , LOW , PUSHPULL , HIGH , FLOATING , 10) \
X(GPIOF,  9, QSPI_BK1_IO1, ALTERNATE  , LOW , PUSHPULL , HIGH , FLOATING , 10) \
X(GPIOF, 10, QSPI_CLK    , ALTERNATE  , LOW , PUSHPULL , HIGH , FLOATING ,  9) \
X(GPIOF, 11, PF11        , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOF, 12, PF12        , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOF, 13, PF13        , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOF, 14, I2C4_SCL    , ALTERNATE  , LOW , PUSHPULL ,  LOW , FLOATING ,  4) \
X(GPIOF, 15, I2C4_SDA    , ALTERNATE  , LOW , PUSHPULL ,  LOW , FLOATING ,  4) \

#define GPIOG_PINS(X) \
X(GPIOG,  0, PG0         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOG,  1, PG1         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOG,  2, PG2         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOG,  3, PG3         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOG,  4, PG4         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOG,  5, PG5         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOG,  6, QSPI_CS     , ALTERNATE  , LOW , PUSHPULL , HIGH , FLOATING , 10) \
X(GPIOG,  7, PG7         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOG,  8, PG8         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOG,  9, PG9         , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOG, 10, PG10        , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOG, 11, PG11        , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOG, 12, PG12        , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \
X(GPIOG, 13, SPI6_SCK    , ALTERNATE  , LOW , PUSHPULL , HIGH , FLOATING ,  5) \
X(GPIOG, 14, SPI6_MOSI   , ALTERNATE  , LOW , PUSHPULL , HIGH , FLOATING ,  5) \
X(GPIOG, 15, PG15        , INPUT      , LOW , PUSHPULL ,  LOW , PULLUP   ,  0) \

#define GPIOH_PINS(X) \
X(GPIOH,  0, XTAL_IN     , INPUT      , LOW , PUSHPULL , HIGH , FLOATING ,  0) \
X(GPIOH,  1, XTAL_OUT    , INPUT      , LOW , PUSHPULL , HIGH , FLOATING ,  0) \

/*
 * I/O ports initial setup, this configuration is established soon after reset
 * in the initialization code.
 * Please refer to the STM32 Reference Manual for details.
 */
#define PIN_MODE_INPUT(n)           (0U << ((n) * 2U))
#define PIN_MODE_OUTPUT(n)          (1U << ((n) * 2U))
#define PIN_MODE_ALTERNATE(n)       (2U << ((n) * 2U))
#define PIN_MODE_ANALOG(n)          (3U << ((n) * 2U))
#define PIN_ODR_LOW(n)              (0U << (n))
#define PIN_ODR_HIGH(n)             (1U << (n))
#define PIN_OTYPE_PUSHPULL(n)       (0U << (n))
#define PIN_OTYPE_OPENDRAIN(n)      (1U << (n))
#define PIN_OSPEED_VERYLOW(n)       (0U << ((n) * 2U))
#define PIN_OSPEED_LOW(n)           (1U << ((n) * 2U))
#define PIN_OSPEED_MEDIUM(n)        (2U << ((n) * 2U))
#define PIN_OSPEED_HIGH(n)          (3U << ((n) * 2U))
#define PIN_PUPDR_FLOATING(n)       (0U << ((n) * 2U))
#define PIN_PUPDR_PULLUP(n)         (1U << ((n) * 2U))
#define PIN_PUPDR_PULLDOWN(n)       (2U << ((n) * 2U))
#define PIN_AFIO_AF(n, v)           ((v) << (((n) % 8U) * 4U))

// Create macros for GPIO pin config
#define GPIO_MODER_VALUE(port, pin, name, mode, odr, otype, ospeed, pupdr, af)  | PIN_MODE_ ## mode(pin)
#define GPIO_ODR_VALUE(port, pin, name, mode, odr, otype, ospeed, pupdr, af)    | PIN_ODR_ ## odr(pin)
#define GPIO_OTYPE_VALUE(port, pin, name, mode, odr, otype, ospeed, pupdr, af)  | PIN_OTYPE_ ## otype(pin)
#define GPIO_OSPEED_VALUE(port, pin, name, mode, odr, otype, ospeed, pupdr, af) | PIN_OSPEED_ ## ospeed(pin)
#define GPIO_PUPDR_VALUE(port, pin, name, mode, odr, otype, ospeed, pupdr, af)  | PIN_PUPDR_ ## pupdr(pin)
#define GPIO_AFRL_VALUE(port, pin, name, mode, odr, otype, ospeed, pupdr, af)   | ((pin < 8) ? PIN_AFIO_AF(pin, af) : 0)
#define GPIO_AFRH_VALUE(port, pin, name, mode, odr, otype, ospeed, pupdr, af)   | ((pin >= 8) ? PIN_AFIO_AF(pin, af) : 0)

#define VAL_GPIOA_MODER   (0 GPIOA_PINS(GPIO_MODER_VALUE))
#define VAL_GPIOA_ODR     (0 GPIOA_PINS(GPIO_ODR_VALUE))
#define VAL_GPIOA_OTYPER  (0 GPIOA_PINS(GPIO_OTYPE_VALUE))
#define VAL_GPIOA_OSPEEDR (0 GPIOA_PINS(GPIO_OSPEED_VALUE))
#define VAL_GPIOA_PUPDR   (0 GPIOA_PINS(GPIO_PUPDR_VALUE))
#define VAL_GPIOA_AFRL    (0 GPIOA_PINS(GPIO_AFRL_VALUE))
#define VAL_GPIOA_AFRH    (0 GPIOA_PINS(GPIO_AFRH_VALUE))

#define VAL_GPIOB_MODER   (0 GPIOB_PINS(GPIO_MODER_VALUE))
#define VAL_GPIOB_ODR     (0 GPIOB_PINS(GPIO_ODR_VALUE))
#define VAL_GPIOB_OTYPER  (0 GPIOB_PINS(GPIO_OTYPE_VALUE))
#define VAL_GPIOB_OSPEEDR (0 GPIOB_PINS(GPIO_OSPEED_VALUE))
#define VAL_GPIOB_PUPDR   (0 GPIOB_PINS(GPIO_PUPDR_VALUE))
#define VAL_GPIOB_AFRL    (0 GPIOB_PINS(GPIO_AFRL_VALUE))
#define VAL_GPIOB_AFRH    (0 GPIOB_PINS(GPIO_AFRH_VALUE))

#define VAL_GPIOC_MODER   (0 GPIOC_PINS(GPIO_MODER_VALUE))
#define VAL_GPIOC_ODR     (0 GPIOC_PINS(GPIO_ODR_VALUE))
#define VAL_GPIOC_OTYPER  (0 GPIOC_PINS(GPIO_OTYPE_VALUE))
#define VAL_GPIOC_OSPEEDR (0 GPIOC_PINS(GPIO_OSPEED_VALUE))
#define VAL_GPIOC_PUPDR   (0 GPIOC_PINS(GPIO_PUPDR_VALUE))
#define VAL_GPIOC_AFRL    (0 GPIOC_PINS(GPIO_AFRL_VALUE))
#define VAL_GPIOC_AFRH    (0 GPIOC_PINS(GPIO_AFRH_VALUE))

#define VAL_GPIOD_MODER   (0 GPIOD_PINS(GPIO_MODER_VALUE))
#define VAL_GPIOD_ODR     (0 GPIOD_PINS(GPIO_ODR_VALUE))
#define VAL_GPIOD_OTYPER  (0 GPIOD_PINS(GPIO_OTYPE_VALUE))
#define VAL_GPIOD_OSPEEDR (0 GPIOD_PINS(GPIO_OSPEED_VALUE))
#define VAL_GPIOD_PUPDR   (0 GPIOD_PINS(GPIO_PUPDR_VALUE))
#define VAL_GPIOD_AFRL    (0 GPIOD_PINS(GPIO_AFRL_VALUE))
#define VAL_GPIOD_AFRH    (0 GPIOD_PINS(GPIO_AFRH_VALUE))

#define VAL_GPIOE_MODER   (0 GPIOE_PINS(GPIO_MODER_VALUE))
#define VAL_GPIOE_ODR     (0 GPIOE_PINS(GPIO_ODR_VALUE))
#define VAL_GPIOE_OTYPER  (0 GPIOE_PINS(GPIO_OTYPE_VALUE))
#define VAL_GPIOE_OSPEEDR (0 GPIOE_PINS(GPIO_OSPEED_VALUE))
#define VAL_GPIOE_PUPDR   (0 GPIOE_PINS(GPIO_PUPDR_VALUE))
#define VAL_GPIOE_AFRL    (0 GPIOE_PINS(GPIO_AFRL_VALUE))
#define VAL_GPIOE_AFRH    (0 GPIOE_PINS(GPIO_AFRH_VALUE))

#define VAL_GPIOF_MODER   (0 GPIOF_PINS(GPIO_MODER_VALUE))
#define VAL_GPIOF_ODR     (0 GPIOF_PINS(GPIO_ODR_VALUE))
#define VAL_GPIOF_OTYPER  (0 GPIOF_PINS(GPIO_OTYPE_VALUE))
#define VAL_GPIOF_OSPEEDR (0 GPIOF_PINS(GPIO_OSPEED_VALUE))
#define VAL_GPIOF_PUPDR   (0 GPIOF_PINS(GPIO_PUPDR_VALUE))
#define VAL_GPIOF_AFRL    (0 GPIOF_PINS(GPIO_AFRL_VALUE))
#define VAL_GPIOF_AFRH    (0 GPIOF_PINS(GPIO_AFRH_VALUE))

#define VAL_GPIOG_MODER   (0 GPIOG_PINS(GPIO_MODER_VALUE))
#define VAL_GPIOG_ODR     (0 GPIOG_PINS(GPIO_ODR_VALUE))
#define VAL_GPIOG_OTYPER  (0 GPIOG_PINS(GPIO_OTYPE_VALUE))
#define VAL_GPIOG_OSPEEDR (0 GPIOG_PINS(GPIO_OSPEED_VALUE))
#define VAL_GPIOG_PUPDR   (0 GPIOG_PINS(GPIO_PUPDR_VALUE))
#define VAL_GPIOG_AFRL    (0 GPIOG_PINS(GPIO_AFRL_VALUE))
#define VAL_GPIOG_AFRH    (0 GPIOG_PINS(GPIO_AFRH_VALUE))

#define VAL_GPIOH_MODER   (0 GPIOH_PINS(GPIO_MODER_VALUE))
#define VAL_GPIOH_ODR     (0 GPIOH_PINS(GPIO_ODR_VALUE))
#define VAL_GPIOH_OTYPER  (0 GPIOH_PINS(GPIO_OTYPE_VALUE))
#define VAL_GPIOH_OSPEEDR (0 GPIOH_PINS(GPIO_OSPEED_VALUE))
#define VAL_GPIOH_PUPDR   (0 GPIOH_PINS(GPIO_PUPDR_VALUE))
#define VAL_GPIOH_AFRL    (0 GPIOH_PINS(GPIO_AFRL_VALUE))
#define VAL_GPIOH_AFRH    (0 GPIOH_PINS(GPIO_AFRH_VALUE))

#define VAL_GPIOI_MODER   (0)  
#define VAL_GPIOI_ODR     (0)  
#define VAL_GPIOI_OTYPER  (0)  
#define VAL_GPIOI_OSPEEDR (0)  
#define VAL_GPIOI_PUPDR   (0x55555555)  
#define VAL_GPIOI_AFRL    (0)  
#define VAL_GPIOI_AFRH    (0)  

#define VAL_GPIOJ_MODER   (0)  
#define VAL_GPIOJ_ODR     (0)  
#define VAL_GPIOJ_OTYPER  (0)  
#define VAL_GPIOJ_OSPEEDR (0)  
#define VAL_GPIOJ_PUPDR   (0x55555555)  
#define VAL_GPIOJ_AFRL    (0)  
#define VAL_GPIOJ_AFRH    (0) 

#define VAL_GPIOK_MODER   (0)  
#define VAL_GPIOK_ODR     (0)  
#define VAL_GPIOK_OTYPER  (0)  
#define VAL_GPIOK_OSPEEDR (0)  
#define VAL_GPIOK_PUPDR   (0x55555555)  
#define VAL_GPIOK_AFRL    (0)  
#define VAL_GPIOK_AFRH    (0) 

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#if !defined(_FROM_ASM_)
#ifdef __cplusplus
extern "C" {
#endif
  void boardInit(void);
#ifdef __cplusplus
}
#endif
#endif /* _FROM_ASM_ */

#endif /* BOARD_H */

/* When using the X-macro logic above, we need a definition of PAL_LINE at the
 * place where we generate line number constants. But board.h is included before
 * that is defined. This is why this block is under separate #ifdef, so that second
 * inclusion of board.h will define the constants.
 */
#ifdef PAL_LINE
#ifndef BOARD_LINES_DEFINED
#define BOARD_LINES_DEFINED

// Create pin number constants, e.g. LINE_SPI6_SCK = PAL_LINE(GPIOG, 13)
enum GPIO_LINES {
#define pindef(port, pin, name, mode, odr, otype, ospeed, pupdr, af) \
  LINE_ ## name = PAL_LINE(port, pin),
GPIOA_PINS(pindef)
GPIOB_PINS(pindef)
GPIOC_PINS(pindef)
GPIOD_PINS(pindef)
GPIOE_PINS(pindef)
GPIOF_PINS(pindef)
GPIOG_PINS(pindef)
GPIOH_PINS(pindef)
#undef pindef
};
#endif
#endif
