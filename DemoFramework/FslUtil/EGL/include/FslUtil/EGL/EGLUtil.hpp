#ifndef FSLUTIL_EGL_EGLUTIL_HPP
#define FSLUTIL_EGL_EGLUTIL_HPP
/****************************************************************************************************************************************************
* Copyright 2018 NXP
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*    * Redistributions of source code must retain the above copyright notice,
*      this list of conditions and the following disclaimer.
*
*    * Redistributions in binary form must reproduce the above copyright notice,
*      this list of conditions and the following disclaimer in the documentation
*      and/or other materials provided with the distribution.
*
*    * Neither the name of the NXP. nor the names of
*      its contributors may be used to endorse or promote products derived from
*      this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
****************************************************************************************************************************************************/

#include <FslUtil/EGL/CheckError.hpp>
#include <vector>
#include <EGL/egl.h>

namespace Fsl
{
  namespace EGLUtil
  {
    //! @brief Get a list of all extensions
    std::vector<std::string> GetExtensions(const EGLDisplay display);

    //! @brief Check if the given EGL extension is available
    //! @note Do not expect mind blowing performance from this!
    //! @note If you need to check for multiple extensions consider rolling your own or wait for the helper method to get added :)
    bool HasExtension(const EGLDisplay display, const char*const pszExtensionName);


    std::vector<EGLConfig> GetConfigs(const EGLDisplay dpy);

    //! Get the currently known config attributes that can be used for eglGetConfigAttribute calls
    std::vector<EGLenum> GetConfigAttribs();
  }
}

#endif