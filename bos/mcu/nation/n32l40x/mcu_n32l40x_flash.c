/**
 *!
 * \file        mcu_n32l40x_flash.c
 * \version     v0.0.1
 * \date        2021/06/13
 * \author      Bean(notrynohigh@outlook.com)
 *******************************************************************************
 * @attention
 *
 * Copyright (c) 2021 Bean
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SGPIOL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *******************************************************************************
 */

/*Includes ----------------------------------------------*/
#include <string.h>

#include "b_config.h"
#include "hal/inc/b_hal_flash.h"

#if (_MCU_PLATFORM == 2001)
#include "n32l40x.h"

#define _FLASH_BASE_ADDR (0x8000000UL)
#define _FLASH_PAGE_SIZE (2048)

static int _FlashInit()
{
    return 0;
}

static int _FlashUnlock()
{
    int retval = 0;
    FLASH_Unlock();
    return retval;
}

static int _FlashLock()
{
    int retval = 0;
    FLASH_Lock();
    return retval;
}

static int _FlashErase(uint32_t raddr, uint8_t pages)
{
    int     retval = 0;
    uint8_t i      = 0;
    raddr = _FLASH_BASE_ADDR + raddr;
    raddr      = raddr / _FLASH_PAGE_SIZE * _FLASH_PAGE_SIZE;
    for (i = 0; i < pages; i++)
    {
        if (FLASH_COMPL == FLASH_EraseOnePage(raddr))
        {
            raddr += _FLASH_PAGE_SIZE;
        }
        else
        {
            retval = -1;
            break;
        }
    }
    return retval;
}

static int _FlashWrite(uint32_t raddr, const uint8_t *pbuf, uint16_t len)
{
    uint32_t wdata  = 0;
    uint16_t wlen = (len + 3) / 4, i = 0;
    raddr = _FLASH_BASE_ADDR + raddr;
    if (pbuf == NULL || (raddr & 0x3))
    {
        return -1;
    }

    for (i = 0; i < wlen; i++)
    {
        wdata = (wdata << 8) | pbuf[i * 4 + 3];
        wdata = (wdata << 8) | pbuf[i * 4 + 2];
        wdata = (wdata << 8) | pbuf[i * 4 + 1];
        wdata = (wdata << 8) | pbuf[i * 4 + 0];
        FLASH_ProgramWord(raddr, wdata);
        raddr += 4;
    }
    return (wlen * 4);
}

static int _FlashRead(uint32_t raddr, uint8_t *pbuf, uint16_t len)
{
    if (pbuf == NULL)
    {
        return -1;
    }
    raddr = _FLASH_BASE_ADDR + raddr;
    memcpy(pbuf, (const uint8_t *)raddr, len);
    return len;
}

bHalFlashDriver_t bHalFlashDriver = {
    .pFlashInit   = _FlashInit,
    .pFlashUnlock = _FlashUnlock,
    .pFlashLock   = _FlashLock,
    .pFlashErase  = _FlashErase,
    .pFlashWrite  = _FlashWrite,
    .pFlashRead   = _FlashRead,
};

#endif

/************************ Copyright (c) 2021 Bean *****END OF FILE****/