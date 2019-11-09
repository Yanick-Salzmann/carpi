#include <iostream>

#include <include/cef_app.h>
#include <include/wrapper/cef_message_router.h>
#include <sys/shm.h>

class CameraFrameCallback : public CefV8Handler {
    IMPLEMENT_REFCOUNTING(CameraFrameCallback);

    static const uint32_t CAMERA_WIDTH = 480;
    static const uint32_t CAMERA_HEIGHT = 368;

    static const uint32_t SHMEM_KEY_MUTEX = 0x42434455;
    static const uint32_t SHMEM_KEY_DATA = 0x42434456;

    pthread_mutex_t* _video_shmem_mutex = nullptr;
    int32_t _mutex_shm_id = 0;
    int32_t _camera_shm_id = 0;

    void* _camera_frame_buffer;

public:
    CameraFrameCallback() {
        pthread_mutexattr_t attr{};
        if(pthread_mutexattr_init(&attr) < 0) {
            throw std::runtime_error{"Error initializing mutex attribute"};
        }

        if(pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED) < 0) {
            throw std::runtime_error{"Error setting shared mutex attribute"};
        }

        if((_mutex_shm_id = shmget(SHMEM_KEY_MUTEX, sizeof(pthread_mutex_t), IPC_CREAT | 0777)) == -1) {
            throw std::runtime_error{"Error creating shared memory"};
        }

        if((_video_shmem_mutex = (pthread_mutex_t*) shmat(_mutex_shm_id, nullptr, 0)) == (void*) -1) {
            throw std::runtime_error{"Error creating shared memory"};
        }

        pthread_mutex_init(_video_shmem_mutex, &attr);
        pthread_mutexattr_destroy(&attr);

        _camera_shm_id = shmget(SHMEM_KEY_DATA, CAMERA_WIDTH * CAMERA_HEIGHT * 4, IPC_CREAT | 0777);
        _camera_frame_buffer = shmat(_camera_shm_id, nullptr, 0);
    }

    bool Execute(const CefString &name, CefRefPtr<CefV8Value> object, const CefV8ValueList &arguments, CefRefPtr<CefV8Value> &retval, CefString &exception) override {
        CefRefPtr<CefV8Value> ptr_value;
        pthread_mutex_lock(_video_shmem_mutex);
        ptr_value = CefV8Value::CreateArrayBuffer(_camera_frame_buffer, CAMERA_WIDTH * CAMERA_HEIGHT * 4, nullptr);
        pthread_mutex_unlock(_video_shmem_mutex);

        retval = ptr_value;
        return true;
    }
};

static CefRefPtr<CameraFrameCallback> camera_frame_callback{};

class MainApp : public CefApp, CefRenderProcessHandler {
    IMPLEMENT_REFCOUNTING(MainApp);

    CefRefPtr<CefMessageRouterRendererSide> _router;

public:
    CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override {
        return this;
    }

    void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) override {
        registrar->AddCustomScheme("carpi", CEF_SCHEME_OPTION_STANDARD | CEF_SCHEME_OPTION_SECURE);
    }

    void OnBeforeCommandLineProcessing(const CefString &process_type, CefRefPtr<CefCommandLine> command_line) override {
        CefApp::OnBeforeCommandLineProcessing(process_type, command_line);
    }

    void OnWebKitInitialized() override {
        CefMessageRouterConfig config;
        config.js_query_function = "sendCefRequest";
        _router = CefMessageRouterRendererSide::Create (config);
    }

    void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override {
        _router->OnContextCreated (browser, frame, context);

        context->GetGlobal()->SetValue("fetch_raw_frame", CefV8Value::CreateFunction("fetch_raw_frame", camera_frame_callback),CefV8Value::PropertyAttribute::V8_PROPERTY_ATTRIBUTE_READONLY);
    }

    void OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override {
        _router->OnContextReleased (browser, frame, context);
    }

    void OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefDOMNode> node) override {
        CefRenderProcessHandler::OnFocusedNodeChanged(browser, frame, node);
    }

    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) override {
        return _router->OnProcessMessageReceived (browser, frame, source_process, message);
    }
};

int main(int argc, char* argv[]) {
    CefMainArgs main_args{argc, argv};
    CefRefPtr<MainApp> app{new MainApp{}};

    camera_frame_callback = new CameraFrameCallback();

    return CefExecuteProcess(main_args, app, nullptr);
}