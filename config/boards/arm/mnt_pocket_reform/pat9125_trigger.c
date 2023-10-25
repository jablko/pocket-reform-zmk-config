#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>

#include "pat9125.h"

#if defined(CONFIG_PAT9125_TRIGGER_OWN_THREAD)
static void pat9125_thread(const struct device *dev) {
    struct pat9125_data *data = dev->data;
    while (true) {
        k_sem_take(&data->sem, K_FOREVER);
        data->trigger_handler(dev, data->trig);
    }
}
#else
static void work_handler(struct k_work *work) {
    const struct pat9125_data *data = CONTAINER_OF(work, struct pat9125_data, work);
    const struct device *dev = CONTAINER_OF(data, struct device, data);
    data->trigger_handler(dev, data->trig);
}
#endif

static void callback_handler(const struct device *port, struct gpio_callback *cb, gpio_port_pins_t pins) {
    struct pat9125_data *data = CONTAINER_OF(cb, struct pat9125_data, cb);
#if defined(CONFIG_PAT9125_TRIGGER_OWN_THREAD)
    k_sem_give(&data->sem);
#else
    k_work_submit(&data->work);
#endif
}

int pat9125_trigger_init(const struct device *dev) {
    const struct pat9125_config *config = dev->config;
    struct pat9125_data *data = dev->data;
#if defined(CONFIG_PAT9125_TRIGGER_OWN_THREAD)
    k_sem_init(&data->sem, 0, K_SEM_MAX_LIMIT);
    k_thread_create(&data->thread, data->thread_stack, CONFIG_PAT9125_THREAD_STACK_SIZE, (k_thread_entry_t)pat9125_thread, (void *)dev, NULL, NULL, K_PRIO_COOP(CONFIG_PAT9125_THREAD_PRIORITY), 0, K_NO_WAIT);
#else
    k_work_init(&data->work, work_handler);
#endif
    gpio_init_callback(&data->cb, callback_handler, BIT(config->int_gpio.pin));
    gpio_add_callback(config->int_gpio.port, &data->cb);
    return 0;
}

static void setup_int(const struct device *dev) {
    const struct pat9125_config *config = dev->config;
    gpio_pin_interrupt_configure_dt(&config->int_gpio, GPIO_INT_EDGE_TO_ACTIVE);
}

int pat9125_trigger_set(const struct device *dev, const struct sensor_trigger *trig, sensor_trigger_handler_t trigger_handler) {
    struct pat9125_data *data = dev->data;
    data->trigger_handler = trigger_handler;
    data->trig = trig;
    setup_int(dev);
    return 0;
}
