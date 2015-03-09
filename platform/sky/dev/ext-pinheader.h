/*
 *
 *
 * -----------------------------------------------------------------
 *
 * Author  : Nadir CHERIFI
 * Created : 2014-10-01
 */

#ifndef __EXT_PINHEADER_H__
#define __EXT_PINHEADER_H__

#include "dev/hwconf.h"

// HW Configuration for the GPIO port of the MSP430
// All the GPIO on the expension connectors are located at
// Port 2 of the MSP430 
HWCONF_PIN(GPIO_0, 2, 0);	// GPIO 0
HWCONF_PIN(GPIO_1, 2, 1);	// GPIO 1
HWCONF_PIN(GPIO_2, 2, 3);	// GPIO 2
HWCONF_PIN(GPIO_3, 2, 6);	// GPIO 3


#endif /* __EXT_PINHEADER_H__ */