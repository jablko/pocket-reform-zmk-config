#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/sensor.h>

#include "pat9125.h"

#define DT_DRV_COMPAT pixart_pat9125el

#define PAT9125_REG_DELTA_X_LO 0x03
#define PAT9125_REG_DELTA_Y_LO 0x04

static int pat9125_init(const struct device *dev) {
    if (!IS_ENABLED(CONFIG_PAT9125_TRIGGER_NONE)) {
        pat9125_trigger_init(dev);
    }
    return 0;
}

static int pat9125_sample_fetch(const struct device *dev, enum sensor_channel chan) {
    const struct pat9125_config *config = dev->config;
    struct pat9125_data *data = dev->data;
    i2c_reg_read_byte_dt(&config->i2c, PAT9125_REG_DELTA_X_LO, &data->delta_x_lo);
    i2c_reg_read_byte_dt(&config->i2c, PAT9125_REG_DELTA_Y_LO, &data->delta_y_lo);
    return 0;
}

static int pat9125_channel_get(const struct device *dev, enum sensor_channel chan, struct sensor_value *val) {
    const struct pat9125_data *data = dev->data;
    switch (chan) {
        case SENSOR_CHAN_POS_DX:
            val->val1 = data->delta_x_lo;
            break;
        case SENSOR_CHAN_POS_DY:
            val->val1 = data->delta_y_lo;
            break;
        default:
            __ASSERT_NO_MSG(0);
    }
    return 0;
}

static struct sensor_driver_api pat9125_api_funcs = {
#if !defined(CONFIG_PAT9125_TRIGGER_NONE)
    .trigger_set = pat9125_trigger_set,
#endif
    .sample_fetch = pat9125_sample_fetch,
    .channel_get = pat9125_channel_get,
};

#define PAT9125_DEFINE(n) \
    static struct pat9125_data pat9125_data_##n; \
    static const struct pat9125_config pat9125_config_##n = { \
        .i2c = I2C_DT_SPEC_INST_GET(n), \
        COND_CODE_1(CONFIG_PAT9125_TRIGGER_NONE, (), (.int_gpio = GPIO_DT_SPEC_INST_GET(n, int_gpios),)) \
    }; \
    DEVICE_DT_INST_DEFINE(n, pat9125_init, NULL, &pat9125_data_##n, &pat9125_config_##n, POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY, &pat9125_api_funcs);

DT_INST_FOREACH_STATUS_OKAY(PAT9125_DEFINE)
