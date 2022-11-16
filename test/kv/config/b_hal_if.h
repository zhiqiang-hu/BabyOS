#ifndef __B_HAL_IF_H__
#define __B_HAL_IF_H__

//----------------------------------------------------------------
//     驱动文件的命名规则 b_drv_<驱动名小写>.c .h
//     每个驱动文件里有宏定义  #define DRIVER_NAME 驱动名大写
//     例如： spiflash驱动
//           驱动文件为 b_drv_spiflash.c .h
//           c文件里面定义宏 #define DRIVER_NAME SPIFLASH
//     驱动需要在此文件（b_hal_if.h）定义HAL层接口
//     #define HAL_<DRIVER_NAME>_IF
//     例如spiflash #define HAL_SPIFLASH_IF {具体的数据接口查看h文件}
//--------------------------------------------------------------------
//     如果有多个spiflash：
//     #define HAL_SPIFLASH_IF {第一个SPIFLASH},{第二个SPIFLASH}
//--------------------------------------------------------------------

#include "b_config.h"

// debug
#define HAL_LOG_UART B_HAL_UART_1

#define HAL_24CXX_IF                                                                               \
    {                                                                                              \
        .dev_addr = 0xa0, .is_simulation = 1, ._if.simulating_i2c.clk = {B_HAL_GPIOB, B_HAL_PIN6}, \
        ._if.simulating_i2c.sda = {B_HAL_GPIOB, B_HAL_PIN7},                                       \
    }

#define HAL_DS18B20_IF          \
    {                           \
        B_HAL_GPIOA, B_HAL_PIN4 \
    }

#define HAL_ESP12F_IF \
    {                 \
        B_HAL_UART_2  \
    }

#define HAL_TESTFLASH_IF            \
    {                               \
        .e_size = 4096, .w_size = 1 \
    }

#endif
