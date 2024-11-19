#include "napi/native_api.h"
#include "hilog/log.h"
#include "Camera.h"


#define NAPI_RET_BOOL(ret) do{                      \
    napi_value nRet;                                \
    napi_value nTmp;                                \
    napi_create_int32(env, ret ? 1 : 0, &nTmp);     \
    napi_coerce_to_bool(env, nTmp, &nRet);          \
    return nRet;                                    \
}while(0)


static napi_value ListCameras(napi_env env, napi_callback_info /*info*/)
{
    bool ret = getCameras();
    
    NAPI_RET_BOOL(ret);
}

static napi_value CreateInput(napi_env env, napi_callback_info /*info*/)
{
    bool ret = createCameraInput() && getSupportedSceneModes() && getSupportedOutputCapability();
    
    NAPI_RET_BOOL(ret);
}

EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        { "ListCameras", nullptr, ListCameras, nullptr, nullptr, nullptr, napi_default, nullptr },
        { "CreateInput", nullptr, CreateInput, nullptr, nullptr, nullptr, napi_default, nullptr }
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
