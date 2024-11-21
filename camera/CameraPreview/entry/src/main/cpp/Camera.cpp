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


static Camera_Manager *cameraManager = nullptr;
static Camera_Input* cameraInput = nullptr;
static Camera_Device* cameras = nullptr;
static Camera_SceneMode* sceneModes = nullptr;

static Camera_OutputCapability* cameraOutputCapability = nullptr;
static Camera_Profile* previewProfile = nullptr;
static Camera_Profile* photoProfile = nullptr;

static Camera_PreviewOutput* previewOutput = nullptr;
static Camera_PhotoOutput* photoOutput = nullptr;
static Camera_VideoOutput* videoOutput = nullptr;
static Camera_CaptureSession* captureSession = nullptr;

static int cameraDeviceIndex = -1;
static int sceneModeIndex = -1;

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

// 监听cameraInput错误信息
static void OnCameraInputError(const Camera_Input* cameraInput, Camera_ErrorCode errorCode)
{
   OH_LOG_INFO(LOG_APP, "OnCameraInput errorCode: %{public}d", errorCode);
}

static CameraInput_Callbacks* GetCameraInputListener(void)
{
   static CameraInput_Callbacks cameraInputCallbacks = {
      .onError = OnCameraInputError
   };
   return &cameraInputCallbacks;
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
        return false;
    }
    
    cameraDeviceIndex = 0;
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

bool createCameraInput()
{
    // 创建相机输入流
    Camera_ErrorCode ret = OH_CameraManager_CreateCameraInput(cameraManager, &cameras[cameraDeviceIndex], &cameraInput);
    if (cameraInput == nullptr || ret != CAMERA_OK) {
        OH_LOG_ERROR(LOG_APP, "OH_CameraManager_CreateCameraInput failed.");
        return false;
    }
    OH_LOG_INFO(LOG_APP, "OH_CameraManager_CreateCameraInput success.");
    
    ret == OH_CameraInput_RegisterCallback(cameraInput, GetCameraInputListener());
    if (ret != CAMERA_OK) {
        OH_LOG_ERROR(LOG_APP, "OH_CameraInput_RegisterCallback failed.");
        return false;
    }
    
    // 打开相机
    ret = OH_CameraInput_Open(cameraInput);
    if (ret != CAMERA_OK) {
        OH_LOG_ERROR(LOG_APP, "OH_CameraInput_open failed.");
        return false;
    }
    OH_LOG_INFO(LOG_APP, "OH_CameraInput_open success.");
    
    return true;
}

bool getSupportedSceneModes()
{
    uint32_t length = 0;
    Camera_ErrorCode ret = OH_CameraManager_GetSupportedSceneModes(&cameras[cameraDeviceIndex], &sceneModes, &length);
    if (sceneModes == nullptr || ret != CAMERA_OK) {
        OH_LOG_ERROR(LOG_APP, "OH_CameraManager_GetSupportedSceneModes failed.");
        return false;
    }
    
    if (length > 0) {
        sceneModeIndex = sceneModes[0];
    }
    for (int index = 0; index < length; index++) {
        OH_LOG_INFO(LOG_APP, "scene mode = %{public}d", sceneModes[index]);    // 获取相机指定模式
    }
    
    return true;
}

bool getSupportedOutputCapability()
{
    // 获取相机设备支持的输出流能力
    Camera_ErrorCode ret = OH_CameraManager_GetSupportedCameraOutputCapabilityWithSceneMode(cameraManager, 
                                                                                            &cameras[cameraDeviceIndex], 
                                                                                            sceneModes[sceneModeIndex],
                                                                                            &cameraOutputCapability);
    if (cameraOutputCapability == nullptr || ret != CAMERA_OK) {
        OH_LOG_ERROR(LOG_APP, "OH_CameraManager_GetSupportedCameraOutputCapability failed.");
        return false;
    }
    OH_LOG_INFO(LOG_APP, "OH_CameraManager_GetSupportedCameraOutputCapability success");
    
    // 以NORMAL_PHOTO模式为例，需要添加预览流、拍照流
    if (cameraOutputCapability->previewProfilesSize < 0) {
        OH_LOG_ERROR(LOG_APP, "previewProfilesSize == null");
    } else {
        previewProfile = cameraOutputCapability->previewProfiles[0];
    }
    
    if (cameraOutputCapability->photoProfilesSize < 0) {
        OH_LOG_ERROR(LOG_APP, "photoProfilesSize == null");
    } else {
        photoProfile = cameraOutputCapability->photoProfiles[0];
    }
    
    if (previewProfile != nullptr && photoProfile != nullptr) {
        OH_LOG_INFO(LOG_APP, "previewProfile %{public}d, photoProfile %{public}d", previewProfile->format, photoProfile->format);
    }
    
    return true;
}

bool createPreviewOutput(const char* surfaceId)
{
    Camera_ErrorCode ret = OH_CameraManager_CreatePreviewOutput(cameraManager, previewProfile, surfaceId, &previewOutput);
    if (previewProfile == nullptr || previewOutput == nullptr || ret != CAMERA_OK) {
        OH_LOG_ERROR(LOG_APP, "OH_CameraManager_CreatePreviewOutput failed, ret %{public}d", ret);
        return false;
    }
    OH_LOG_INFO(LOG_APP, "OH_CameraManager_CreatePreviewOutput success");
    return true;
}

bool createCameraSession()
{
    Camera_ErrorCode ret = OH_CameraManager_CreateCaptureSession(cameraManager, &captureSession);
    if (captureSession == nullptr || ret != CAMERA_OK) {
        OH_LOG_ERROR(LOG_APP, "OH_CameraManager_CreateCaptureSession failed.");
        return false;
    }
    
    ret = OH_CaptureSession_SetSessionMode(captureSession, NORMAL_PHOTO);
    if (ret != CAMERA_OK) {
        OH_LOG_ERROR(LOG_APP, "OH_CaptureSession_SetSessionMode failed.");
        return false;
    }
    
    ret = OH_CaptureSession_BeginConfig(captureSession);
    if (ret != CAMERA_OK) {
        OH_LOG_ERROR(LOG_APP, "OH_CaptureSession_BeginConfig failed.");
        return false;
    }
    
    // 向会话中添加相机输入流
    ret = OH_CaptureSession_AddInput(captureSession, cameraInput);
    if (ret != CAMERA_OK) {
        OH_LOG_ERROR(LOG_APP, "OH_CaptureSession_AddInput failed.");
        return false;
    }
    
    // 向会话中添加预览输出流
    ret = OH_CaptureSession_AddPreviewOutput(captureSession, previewOutput);
    if (ret != CAMERA_OK) {
        OH_LOG_ERROR(LOG_APP, "OH_CaptureSession_AddPreviewOutput failed, ret %{public}d", ret);
        return false;
    }
    
    // 提交会话配置
    ret = OH_CaptureSession_CommitConfig(captureSession);
    if (ret != CAMERA_OK) {
        OH_LOG_ERROR(LOG_APP, "OH_CaptureSession_CommitConfig failed, ret %{public}d", ret);
        return false;
    }
    
    // 启动会话
    ret = OH_CaptureSession_Start(captureSession);
    if (ret != CAMERA_OK) {
        OH_LOG_ERROR(LOG_APP, "OH_CaptureSession_Start failed, ret %{public}d", ret);
        return false;
    }
    OH_LOG_INFO(LOG_APP, "OH_CaptureSession_Start success.");
    
    return true;
}

bool destroyCameraSession()
{
    Camera_ErrorCode ret = OH_CaptureSession_Stop(captureSession);
    if (ret == CAMERA_OK) {
        OH_LOG_INFO(LOG_APP, "OH_CaptureSession_Stop success ");
    } else {
        OH_LOG_ERROR(LOG_APP, "OH_CaptureSession_Stop failed. %d ", ret);
        return false;
    }
    
    ret = OH_CaptureSession_BeginConfig(captureSession);
    if (ret != CAMERA_OK) {
        OH_LOG_ERROR(LOG_APP, "OH_CaptureSession_BeginConfig failed.");
    }
    
    ret = OH_CaptureSession_RemovePreviewOutput(captureSession, previewOutput);
    if (ret == CAMERA_OK) {
        OH_LOG_INFO(LOG_APP, "OH_CaptureSession_RemovePreviewOutput success ");
    } else {
        OH_LOG_ERROR(LOG_APP, "OH_CaptureSession_RemovePreviewOutput failed, %{public}d", ret);
        return false;
    }
    
    return true;
}
