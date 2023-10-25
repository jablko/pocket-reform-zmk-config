#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/init.h>
#include <zmk/hid.h>

static void trigger_handler(const struct device *dev, const struct sensor_trigger *trig) {
    sensor_sample_fetch(dev);
    struct sensor_value pos_dx, pos_dy;
    sensor_channel_get(dev, SENSOR_CHAN_POS_DX, &pos_dx);
    sensor_channel_get(dev, SENSOR_CHAN_POS_DY, &pos_dy);
    zmk_hid_mouse_movement_set(pos_dx.val1, pos_dy.val1);
}

static int board_init(const struct device *) {
    const struct device *dev = DEVICE_DT_GET_ONE(pixart_pat9125el);
    const struct sensor_trigger trig = {
        .type = SENSOR_TRIG_DATA_READY,
        .chan = SENSOR_CHAN_ALL,
    };
    sensor_trigger_set(dev, &trig, trigger_handler);
    return 0;
}

SYS_INIT(board_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
