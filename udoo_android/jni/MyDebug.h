/*
 * MyDebug.h
 *
 *  Created on: Apr 17, 2015
 *      Author: francesco
 */

#ifndef BOSSAC_MYDEBUG_H_
#define BOSSAC_MYDEBUG_H_

#include <android/log.h>

#ifndef BOSSAC_LOG
#define printf(...)
#else
#define printf(...) __android_log_print(ANDROID_LOG_DEBUG, "Bossac", __VA_ARGS__);
#endif

#endif /* BOSSAC_MYDEBUG_H_ */
