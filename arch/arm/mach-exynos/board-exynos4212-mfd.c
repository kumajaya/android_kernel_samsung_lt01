/* linux/arch/arm/mach-exynos/board-exynos4212-mfd.c
 *
 * Copyright (c) 2011 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifdef CONFIG_MFD_MAX77693
#include <linux/mfd/max77693.h>
#include <linux/mfd/max77693-private.h>

#ifdef CONFIG_LEDS_MAX77693
#include <linux/leds-max77693.h>
#endif /* CONFIG_LEDS_MAX77693 */
#endif /* CONFIG_MFD_MAX77693 */

#include <linux/gpio.h>
#include <linux/i2c-gpio.h>
#include <mach/irqs.h>
#include <linux/platform_device.h>

#include "board-exynos4212.h"

#ifdef CONFIG_MFD_MAX77693
extern struct max77693_muic_data max77693_muic;
extern struct max77693_regulator_data max77693_regulators;

#ifdef CONFIG_LEDS_MAX77693
struct max77693_led_platform_data max77693_led_pdata = {
	.num_leds = 4,

	.leds[0].name = "leds-sec1",
	.leds[0].id = MAX77693_FLASH_LED_1,
	.leds[0].timer = MAX77693_FLASH_TIME_500MS,
	.leds[0].timer_mode = MAX77693_TIMER_MODE_MAX_TIMER,
	.leds[0].cntrl_mode = MAX77693_LED_CTRL_BY_FLASHSTB,
	.leds[0].brightness = 0x1F,

	.leds[1].name = "leds-sec2",
	.leds[1].id = MAX77693_FLASH_LED_2,
	.leds[1].timer = MAX77693_FLASH_TIME_500MS,
	.leds[1].timer_mode = MAX77693_TIMER_MODE_MAX_TIMER,
	.leds[1].cntrl_mode = MAX77693_LED_CTRL_BY_FLASHSTB,
	.leds[1].brightness = 0x1F,

	.leds[2].name = "torch-sec1",
	.leds[2].id = MAX77693_TORCH_LED_1,
	.leds[2].cntrl_mode = MAX77693_LED_CTRL_BY_FLASHSTB,
	.leds[2].brightness = 0x03,

	.leds[3].name = "torch-sec2",
	.leds[3].id = MAX77693_TORCH_LED_2,
	.leds[3].cntrl_mode = MAX77693_LED_CTRL_BY_FLASHSTB,
	.leds[3].brightness = 0x04,
};
#endif

#if defined(CONFIG_MACH_GC1)
static void motor_init_hw(void)
{
	if (gpio_request(EXYNOS4_GPD0(0), "VIBTONE_PWM") < 0)
		printk(KERN_ERR "[VIB] gpio requst is failed\n");
	else {
		gpio_direction_output(EXYNOS4_GPD0(0), 0);
		printk(KERN_DEBUG "[VIB] gpio request is succeed\n");
	}
}

static void motor_en(bool enable)
{
	gpio_direction_output(EXYNOS4_GPD0(0), enable);
	printk(KERN_DEBUG "[VIB] motor_enabled GPIO GPD0(0) : %d\n",
	       gpio_get_value(EXYNOS4_GPD0(0)));
}
#endif
#ifdef CONFIG_MACH_BAFFIN
static void motor_en(bool enable)
{
	gpio_direction_output(EXYNOS4_GPY2(2), enable);
	printk(KERN_DEBUG "[VIB] motor_enabled GPIO GPY2(2) : %d\n",
	       gpio_get_value(EXYNOS4_GPY2(2)));
}
#endif
#if defined(CONFIG_MACH_T0) && defined(CONFIG_TARGET_LOCALE_KOR) || \
	defined(CONFIG_MACH_T0_JPN_LTE_DCM)
static void motor_en(bool enable)
{
	gpio_direction_output(EXYNOS4_GPC0(3), enable);
	printk(KERN_DEBUG "[VIB] motor_enabled GPIO GPC0(3) : %d\n",
	       gpio_get_value(EXYNOS4_GPC0(3)));
}
#endif
#ifdef CONFIG_VIBETONZ
static struct max77693_haptic_platform_data max77693_haptic_pdata = {
#if defined(CONFIG_MACH_GC1)
	.reg2 = MOTOR_ERM,
	.pwm_id = 1,
	.init_hw = motor_init_hw,
	.motor_en = motor_en,
#else
	.reg2 = MOTOR_LRA | EXT_PWM | DIVIDER_128,
	.pwm_id = 0,
	.init_hw = NULL,
	.motor_en = NULL,
#endif
	.max_timeout = 10000,
#if defined(CONFIG_MACH_GC2PD)
	.duty = 37900,
#else
	.duty = 35500,
#endif
#if defined(CONFIG_MACH_SUPERIOR_KOR_SKT)
	.period = 38295,
#elif defined(CONFIG_MACH_ZEST)
	.period = 38054,
#else
	.period = 37904,
#endif
	.regulator_name = "vmotor",
};
#endif
#ifdef CONFIG_BATTERY_MAX77693_CHARGER
static struct max77693_charger_platform_data max77693_charger_pdata = {
#ifdef CONFIG_BATTERY_WPC_CHARGER
	.wpc_irq_gpio = GPIO_WPC_INT,
#if defined(CONFIG_MACH_M0) || defined(CONFIG_MACH_C1) || \
	defined(CONFIG_MACH_M3) || defined(CONFIG_MACH_T0) || \
	defined(CONFIG_MACH_GD2)
	.vbus_irq_gpio = GPIO_V_BUS_INT,
#endif
#if defined(CONFIG_MACH_T0) ||  \
	defined(CONFIG_MACH_GD2)
	.wc_pwr_det = true,
#else
	.wc_pwr_det = false,
#endif
#endif
};
#endif

struct max77693_platform_data exynos4_max77693_info = {
	.irq_base	= IRQ_BOARD_IFIC_START,
	.irq_gpio	= GPIO_IF_PMIC_IRQ,
	.wakeup		= 1,
	.muic = &max77693_muic,
	.regulators = &max77693_regulators,
	.num_regulators = MAX77693_REG_MAX,
#if defined(CONFIG_CHARGER_MAX77693_BAT)
	.charger_data	= &sec_battery_pdata,
#elif defined(CONFIG_BATTERY_MAX77693_CHARGER)
	.charger_data = &max77693_charger_pdata,
#endif
#ifdef CONFIG_VIBETONZ
	.haptic_data = &max77693_haptic_pdata,
#endif
#ifdef CONFIG_LEDS_MAX77693
	.led_data = &max77693_led_pdata,
#endif
};
#endif /* CONFIG_MFD_MAX77693 */

#if defined(CONFIG_MUIC_I2C_USE_I2C17_EMUL)
/* I2C17 */
static struct i2c_board_info i2c_devs17_emul[] __initdata = {
#ifdef CONFIG_MFD_MAX77693
	{
		I2C_BOARD_INFO("max77693", (0xCC >> 1)),
		.platform_data	= &exynos4_max77693_info,
	}
#endif
};

static struct i2c_gpio_platform_data gpio_i2c_data17 = {
	.sda_pin = GPIO_IF_PMIC_SDA,
	.scl_pin = GPIO_IF_PMIC_SCL,
};

struct platform_device s3c_device_i2c17 = {
	.name = "i2c-gpio",
	.id = 17,
	.dev.platform_data = &gpio_i2c_data17,
};
#endif /* CONFIG_MUIC_I2C_USE_I2C17_EMUL */

static struct platform_device *exynos4_mfd_device[] __initdata = {
#if defined(CONFIG_MUIC_I2C_USE_I2C17_EMUL)
	&s3c_device_i2c17,
#endif /* CONFIG_MUIC_I2C_USE_I2C17_EMUL */
};

void __init exynos4_exynos4212_mfd_init(void)
{
	pr_info("%s\n", __func__);

#if defined(CONFIG_MUIC_I2C_USE_I2C17_EMUL)
	i2c_register_board_info(17, i2c_devs17_emul,
				ARRAY_SIZE(i2c_devs17_emul));
#endif /* CONFIG_MUIC_I2C_USE_I2C17_EMUL */

	platform_add_devices(exynos4_mfd_device, ARRAY_SIZE(exynos4_mfd_device));
}
