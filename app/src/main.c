/*
 * Minimal blinky + serial print to verify the toolchain, OpenOCD flashing
 * and the forwarded serial console end-to-end.
 *
 * Works on any board that defines the `led0` devicetree alias. If your board
 * has no LED, the build still runs and keeps printing over the console.
 */
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

#define SLEEP_MS 1000

#if DT_NODE_HAS_STATUS(DT_ALIAS(led0), okay)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
#define HAVE_LED 1
#endif

int main(void)
{
	uint32_t count = 0;

#ifdef HAVE_LED
	if (gpio_is_ready_dt(&led)) {
		gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	}
#endif

	printk("ncs-container: hello from %s\n", CONFIG_BOARD_TARGET);

	while (1) {
#ifdef HAVE_LED
		if (gpio_is_ready_dt(&led)) {
			gpio_pin_toggle_dt(&led);
		}
#endif
		printk("tick %u\n", count++);
		k_msleep(SLEEP_MS);
	}
	return 0;
}
