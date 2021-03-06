#!/bin/bash
set -o errexit

# Prepare the sdk
source /opt/fsl-imx-internal-xwayland/5.10-hardknott/environment-setup-cortexa35-poky-linux

if [ ! -n "${FSL_CI_FEATURES+1}" ]; then
  # all
  # export FSL_CI_FEATURES=[EarlyAccess,EGL,G2D,GoogleUnitTest,HW_CAMERA_MIPI8X,Lib_NlohmannJson,OpenCL1.2,OpenCV4,OpenGLES2,OpenGLES3,OpenGLES3.1,OpenGLES3.2,OpenVG,OpenVX1.1,Vulkan1.2]
  # No GLES3.2, Lib_NlohmannJson, Vulkan, Vulkan1.2
  export FSL_CI_FEATURES=[EarlyAccess,EGL,G2D,GoogleUnitTest,HW_CAMERA_MIPI8X,OpenCL1.2,OpenCV4,OpenGLES2,OpenGLES3,OpenGLES3.1,OpenVG,OpenVX1.1]
fi

source $WORKSPACE/.Config/Jenkins/yocto/PrepareJenkinsEnvironment.sh

FslBuild.py --debug -t sdk -vv --BuildTime --UseFeatures $FSL_CI_FEATURES --Variants [WindowSystem=X11] -c install --CMakeInstallPrefix bin
