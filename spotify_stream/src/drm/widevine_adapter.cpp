#include <dlfcn.h>
#include "widevine_adapter.hpp"
#include "widevine_host.hpp"
#include <string>
#include <stdexcept>

namespace carpi::spotify::drm {
    const std::string WIDEVINE_KEY_SYSTEM{"com.widevine.alpha"};

    WidevineAdapter::WidevineAdapter() {
        _cdm_library = dlopen("/opt/google/chrome/WidevineCdm/_platform_specific/linux_x64/libwidevinecdm.so", RTLD_NOW);
        _create_cdm_instance = (decltype(_create_cdm_instance)) dlsym(_cdm_library, "CreateCdmInstance");
        const auto cdm = _create_cdm_instance(cdm::ContentDecryptionModule_10::kVersion, WIDEVINE_KEY_SYSTEM.c_str(), static_cast<uint32_t>(WIDEVINE_KEY_SYSTEM.size()), WidevineAdapter::create_cdm_host, this);
        _cdm = (cdm::ContentDecryptionModule_10*) cdm;

        if(_host != nullptr) {
            _host->module(_cdm);
        }
    }

    void *WidevineAdapter::create_cdm_host(int interface_version, void *user_data) {
        if(interface_version != cdm::Host_10::kVersion) {
            throw std::runtime_error{"Only host version 10 supported"};
        }

        auto adapter = (WidevineAdapter*) user_data;
        auto host = new WidevineHost();
        adapter->_host = host;
        return static_cast<cdm::Host_10*>(host);
    }
}