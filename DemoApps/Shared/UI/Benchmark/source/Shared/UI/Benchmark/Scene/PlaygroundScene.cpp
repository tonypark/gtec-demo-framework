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

#include "PlaygroundScene.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/IO/FmtPath.hpp>
#include <FslBase/Math/MathHelper_Clamp.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslService/Consumer/ServiceProvider.hpp>
#include <FslSimpleUI/App/DemoPerformanceCapture.hpp>
#include <FslSimpleUI/App/UIDemoAppMaterialConfig.hpp>
#include <FslSimpleUI/App/UIDemoAppMaterialCreateInfo.hpp>
#include <FslSimpleUI/Base/Control/Background.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Control/SlidingPanel.hpp>
#include <FslSimpleUI/Base/DefaultAnim.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <FslSimpleUI/Base/Layout/GridLayout.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Base/Layout/UniformStackLayout.hpp>
#include <FslSimpleUI/Base/IWindowManager.hpp>
#include <FslSimpleUI/Controls/Charts/AreaChart.hpp>
#include <FslSimpleUI/Render/Base/IRenderSystemBase.hpp>
#include <FslSimpleUI/Render/Base/RenderSystemStats.hpp>
#include <FslSimpleUI/Render/IMBatch/FlexRenderSystemConfig.hpp>
#include <FslSimpleUI/Render/IMBatch/IFlexRenderSystemConfig.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <FslSimpleUI/Theme/Base/IThemeFactory.hpp>
#include <FslSimpleUI/Theme/Base/IThemeResources.hpp>
#include <Shared/UI/Benchmark/Activity/ActivityStack.hpp>
#include <Shared/UI/Benchmark/App/TestAppFactory.hpp>
#include <Shared/UI/Benchmark/Scene/Dialog/BenchConfigDialogActivity.hpp>
#include <Shared/UI/Benchmark/Scene/Dialog/FrameAnalysisDialogActivity.hpp>
#include <Shared/UI/Benchmark/Scene/Dialog/SettingsDialogActivity.hpp>
#include <Shared/UI/Benchmark/TextConfig.hpp>
#include <array>
#include "Config/BenchSceneConfig.hpp"
#include "Control/CustomControlFactory.hpp"
#include "Control/CustomUIConfig.hpp"
#include "Control/RenderOptionControlsFactory.hpp"
#include "SceneCreateInfo.hpp"
#include "RenderConfig.hpp"
#include "RenderSystemRuntimeSettingsUtil.hpp"
#include "TestHostForwarderTweak.hpp"

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr uint32_t AverageEntries = 20;

      constexpr int32_t DpiMin = 80;
      constexpr int32_t DpiMax = 640;
      constexpr int32_t SliderDpiTick = 40u;
      constexpr int32_t SliderDpiTickSlow = 1u;
    }

    namespace LocalStrings
    {
      constexpr StringViewLite Meshes("Meshes:");
      constexpr StringViewLite Batches("Batches:");
      constexpr StringViewLite VBs("VBs:");
      constexpr StringViewLite IBs("IBs:");
      constexpr StringViewLite Vertices("Vertices:");
      constexpr StringViewLite Indices("Indices:");
      constexpr StringViewLite Draw("Draw:");
      constexpr StringViewLite DrawIndexed("DrawIndexed:");
      constexpr StringViewLite EmulateDpi("Emulate dpi");
      constexpr StringViewLite SdfFont("SDF font");
    }

    namespace IdleColor
    {
      constexpr Color Idle = Color::Green();
      constexpr Color SystemBusy = Color::Orange();
      constexpr Color Busy = Color::Red();
    }

    Color DetermineIdleColor(const bool appIdle, const bool systemIdle)
    {
      if (systemIdle)
      {
        if (appIdle)
        {
          return IdleColor::Idle;
        }
        return IdleColor::Busy;
      }
      return IdleColor::SystemBusy;
    }
  }


  PlaygroundScene::PlaygroundScene(const SceneCreateInfo& createInfo)
    : m_forwarder(createInfo.Forwarder)
    , m_uiControlFactory(createInfo.ControlFactory)
    , m_windowMetrics(createInfo.WindowMetrics)
    , m_uiExtension(createInfo.UIExtension)
    , m_gpuProfilerSupported(createInfo.GpuProfiler)
    , m_renderRecords(ReadOnlySpanUtil::ToVector(RenderConfig::Get()))
    , m_data(std::make_shared<UI::AreaChartData>(m_windowMetrics.ExtentPx.Width, CustomControlFactory::MaxCpuProfileDataEntries()))
    , m_dataAverage(LocalConfig::AverageEntries)
    , m_anim(m_uiControlFactory->GetContext()->UITransitionCache, UI::DefaultAnim::ColorChangeTime)
    , m_testAppHost(std::make_shared<TestAppHost>(createInfo.DemoServiceProvider, m_windowMetrics))
    , m_settings(createInfo.Settings)
  {
    m_uiProfile = CreateProfileUI(*m_uiControlFactory, NumericCast<uint16_t>(m_windowMetrics.DensityDpi), m_data,
                                  ReadOnlySpanUtil::AsSpan(m_renderRecords), m_activeRenderIndex, m_settings->UI);
    RestoreUISettings(m_renderRecords[m_activeRenderIndex]);

    createInfo.RootLayout->AddChild(m_uiProfile.MainLayout);

    FSLLOG3_INFO("Launching test app");
    RestartTestApp(false);
    FSLLOG3_INFO("Ready");

    m_forwarder->SetExtension1(m_testAppHost);
    m_forwarder->SetTweak(std::make_shared<TestHostForwarderTweak>(m_uiProfile.ContentArea, m_testAppHost));
  }


  PlaygroundScene::~PlaygroundScene()
  {
    m_forwarder->ClearTweak();
    m_forwarder->ClearExtension1();
  }


  void PlaygroundScene::OnFrameSequenceBegin()
  {
    m_testAppHost->ClearProfileData();
  }


  void PlaygroundScene::OnSelect(const std::shared_ptr<UI::WindowSelectEvent>& theEvent)
  {
    if (m_inputState == InputState::Playground)
    {
      if (theEvent->GetSource() == m_uiProfile.BottomBar.ButtonClear)
      {
        ClearGraph();
        theEvent->Handled();
      }
      else if (theEvent->GetSource() == m_uiProfile.OptionsBar.ButtonConfig)
      {
        m_settingsResult = std::make_shared<AppTestSettings>(m_settings->Test);
        theEvent->Handled();
        m_uiProfile.ActivityStack->Push(std::make_shared<UI::SettingsDialogActivity>(m_uiProfile.ActivityStack, m_uiControlFactory,
                                                                                     UI::Theme::WindowType::DialogNormal, m_settingsResult));
        m_inputState = InputState::SettingsSubActivity;
      }
      else if (theEvent->GetSource() == m_uiProfile.OptionsBar.ButtonFrameAnalysis)
      {
        theEvent->Handled();
        m_frameAnalysisDialog = std::make_shared<UI::FrameAnalysisDialogActivity>(
          m_uiProfile.ActivityStack, m_uiControlFactory, UI::Theme::WindowType::DialogNormal, FrameAnalysisGetCurrentMaxDrawCalls());
        m_uiProfile.ActivityStack->Push(m_frameAnalysisDialog);
        m_inputState = InputState::FrameAnalysisSubActivity;
      }
      else if (theEvent->GetSource() == m_uiProfile.OptionsBar.ButtonRecord)
      {
        theEvent->Handled();
        BeginClose(SceneId::Record);
      }
      else if (theEvent->GetSource() == m_uiProfile.OptionsBar.ButtonBench)
      {
        m_benchResult = std::make_shared<AppBenchSettings>(m_settings->Bench);
        theEvent->Handled();
        m_configDialogPromise = m_uiProfile.ActivityStack->Push(std::make_shared<UI::BenchConfigDialogActivity>(
          m_uiProfile.ActivityStack, m_uiControlFactory, m_gpuProfilerSupported, ReadOnlySpanUtil::AsSpan(m_renderRecords), m_benchResult));
        m_inputState = InputState::BenchConfigSubActivity;
      }
    }
  }

  void PlaygroundScene::OnContentChanged(const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent)
  {
    if (theEvent->IsHandled())
    {
      return;
    }

    if (theEvent->GetSource() == m_uiProfile.OptionsBar.UI.SwitchStats)
    {
      theEvent->Handled();
      m_settings->UI.ShowStats = m_uiProfile.OptionsBar.UI.SwitchStats->IsChecked();
    }
    else if (theEvent->GetSource() == m_uiProfile.OptionsBar.UI.SwitchChart)
    {
      theEvent->Handled();
      m_settings->UI.ShowChart = m_uiProfile.OptionsBar.UI.SwitchChart->IsChecked();
    }
    else if (theEvent->GetSource() == m_uiProfile.OptionsBar.SliderEmulatedDpi)
    {
      theEvent->Handled();
      SetDpi(m_windowMetrics.DensityDpi);
    }
    else if (m_renderRecords[m_activeRenderIndex].RenderMethod == AppRenderMethod::FlexImBatch)
    {
      if (theEvent->GetSource() == m_uiProfile.OptionsBar.RenderOptions.SwitchBatch ||
          theEvent->GetSource() == m_uiProfile.OptionsBar.RenderOptions.SwitchFillBuffers ||
          theEvent->GetSource() == m_uiProfile.OptionsBar.RenderOptions.SwitchDepthBuffer ||
          theEvent->GetSource() == m_uiProfile.OptionsBar.RenderOptions.SwitchDrawReorder)
      {
        theEvent->Handled();

        m_uiProfile.OptionsBar.RenderOptions.StoreUISettings(m_renderRecords[m_activeRenderIndex].SystemInfo);
        RenderSystemRuntimeSettingsUtil::Apply(*m_testAppHost, m_renderRecords[m_activeRenderIndex].SystemInfo.Settings,
                                               m_uiProfile.OptionsBar.SwitchSdfFont->IsChecked());
      }
    }


    if (theEvent->GetSource() == m_uiProfile.OptionsBar.SwitchSdfFont)
    {
      theEvent->Handled();
      UIDemoAppMaterialConfig config = m_testAppHost->GetDefaultMaterialConfig();
      config.UseSdfFont = m_uiProfile.OptionsBar.SwitchSdfFont->IsChecked();
      m_testAppHost->SetDefaultMaterialConfig(config);
    }
    else if (theEvent->GetSource() == m_uiProfile.OptionsBar.SwitchEmulateDpi)
    {
      const bool enabled = m_uiProfile.OptionsBar.SwitchEmulateDpi->IsChecked();
      assert(m_uiProfile.OptionsBar.SliderEmulatedDpi);
      m_uiProfile.OptionsBar.SliderEmulatedDpi->SetEnabled(enabled);
      SetDpi(NumericCast<int32_t>(m_windowMetrics.DensityDpi));
    }
    else
    {
      const auto count = NumericCast<uint32_t>(m_uiProfile.OptionsBar.RenderMethod.Methods.size());
      for (uint32_t i = 0; !theEvent->IsHandled() && i < count; ++i)
      {
        if (theEvent->GetSource() == m_uiProfile.OptionsBar.RenderMethod.Methods[i])
        {
          theEvent->Handled();
          if (m_activeRenderIndex != i)
          {
            // Store the current settings for the render (so its the same when we switch back)
            m_uiProfile.OptionsBar.RenderOptions.StoreUISettings(m_renderRecords[m_activeRenderIndex].SystemInfo);
            m_activeRenderIndex = i;
            // Restore the settings of the just selected UI
            RestoreUISettings(m_renderRecords[m_activeRenderIndex]);

            RestartTestApp(false);
          }
        }
      }
    }
  }


  void PlaygroundScene::OnKeyEvent(const KeyEvent& event)
  {
    if (m_uiProfile.ActivityStack && !m_uiProfile.ActivityStack->IsEmpty())
    {
      m_uiProfile.ActivityStack->OnKeyEvent(event);
    }

    if (event.IsHandled() || !event.IsPressed() || m_inputState != InputState::Playground)
    {
      return;
    }
    switch (event.GetKey())
    {
    case VirtualKey::Space:
      ClearGraph();
      break;
    case VirtualKey::B:
      if (m_uiProfile.OptionsBar.RenderOptions.SwitchBatch->IsEnabled())
      {
        m_uiProfile.OptionsBar.RenderOptions.SwitchBatch->Toggle();
      }
      break;
    case VirtualKey::C:
      m_uiProfile.OptionsBar.UI.SwitchChart->Toggle();
      break;
    case VirtualKey::D:
      if (m_uiProfile.OptionsBar.RenderOptions.SwitchDepthBuffer->IsEnabled())
      {
        m_uiProfile.OptionsBar.RenderOptions.SwitchDepthBuffer->Toggle();
      }
      break;
    case VirtualKey::E:
      if (m_uiProfile.OptionsBar.SwitchEmulateDpi->IsEnabled())
      {
        m_uiProfile.OptionsBar.SwitchEmulateDpi->Toggle();
      }
      break;
    case VirtualKey::F:
      if (m_uiProfile.OptionsBar.RenderOptions.SwitchFillBuffers->IsEnabled())
      {
        m_uiProfile.OptionsBar.RenderOptions.SwitchFillBuffers->Toggle();
      }
      break;
    case VirtualKey::O:
      if (m_uiProfile.OptionsBar.SwitchOnDemand->IsEnabled())
      {
        m_uiProfile.OptionsBar.SwitchOnDemand->Toggle();
      }
      break;
    case VirtualKey::M:
      if (m_uiProfile.OptionsBar.RenderOptions.SwitchMeshCaching->IsEnabled())
      {
        m_uiProfile.OptionsBar.RenderOptions.SwitchMeshCaching->Toggle();
      }
      break;
    case VirtualKey::R:
      if (m_uiProfile.OptionsBar.RenderOptions.SwitchDrawReorder->IsEnabled())
      {
        m_uiProfile.OptionsBar.RenderOptions.SwitchDrawReorder->Toggle();
      }
      break;
    case VirtualKey::S:
      if (m_uiProfile.OptionsBar.SwitchSdfFont->IsEnabled())
      {
        m_uiProfile.OptionsBar.SwitchSdfFont->Toggle();
      }
      break;
    case VirtualKey::T:
      m_uiProfile.OptionsBar.UI.SwitchStats->Toggle();
      break;
    case VirtualKey::DownArrow:
      if (m_uiProfile.OptionsBar.SliderEmulatedDpi->IsEnabled())
      {
        m_uiProfile.OptionsBar.SliderEmulatedDpi->SubValue(LocalConfig::SliderDpiTickSlow);
      }
      break;
    case VirtualKey::UpArrow:
      if (m_uiProfile.OptionsBar.SliderEmulatedDpi->IsEnabled())
      {
        m_uiProfile.OptionsBar.SliderEmulatedDpi->AddValue(LocalConfig::SliderDpiTickSlow);
      }
      break;
    case VirtualKey::LeftArrow:
      if (m_uiProfile.OptionsBar.SliderEmulatedDpi->IsEnabled())
      {
        m_uiProfile.OptionsBar.SliderEmulatedDpi->SubValue(LocalConfig::SliderDpiTick);
      }
      break;
    case VirtualKey::RightArrow:
      if (m_uiProfile.OptionsBar.SliderEmulatedDpi->IsEnabled())
      {
        m_uiProfile.OptionsBar.SliderEmulatedDpi->AddValue(LocalConfig::SliderDpiTick);
      }
      break;
    default:
      break;
    }
  }


  void PlaygroundScene::OnConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    m_windowMetrics = windowMetrics;
    SetDpi(NumericCast<int32_t>(windowMetrics.DensityDpi));
  }


  bool PlaygroundScene::Resolve(const DemoTime& demoTime)
  {
    FSL_PARAM_NOT_USED(demoTime);
    bool isIdle = false;
    if (m_uiProfile.OptionsBar.SwitchOnDemand)
    {
      const bool allowOnDemand = m_uiProfile.OptionsBar.SwitchOnDemand->IsChecked();
      isIdle = allowOnDemand ? m_testAppHost->IsUIIdle() && m_uiExtension->IsIdle() : false;
    }
    return isIdle;
  }


  void PlaygroundScene::Update(const DemoTime& demoTime)
  {
    if (m_frameAnalysisDialog)
    {
      FrameAnalysisSetMaxDrawCalls(m_frameAnalysisDialog->GetCurrentDrawCalls());
    }

    {
      UI::RenderSystemStats stats;
      stats = m_testAppHost->GetRenderSystem().GetStats();
      UpdateStats(m_uiProfile.StatsOverlay, stats);
      stats = m_uiExtension->GetRenderSystem().GetStats();
      UpdateStats(m_uiProfile.StatsOverlay2, stats);
    }

    auto cpuProfileRecords = CustomControlFactory::GetCpuProfileRecords();

    if (m_uiProfile.BottomBar.Enabled && cpuProfileRecords.size() <= m_uiProfile.BottomBar.CpuLegendUI.ChartCpuUI.Controls.size())
    {    // Update average numbers
      ReadOnlySpan<double> span = m_dataAverage.CalcAverage();
      assert(span.size() >= cpuProfileRecords.size());
      double total = 0.0;
      for (std::size_t i = 0; i < cpuProfileRecords.size(); ++i)
      {
        const auto ctrlIndex = cpuProfileRecords[i].Index;
        CustomControlFactory::SetContent(m_uiProfile.BottomBar.CpuLegendUI.ChartCpuUI.Controls[ctrlIndex].AverageTimeLabel.get(), span[i]);
        total += span[i];
      }
      CustomControlFactory::SetContent(m_uiProfile.BottomBar.CpuLegendUI.AverageTimeLabel.get(), total);
    }

    const Color overlayColor = (m_settings->UI.ShowStats) ? Color::White() : Color(0x00FFFFFF);
    m_anim.OverlayColorStatsApp.SetValue(overlayColor);
    m_anim.OverlayColorStats.SetValue(m_settings->Test.ShowStats ? overlayColor : Color(0x00FFFFFF));
    m_anim.Update(TransitionTimeSpan(demoTime.ElapsedTime.Ticks()));

    m_uiProfile.StatsOverlay.MainLayout->SetBaseColor(m_anim.OverlayColorStatsApp.GetValue());
    m_uiProfile.StatsOverlay2.MainLayout->SetBaseColor(m_anim.OverlayColorStats.GetValue());

    assert(m_uiProfile.BottomSlidingPanel);
    m_uiProfile.BottomSlidingPanel->SetShow(m_settings->UI.ShowChart);

    m_testAppHost->AppUpdate(demoTime);

    // If we dont want to show the system UI idle/busy state we force the systemUIIdle to true
    const bool systemUIIdle = m_cachedState.WasAppUIIdle || !m_settings->Test.ShowIdle;
    const Color idleColor = DetermineIdleColor(m_cachedState.WasTestAppUIIdle, systemUIIdle);
    m_uiProfile.OptionsBar.ImageIdle->SetContentColor(idleColor);

    m_uiProfile.AppLayout->SetBaseColor(m_uiProfile.ActivityStack->GetDesiredParentColor());

    // const bool inTransition = m_uiProfile.BottomSlidingPanel->IsBusy() || m_anim.IsBusy();

    switch (m_inputState)
    {
    case InputState::SettingsSubActivity:
      UpdateSettingsSubActivityState();
      break;
    case InputState::FrameAnalysisSubActivity:
      UpdateFrameAnalysisSubActivityState();
      break;
    case InputState::BenchConfigSubActivity:
      UpdateBenchConfigSubActivityState();
      break;
    default:
      break;
    }
  }

  void PlaygroundScene::Draw(const DemoTime& demoTime)
  {
    m_testAppHost->AppDraw(demoTime);
  }


  void PlaygroundScene::OnDrawSkipped(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);
  }


  void PlaygroundScene::OnFrameSequenceEnd()
  {
    if (m_settings->UI.ShowChart)
    {
      const DemoPerformanceCapture& profiler = m_testAppHost->GetProfiler();

      int64_t valUIProcessEvents = profiler.GetResult(DemoPerformanceCaptureId::UIProcessEvents);
      int64_t valUIUpdate = profiler.GetResult(DemoPerformanceCaptureId::UIUpdate);
      int64_t valUIDraw = profiler.GetResult(DemoPerformanceCaptureId::UIDraw);
      int64_t valUIDrawPreprocess = profiler.GetResult(DemoPerformanceCaptureId::UIDrawPreprocessDrawCommands);
      int64_t valUIDrawGenMesh = profiler.GetResult(DemoPerformanceCaptureId::UIDrawGenerateMeshes);
      int64_t valUIDrawFillBuffers = profiler.GetResult(DemoPerformanceCaptureId::UIDrawUpdateBuffers);
      int64_t valUIDrawSchedule = profiler.GetResult(DemoPerformanceCaptureId::UIDrawScheduleDraw);
      // int64_t valRoundtrip = profiler.GetResult(DemoPerformanceCaptureId::Roundtrip);

      // valRoundtrip -= valUIProcessEvents;
      // valRoundtrip -= valUIUpdate;
      // valRoundtrip -= valUIDraw;

      valUIDraw -= valUIDrawPreprocess;
      valUIDraw -= valUIDrawGenMesh;
      valUIDraw -= valUIDrawFillBuffers;
      valUIDraw -= valUIDrawSchedule;

      UI::ChartComplexDataEntry entry;
      entry.Values[0] = static_cast<uint32_t>(MathHelper::Clamp(valUIDrawSchedule, int64_t(0), int64_t(0xFFFFFFFF)));
      entry.Values[1] = static_cast<uint32_t>(MathHelper::Clamp(valUIDrawFillBuffers, int64_t(0), int64_t(0xFFFFFFFF)));
      entry.Values[2] = static_cast<uint32_t>(MathHelper::Clamp(valUIDrawGenMesh, int64_t(0), int64_t(0xFFFFFFFF)));
      entry.Values[3] = static_cast<uint32_t>(MathHelper::Clamp(valUIDrawPreprocess, int64_t(0), int64_t(0xFFFFFFFF)));
      entry.Values[4] = static_cast<uint32_t>(MathHelper::Clamp(valUIDraw, int64_t(0), int64_t(0xFFFFFFFF)));
      entry.Values[5] = static_cast<uint32_t>(MathHelper::Clamp(valUIUpdate, int64_t(0), int64_t(0xFFFFFFFF)));
      entry.Values[6] = static_cast<uint32_t>(MathHelper::Clamp(valUIProcessEvents, int64_t(0), int64_t(0xFFFFFFFF)));

      m_data->Append(entry);
      m_dataAverage.Append(entry);
    }

    // Check what the app state is at frame end
    m_cachedState.WasTestAppUIIdle = m_testAppHost->IsUIIdle();
    m_cachedState.WasAppUIIdle = m_uiExtension->IsIdle();
  }


  void PlaygroundScene::UpdateSettingsSubActivityState()
  {
    assert(m_inputState == InputState::SettingsSubActivity);

    // Simple little work around for not having proper activities, so we basically do a form of 'busy' wait.
    if (m_uiProfile.ActivityStack->IsEmpty())
    {
      assert(m_settingsResult);
      const bool restartApp = m_settings->Test.NoOpaqueMaterials != m_settingsResult->NoOpaqueMaterials;
      m_settings->Test = *m_settingsResult;
      m_settingsResult.reset();
      m_inputState = InputState::Playground;

      if (restartApp)
      {
        RestartTestApp(true);
      }
    }
  }


  void PlaygroundScene::UpdateFrameAnalysisSubActivityState()
  {
    assert(m_inputState == InputState::FrameAnalysisSubActivity);
    // Simple little work around for not having proper activities, so we basically do a form of 'busy' wait.

    if (m_uiProfile.ActivityStack->IsEmpty())
    {
      m_frameAnalysisDialog.reset();
      FrameAnalysisSetMaxDrawCalls(0xFFFFFFFF);
      m_inputState = InputState::Playground;
    }
  }


  void PlaygroundScene::UpdateBenchConfigSubActivityState()
  {
    assert(m_inputState == InputState::BenchConfigSubActivity);

    // Simple little work around for not having proper activities, so we basically do a form of 'busy' wait.
    if (m_uiProfile.ActivityStack->IsEmpty())
    {
      assert(m_benchResult);
      if (!m_configDialogPromise.valid() || !m_configDialogPromise.get())
      {
        m_inputState = InputState::Playground;
      }
      else
      {
        m_settings->Bench = *m_benchResult;
        m_inputState = InputState::Playground;
        BeginClose(SceneId::Benchmark, std::make_shared<BenchSceneConfig>(m_settings->Bench));
      }
      m_benchResult.reset();
    }
  }


  void PlaygroundScene::SetDefaultValues()
  {
  }


  void PlaygroundScene::ClearGraph()
  {
    m_data->Clear();
    m_dataAverage.Clear();
  }


  void PlaygroundScene::RestoreUISettings(const RenderMethodInfo& renderMethodInfo)
  {
    m_uiProfile.OptionsBar.RenderOptions.RestoreUISettings(renderMethodInfo.SystemInfo);
    if (m_uiProfile.OptionsBar.ButtonFrameAnalysis)
    {
      const bool frameAnalysisEnabled = (renderMethodInfo.RenderMethod == AppRenderMethod::FlexImBatch);
      m_uiProfile.OptionsBar.ButtonFrameAnalysis->SetEnabled(frameAnalysisEnabled);
    }
  }


  uint32_t PlaygroundScene::FrameAnalysisGetCurrentMaxDrawCalls() const
  {
    auto* pRenderSystem = m_testAppHost->TryGetRenderSystem();
    if (pRenderSystem == nullptr)
    {
      return 0;
    }
    const UI::RenderSystemStats stats = pRenderSystem->GetStats();
    return stats.DrawCalls + stats.DrawIndexCalls;
  }


  void PlaygroundScene::FrameAnalysisSetMaxDrawCalls(const uint32_t maxDrawCalls)
  {
    auto* pFlexConfig = dynamic_cast<UI::RenderIMBatch::IFlexRenderSystemConfig*>(m_testAppHost->TryGetRenderSystem());
    if (pFlexConfig != nullptr)
    {
      pFlexConfig->SetMaxDrawCalls(maxDrawCalls);
    }
  }


  void PlaygroundScene::UpdateStats(StatsOverlayUI& overlay, const UI::RenderSystemStats& stats)
  {
    overlay.Entries[0]->SetContent(stats.MeshCount);
    overlay.Entries[1]->SetContent(stats.BatchCount);
    overlay.Entries[2]->SetContent(stats.VertexBufferCount);
    overlay.Entries[3]->SetContent(stats.IndexBufferCount);
    overlay.Entries[4]->SetContent(stats.VertexCount);
    overlay.Entries[5]->SetContent(stats.IndexCount);
    overlay.Entries[6]->SetContent(stats.DrawCalls);
    overlay.Entries[7]->SetContent(stats.DrawIndexCalls);
  }


  void PlaygroundScene::RestartTestApp(const bool storeSettingsBeforeRestart)
  {
    if (storeSettingsBeforeRestart)
    {
      // Store the current settings for the render (so its the same when we switch back)
      m_uiProfile.OptionsBar.RenderOptions.StoreUISettings(m_renderRecords[m_activeRenderIndex].SystemInfo);
    }

    const UI::RenderSystemInfo& systemInfo = m_renderRecords[m_activeRenderIndex].SystemInfo;
    const bool depthBuffer = UI::RenderOptionFlagsUtil::IsEnabled(systemInfo.Settings, UI::RenderOptionFlags::DepthBuffer);
    const bool useSdf = m_uiProfile.OptionsBar.SwitchSdfFont->IsChecked();

    {    // restart the test app with the new render code
      m_testAppHost->StopTestApp();
      TestAppFactory appFactory(m_renderRecords[m_activeRenderIndex].RenderMethod);

      UIDemoAppMaterialCreateInfo materialCreateInfo(m_settings->Test.NoOpaqueMaterials, true);
      UIDemoAppMaterialConfig materialConfig(useSdf, depthBuffer);
      m_testAppHost->StartTestApp(appFactory, materialCreateInfo, materialConfig);
    }
    RenderSystemRuntimeSettingsUtil::Apply(*m_testAppHost, systemInfo.Settings, useSdf);
  }


  PlaygroundScene::ProfileUI PlaygroundScene::CreateProfileUI(UI::Theme::IThemeControlFactory& uiFactory, const uint16_t currentDensityDpi,
                                                              const std::shared_ptr<UI::AreaChartData>& data,
                                                              const ReadOnlySpan<RenderMethodInfo> renderRecordSpan, const uint32_t activeRenderIndex,
                                                              const AppUISettings& settings)
  {
    auto context = uiFactory.GetContext();
    auto contentArea = std::make_shared<UI::BaseWindow>(context);
    contentArea->SetAlignmentX(UI::ItemAlignment::Stretch);
    contentArea->SetAlignmentY(UI::ItemAlignment::Stretch);

    auto optionBarUI = CreateOptionBarUI(uiFactory, context, currentDensityDpi, renderRecordSpan, activeRenderIndex, settings);

    auto optionBar = uiFactory.CreateRightBar(optionBarUI.Layout);

    auto bottomBar = CreateBottomBar(uiFactory, context, data);

    auto overlay = CreateStatsOverlayUI(uiFactory, context);
    auto overlay2 = CreateStatsOverlayUI(uiFactory, context);

    auto overlayStack = std::make_shared<UI::UniformStackLayout>(context);
    overlayStack->SetAlignmentX(UI::ItemAlignment::Far);
    overlayStack->SetSpacing(4);
    overlayStack->SetLayoutOrientation(UI::LayoutOrientation::Vertical);
    overlayStack->AddChild(overlay.MainLayout);
    overlayStack->AddChild(overlay2.MainLayout);

    auto layout = std::make_shared<UI::GridLayout>(context);
    layout->SetAlignmentX(UI::ItemAlignment::Stretch);
    layout->SetAlignmentY(UI::ItemAlignment::Stretch);
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
    layout->AddChild(contentArea, 0, 0);
    layout->AddChild(overlayStack, 0, 0);
    // layout->AddChild(optionBar, 1, 0);
    layout->AddChild(optionBar, 1, 0);

    auto bottomSlidingPanel = std::make_shared<UI::SlidingPanel>(context);
    bottomSlidingPanel->SetDirection(UI::SlideDirection::Down);
    bottomSlidingPanel->SetContent(bottomBar.Main);
    bottomSlidingPanel->SetShow(settings.ShowChart);
    bottomSlidingPanel->FinishAnimation();

    auto uiMainLayout = std::make_shared<UI::ComplexStackLayout>(context);
    uiMainLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
    uiMainLayout->SetAlignmentY(UI::ItemAlignment::Stretch);
    uiMainLayout->SetLayoutOrientation(UI::LayoutOrientation::Vertical);

    uiMainLayout->AddChild(layout, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));
    uiMainLayout->AddChild(bottomSlidingPanel, UI::LayoutLength(UI::LayoutUnitType::Auto));

    auto fillLayout = std::make_shared<UI::FillLayout>(context);
    fillLayout->AddChild(uiMainLayout);

    auto activityStack = std::make_shared<UI::ActivityStack>(context);
    fillLayout->AddChild(activityStack);

    return {bottomSlidingPanel, bottomBar, overlay, overlay2, optionBarUI, fillLayout, uiMainLayout, activityStack, contentArea};
  }


  PlaygroundScene::OptionBarUI PlaygroundScene::CreateOptionBarUI(UI::Theme::IThemeControlFactory& uiFactory,
                                                                  const std::shared_ptr<UI::WindowContext>& context, const uint16_t currentDensityDpi,
                                                                  const ReadOnlySpan<RenderMethodInfo> renderRecordSpan,
                                                                  const uint32_t activeRenderIndex, const AppUISettings& settings)
  {
    auto radioGroup = uiFactory.CreateRadioGroup("Render");

    RenderOptionControls renderOptions = RenderOptionControlsFactory::CreateRenderMethodControls(uiFactory);

    auto switchOnDemand = uiFactory.CreateSwitch(TextConfig::OnDemandRendering, false);
    auto switchSdfFont = uiFactory.CreateSwitch(LocalStrings::SdfFont, false);

    auto switchButtons = CreateUISwitchButtons(uiFactory, context, settings);
    auto switchButtonLayout = std::make_shared<UI::ComplexStackLayout>(context);
    switchButtonLayout->SetLayoutOrientation(UI::LayoutOrientation::Horizontal);
    switchButtonLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
    switchButtonLayout->AddChild(switchButtons.SwitchStats, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));
    switchButtonLayout->AddChild(switchButtons.SwitchChart, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));

    auto layout = std::make_shared<UI::ComplexStackLayout>(context);
    layout->SetLayoutOrientation(UI::LayoutOrientation::Vertical);
    layout->SetAlignmentX(UI::ItemAlignment::Stretch);
    layout->SetAlignmentY(UI::ItemAlignment::Stretch);

    layout->AddChild(switchButtonLayout, UI::LayoutLength(UI::LayoutUnitType::Auto));
    layout->AddChild(uiFactory.CreateDivider(UI::LayoutOrientation::Horizontal), UI::LayoutLength(UI::LayoutUnitType::Auto));
    layout->AddChild(uiFactory.CreateLabel(TextConfig::HeaderRenderMethod, UI::Theme::FontType::Header), UI::LayoutLength(UI::LayoutUnitType::Auto));

    RenderMethodUI renderMethodUI;
    for (uint32_t i = 0; i < renderRecordSpan.size(); ++i)
    {
      auto radioButton = uiFactory.CreateRadioButton(radioGroup, renderRecordSpan[i].Name, i == activeRenderIndex);
      layout->AddChild(radioButton, UI::LayoutLength(UI::LayoutUnitType::Auto));
      renderMethodUI.Methods.push_back(radioButton);
    }

    auto switchDpi = uiFactory.CreateSwitch(LocalStrings::EmulateDpi);
    auto sliderDpi = uiFactory.CreateSliderFmtValue(UI::LayoutOrientation::Horizontal,
                                                    ConstrainedValue<int32_t>(currentDensityDpi, LocalConfig::DpiMin, LocalConfig::DpiMax));
    if (!switchDpi->IsChecked())
    {
      sliderDpi->SetEnabled(false);
      sliderDpi->FinishAnimation();
    }

    auto labelIdle = uiFactory.CreateLabel("UI idle");
    labelIdle->SetAlignmentY(UI::ItemAlignment::Center);
    auto imageIdle = uiFactory.CreateImage(uiFactory.GetResources().GetColorMarkerNineSliceSprite());
    imageIdle->SetAlignmentX(UI::ItemAlignment::Center);
    imageIdle->SetAlignmentY(UI::ItemAlignment::Center);
    imageIdle->SetContentColor(IdleColor::Busy);
    imageIdle->FinishAnimation();
    auto buttonConfig = uiFactory.CreateTextButton(UI::Theme::ButtonType::Text, "Settings");
    buttonConfig->SetAlignmentX(UI::ItemAlignment::Far);
    buttonConfig->SetAlignmentY(UI::ItemAlignment::Center);

    auto buttonFrameAnalysis = uiFactory.CreateTextButton(UI::Theme::ButtonType::Outlined, "Frame analysis");
    buttonFrameAnalysis->SetAlignmentX(UI::ItemAlignment::Stretch);

    auto buttonRecord = uiFactory.CreateTextButton(UI::Theme::ButtonType::Outlined, "Record");
    buttonRecord->SetAlignmentX(UI::ItemAlignment::Stretch);
    auto buttonBench = uiFactory.CreateTextButton(UI::Theme::ButtonType::Contained, "Bench");
    buttonBench->SetAlignmentX(UI::ItemAlignment::Stretch);

    auto buttonStack = std::make_shared<UI::UniformStackLayout>(context);
    buttonStack->SetLayoutOrientation(UI::LayoutOrientation::Horizontal);
    buttonStack->SetAlignmentX(UI::ItemAlignment::Center);
    buttonStack->AddChild(buttonRecord);
    buttonStack->AddChild(buttonBench);

    auto infoStack0 = std::make_shared<UI::StackLayout>(context);
    infoStack0->SetSpacing(CustomUIConfig::FixedSpacingDp.Value());
    infoStack0->SetLayoutOrientation(UI::LayoutOrientation::Horizontal);
    infoStack0->SetAlignmentX(UI::ItemAlignment::Center);
    infoStack0->SetAlignmentY(UI::ItemAlignment::Center);
    infoStack0->AddChild(imageIdle);
    infoStack0->AddChild(labelIdle);

    auto infoStack = std::make_shared<UI::ComplexStackLayout>(context);
    infoStack->SetLayoutOrientation(UI::LayoutOrientation::Horizontal);
    infoStack->SetAlignmentX(UI::ItemAlignment::Stretch);
    infoStack->SetSpacing(CustomUIConfig::FixedSpacingDp.Value());
    infoStack->AddChild(infoStack0, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));
    infoStack->AddChild(buttonConfig, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));

    auto lastRow = std::make_shared<UI::StackLayout>(context);
    lastRow->SetLayoutOrientation(UI::LayoutOrientation::Vertical);
    lastRow->SetAlignmentX(UI::ItemAlignment::Stretch);
    lastRow->SetAlignmentY(UI::ItemAlignment::Far);
    lastRow->AddChild(infoStack);
    lastRow->AddChild(buttonFrameAnalysis);
    lastRow->AddChild(buttonStack);

    layout->AddChild(uiFactory.CreateDivider(UI::LayoutOrientation::Horizontal), UI::LayoutLength(UI::LayoutUnitType::Auto));
    layout->AddChild(uiFactory.CreateLabel(TextConfig::HeaderRenderOptions, UI::Theme::FontType::Header), UI::LayoutLength(UI::LayoutUnitType::Auto));
    layout->AddChild(renderOptions.SwitchFillBuffers, UI::LayoutLength(UI::LayoutUnitType::Auto));
    layout->AddChild(renderOptions.SwitchBatch, UI::LayoutLength(UI::LayoutUnitType::Auto));
    layout->AddChild(renderOptions.SwitchDrawReorder, UI::LayoutLength(UI::LayoutUnitType::Auto));
    layout->AddChild(renderOptions.SwitchDepthBuffer, UI::LayoutLength(UI::LayoutUnitType::Auto));
    layout->AddChild(renderOptions.SwitchMeshCaching, UI::LayoutLength(UI::LayoutUnitType::Auto));
    layout->AddChild(uiFactory.CreateDivider(UI::LayoutOrientation::Horizontal), UI::LayoutLength(UI::LayoutUnitType::Auto));
    layout->AddChild(uiFactory.CreateLabel(TextConfig::HeaderOptions, UI::Theme::FontType::Header), UI::LayoutLength(UI::LayoutUnitType::Auto));
    layout->AddChild(switchOnDemand, UI::LayoutLength(UI::LayoutUnitType::Auto));
    layout->AddChild(switchSdfFont, UI::LayoutLength(UI::LayoutUnitType::Auto));
    layout->AddChild(switchDpi, UI::LayoutLength(UI::LayoutUnitType::Auto));
    layout->AddChild(sliderDpi, UI::LayoutLength(UI::LayoutUnitType::Auto));
    layout->AddChild(lastRow, UI::LayoutLength(UI::LayoutUnitType::Star));

    return {layout,    switchButtons, renderMethodUI,      renderOptions, switchOnDemand, switchSdfFont, switchDpi,
            sliderDpi, buttonConfig,  buttonFrameAnalysis, buttonRecord,  buttonBench,    imageIdle};
  }

  PlaygroundScene::UISwitchButtons PlaygroundScene::CreateUISwitchButtons(UI::Theme::IThemeControlFactory& uiFactory,
                                                                          const std::shared_ptr<UI::WindowContext>& context,
                                                                          const AppUISettings& settings)
  {
    FSL_PARAM_NOT_USED(context);
    auto stats = uiFactory.CreateSwitch("Stats", settings.ShowStats);
    stats->SetAlignmentX(UI::ItemAlignment::Near);
    auto chart = uiFactory.CreateSwitch("Chart", settings.ShowChart);
    chart->SetAlignmentX(UI::ItemAlignment::Far);
    chart->FinishAnimation();
    return {stats, chart};
  }


  PlaygroundScene::BottomBarUI PlaygroundScene::CreateBottomBar(UI::Theme::IThemeControlFactory& uiFactory,
                                                                const std::shared_ptr<UI::WindowContext>& context,
                                                                const std::shared_ptr<UI::AreaChartData>& data)
  {
    auto cpuLegend = CustomControlFactory::CreateDetailedCpuLegend(uiFactory, UI::DpLayoutSize1D(CustomUIConfig::FixedSpacingDp.Value()));
    auto cpuTimeChart = CustomControlFactory::CreateAreaChart(uiFactory, data);

    auto buttonClearTimeChart = uiFactory.CreateTextButton(UI::Theme::ButtonType::Outlined, "Clear graph");
    buttonClearTimeChart->SetAlignmentX(UI::ItemAlignment::Center);
    buttonClearTimeChart->SetAlignmentY(UI::ItemAlignment::Center);

    auto rightStack = std::make_shared<UI::ComplexStackLayout>(context);
    rightStack->SetAlignmentY(UI::ItemAlignment::Stretch);
    rightStack->SetLayoutOrientation(UI::LayoutOrientation::Vertical);
    rightStack->AddChild(buttonClearTimeChart, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));


    auto marginLeftDp = uiFactory.GetDefaultMarginDp(UI::Theme::ElementType::LeftBar);
    auto marginRightDp = uiFactory.GetDefaultMarginDp(UI::Theme::ElementType::RightBar);

    auto profileLayout = std::make_shared<UI::GridLayout>(context);
    profileLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
    profileLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, marginLeftDp.Left()));
    profileLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    profileLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, CustomUIConfig::FixedSpacingDp.Value()));
    profileLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
    profileLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, CustomUIConfig::FixedSpacingDp.Value()));
    profileLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    profileLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, marginRightDp.Right()));
    profileLayout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    profileLayout->AddChild(cpuLegend.Main, 1, 0);
    profileLayout->AddChild(cpuTimeChart, 3, 0);
    profileLayout->AddChild(rightStack, 5, 0);
    return {true, uiFactory.CreateBottomBar(profileLayout), cpuLegend, buttonClearTimeChart};
  }

  PlaygroundScene::StatsOverlayUI PlaygroundScene::CreateStatsOverlayUI(UI::Theme::IThemeControlFactory& uiFactory,
                                                                        const std::shared_ptr<UI::WindowContext>& context)
  {
    StatsOverlayUI overlay;

    auto lblDesc0 = uiFactory.CreateLabel(LocalStrings::Meshes);
    auto lblDesc1 = uiFactory.CreateLabel(LocalStrings::Batches);
    auto lblDesc2 = uiFactory.CreateLabel(LocalStrings::VBs);
    auto lblDesc3 = uiFactory.CreateLabel(LocalStrings::IBs);
    auto lblDesc4 = uiFactory.CreateLabel(LocalStrings::Vertices);
    auto lblDesc5 = uiFactory.CreateLabel(LocalStrings::Indices);
    auto lblDesc6 = uiFactory.CreateLabel(LocalStrings::Draw);
    auto lblDesc7 = uiFactory.CreateLabel(LocalStrings::DrawIndexed);

    auto lbl0 = uiFactory.CreateFmtValueLabel(uint32_t(0));
    auto lbl1 = uiFactory.CreateFmtValueLabel(uint32_t(0));
    auto lbl2 = uiFactory.CreateFmtValueLabel(uint32_t(0));
    auto lbl3 = uiFactory.CreateFmtValueLabel(uint32_t(0));
    auto lbl4 = uiFactory.CreateFmtValueLabel(uint32_t(0));
    auto lbl5 = uiFactory.CreateFmtValueLabel(uint32_t(0));
    auto lbl6 = uiFactory.CreateFmtValueLabel(uint32_t(0));
    auto lbl7 = uiFactory.CreateFmtValueLabel(uint32_t(0));
    lbl0->SetAlignmentX(UI::ItemAlignment::Far);
    lbl1->SetAlignmentX(UI::ItemAlignment::Far);
    lbl2->SetAlignmentX(UI::ItemAlignment::Far);
    lbl3->SetAlignmentX(UI::ItemAlignment::Far);
    lbl4->SetAlignmentX(UI::ItemAlignment::Far);
    lbl5->SetAlignmentX(UI::ItemAlignment::Far);
    lbl6->SetAlignmentX(UI::ItemAlignment::Far);
    lbl7->SetAlignmentX(UI::ItemAlignment::Far);

    overlay.Entries[0] = lbl0;
    overlay.Entries[1] = lbl1;
    overlay.Entries[2] = lbl2;
    overlay.Entries[3] = lbl3;
    overlay.Entries[4] = lbl4;
    overlay.Entries[5] = lbl5;
    overlay.Entries[6] = lbl6;
    overlay.Entries[7] = lbl7;

    auto layout = std::make_shared<UI::GridLayout>(context);
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, 70.0f));

    layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));

    layout->AddChild(lblDesc0, 0, 0);
    layout->AddChild(lblDesc1, 0, 1);
    layout->AddChild(lblDesc2, 0, 2);
    layout->AddChild(lblDesc3, 0, 3);
    layout->AddChild(lblDesc4, 0, 4);
    layout->AddChild(lblDesc5, 0, 5);
    layout->AddChild(lblDesc6, 0, 6);
    layout->AddChild(lblDesc7, 0, 7);

    layout->AddChild(overlay.Entries[0], 1, 0);
    layout->AddChild(overlay.Entries[1], 1, 1);
    layout->AddChild(overlay.Entries[2], 1, 2);
    layout->AddChild(overlay.Entries[3], 1, 3);
    layout->AddChild(overlay.Entries[4], 1, 4);
    layout->AddChild(overlay.Entries[5], 1, 5);
    layout->AddChild(overlay.Entries[6], 1, 6);
    layout->AddChild(overlay.Entries[7], 1, 7);

    overlay.MainLayout = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Transparent, layout);
    overlay.MainLayout->SetAlignmentX(UI::ItemAlignment::Far);
    return overlay;
  }

  void PlaygroundScene::SetDpi(const uint16_t densityDpi)
  {
    if (!m_uiProfile.OptionsBar.SwitchEmulateDpi)
    {
      return;
    }
    assert(m_uiProfile.OptionsBar.SliderEmulatedDpi);
    const bool emulateDpiEnabled = m_uiProfile.OptionsBar.SwitchEmulateDpi->IsChecked();
    if (!emulateDpiEnabled)
    {
      m_uiProfile.OptionsBar.SliderEmulatedDpi->SetValue(densityDpi);
    }
    const auto emulatedDpiValue = NumericCast<uint16_t>(m_uiProfile.OptionsBar.SliderEmulatedDpi->GetValue());
    m_testAppHost->SetEmulatedDpi(emulateDpiEnabled, emulatedDpiValue);
  }

  void PlaygroundScene::BeginClose(const SceneId nextSceneId, std::shared_ptr<ISceneConfig> sceneConfig)
  {
    if (m_inputState == InputState::Playground)
    {
      m_inputState = InputState::Closing;
      m_nextSceneRecord = NextSceneRecord(nextSceneId, std::move(sceneConfig));
    }
  }
}
