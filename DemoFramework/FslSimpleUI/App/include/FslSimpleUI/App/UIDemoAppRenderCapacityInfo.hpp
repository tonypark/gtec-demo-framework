#ifndef FSLSIMPLEUI_APP_UIDEMOAPPRENDERCAPACITYINFO_HPP
#define FSLSIMPLEUI_APP_UIDEMOAPPRENDERCAPACITYINFO_HPP
/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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

#include <FslBase/BasicTypes.hpp>

namespace Fsl
{
  struct UIDemoAppRenderCapacityInfo
  {
    //! If the capacity is set to to zero its up to the render to select a good default.
    uint32_t InitalVertexCapacity{0};
    //! If the capacity is set to to zero its up to the render to select a good default.
    uint32_t InitalIndexCapacity{0};

    constexpr UIDemoAppRenderCapacityInfo() noexcept = default;

    constexpr UIDemoAppRenderCapacityInfo(const uint32_t initalVertexCapacity, const uint32_t initalIndexCapacity) noexcept
      : InitalVertexCapacity(initalVertexCapacity)
      , InitalIndexCapacity(initalIndexCapacity)
    {
    }

    constexpr bool operator==(const UIDemoAppRenderCapacityInfo& rhs) const noexcept
    {
      return InitalVertexCapacity == rhs.InitalVertexCapacity && InitalIndexCapacity == rhs.InitalIndexCapacity;
    }

    constexpr bool operator!=(const UIDemoAppRenderCapacityInfo& rhs) const noexcept
    {
      return !(*this == rhs);
    }
  };
}

#endif
