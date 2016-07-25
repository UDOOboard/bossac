///////////////////////////////////////////////////////////////////////////////
// BOSSA
//
// Copyright (C) 2011-2012 ShumaTech http://www.shumatech.com/
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
///////////////////////////////////////////////////////////////////////////////
#include "bossacjni.h"
#include <string>
#include <exception>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <iostream>
#include <unistd.h>
#include <dlfcn.h>
#include "Samba.h"
#include "PortFactory.h"
#include "FlashFactory.h"
#include "Flasher.h"
#include "MyDebug.h"

using namespace std;

typedef enum {
	eOk = 0,
	eNoTTyDeviceFound,
	eFlashNotSupported,
	eSambaError,
	eUnhandledException,
	eVerifyFailed
}eBossac;

class BossaConfig
{
public:
    BossaConfig();
    virtual ~BossaConfig(){};

    bool erase;
    bool write;
    bool read;
    bool verify;
    bool reset;
    bool port;
    bool boot;
    bool bor;
    bool bod;
    bool lock;
    bool unlock;
    bool security;
    bool info;
    bool debug;
    bool help;
    bool forceUsb;
    string forceUsbArg;

    int readArg;
    string portArg;
    int bootArg;
    int bodArg;
    int borArg;
    string lockArg;
    string unlockArg;
};

BossaConfig::BossaConfig()
{
    erase = false;
    write = false;
    read = false;
    verify = false;
    port = false;
    boot = false;
    bod = false;
    bor = false;
    lock = false;
    security = false;
    info = false;
    help = false;
    forceUsb = false;

    readArg = 0;
    bootArg = 1;
    bodArg = 1;
    borArg = 1;
    unlock = false;
    debug = false;

    reset = false;
}

bool
autoScan(Samba& samba, PortFactory& portFactory, string& port)
{
    for (port = portFactory.begin();
         port != portFactory.end();
         port = portFactory.next())
    {
        if (samba.connect(portFactory.create(port)))
            return true;
    }

    return false;
}

int
help(const char* program)
{
    fprintf(stderr, "Try '%s -h' or '%s --help' for more information\n", program, program);
    return 1;
}

#define VERSION "1.3a"

eBossac
WriteImage(const char* samImage, bool verify)
{
	BossaConfig config;

    config.write = true;
    config.verify = verify;
    config.erase = true;
    config.boot = true;
    config.reset = true;
    config.port = true;
    config.portArg = "ttymxc3";

    try
    {
        Samba samba;
        PortFactory portFactory;
        FlashFactory flashFactory;

        if (config.debug)
            samba.setDebug(true);

        if (config.port)
        {
            bool res;
            res = samba.connect(portFactory.create(config.portArg));
            if (!res)
            {
                FILE *udoo_ard;
                udoo_ard = fopen("/dev/udoo_ard", "ab");
                if (udoo_ard == NULL) {
                    printf("Cannot call erase/reset on Arduno.");
                    return false;
                }
                const char* erase = "erase";
                fputs(erase, udoo_ard);
                fclose(udoo_ard);

            	printf("No device found on %s\n", config.portArg.c_str());
            	return(eNoTTyDeviceFound);
            }
        }

        uint32_t chipId = samba.chipId();
        Flash::Ptr flash = flashFactory.create(samba, chipId);
        if (flash.get() == NULL)
        {
            printf("Flash for chip ID %08x is not supported\n", chipId);
            return (eFlashNotSupported);
        }

        Flasher flasher(flash);

        if (config.unlock)
            flasher.lock(config.unlockArg, false);

        if (config.erase)
            flasher.erase();

        if (config.write)
            flasher.write(samImage);

        if (config.verify)
            if  (!flasher.verify(samImage))
                return (eVerifyFailed);

        if (config.boot)
        {
            printf("Set boot flash %s\n", config.bootArg ? "true" : "false");
            flash->setBootFlash(config.bootArg);
        }

        if (config.bod)
        {
            printf("Set brownout detect %s\n", config.bodArg ? "true" : "false");
            flash->setBod(config.bodArg);
        }

        if (config.bor)
        {
            printf("Set brownout reset %s\n", config.borArg ? "true" : "false");
            flash->setBor(config.borArg);
        }

        if (config.security)
        {
            printf("Set security\n");
            flash->setSecurity();
        }

        if (config.lock)
            flasher.lock(config.lockArg, true);

        if (config.info)
            flasher.info(samba);

        if (config.reset)
            samba.reset();
    }
    catch (exception& e)
    {
        printf("%s", e.what());
        return (eSambaError);
    }
    catch(...)
    {
        printf("\nUnhandled exception\n");
        return (eUnhandledException);
    }

    return (eOk);
}

eBossac
ReadImage(const char* samImage, uint32_t sizeToRead)
{
	BossaConfig config;

    config.read = true;
    config.port = true;
    config.portArg = "ttymxc3";
    config.readArg = sizeToRead;

    try
    {
        Samba samba;
        PortFactory portFactory;
        FlashFactory flashFactory;

        if (config.debug)
            samba.setDebug(true);

        if (config.port)
        {
            bool res;
            res = samba.connect(portFactory.create(config.portArg));
            if (!res)
            {
                /*
            	fprintf(stderr, "No device found on %s\n", config.portArg.c_str());
                return 1;*/
            	printf("No device found on %s\n", config.portArg.c_str());
                return (eNoTTyDeviceFound);
            }
        }

        uint32_t chipId = samba.chipId();
        Flash::Ptr flash = flashFactory.create(samba, chipId);
        if (flash.get() == NULL)
        {
        	/*
            fprintf(stderr, "Flash for chip ID %08x is not supported\n", chipId);
            return 1; */
            printf("Flash for chip ID %08x is not supported\n", chipId);
            return (eFlashNotSupported);

        }

        Flasher flasher(flash);

        if (config.read)
            flasher.read(samImage, config.readArg);
    }
    catch (exception& e)
    {
    	/*
        fprintf(stderr, "\n%s\n", e.what());
        return 1;*/
        printf("%s", e.what());
        return (eSambaError);
    }
    catch(...)
    {
    	/*
        fprintf(stderr, "\nUnhandled exception\n");
        return 1;*/
        printf("\nUnhandled exception\n");
        return (eUnhandledException);
    }

    return (eOk);
}

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNICALL Java_bossacjni_BossacWriteImage
  (JNIEnv *jebv, jobject jobj, jstring jPathFileSAM_Image, jboolean verify)
{
	int res;
	int i;
	jboolean isCopy;
	const char *samImage;

    printf("Start BossacWriteImage");

    samImage = jebv->GetStringUTFChars(jPathFileSAM_Image, &isCopy);

	res = WriteImage(samImage, verify);

    jebv->ReleaseStringUTFChars(jPathFileSAM_Image, samImage);

    printf("retBossac=%d", res);

    return (res);

}

JNIEXPORT jint JNICALL Java_bossacjni_BossacReadImage
  (JNIEnv *jebv, jobject jobj, jstring jPathFileSAM_Image, jlong sizeToRead)
{
	int res;
	int i;
	jboolean isCopy;
	const char *samImage;

    printf("Start BossacReadImage");

    samImage = jebv->GetStringUTFChars(jPathFileSAM_Image, &isCopy);

	res = ReadImage(samImage, sizeToRead);

    jebv->ReleaseStringUTFChars(jPathFileSAM_Image, samImage);

    printf("retBossac=%d", res);

    return (res);
}


#ifdef __cplusplus
} // extern "C"
#endif
