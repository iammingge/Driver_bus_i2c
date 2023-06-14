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
 *   06-14-2023        iammingge                Fix bus timing and strictly follow standard timing
 *
**/

#include "bus_i2c.h"

/**
 * @brief  Init I2C Bus IO
 * @param  {sw_i2c_t} *port : device port
 * @return none
 * @note   none
 */
void swi2c_config(sw_i2c_t *port)
{
    /* Default pin output high level */
    port->sda_mode(IO_OUT);
    port->scl_mode(IO_OUT);
    port->set_sda(1);
    port->set_scl(1);
}
/**
 * @brief  I2C start signal
 * @param  {sw_i2c_t} *port : device port
 * @return none
 * @note   none
 */
void swi2c_strt(sw_i2c_t *port)
{
    /* convert SDA and SCL IO mode */
    port->sda_mode(IO_OUT);
    port->scl_mode(IO_OUT);

    /* Data valid : SCL High
       Bus  start : SDA High -> Low */
    port->set_sda(1);
    port->set_scl(1);
    port->holdtime(2);
    port->set_sda(0);
    port->holdtime(2);
}
/**
 * @brief  I2C stop signal
 * @param  {sw_i2c_t} *port : device port
 * @return none
 * @note   none
 */
void swi2c_stop(sw_i2c_t *port)
{
    /* convert SDA and SCL IO mode */
    port->sda_mode(IO_OUT);
    port->scl_mode(IO_OUT);

    /* Data valid : SCL High
       Bus  stop  : SDA Low -> High */
    port->set_sda(0);
    port->set_scl(1);
    port->holdtime(2);
    port->set_sda(1);
    port->holdtime(2);
}
/**
 * @brief  I2C send response signal
 * @param  {sw_i2c_t} *port : device port
 * @param  {I2C_RSP} ack    : response signal
 *                            ACK (Acknowledge signal)
 *                            NACK (Not acknowledge signal)
 * @return none
 * @note   The response signal must appear in pairs with the read/write signal.
 */
static void swi2c_send_rsp(sw_i2c_t *port, I2C_RSP ack)
{
    /* convert SDA IO mode */
    port->sda_mode(IO_OUT);

    /* Data variable / invalid : SCL Low */
    port->set_scl(0);
    port->holdtime(1);

    /* Set ack level */
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
    port->holdtime(1);

    /* Data hold / valid  : SCL High */
    port->set_scl(1);
    port->holdtime(2);

    /* Data variable / invalid : SCL Low */
    port->set_scl(0);
}
/**
 * @brief  I2C receive response signal
 * @param  {sw_i2c_t} *port : device port
 * @return {I2C_RSP}        : response signal
 *                            ACK (Acknowledge signal)
 *                            NACK (Not acknowledge signal)
 * @note   none
 */
static I2C_RSP swi2c_recv_rsp(sw_i2c_t *port)
{
    I2C_RSP ret_ack;

    /* convert SDA IO mode */
    port->sda_mode(IO_IN);

    /* Data variable / invalid : SCL Low */
    port->set_scl(0);
    port->holdtime(2);

    /* Data hold / valid : SCL High */
    port->set_scl(1);
    port->holdtime(2);

    /* Get response signal
     * 0 : ACK
     * 1 : NACK
    **/
    ret_ack = (I2C_RSP)port->get_sda();

    /* Data variable / invalid : SCL Low */
    port->set_scl(0);

    /* success */
    return ret_ack;
}
/**
 * @brief  I2C transmit one byte data
 * @param  {sw_i2c_t} *port : device port
 * @param  {uint8_t} byte   : write one byte data
 * @return {I2C_RSP}        : response signal
 *                            ACK (Acknowledge signal)
 *                            NACK (Not acknowledge signal)
 * @note   none
 */
I2C_RSP swi2c_wbyte(sw_i2c_t *port, uint8_t byte)
{
    uint8_t i = 0;
    I2C_RSP ret_ack;

    /* convert SDA IO mode */
    port->sda_mode(IO_OUT);

    /* start write data */
    for (i = 0; i < 8; i++)
    {
        /* Data variable / invalid : SCL Low */
        port->set_scl(0);
        port->holdtime(1);

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
        port->holdtime(1);

        /* Data hold / valid : SCL High */
        port->set_scl(1);
        port->holdtime(2);
    }

    /* Data variable / invalid : SCL Low
     * Note : release SDA line
    **/
    port->set_scl(0);

    /* slave -> master wait for answer signal */
    ret_ack = swi2c_recv_rsp(port);

    return ret_ack;
}
/**
 * @brief  I2C receive one byte data
 * @param  {sw_i2c_t} *port : device port
 * @param  {I2C_RSP} ack    : ACK (Acknowledge signal)
 *                            NACK (Not acknowledge signal)
 * @return {uint8_t}        : read one byte data
 * @note   none
 */
uint8_t swi2c_rbyte(sw_i2c_t *port, I2C_RSP ack)
{
    uint8_t i = 0;
    uint8_t rbyte = 0;

    /* convert SDA IO mode */
    port->sda_mode(IO_IN);

    /* start read data */
    for (i = 0; i < 8; i++)
    {
        /* Data variable / invalid : SCL Low */
        port->set_scl(0);
        port->holdtime(2);

        /* Data hold / valid : SCL High */
        port->set_scl(1);
        port->holdtime(2);

        /* Get data from SDA */
        rbyte <<= 1;
        if (port->get_sda())
        {
            rbyte |= 0x01;
        }
    }

    /* Data variable / invalid : SCL Low
     * Note : Release SDA line
    **/
    port->set_scl(0);

    /* master -> slave send to answer signal
       ACK  : Acknowledge
       NACK : Not Acknowledge */
    if (ack == ACK)
    {
        swi2c_send_rsp(port, ACK);
    }
    else
    {
        swi2c_send_rsp(port, NACK);
    }

    return rbyte;
}
/**
 * @brief  I2C bus clear (SDA is stuck LOW)
 * @param  {sw_i2c_t} *port
 * @return none
 * @note   If the data line (SDA) is stuck LOW, the controller should send nine clock pulses. The
 *         device that held the bus LOW should release it sometime within those nine clocks. If not,
 *         then use the HW reset or cycle power to clear the bus.
 */
void swi2c_reset(sw_i2c_t *port)
{
    /* convert SDA and SCL IO mode */
    port->sda_mode(IO_OUT);
    port->scl_mode(IO_OUT);

    /* Reset IO level */
    port->set_sda(1);
    port->set_scl(1);

    /* Clock nine cycles */
    for (uint8_t i = 0; i < 9; i++)
    {
        port->set_scl(0);
        port->holdtime(2);
        port->set_scl(1);
        port->holdtime(2);
    }

    /* IIC start */
    swi2c_strt(port);

    /* IIC stop */
    swi2c_stop(port);
}



























































































