#ifndef FSLNATIVEWINDOW_BASE_VIRTUALMOUSEBUTTONFLAGS_HPP
#define FSLNATIVEWINDOW_BASE_VIRTUALMOUSEBUTTONFLAGS_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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
#include <FslNativeWindow/Base/VirtualMouseButton.hpp>

namespace Fsl
{
  struct VirtualMouseButtonFlags
  {
    uint32_t Flags{0};

    constexpr VirtualMouseButtonFlags() noexcept = default;


    constexpr explicit VirtualMouseButtonFlags(const uint32_t flags) noexcept
      : Flags(flags)
    {
    }

    constexpr explicit VirtualMouseButtonFlags(const VirtualMouseButton::Enum flags) noexcept
      : Flags(static_cast<uint32_t>(flags))
    {
    }


    constexpr bool IsEnabled(const VirtualMouseButton::Enum flags) const noexcept
    {
      return (Flags & static_cast<uint32_t>(flags)) != 0;
    }

    constexpr bool IsLeftButtonPressed() const noexcept
    {
      return (Flags & VirtualMouseButton::Left) != 0;
    }


    constexpr bool IsMiddleButtonPressed() const noexcept
    {
      return (Flags & VirtualMouseButton::Middle) != 0;
    }


    constexpr bool IsRightButtonPressed() const noexcept
    {
      return (Flags & VirtualMouseButton::Right) != 0;
    }


    constexpr bool IsXButton1Pressed() const noexcept
    {
      return (Flags & VirtualMouseButton::X1) != 0;
    }


    constexpr bool IsXButton2Pressed() const noexcept
    {
      return (Flags & VirtualMouseButton::X2) != 0;
    }


    constexpr bool IsXButton3Pressed() const noexcept
    {
      return (Flags & VirtualMouseButton::X3) != 0;
    }


    constexpr bool IsXButton4Pressed() const noexcept
    {
      return (Flags & VirtualMouseButton::X4) != 0;
    }


    constexpr bool IsXButton5Pressed() const noexcept
    {
      return (Flags & VirtualMouseButton::X5) != 0;
    }

    constexpr bool IsUndefined() const noexcept
    {
      return (Flags & VirtualMouseButton::Undefined) != 0;
    }

    constexpr void SetFlag(const VirtualMouseButton::Enum flags, const bool enabled) noexcept
    {
      if (enabled)
      {
        Flags |= static_cast<uint32_t>(flags);
      }
      else
      {
        Flags = (flags & (~static_cast<uint32_t>(flags)));
      }
    }
  };
}

#endif
