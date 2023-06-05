/**
 * Copyright (c) 2023 iammingge
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file    bus_i2c.h
 * @brief   I2C Bus header file
 * @author  iammingge
 *
 *      DATE             NAME                      DESCRIPTION
 *
 *   06-05-2023        iammingge                Initial Version 1.0
 *
**/

#ifndef __BUS_I2C_H
#define __BUS_I2C_H
#include <stdint.h>
#include <stddef.h>
#include "hwtype.h"

/**
 * @brief Acknowledge or Not Acknowledge
 */
typedef enum
{
    ACK = 0U,
    NACK
} I2C_RSP;

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
 * @brief Software i2c bus function (master mode)
 */
void i2c_config(sw_i2c_t *port);                    /* Init I2C Bus IO */
void i2c_strt(sw_i2c_t *port);                    	/* I2C start signal */
void i2c_stop(sw_i2c_t *port);                     	/* I2C stop signal */
uint8_t i2c_wbyte(sw_i2c_t *port, uint8_t byte);  	/* I2C transmit one byte data */
uint8_t i2c_rbyte(sw_i2c_t *port, I2C_RSP ack);   	/* I2C receive one byte data */

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

#endif






