/********************************************************************************************************
 * @file     ev_timer.h
 *
 * @brief    Interface of event timer
 *
 * @author
 * @date     Oct. 8, 2016
 *
 * @par      Copyright (c) 2016, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *           The information contained herein is confidential property of Telink
 *           Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *           of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *           Co., Ltd. and the licensee or the terms described here-in. This heading
 *           MUST NOT be removed from this file.
 *
 *           Licensees are granted free, non-transferable use of the information in this
 *           file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/

#pragma once

#include "../common/types.h"

/** @addtogroup  TELINK_COMMON_MODULE TELINK Common Module
 *  @{
 */

/** @addtogroup  EV_TIMER EV Timer
 *  @{
 */
 
/** @defgroup EV_TIMER_TYPE EV Timer Types
 *  @{
 */

/**
 *  @brief Type definition for timer callback function
 */
typedef int (*ev_timer_callback_t)(void *data);
typedef void (*ev_zb_callback_t)(u8 param) ;
 
/**
 *  @brief Definition for timer event
 */
#define TIMER_EVENT_NUM			32

#define TIMER_EVENT_NUM_BRC		(TIMER_EVENT_NUM-4)

typedef struct ev_time_event_t {
    ev_timer_callback_t     cb;            //!< Callback function when expire, this must be specified

    u32                     t;             //!< Used internal

    u32                     interval;      //!< Used internal

    void                    *data;         //!< Callback function arguments.

    struct ev_time_event_t  *next;         //!< Used internal

    u8                      used;
	u8						resv;
	u8						allig[2];
} ev_time_event_t;

typedef struct ev_time_event_pool_s {
	s16 used_num;
	s16 brc_used_num;
	ev_time_event_t evt[TIMER_EVENT_NUM];
} ev_time_event_pool_t __attribute__((aligned(4)));


/** @} end of group EV_TIMER_TYPE */

/** @defgroup EV_TIMER_FUNCTION EV Timer API
 *  @{
 */

/**
  * @brief       Set a new timer
  *
  * @param[in]   e  - The timer event including the callback function
  * @param[in]   us - Interval time in microsecond
  *
  * @return      None
  */
void ev_on_timer(ev_time_event_t *e, u32 t_us);

/**
  * @brief       Cancel an existed timer
  *
  * @param[in]   e  - The existed timer event
  *
  * @return      None
  */
void ev_unon_timer(ev_time_event_t *e);

/**
  * @brief       Check whether a specified timer expired or not
  *
  * @param[in]   e  - The specified timer event
  *
  * @return      True indicating the timer is already expired. <BR>
  *              False indicating the timer is still exist. <BR>
  */
int is_timer_expired(ev_time_event_t *e);

/**
  * @brief       Check whether a specified timer exist or not
  *
  * @param[in]   e  - The specified timer event
  *
  * @return      True indicating the timer is already exist. <BR>
  *              False indicating the timer is not exist. <BR>
  */
u8 ev_timer_exist(const ev_time_event_t *e);

/**
  * @brief       Get the nearest fired timer interval
  *
  * @param[in]   None
  *
  * @return      Value of the interval.
  */
u32 ev_nearestInterval(void);

/**  @} end of group EV_TIMER_FUNCTION */

/**  @} end of group EV_TIMER */

/**  @} end of group TELINK_COMMON_MODULE */
