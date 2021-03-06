#!/usr/bin/env python3

#****************************************************************************************************************************************************
# Copyright (c) 2014 Freescale Semiconductor, Inc.
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
#    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

from typing import Dict
from FslBuildGen.BasicConfig import BasicConfig
from FslBuildGen.Config import Config
from FslBuildGen.Xml.Exceptions import ImportTemplateNotFoundException
from FslBuildGen.Xml.XmlGenFslBuildTemplate import XmlGenFslBuildTemplate
from FslBuildGen.Xml.XmlStuff import XmlGenFileImportTemplate


class PackageTemplateLoader(object):
    def __init__(self, config: Config, templateLocationCache: Dict[str, str]) -> None:
        super().__init__()
        self.__Config = config  # type: Config
        self.BasicConfig = config  # type: BasicConfig
        self.TemplateLocationCache = templateLocationCache
        self.TemplateDict = {}  # type: Dict[str, XmlGenFslBuildTemplate]


    def Import(self, cmd: XmlGenFileImportTemplate, name: str) -> XmlGenFslBuildTemplate:
        if name in self.TemplateDict:
            return self.TemplateDict[name]

        if not name in self.TemplateLocationCache:
            raise ImportTemplateNotFoundException(cmd.XMLElement, name)

        template = XmlGenFslBuildTemplate(self.__Config, self.__Config.ToolConfig.RequirementTypes, self.TemplateLocationCache[name])
        self.TemplateDict[name] = template
        return template
