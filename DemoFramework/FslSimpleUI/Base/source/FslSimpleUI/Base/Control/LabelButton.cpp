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

#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/Pixel/TypeConverter_Math.hpp>
#include <FslBase/String/StringViewLiteUtil.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Sprite/Font/SpriteFont.hpp>
#include <FslSimpleUI/Base/Control/LabelButton.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Render/Base/DrawCommandBuffer.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <cassert>

namespace Fsl
{
  namespace UI
  {
    LabelButton::LabelButton(const std::shared_ptr<WindowContext>& context)
      : ButtonBase(context)
      , m_windowContext(context)
      , m_fontMesh(context->TheUIContext.Get()->MeshManager, context->DefaultFont)
    {
      Enable(WindowFlags(WindowFlags::DrawEnabled));
    }


    void LabelButton::SetContent(const std::string& value)
    {
      if (m_fontMesh.SetText(value))
      {
        PropertyUpdated(PropertyType::Content);
      }
    }


    void LabelButton::SetFont(const std::shared_ptr<SpriteFont>& value)
    {
      if (m_fontMesh.SetSprite(value))
      {
        PropertyUpdated(PropertyType::Content);
      }
    }


    void LabelButton::SetColorUp(const Color& value)
    {
      if (value != m_colorUp)
      {
        m_colorUp = value;
        PropertyUpdated(PropertyType::Other);
      }
    }


    void LabelButton::SetColorDown(const Color& value)
    {
      if (value != m_colorDown)
      {
        m_colorDown = value;
        PropertyUpdated(PropertyType::Other);
      }
    }


    void LabelButton::WinDraw(const UIDrawContext& context)
    {
      ButtonBase::WinDraw(context);

      if (m_fontMesh.IsValid())
      {
        const auto color = !IsDown() ? m_colorUp : m_colorDown;
        context.CommandBuffer.Draw(m_fontMesh.Get(), context.TargetRect.TopLeft(), m_cachedMeasureMinimalFontSizePx, GetFinalBaseColor() * color);
      }
    }


    PxSize2D LabelButton::ArrangeOverride(const PxSize2D& finalSizePx)
    {
      return finalSizePx;
    }


    PxSize2D LabelButton::MeasureOverride(const PxAvailableSize& availableSizePx)
    {
      FSL_PARAM_NOT_USED(availableSizePx);
      const auto measureInfo = m_fontMesh.ComplexMeasure();
      m_cachedMeasureMinimalFontSizePx = measureInfo.MinimalSizePx;
      return measureInfo.MeasureSizePx;
    }
  }
}
