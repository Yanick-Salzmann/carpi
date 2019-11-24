#include "ch_address_reader.hpp"

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <common_utils/string.hpp>

namespace carpi {
    struct AddressElement {
        std::string street;
        std::string number;
        std::string plz;
        std::string city;
        std::string east;
        std::string north;
    };

    auto ChAddressReader::Execute(const CefString &name, CefRefPtr<CefV8Value> object, const CefV8ValueList &arguments, CefRefPtr<CefV8Value> &retval, CefString &exception) -> bool {
        static std::filesystem::path addr_path{"../../carpi_browser_subprocess/data/CH_addresses.csv"};

        std::ifstream is{addr_path};

        std::string line{};

        std::vector<AddressElement> elements{};

        while(std::getline(is, line)) {
            const auto parts = utils::split(line, ';');
            elements.emplace_back(parts[5], parts[6], parts[7], parts[9], parts[10], parts[11]);
        }

        retval = CefV8Value::CreateArray(elements.size());
        std::size_t cur_index = 0;
        for(const auto& elem : elements) {
            CefRefPtr<CefV8Value> obj = CefV8Value::CreateObject(nullptr, nullptr);
            obj->SetValue("street", CefV8Value::CreateString(elem.street), V8_PROPERTY_ATTRIBUTE_READONLY);
            obj->SetValue("number", CefV8Value::CreateString(elem.number), V8_PROPERTY_ATTRIBUTE_READONLY);
            obj->SetValue("plz", CefV8Value::CreateString(elem.plz), V8_PROPERTY_ATTRIBUTE_READONLY);
            obj->SetValue("city", CefV8Value::CreateString(elem.city), V8_PROPERTY_ATTRIBUTE_READONLY);
            obj->SetValue("east", CefV8Value::CreateString(elem.east), V8_PROPERTY_ATTRIBUTE_READONLY);
            obj->SetValue("north", CefV8Value::CreateString(elem.north), V8_PROPERTY_ATTRIBUTE_READONLY);

            retval->SetValue(cur_index++, obj);
        }

        return true;
    }
}