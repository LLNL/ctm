//  To parse this JSON data, first install
//
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     CtmTimeSeries data = nlohmann::json::parse(jsonString);

#pragma once

#include <optional>
#include <variant>
#include "json.hpp"

#ifndef NLOHMANN_OPT_HELPER
#define NLOHMANN_OPT_HELPER
namespace nlohmann {
    template <typename T>
    struct adl_serializer<std::shared_ptr<T>> {
        static void to_json(json & j, const std::shared_ptr<T> & opt) {
            if (!opt) j = nullptr; else j = *opt;
        }

        static std::shared_ptr<T> from_json(const json & j) {
            if (j.is_null()) return std::make_shared<T>(); else return std::make_shared<T>(j.get<T>());
        }
    };
    template <typename T>
    struct adl_serializer<std::optional<T>> {
        static void to_json(json & j, const std::optional<T> & opt) {
            if (!opt) j = nullptr; else j = *opt;
        }

        static std::optional<T> from_json(const json & j) {
            if (j.is_null()) return std::make_optional<T>(); else return std::make_optional<T>(j.get<T>());
        }
    };
}
#endif

namespace quicktype {
    using nlohmann::json;

    #ifndef NLOHMANN_UNTYPED_quicktype_HELPER
    #define NLOHMANN_UNTYPED_quicktype_HELPER
    inline json get_untyped(const json & j, const char * property) {
        if (j.find(property) != j.end()) {
            return j.at(property).get<json>();
        }
        return json();
    }

    inline json get_untyped(const json & j, std::string property) {
        return get_untyped(j, property.data());
    }
    #endif

    #ifndef NLOHMANN_OPTIONAL_quicktype_HELPER
    #define NLOHMANN_OPTIONAL_quicktype_HELPER
    template <typename T>
    inline std::shared_ptr<T> get_heap_optional(const json & j, const char * property) {
        auto it = j.find(property);
        if (it != j.end() && !it->is_null()) {
            return j.at(property).get<std::shared_ptr<T>>();
        }
        return std::shared_ptr<T>();
    }

    template <typename T>
    inline std::shared_ptr<T> get_heap_optional(const json & j, std::string property) {
        return get_heap_optional<T>(j, property.data());
    }
    template <typename T>
    inline std::optional<T> get_stack_optional(const json & j, const char * property) {
        auto it = j.find(property);
        if (it != j.end() && !it->is_null()) {
            return j.at(property).get<std::optional<T>>();
        }
        return std::optional<T>();
    }

    template <typename T>
    inline std::optional<T> get_stack_optional(const json & j, std::string property) {
        return get_stack_optional<T>(j, property.data());
    }
    #endif

    using PathToFile = std::variant<std::vector<std::string>, std::string>;

    using CtmTimeSeriesSchema = std::variant<int64_t, std::string>;

    /**
     * structure to contain all time variant data of the system/case. All time series are
     * synchronized to the same timestamps, which should should be stored using Unix time.
     * Structure is quasi-tabular, with uid, name, path_to_file, values, and ext being arrays in
     * the same order of said field. This is done in order to allow for better compression
     * (e.g., using HDF5) for the values field.
     */
    struct TimeSeriesData {
        /**
         * additional time series information not currently supported by CTM
         */
        std::optional<std::vector<nlohmann::json>> ext;
        /**
         * array of names of time series
         */
        std::optional<std::vector<std::string>> name;
        /**
         * path to file containing all time series information or a separate path for each time
         * series
         */
        std::optional<PathToFile> path_to_file;
        /**
         * [seconds] seconds since epoch (Unix time) for each instant for which time series values
         * are provided
         */
        std::optional<std::vector<double>> timestamp;
        /**
         * array of uids of time series
         */
        std::vector<CtmTimeSeriesSchema> uid;
        /**
         * array of time series values
         */
        std::optional<std::vector<std::vector<nlohmann::json>>> values;
    };

    /**
     * Common Transmission Model (CTM) Time Series Data Schema v0.1
     */
    struct CtmTimeSeries {
        /**
         * release version of CTM specification
         */
        std::string ctm_version;
        /**
         * structure to contain all time variant data of the system/case. All time series are
         * synchronized to the same timestamps, which should should be stored using Unix time.
         * Structure is quasi-tabular, with uid, name, path_to_file, values, and ext being arrays in
         * the same order of said field. This is done in order to allow for better compression
         * (e.g., using HDF5) for the values field.
         */
        TimeSeriesData time_series_data;
    };
}

namespace quicktype {
void from_json(const json & j, TimeSeriesData & x);
void to_json(json & j, const TimeSeriesData & x);

void from_json(const json & j, CtmTimeSeries & x);
void to_json(json & j, const CtmTimeSeries & x);
}
namespace nlohmann {
template <>
struct adl_serializer<std::variant<std::vector<std::string>, std::string>> {
    static void from_json(const json & j, std::variant<std::vector<std::string>, std::string> & x);
    static void to_json(json & j, const std::variant<std::vector<std::string>, std::string> & x);
};

template <>
struct adl_serializer<std::variant<int64_t, std::string>> {
    static void from_json(const json & j, std::variant<int64_t, std::string> & x);
    static void to_json(json & j, const std::variant<int64_t, std::string> & x);
};
}
namespace quicktype {
    inline void from_json(const json & j, TimeSeriesData& x) {
        x.ext = get_stack_optional<std::vector<nlohmann::json>>(j, "ext");
        x.name = get_stack_optional<std::vector<std::string>>(j, "name");
        x.path_to_file = get_stack_optional<std::variant<std::vector<std::string>, std::string>>(j, "path_to_file");
        x.timestamp = get_stack_optional<std::vector<double>>(j, "timestamp");
        x.uid = j.at("uid").get<std::vector<CtmTimeSeriesSchema>>();
        x.values = get_stack_optional<std::vector<std::vector<nlohmann::json>>>(j, "values");
    }

    inline void to_json(json & j, const TimeSeriesData & x) {
        j = json::object();
        j["ext"] = x.ext;
        j["name"] = x.name;
        j["path_to_file"] = x.path_to_file;
        j["timestamp"] = x.timestamp;
        j["uid"] = x.uid;
        j["values"] = x.values;
    }

    inline void from_json(const json & j, CtmTimeSeries& x) {
        x.ctm_version = j.at("ctm_version").get<std::string>();
        x.time_series_data = j.at("time_series_data").get<TimeSeriesData>();
    }

    inline void to_json(json & j, const CtmTimeSeries & x) {
        j = json::object();
        j["ctm_version"] = x.ctm_version;
        j["time_series_data"] = x.time_series_data;
    }
}
namespace nlohmann {
    inline void adl_serializer<std::variant<std::vector<std::string>, std::string>>::from_json(const json & j, std::variant<std::vector<std::string>, std::string> & x) {
        if (j.is_string())
            x = j.get<std::string>();
        else if (j.is_array())
            x = j.get<std::vector<std::string>>();
        else throw std::runtime_error("Could not deserialise!");
    }

    inline void adl_serializer<std::variant<std::vector<std::string>, std::string>>::to_json(json & j, const std::variant<std::vector<std::string>, std::string> & x) {
        switch (x.index()) {
            case 0:
                j = std::get<std::vector<std::string>>(x);
                break;
            case 1:
                j = std::get<std::string>(x);
                break;
            default: throw std::runtime_error("Input JSON does not conform to schema!");
        }
    }

    inline void adl_serializer<std::variant<int64_t, std::string>>::from_json(const json & j, std::variant<int64_t, std::string> & x) {
        if (j.is_number_integer())
            x = j.get<int64_t>();
        else if (j.is_string())
            x = j.get<std::string>();
        else throw std::runtime_error("Could not deserialise!");
    }

    inline void adl_serializer<std::variant<int64_t, std::string>>::to_json(json & j, const std::variant<int64_t, std::string> & x) {
        switch (x.index()) {
            case 0:
                j = std::get<int64_t>(x);
                break;
            case 1:
                j = std::get<std::string>(x);
                break;
            default: throw std::runtime_error("Input JSON does not conform to schema!");
        }
    }
}
