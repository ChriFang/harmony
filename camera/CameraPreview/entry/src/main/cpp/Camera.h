//
// Created on 2024/11/19.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef CAMERAMANAGER_CAMERA_H
#define CAMERAMANAGER_CAMERA_H

bool getCameras();
bool createCameraInput();
bool getSupportedSceneModes();
bool getSupportedOutputCapability();
bool createPreviewOutput(const char* surfaceId);

bool createCameraSession();
bool destroyCameraSession();

#endif //CAMERAMANAGER_CAMERA_H
