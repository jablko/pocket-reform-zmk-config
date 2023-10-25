#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>

struct pat9125_config {
    const struct i2c_dt_spec i2c;
#if !defined(CONFIG_PAT9125_TRIGGER_NONE)
    const struct gpio_dt_spec int_gpio;
#endif
};

struct pat9125_data {
    uint8_t delta_x_lo, delta_y_lo;
#if !defined(CONFIG_PAT9125_TRIGGER_NONE)
#if defined(CONFIG_PAT9125_TRIGGER_OWN_THREAD)
    struct k_sem sem;
    struct k_thread thread;
    K_KERNEL_STACK_MEMBER(thread_stack, CONFIG_PAT9125_THREAD_STACK_SIZE);
#else
    struct k_work work;
#endif
    struct gpio_callback cb;
    sensor_trigger_handler_t trigger_handler;
    const struct sensor_trigger *trig;
#endif
};

int pat9125_trigger_init(const struct device *dev);
int pat9125_trigger_set(const struct device *dev, const struct sensor_trigger *trig, sensor_trigger_handler_t trigger_handler);
