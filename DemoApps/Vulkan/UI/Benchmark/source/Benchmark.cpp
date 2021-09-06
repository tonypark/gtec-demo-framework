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

#include "Benchmark.hpp"
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <RapidVulkan/Check.hpp>
#include <vulkan/vulkan.h>
#include "BasicGpuProfiler.hpp"

namespace Fsl
{
  namespace
  {
    VulkanBasic::DemoAppVulkanSetup GetVulkanSetup()
    {
      VulkanBasic::DemoAppVulkanSetup setup;
      setup.DepthBuffer = VulkanBasic::DepthBufferMode::Enabled;
      return setup;
    }

    std::shared_ptr<BasicGpuProfiler> TryCreateGpuProfiler(const Vulkan::VUDevice& device)
    {
      return BasicGpuProfiler::IsTimestampSupported(device.GetPhysicalDevice().Properties)
               ? std::make_shared<BasicGpuProfiler>(device.GetPhysicalDevice().Properties, device.Get())
               : std::shared_ptr<BasicGpuProfiler>();
    }
  }


  Benchmark::Benchmark(const DemoAppConfig& config)
    : VulkanBasic::DemoAppVulkanBasic(config, GetVulkanSetup())
    , m_gpuProfiler(TryCreateGpuProfiler(m_device))
    , m_shared(config, m_gpuProfiler)
  {
    // Give the UI a chance to intercept the various DemoApp events.
    RegisterExtension(m_shared.GetDemoAppExtension());
  }


  void Benchmark::OnFrameSequenceBegin()
  {
    if (m_gpuProfiler)
    {
      m_gpuProfiler->ExtractResult();
    }
    m_shared.OnFrameSequenceBegin();
  }


  void Benchmark::OnKeyEvent(const KeyEvent& event)
  {
    m_shared.OnKeyEvent(event);
  }


  void Benchmark::ConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    VulkanBasic::DemoAppVulkanBasic::ConfigurationChanged(windowMetrics);
    m_shared.OnConfigurationChanged(windowMetrics);
  }


  void Benchmark::Update(const DemoTime& demoTime)
  {
    m_shared.Update(demoTime);
  }


  void Benchmark::VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext)
  {
    const uint32_t currentFrameIndex = drawContext.CurrentFrameIndex;

    const VkCommandBuffer hCmdBuffer = rCmdBuffers[currentFrameIndex];

    rCmdBuffers.Begin(currentFrameIndex, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
    {
      if (m_gpuProfiler)
      {
        m_gpuProfiler->BeginDraw(hCmdBuffer);
      }

      std::array<VkClearValue, 2> clearValues{};
      clearValues[0].color = {{0.5f, 0.5f, 0.5f, 1.0f}};
      clearValues[1].depthStencil = {1.0f, 0};

      VkRenderPassBeginInfo renderPassBeginInfo{};
      renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      renderPassBeginInfo.renderPass = m_dependentResources.MainRenderPass.Get();
      renderPassBeginInfo.framebuffer = drawContext.Framebuffer;
      renderPassBeginInfo.renderArea.offset.x = 0;
      renderPassBeginInfo.renderArea.offset.y = 0;
      renderPassBeginInfo.renderArea.extent = drawContext.SwapchainImageExtent;
      renderPassBeginInfo.clearValueCount = UncheckedNumericCast<uint32_t>(clearValues.size());
      renderPassBeginInfo.pClearValues = clearValues.data();

      rCmdBuffers.CmdBeginRenderPass(currentFrameIndex, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
      {
        m_shared.Draw(demoTime);

        // Remember to call this as the last operation in your renderPass
        AddSystemUI(hCmdBuffer, currentFrameIndex);
      }
      rCmdBuffers.CmdEndRenderPass(currentFrameIndex);

      if (m_gpuProfiler)
      {
        m_gpuProfiler->EndDraw();
      }
    }
    rCmdBuffers.End(currentFrameIndex);
  }

  void Benchmark::OnFrameSequenceEnd()
  {
    m_shared.OnFrameSequenceEnd();
  }


  VkRenderPass Benchmark::OnBuildResources(const VulkanBasic::BuildResourcesContext& context)
  {
    FSL_PARAM_NOT_USED(context);
    m_dependentResources.MainRenderPass = CreateBasicRenderPass();
    return m_dependentResources.MainRenderPass.Get();
  }


  void Benchmark::OnFreeResources()
  {
    m_dependentResources = {};
  }
}
