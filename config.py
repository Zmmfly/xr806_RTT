# 
# Copyright (C) 2021 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#    1. Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#    2. Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the
#       distribution.
#    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
#       its contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
#  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
#  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
#  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
#  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
#  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
#  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#!/usr/bin/python
# -*- coding: UTF-8 -*-
import os

pwd = os.path.dirname (__file__)
f = open('.{0}/.config'.format(pwd),'r')


DATA = ['#XR806 config']
for line in f.readlines():
    if line[0] != '#' and line[0] != '\n':
        line = line.strip('\n')
        DATA.append(line)

GNf = open('.{0}/../liteos_m/SDKconfig.gni'.format(pwd),'w')
GNf.write('#Build by config.py DO NOT EDIT!\n\n')
GNf.write('SDK_cflags = [\n\
    "-mcpu=cortex-m33",\n\
    "-mtune=cortex-m33",\n\
    "-march=armv8-m.main+dsp",\n\
    "-mfpu=fpv5-sp-d16",\n\
    "-mfloat-abi=softfp",\n\
    "-mcmse",\n\
    "-mthumb",\n\
    "-c",\n\
    "-g",\n\
    "-fno-common",\n\
    "-fmessage-length=0",\n\
    "-fno-exceptions",\n\
    "-ffunction-sections",\n\
    "-fdata-sections",\n\
    "-fomit-frame-pointer",\n\
    "-Wall",\n\
    #"-Werror",\n\
    "-Wno-cpp",\n\
    "-Wpointer-arith",\n\
    "-Wno-error=unused-function",\n\
    "-MMD",\n\
    "-MP",\n\
    "-Os",\n\
    "-DNDEBUG",\n\
    "-Wno-error=stringop-truncation",\n\
    "-Wno-error=restrict",\n\
    "-includexr_config.h",\n\
    "-includecommon/prj_conf_opt.h",\n\
    "-DCONFIG_CHIP_ARCH_VER=3",\n\
    "-DCONFIG_ARCH_APP_CORE",\n\
    "-DCONFIG_CPU_CM33F",\n\
]\n\n')

PROJECT = [x for i,x in enumerate(DATA) if x.find('CONFIG_PROJECT=') != -1]
if len(PROJECT) == 1:
    ProjectPath = PROJECT[0]
    ProjectPath = ProjectPath.strip('CONFIG_PROJECT=')
GNf.write('ProjectPath = {0}\n\n'.format(ProjectPath))
if ProjectPath == '"bootloader"' or ProjectPath == '"test/etf"':
    GNf.write('declare_args() {IsBootloader = "true"}\n')
else:
    GNf.write('declare_args() {IsBootloader = "false"}\n')



#print (DATA)