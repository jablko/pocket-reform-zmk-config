#include <stdlib.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/init.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>

static void sysctl_cb(const struct device *dev, void *user_data) {
    uint8_t rx_data[4] = {0};
    uart_fifo_read(dev, rx_data, sizeof(rx_data) - 1);
    uint8_t state_of_charge = atoi(rx_data);
    ZMK_EVENT_RAISE(new_zmk_battery_state_changed((const struct zmk_battery_state_changed){
        .state_of_charge = state_of_charge,
    }));
}

static int sysctl_init(const struct device *) {
    const struct device *dev = DEVICE_DT_GET_ONE(raspberrypi_pico_uart);
    uart_irq_callback_user_data_set(dev, sysctl_cb, NULL);
    return 0;
}

SYS_INIT(sysctl_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
