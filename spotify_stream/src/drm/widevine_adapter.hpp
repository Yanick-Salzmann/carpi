#ifndef CARPI_WIDEVINE_ADAPTER_HPP
#define CARPI_WIDEVINE_ADAPTER_HPP

#include "../cdm/content_decryption_module.hpp"

namespace carpi::spotify::drm {
    class WidevineHost;

    class WidevineAdapter {
        WidevineHost* _host;

        void* _cdm_library = nullptr;
        decltype(::CreateCdmInstance)* _create_cdm_instance;

        cdm::ContentDecryptionModule_10* _cdm;

        static void* create_cdm_host(int interface_version, void* user_data);

    public:
        WidevineAdapter();
    };
}

#endif //CARPI_WIDEVINE_ADAPTER_HPP
