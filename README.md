# Driver_bus_i2c
### *Usage*

#### Step 1 ：User add I2C bus port function

```c
/**
 * @brief Software i2c bus port (master mode)
 */
typedef struct
{
    void (*holdtime)(void);
    void (*sda_mode)(IO_MODE mode);
    void (*scl_mode)(IO_MODE mode);
    void (*set_scl)(uint8_t level);
    void (*set_sda)(uint8_t level);
    uint8_t(*get_sda)(void);
} sw_i2c_t;

/**
 * @brief Hardware i2c bus port (master mode)
 */
typedef struct
{
    uint8_t(*send)(uint16_t devaddr, uint8_t *pdata, uint32_t size);
    uint8_t(*recv)(uint16_t devaddr, uint8_t *pdata, uint32_t size);
    uint8_t(*wmem)(uint16_t devaddr, uint16_t memaddr, uint8_t memaddrsize, uint8_t *pdata, uint32_t size);
    uint8_t(*rmem)(uint16_t devaddr, uint16_t memaddr, uint8_t memaddrsize, uint8_t *pdata, uint32_t size);
} hw_i2c_t;
```

#### Step 2 ：Init I2C bus Device （Mounted Devices）

```c
/* I2C Bus (Mounted Devices) */
sw_i2c_t sw_i2c;
sw_i2c.set_scl = set_SCL;
sw_i2c.scl_mode = mode_SCL;
sw_i2c.set_sda = set_SDA;
sw_i2c.get_sda = get_SDA;
sw_i2c.sda_mode = mode_SDA;
sw_i2c.holdtime = bus_delay;
i2c_config(&sw_i2c);
```





