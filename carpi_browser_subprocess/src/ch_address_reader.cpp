#include "ch_address_reader.hpp"

#include <filesystem>
#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <common_utils/string.hpp>
#include <iostream>

namespace carpi {
    struct AddressElement {
        std::string street;
        std::string number;
        std::string plz;
        std::string city;
        std::string east;
        std::string north;

        AddressElement() = default;

        AddressElement(
                std::string street,
                std::string number,
                std::string plz,
                std::string city,
                std::string east,
                std::string north) :
                street(std::move(street)),
                number(std::move(number)),
                plz(std::move(plz)),
                city(std::move(city)),
                east(std::move(east)),
                north(std::move(north)) {

        }
    };

    auto ChAddressReader::Execute(const CefString &name, CefRefPtr<CefV8Value> object, const CefV8ValueList &arguments, CefRefPtr<CefV8Value> &retval, CefString &exception) -> bool {
        if(arguments.empty()) {
            retval = CefV8Value::CreateArray(0);
            return true;
        }

        CefRefPtr<CefV8Value> val = arguments[0];
        if(!(val->IsInt() || val->IsUInt())) {
            retval = CefV8Value::CreateArray(0);
            return true;
        }

        const auto search_plz = val->GetUIntValue();

        std::ifstream is{"../../carpi_browser_subprocess/data/CH_addresses.csv"};

        std::string line{};

        std::vector<AddressElement> elements{};

        std::vector<std::string> data_container{};
        data_container.resize(1024);

        auto start1 = std::chrono::high_resolution_clock::now();

        auto is_first = true;
        while (std::getline(is, line)) {
            if(is_first) {
                is_first = false;
                continue;
            }

            utils::split(line, ';', data_container);
            auto plz = std::stoi(data_container[7]);
            if(plz != search_plz) {
                continue;
            }

            elements.emplace_back(data_container[5], data_container[6], data_container[7], data_container[9], data_container[10], data_container[11]);
        }

        const auto end1 = std::chrono::high_resolution_clock::now();

        std::cout << "Filtering time:" << std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count() << std::endl;

        retval = CefV8Value::CreateArray(elements.size());
        std::size_t cur_index = 0;
        for (const auto &elem : elements) {
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