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

#include <FslSimpleUI/Base/System/UIManager.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslSimpleUI/Base/BaseWindowContext.hpp>
#include <FslSimpleUI/Base/Event/WindowEventPool.hpp>
#include <FslSimpleUI/Base/Event/WindowEventSender.hpp>
#include <FslSimpleUI/Render/Base/IRenderSystem.hpp>
#include <FslSimpleUI/Base/UIContext.hpp>
#include "Event/EventRouter.hpp"
#include "Event/SimpleEventSender.hpp"
#include "Event/WindowEventQueueEx.hpp"
#include "Modules/ModuleHost.hpp"
#include "Modules/ModuleCallbackRegistry.hpp"
#include "Modules/External/ExternalModules.hpp"
#include "Modules/Input/InputModule.hpp"
#include "UITree.hpp"
#include "RootWindow.hpp"
#include <cassert>

namespace Fsl
{
  namespace UI
  {
    UIManager::UIManager(std::unique_ptr<IRenderSystem> renderSystem, const bool useYFlipTextureCoordinates, const BasicWindowMetrics& windowMetrics)
      : UIManager(std::move(renderSystem), useYFlipTextureCoordinates, windowMetrics, {})
    {
    }


    UIManager::UIManager(std::unique_ptr<IRenderSystem> renderSystem, const bool useYFlipTextureCoordinates, const BasicWindowMetrics& windowMetrics,
                         ReadOnlySpan<std::shared_ptr<IExternalModuleFactory>> externalModuleFactories)
      : m_renderSystem(std::move(renderSystem), useYFlipTextureCoordinates)
      , m_windowMetrics(windowMetrics)
      , m_moduleCallbackRegistry(std::make_shared<ModuleCallbackRegistry>())
      , m_eventPool(std::make_shared<WindowEventPool>())
      , m_eventQueue(std::make_shared<WindowEventQueueEx>())
      , m_tree(std::make_shared<UITree>(m_moduleCallbackRegistry, m_eventPool, m_eventQueue))
      , m_eventSender(std::make_shared<WindowEventSender>(m_eventQueue, m_eventPool, m_tree))
      , m_uiContext(std::make_shared<UIContext>(m_tree, m_eventSender, m_renderSystem.GetMeshManager()))
      , m_baseWindowContext(std::make_shared<BaseWindowContext>(m_uiContext, windowMetrics.DensityDpi))
      , m_rootWindow(std::make_shared<RootWindow>(m_baseWindowContext, windowMetrics.ExtentPx, windowMetrics.DensityDpi))
      , m_leftButtonDown(false)
    {
      m_tree->Init(m_rootWindow);
      try
      {
        m_simpleEventSender = std::make_shared<SimpleEventSender>(m_tree, m_eventPool, m_tree, m_tree->GetRootNode(), m_tree);

        m_moduleHost = std::make_shared<ModuleHost>(m_moduleCallbackRegistry, m_tree, m_tree->GetRootNode(), m_tree, m_tree, m_tree, m_eventPool,
                                                    m_eventSender, m_simpleEventSender);
        m_inputModule = std::make_shared<InputModule>(m_moduleHost);

        if (!externalModuleFactories.empty())
        {
          // Only create the external module if needed.
          m_externalModules = std::make_shared<ExternalModules>(m_moduleHost, externalModuleFactories);
          m_moduleCallbackRegistry->AddCallbackReceiver(m_externalModules);
        }
        // Give the tree a chance to send module events about the root window to all registered entities
        m_tree->PostInit();
      }
      catch (const std::exception&)
      {
        if (m_externalModules)
        {
          m_moduleCallbackRegistry->RemoveCallbackReceiver(m_externalModules);
          m_externalModules.reset();
        }
        m_inputModule.reset();
        m_moduleHost.reset();
        m_simpleEventSender.reset();
        m_tree->Shutdown();
        throw;
      }
    }


    UIManager::~UIManager()
    {
      if (m_tree)
      {
        m_tree->PreShutdown();
      }

      if (m_externalModules)
      {
        m_moduleCallbackRegistry->RemoveCallbackReceiver(m_externalModules);
        m_externalModules.reset();
      }
      m_inputModule.reset();
      m_moduleHost.reset();
      m_simpleEventSender.reset();
      if (m_tree)
      {
        try
        {
          m_tree->Shutdown();
        }
        catch (const std::exception& ex)
        {
          FSLLOG3_ERROR("A exception was thrown during shutdown: {}", ex.what());
        }
        m_tree.reset();
      }
    }


    std::shared_ptr<AExternalModule> UIManager::GetExternalModule(const ExternalModuleId& moduleId) const
    {
      if (m_externalModules)
      {
        return m_externalModules->GetExternalModule(moduleId);
      }
      throw NotFoundException("The requested external module was not found");
    }


    const std::shared_ptr<UIContext>& UIManager::GetUIContext() const
    {
      return m_uiContext;
    }

    std::shared_ptr<IWindowManager> UIManager::GetWindowManager() const
    {
      return m_tree;
    }


    const std::shared_ptr<WindowEventPool>& UIManager::GetEventPool() const
    {
      return m_eventPool;
    }


    const std::shared_ptr<WindowEventSender>& UIManager::GetEventSender() const
    {
      return m_eventSender;
    }


    // bool UIManager::SendKeyEvent(const KeyEvent& event)
    //{
    //}


    bool UIManager::SendMouseButtonEvent(const PxPoint2& positionPx, const bool LeftButtonDown)
    {
      assert(m_inputModule);

      m_leftButtonDown = LeftButtonDown;
      const auto transactionState = m_leftButtonDown ? EventTransactionState::Begin : EventTransactionState::End;
      return m_inputModule->SendClickEvent(0, 0, transactionState, false, positionPx);
    }


    bool UIManager::SendMouseMoveEvent(const PxPoint2& positionPx)
    {
      bool isHandled = m_inputModule->MouseMove(0, 0, positionPx);

      if (m_leftButtonDown)
      {
        if (m_inputModule->SendClickEvent(0, 0, EventTransactionState::Begin, true, positionPx))
        {
          isHandled = true;
        }
      }
      return isHandled;
    }


    // bool UIManager::SendMouseWheelEvent(const MouseWheelEvent& event)
    //{
    //}

    bool UIManager::IsIdle() const
    {
      return (!m_tree || m_tree->IsIdle()) && (!m_inputModule || m_inputModule->IsIdle());
    }

    UIStats UIManager::GetStats() const
    {
      return m_tree ? m_tree->GetStats() : UIStats();
    }

    void UIManager::ProcessEvents()
    {
      m_tree->ProcessEvents();
    }

    void UIManager::Resized(const BasicWindowMetrics& windowMetrics)
    {
      if (windowMetrics == m_windowMetrics)
      {
        return;
      }
      m_windowMetrics = windowMetrics;

      m_renderSystem.ConfigurationChanged(windowMetrics);
      if (m_baseWindowContext)
      {
        m_baseWindowContext->UnitConverter.SetDensityDpi(windowMetrics.DensityDpi);
      }
      if (m_tree)
      {
        m_tree->Resized(windowMetrics.ExtentPx, windowMetrics.DensityDpi);
      }
    }


    void UIManager::FixedUpdate(const TransitionTimeSpan& timespan)    // NOLINT(readability-convert-member-functions-to-static)
    {
      FSL_PARAM_NOT_USED(timespan);
    }


    void UIManager::Update(const TransitionTimeSpan& timespan)
    {
      m_tree->Update(timespan);
    }


    void UIManager::PreDraw()
    {
      m_renderSystem.PreDraw();
    }

    void UIManager::Draw(RenderPerformanceCapture* const pPerformanceCapture)
    {
      {    // Record the draw command list
        UIRenderSystem::ScopedDrawCommandBufferAccess scopedAccess(m_renderSystem);
        m_tree->Draw(scopedAccess.GetDrawCommandBuffer());
      }
      m_renderSystem.Draw(pPerformanceCapture);
    }

    void UIManager::PostDraw()
    {
      m_renderSystem.PostDraw();
    }

    const UI::IRenderSystemBase& UIManager::GetRenderSystem() const
    {
      return m_renderSystem.GetRenderSystem();
    }

    UI::IRenderSystemBase* UIManager::TryGetRenderSystem()
    {
      return m_renderSystem.TryGetRenderSystem();
    }

    void UIManager::RegisterEventListener(const std::weak_ptr<IEventListener>& eventListener)
    {
      m_tree->RegisterEventListener(eventListener);
    }


    void UIManager::UnregisterEventListener(const std::weak_ptr<IEventListener>& eventListener)
    {
      m_tree->UnregisterEventListener(eventListener);
    }


    void UIManager::ForceInvalidateLayout()
    {
      BasicWindowMetrics cachedMetrics = m_windowMetrics;
      BasicWindowMetrics changedWindowMetrics(cachedMetrics.ExtentPx + PxExtent2D(1, 1), cachedMetrics.ExactDpi, cachedMetrics.DensityDpi);
      Resized(changedWindowMetrics);
      Resized(cachedMetrics);
    }

  }
}
