//
// Created on 2024/11/19.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "Camera.h"

#include "hilog/log.h"
#include "ohcamera/camera.h"
#include "ohcamera/camera_input.h"
#include "ohcamera/capture_session.h"
#include "ohcamera/photo_output.h"
#include "ohcamera/preview_output.h"
#include "ohcamera/video_output.h"
#include "ohcamera/camera_manager.h"


#undef LOG_DOMAIN
#undef LOG_TAG
#define LOG_DOMAIN 0x3200  // 全局domain宏，标识业务领域
#define LOG_TAG "camera-test"   // 全局tag宏，标识模块日志tag


Camera_Manager *cameraManager = nullptr;
Camera_Input* cameraInput = nullptr;
Camera_Device* cameras = nullptr;

static void CameraManagerStatusCallback(Camera_Manager* cameraManager, Camera_StatusInfo* status)
{
   OH_LOG_INFO(LOG_APP, "CameraManagerStatusCallback is called");
}

static CameraManager_Callbacks* GetCameraManagerListener()
{
   static CameraManager_Callbacks cameraManagerListener = {
      .onCameraStatus = CameraManagerStatusCallback
   };
   return &cameraManagerListener;
}

bool getCameras()
{
    uint32_t size = 0;
    // 创建CameraManager对象
    Camera_ErrorCode ret = OH_Camera_GetCameraManager(&cameraManager);
    if (cameraManager == nullptr || ret != CAMERA_OK) {
       OH_LOG_ERROR(LOG_APP, "OH_Camera_GetCameraManager failed.");
       return false;
    }
    
    // 获取相机列表
    ret = OH_CameraManager_GetSupportedCameras(cameraManager, &cameras, &size);
    if (cameras == nullptr || size < 0 || ret != CAMERA_OK) {
       OH_LOG_ERROR(LOG_APP, "OH_CameraManager_GetSupportedCameras failed.");
    }
    for (int index = 0; index < size; index++) {
       OH_LOG_INFO(LOG_APP, "cameraId  =  %{public}s ", cameras[index].cameraId);              // 获取相机ID
       OH_LOG_INFO(LOG_APP, "cameraPosition  =  %{public}d ", cameras[index].cameraPosition);  // 获取相机位置
       OH_LOG_INFO(LOG_APP, "cameraType  =  %{public}d ", cameras[index].cameraType);          // 获取相机类型
       OH_LOG_INFO(LOG_APP, "connectionType  =  %{public}d ", cameras[index].connectionType);  // 获取相机连接类型
    }
    
    ret = OH_CameraManager_RegisterCallback(cameraManager, GetCameraManagerListener());
    if (ret != CAMERA_OK) {
        OH_LOG_ERROR(LOG_APP, "OH_CameraManager_RegisterCallback failed.");
        return false;
    }
    
    return true;
}
