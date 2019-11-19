#include <gps/gps_listener.hpp>
#include "gps_callback.hpp"
#include "gps_listener_thread.hpp"

namespace carpi {

    bool GpsCallback::Execute(const CefString &name, CefRefPtr<CefV8Value> object, const CefV8ValueList &arguments, CefRefPtr<CefV8Value> &retval, CefString &exception) {
        sGpsListener->start();

        gps::GpsMeasurement measurement{};
        sGpsListener->active_measurement(measurement);

        auto ret_obj = CefV8Value::CreateObject(nullptr, nullptr);
        ret_obj->SetValue("lat", CefV8Value::CreateDouble(measurement.lat), V8_PROPERTY_ATTRIBUTE_READONLY);
        ret_obj->SetValue("lon", CefV8Value::CreateDouble(measurement.lon), V8_PROPERTY_ATTRIBUTE_READONLY);
        ret_obj->SetValue("alt", CefV8Value::CreateDouble(measurement.alt), V8_PROPERTY_ATTRIBUTE_READONLY);

        retval = ret_obj;

        return true;
    }
}