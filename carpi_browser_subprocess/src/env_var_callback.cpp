#include "env_var_callback.hpp"

extern char** environ;

namespace carpi {

    bool EnvVarCallback::Execute(const CefString &name, CefRefPtr<CefV8Value> object, const CefV8ValueList &arguments, CefRefPtr<CefV8Value> &retval, CefString &exception) {
        if(arguments.empty()) {
            CefRefPtr<CefV8Value> ret_obj = CefV8Value::CreateObject(nullptr, nullptr);
            for(auto cur_ptr = environ; *cur_ptr; ++cur_ptr) {
                std::string env_val{*cur_ptr};
                const auto idx_eq = env_val.find('=');
                if(idx_eq == std::string::npos) {
                    ret_obj->SetValue(env_val, CefV8Value::CreateString(""), V8_PROPERTY_ATTRIBUTE_READONLY);
                } else {
                    const auto key = env_val.substr(0, idx_eq);
                    const auto value = env_val.substr(idx_eq + 1);
                    ret_obj->SetValue(key, CefV8Value::CreateString(value), V8_PROPERTY_ATTRIBUTE_READONLY);
                }
            }

            retval = ret_obj;
        } else {
            if(arguments.size() == 1) {
                const auto key = arguments[0]->GetStringValue().ToString();
                const auto value = getenv(key.c_str());
                if(value == nullptr) {
                    retval = CefV8Value::CreateNull();
                } else {
                    retval = CefV8Value::CreateString(CefString{value});
                }
            } else {
                CefRefPtr<CefV8Value> ret_obj = CefV8Value::CreateObject(nullptr, nullptr);
                for(const auto& arg : arguments) {
                    const auto key = arg->GetStringValue().ToString();
                    const auto value = getenv(key.c_str());
                    if(value == nullptr) {
                        ret_obj->SetValue(key, CefV8Value::CreateNull(), V8_PROPERTY_ATTRIBUTE_READONLY);
                    } else {
                        ret_obj->SetValue(key, CefV8Value::CreateString(value), V8_PROPERTY_ATTRIBUTE_READONLY);
                    }
                }

                retval = ret_obj;
            }
        }

        return true;
    }
}