/**
 *!
 * \file        b_srv_tcpip.h
 * \version     v0.0.1
 * \date        2023/08/27
 * \author      Bean(notrynohigh@outlook.com)
 *******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 Bean
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *******************************************************************************
 */
#ifndef __B_SRV_TCPIP_H__
#define __B_SRV_TCPIP_H__

#ifdef __cplusplus
extern "C" {
#endif

/*Includes ----------------------------------------------*/
#include <stdint.h>

#include "b_config.h"

#if (defined(_TCPIP_SERVICE_ENABLE) && (_TCPIP_SERVICE_ENABLE == 1))

#include "modules/inc/b_mod_netif.h"

/**
 * \addtogroup BABYOS
 * \{
 */

/**
 * \addtogroup SERVICES
 * \{
 */

/**
 * \addtogroup TCPIP
 * \{
 */

/**
 * \defgroup TCPIP_Exported_TypesDefinitions
 * \{
 */

/**
 * \}
 */

/**
 * \defgroup TCPIP_Exported_Defines
 * \{
 */

/**
 * \}
 */

/**
 * \defgroup TCPIP_Exported_Functions
 * \{
 */

int bSntpStart(uint32_t interval_s);

/**
 * \}
 */

/**
 * \}
 */

/**
 * \}
 */

/**
 * \}
 */

#endif

#ifdef __cplusplus
}
#endif

#endif

/************************ Copyright (c) 2023 Bean *****END OF FILE****/
