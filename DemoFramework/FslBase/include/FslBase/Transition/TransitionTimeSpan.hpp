#ifndef FSLBASE_TRANSITION_TRANSITIONTIMESPAN_HPP
#define FSLBASE_TRANSITION_TRANSITIONTIMESPAN_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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
#include <FslBase/Exceptions.hpp>
#include <FslBase/Transition/TransitionTimeUnit.hpp>
#include <cassert>
#include <limits>

namespace Fsl
{
  struct TransitionTimeSpan
  {
    //! The number of ticks that represent the value of the current TimeSpan structure.
    //! The smallest unit of time is the tick, which is equal to 100 nanoseconds or one ten-millionth of a second.
    //! There are 10,000 ticks in a millisecond. The value of the Ticks property can be negative or positive to represent
    //! a negative or positive time interval
    int64_t Ticks{0};

    constexpr TransitionTimeSpan() noexcept = default;

    constexpr explicit TransitionTimeSpan(const int64_t ticks) noexcept
      : Ticks(ticks)
    {
    }

    constexpr TransitionTimeSpan(const int32_t time, TransitionTimeUnit unit)
    {
      switch (unit)
      {
      case TransitionTimeUnit::Milliseconds:
        Ticks = static_cast<int64_t>(time) * 10000;
        break;
      case TransitionTimeUnit::Microseconds:
        Ticks = static_cast<int64_t>(time) * 10;
        break;
      default:
        throw NotSupportedException("The given time unit has not been implemented");
      }
    }

    constexpr TransitionTimeSpan(const uint32_t time, TransitionTimeUnit unit)
    {
      switch (unit)
      {
      case TransitionTimeUnit::Milliseconds:
        Ticks = static_cast<int64_t>(time) * 10000;
        break;
      case TransitionTimeUnit::Microseconds:
        Ticks = static_cast<int64_t>(time) * 10;
        break;
      default:
        throw NotSupportedException("The given time unit has not been implemented");
      }
    }

    constexpr TransitionTimeSpan(const int64_t time, TransitionTimeUnit unit)
    {
      switch (unit)
      {
      case TransitionTimeUnit::Milliseconds:
        assert(time <= (std::numeric_limits<int64_t>::max() / 10000));
        Ticks = time * 10000;
        break;
      case TransitionTimeUnit::Microseconds:
        assert(time <= (std::numeric_limits<int64_t>::max() / 10));
        Ticks = time * 10;
        break;
      default:
        throw NotSupportedException("The given time unit has not been implemented");
      }
    }

    constexpr TransitionTimeSpan(const uint64_t time, TransitionTimeUnit unit)
    {
      switch (unit)
      {
      case TransitionTimeUnit::Milliseconds:
        assert(time <= static_cast<uint64_t>(std::numeric_limits<int64_t>::max() / 10000));
        Ticks = time * 10000;
        break;
      case TransitionTimeUnit::Microseconds:
        assert(time <= static_cast<uint64_t>(std::numeric_limits<int64_t>::max() / 10));
        Ticks = time * 10;
        break;
      default:
        throw NotSupportedException("The given time unit has not been implemented");
      }
    }

    constexpr float DeltaTime() const
    {
      return float(double(Ticks) / 10000000.0);
    }

    constexpr bool operator==(const TransitionTimeSpan& rhs) const
    {
      return Ticks == rhs.Ticks;
    }

    constexpr bool operator!=(const TransitionTimeSpan& rhs) const
    {
      return Ticks != rhs.Ticks;
    }
  };
}

#endif
