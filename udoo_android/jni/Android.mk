# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#------------------------------------------------
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := bossacjni
LOCAL_SRC_FILES := \
	WordCopyArm.cpp \
	Samba.cpp \
	Flash.cpp \
	EfcFlash.cpp \
	EefcFlash.cpp \
	FlashFactory.cpp \
	Applet.cpp \
	WordCopyApplet.cpp \
	bossac.cpp \
	Flasher.cpp \
	PosixSerialPort.cpp \
	LinuxPortFactory.cpp
	
	ifeq ($(APP_OPTIM),debug)
		LOCAL_CPPFLAGS += -O0 -g3
	endif

	# ----------------------------
	#BOSSAC_LOG = 1, log on LogCat
	# ----------------------------
	
	BOSSAC_LOG = 1
	ifdef BOSSAC_LOG
		LOCAL_LDLIBS := -llog
		LOCAL_CPPFLAGS += -DBOSSAC_LOG
	endif
	
	LOCAL_CPP_FEATURES += exceptions		
	LOCAL_CPPFLAGS += -fpermissive
	LOCAL_CFLAGS += -DVERBOSE=1
	LOCAL_CPPFLAGS += -DVERBOSE=1
	include $(BUILD_SHARED_LIBRARY)
