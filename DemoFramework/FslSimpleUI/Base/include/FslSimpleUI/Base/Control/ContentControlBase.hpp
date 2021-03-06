#ifndef FSLSIMPLEUI_BASE_CONTROL_CONTENTCONTROLBASE_HPP
#define FSLSIMPLEUI_BASE_CONTROL_CONTENTCONTROLBASE_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2015 Freescale Semiconductor, Inc.
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

#include <FslBase/Math/Dp/DpThickness.hpp>
#include <FslSimpleUI/Base/BaseWindow.hpp>
#include <memory>

namespace Fsl
{
  namespace UI
  {
    class ContentControlBase : public BaseWindow
    {
      std::shared_ptr<BaseWindow> m_content;
      DpThickness m_paddingDp;
      bool m_isInitialized;

    public:
      explicit ContentControlBase(const std::shared_ptr<BaseWindowContext>& context);

      void WinInit() override;

    protected:
      void OnPropertiesUpdated(const PropertyTypeFlags& flags) override;

      void SetContentVisibility(const ItemVisibility visibility)
      {
        if (m_content)
        {
          m_content->SetVisibility(visibility);
        }
      }

      //! @brief This includes any padding set
      PxSize2D GetContentDesiredSizePx() const;

      DpThickness DoGetPadding() const
      {
        return m_paddingDp;
      }
      void DoSetPadding(const DpThickness& valueDp);

      std::shared_ptr<BaseWindow> DoGetContent() const
      {
        return m_content;
      }
      void DoSetContent(const std::shared_ptr<BaseWindow>& value);

      PxSize2D ArrangeOverride(const PxSize2D& finalSizePx) override;
      PxSize2D MeasureOverride(const PxAvailableSize& availableSizePx) override;

      //! @brief Alternative method to ArrangeOverride that can be called to tweak the content offset
      PxSize2D CustomArrange(const PxSize2D& finalSizePx, const PxPoint2& positionOffset);
    };
  }
}

#endif
