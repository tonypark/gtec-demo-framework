#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#****************************************************************************************************************************************************
# Copyright 2019 NXP
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#    * Redistributions of source code must retain the above copyright notice,
#      this list of conditions and the following disclaimer.
#
#    * Redistributions in binary form must reproduce the above copyright notice,
#      this list of conditions and the following disclaimer in the documentation
#      and/or other materials provided with the distribution.
#
#    * Neither the name of the NXP. nor the names of
#      its contributors may be used to endorse or promote products derived from
#      this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#****************************************************************************************************************************************************

from typing import Optional
from FslBuildGen.BuildConfig.CMakeConfiguration import CMakeConfiguration
from FslBuildGen.BuildExternal import CMakeTypes
from FslBuildGen.CMakeUtil import CMakeUtil
from FslBuildGen.CMakeUtil import CMakeVersion
from FslBuildGen.Generator.GeneratorCMakeConfig import GeneratorCMakeConfig
from FslBuildGen.Tool.UserCMakeConfig import UserCMakeConfig

def BuildGeneratorCMakeConfig(platformName: str, userCMakeConfig: Optional[UserCMakeConfig],
                              cmakeConfiguration: CMakeConfiguration, defaultCompilerVersion: int) -> GeneratorCMakeConfig:
    """
    Build the CMake config based on the supplied parameters and the default settings from the toolconfig
    """

    # Setup default configuration
    buildDir = cmakeConfiguration.DefaultBuildDir
    generatorName = ""
    installPrefix = cmakeConfiguration.DefaultInstallPrefix

    # Give the platform a chance to override the config
    platformConfig = cmakeConfiguration.TryGetPlatformConfig(platformName)
    if platformConfig is not None:
        if platformConfig.DefaultGeneratorName is not None:
            generatorName = platformConfig.DefaultGeneratorName
        if platformConfig.DefaultInstallPrefix is not None:
            installPrefix = platformConfig.DefaultInstallPrefix

    # Apply the commandline overrides (so the user gets the final say)
    if userCMakeConfig is not None:
        if userCMakeConfig.BuildDir is not None:
            buildDir = userCMakeConfig.BuildDir
        if userCMakeConfig.GeneratorName is not None:
            generatorName = userCMakeConfig.GeneratorName
        if userCMakeConfig.InstallPrefix is not None:
            installPrefix = userCMakeConfig.InstallPrefix

    # If we still dont have a generator name then try to select a good default
    if len(generatorName) <= 0:
        # Try to determine the default generator name for the platform
        generatorName = CMakeTypes.GetPlatformDefaultCMakeGenerator(platformName, defaultCompilerVersion)

    cmakeVersion = CMakeUtil.GetVersion()

    return GeneratorCMakeConfig(buildDir, generatorName, installPrefix, cmakeVersion)