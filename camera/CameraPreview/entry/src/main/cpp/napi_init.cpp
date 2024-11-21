#include "napi/native_api.h"
#include "hilog/log.h"
#include "Camera.h"
#include <string>


#define NAPI_RET_BOOL(ret) do{                      \
    napi_value nRet;                                \
    napi_value nTmp;                                \
    napi_create_int32(env, ret ? 1 : 0, &nTmp);     \
    napi_coerce_to_bool(env, nTmp, &nRet);          \
    return nRet;                                    \
}while(0)

const int32_t MAX_STR_LENGTH = 2048;
static void jsValueToString(const napi_env & env, const napi_value & value, std::string & target)
{
    size_t stringSize = 0;
    napi_get_value_string_utf8(env, value, nullptr, 0, &stringSize); // 获取字符串长度
    
    target.resize(stringSize);
    napi_get_value_string_utf8(env, value, &target[0], stringSize + 1, &stringSize); // 根据长度传换成字符串
}


static napi_value ListCameras(napi_env env, napi_callback_info /*info*/)
{
    bool ret = getCameras();
    
    NAPI_RET_BOOL(ret);
}

static napi_value CreateInput(napi_env env, napi_callback_info info)
{
    const uint32_t kArgSize = 1;
    size_t argc = kArgSize;
    napi_value args[kArgSize] = {nullptr};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    
    std::string sufaceId;
    jsValueToString(env, args[0], sufaceId);
    
    bool ret = createCameraInput() && getSupportedSceneModes() 
                && getSupportedOutputCapability() && createPreviewOutput(sufaceId.c_str());
    
    NAPI_RET_BOOL(ret);
}

static napi_value CreateSession(napi_env env, napi_callback_info /*info*/)
{
    bool ret = createCameraSession();
    NAPI_RET_BOOL(ret);
}

static napi_value DestroySession(napi_env env, napi_callback_info /*info*/)
{
    bool ret = destroyCameraSession();
    NAPI_RET_BOOL(ret);
}

EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        { "ListCameras", nullptr, ListCameras, nullptr, nullptr, nullptr, napi_default, nullptr },
        { "CreateInput", nullptr, CreateInput, nullptr, nullptr, nullptr, napi_default, nullptr },
        { "CreateSession", nullptr, CreateSession, nullptr, nullptr, nullptr, napi_default, nullptr },
        { "DestroySession", nullptr, DestroySession, nullptr, nullptr, nullptr, napi_default, nullptr }
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    return exports;
}
EXTERN_C_END

static napi_module demoModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "entry",
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};

extern "C" __attribute__((constructor)) void RegisterEntryModule(void)
{
    napi_module_register(&demoModule);
}
