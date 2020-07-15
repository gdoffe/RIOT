#include <assert.h>

#include "board.h"
#include "periph/i2c.h"

uint16_t current_register = 0;

void i2c_init(i2c_t dev) {
    (void)dev;

    assert(dev < I2C_NUMOF);
}

int i2c_acquire(i2c_t dev) {
    (void)dev;

    assert(dev < I2C_NUMOF);

    return 0;
}

void i2c_release(i2c_t dev) {
    (void)dev;

    assert(dev < I2C_NUMOF);
}

static const i2c_native_dev_t* find_device(i2c_t dev, uint16_t addr,
        uint8_t is_addr10) {
    const uint16_t addr_max = is_addr10 ? 1024 : 128;
    uint16_t i = 0;
    const i2c_conf_t* conf = &i2c_config[dev];

    if (conf) {
        for(i = 0; i < addr_max; i++) {
            if (conf->emulated[i].addr == addr) {
                puts("********* FOUND *********");
                return &conf->emulated[i];
            }
        }
    }

    return NULL;
}

int i2c_read_bytes(i2c_t dev, uint16_t addr, void *data,
        size_t len, uint8_t flags) {
    (void)data;
    (void)len;
    (void)flags;

    const i2c_native_dev_t* native_dev = find_device(dev, addr,
            flags & I2C_ADDR10);

    if ( (native_dev) && (native_dev->read_bytes) ) {
        native_dev->read_bytes(data, len, flags);
    }

    return 0;
}

int i2c_write_bytes(i2c_t dev, uint16_t addr, const void *data,
        size_t len, uint8_t flags) {
    (void)data;
    (void)len;
    (void)flags;

    const i2c_native_dev_t* native_dev = find_device(dev, addr,
            flags & I2C_ADDR10);

    if ( (native_dev) && (native_dev->write_bytes) ) {
        native_dev->write_bytes(data, len, flags);
    }

    return 0;
}
