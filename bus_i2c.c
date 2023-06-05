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
 * @file    bus_i2c.c
 * @brief   I2C Bus source file
 * @author  iammingge
 *
 *      DATE             NAME                      DESCRIPTION
 *
 *   06-05-2023        iammingge                Initial Version 1.0
 *
**/

#include "bus_i2c.h"

/* Init I2C Bus IO */
void i2c_config(sw_i2c_t *port)
{
    /* Default pin output high level */
    port->sda_mode(IO_OUT);
    port->scl_mode(IO_OUT);
    port->set_sda(1);
    port->set_scl(1);
}
/* I2C start signal */
void i2c_strt(sw_i2c_t *port)
{
    /* Data invalid : SCL Low */
    port->set_scl(0);

    /* convert SDA IO mode */
    port->sda_mode(IO_OUT);

    /* Data valid : SCL High
       Bus  start : SDA High -> Low */
    port->set_sda(1);
    port->set_scl(1);
    port->holdtime();
    port->set_sda(0);
    port->holdtime();
}
/* I2C stop signal */
void i2c_stop(sw_i2c_t *port)
{
    /* Data invalid : SCL Low */
    port->set_scl(0);

    /* convert SDA IO mode */
    port->sda_mode(IO_OUT);

    /* Data valid : SCL High
       Bus  stop  : SDA Low -> High */
    port->set_sda(0);
    port->set_scl(1);
    port->holdtime();
    port->set_sda(1);
    port->holdtime();
}
/* I2C send response signal
 *  ACK : Acknowledge signal
 * NACK : Not acknowledge signal
 * note : The response signal must appear in pairs with the read/write signal.
**/
static void i2c_send_rsp(sw_i2c_t *port, I2C_RSP ack)
{
    /* Data invalid : SCL Low */
    port->set_scl(0);

    /* convert SDA IO mode */
    port->sda_mode(IO_OUT);

    if (ack == ACK)
    {
        /* ACK signal : SDA Low */
        port->set_sda(0);
    }
    else
    {
        /* NACK signal : SDA High */
        port->set_sda(1);
    }

    /* Data valid  : SCL High */
    port->set_scl(1);
    port->holdtime();

    /* Data invalid : SCL Low */
    port->set_scl(0);
}
/* I2C receive response signal */
static uint8_t i2c_recv_rsp(sw_i2c_t *port)
{
    uint8_t ret = 0;

    /* The transmitter releases the SDA */
    port->sda_mode(IO_IN);

    /* Data valid : SCL High */
    port->set_scl(1);

    /* Get response signal
     * 0 : ACK
     * 1 : NACK
    **/
    ret = port->get_sda();

    /* Data invalid : SCL Low */
    port->set_scl(0);

    /* success */
    return ret;
}
/* I2C transmit one byte data */
uint8_t i2c_wbyte(sw_i2c_t *port, uint8_t byte)
{
    uint8_t i = 0;
    uint8_t ret = 0;

    /* Data invalid : SCL Low */
    port->set_scl(0);

    /* convert SDA IO mode */
    port->sda_mode(IO_OUT);

    /* start write data */
    for (i = 0; i < 8; i++)
    {
        /* Set data to SDA */
        if (byte & 0x80)
        {
            port->set_sda(1);
        }
        else
        {
            port->set_sda(0);
        }
        byte <<= 1;

        /* Data valid : SCL High */
        port->set_scl(1);

        /* Data hold */
        port->holdtime();

        /* Data invalid : SCL Low */
        port->set_scl(0);
    }

    /* slave -> master wait for answer signal */
    ret = i2c_recv_rsp(port);

    return ret;
}
/* I2C receive one byte data */
uint8_t i2c_rbyte(sw_i2c_t *port, I2C_RSP ack)
{
    uint8_t i = 0;
    uint8_t rbyte = 0;

    /* Data invalid : SCL Low */
    port->set_scl(0);

    /* convert SDA IO mode */
    port->sda_mode(IO_IN);

    /* start read data */
    for (i = 0; i < 8; i++)
    {
        /* Data invalid : SCL Low */
        port->set_scl(0);

        /* Data hold */
        port->holdtime();

        /* Data valid : SCL High */
        port->set_scl(1);

        /* Get data from SDA */
        rbyte <<= 1;
        if (port->get_sda())
        {
            rbyte |= 0x01;
        }
    }

    /* Data invalid : SCL Low */
    port->set_scl(0);

    /* master -> slave send to answer signal
       ACK  : Acknowledge
       NACK : Not Acknowledge */
    if (ack == ACK)
    {
        i2c_send_rsp(port, ACK);
    }
    else
    {
        i2c_send_rsp(port, NACK);
    }

    return rbyte;
}
























































