#ifndef FSLSIMPLEUI_BASE_LAYOUT_UNIFORMSTACKLAYOUT_HPP
#define FSLSIMPLEUI_BASE_LAYOUT_UNIFORMSTACKLAYOUT_HPP
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

#include <FslSimpleUI/Base/Layout/SimpleLayout.hpp>
#include <FslSimpleUI/Base/Layout/LayoutOrientation.hpp>

namespace Fsl
{
  namespace UI
  {
    class UniformStackLayout : public SimpleLayout
    {
      LayoutOrientation m_orientation;
      float m_spacingDp;
      int32_t m_maxSizePx{0};

    public:
      explicit UniformStackLayout(const std::shared_ptr<BaseWindowContext>& context);

      LayoutOrientation GetLayoutOrientation() const
      {
        return m_orientation;
      }
      void SetLayoutOrientation(const LayoutOrientation& value);
      float GetSpacing() const
      {
        return m_spacingDp;
      }
      void SetSpacing(const float& value);

    protected:
      PxSize2D ArrangeOverride(const PxSize2D& finalSizePx) override;
      PxSize2D MeasureOverride(const PxAvailableSize& availableSizePx) override;
    };
  }
}

#endif
