//  To parse this JSON data, first install
//
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     CtmSolution data = nlohmann::json::parse(jsonString);

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

    using Uid = std::variant<int64_t, std::string>;

    /**
     * structure to hold a reference (possibly, to be scaled) to a time series
     */
    struct CtmSolutionSchema {
        /**
         * [-] scale factor to be applied to the pointed-to time series to obtain this field's values
         */
        double scale_factor;
        /**
         * uid of time series (in time_series_data) this reference points to
         */
        Uid uid;
    };

    using PlFr = std::variant<CtmSolutionSchema, double>;

    /**
     * structure to hold switch solution data
     */
    struct AcLine {
        /**
         * additional switch parameters currently not supported by CTM
         */
        nlohmann::json ext;
        /**
         * [MW or pu] active power entering the ac line at its from terminal
         */
        std::optional<PlFr> pl_fr;
        /**
         * [MVAr or pu] active power entering the ac line at its from terminal
         */
        std::optional<PlFr> pl_to;
        /**
         * [MVAr or pu] reactive power entering the ac line at its from terminal
         */
        std::optional<PlFr> ql_fr;
        /**
         * [MVAr or pu] reactive power entering the ac line at its to terminal
         */
        std::optional<PlFr> ql_to;
        Uid uid;
    };

    /**
     * structure to hold bus solution data
     */
    struct Bus {
        /**
         * additional bus parameters currently not supported by CTM
         */
        nlohmann::json ext;
        /**
         * [MW or pu] signed power imbalance; positive indicates active load loss
         */
        std::optional<PlFr> p_imbalance;
        /**
         * [$/MW or $/pu] dual of active power balance constraints
         */
        std::optional<PlFr> p_lambda;
        /**
         * [MVAr or pu] signed power imbalance; positive indicates reactive load loss
         */
        std::optional<PlFr> q_imbalance;
        /**
         * [$/MVAr or $/pu] dual of reactive power balance constraints
         */
        std::optional<PlFr> q_lambda;
        Uid uid;
        /**
         * [deg] voltage magnitude
         */
        PlFr va;
        /**
         * [kV or pu] voltage magnitude
         */
        std::optional<PlFr> vm;
    };

    using InService = std::variant<CtmSolutionSchema, int64_t>;

    using Rg = std::variant<CtmSolutionSchema, double>;

    /**
     * structure to hold reserve provision to a single reserve product
     */
    struct ReserveProvision {
        /**
         * [MW or pu] contribution to reserve
         */
        Rg rg;
        /**
         * uid of reserve product rg contributes to
         */
        Uid uid;
    };

    /**
     * structure to hold generator solution data
     */
    struct Gen {
        /**
         * additional generator parameters currently not supported by CTM
         */
        nlohmann::json ext;
        /**
         * commitment binary indicator; 0=>unit is turned off, 1=>unit is online
         */
        std::optional<InService> in_service;
        /**
         * [MW or pu] active power injection
         */
        PlFr pg;
        /**
         * [MVAr or pu] reactive power injection
         */
        std::optional<PlFr> qg;
        std::optional<std::vector<ReserveProvision>> reserve_provision;
        Uid uid;
    };

    /**
     * units used for physical network parameters
     */
    enum class UnitConvention : int { NATURAL_UNITS, PER_UNIT_COMPONENT_BASE, PER_UNIT_SYSTEM_BASE };

    /**
     * structure to hold global settings for parameters in the network
     */
    struct GlobalParams {
        /**
         * [MVA] system-wide apparent power base
         */
        std::optional<double> base_mva;
        /**
         * units used for physical network parameters
         */
        UnitConvention unit_convention;
    };

    /**
     * structure to hold point-to-point hvdc line solution data
     */
    struct HvdcP2P {
        /**
         * additional hvdc point-to-point parameters currently not supported by CTM
         */
        nlohmann::json ext;
        /**
         * [MW or pu] active power entering the hvdc line at its from terminal
         */
        PlFr pdc_fr;
        /**
         * [MW or pu] active power entering the hvdc line at its to terminal
         */
        PlFr pdc_to;
        /**
         * [MVAr or pu] reactive power entering the hvdc line at its from terminal
         */
        std::optional<PlFr> qdc_fr;
        /**
         * [MVAr or pu] reactive power entering the hvdc line at its to terminal
         */
        std::optional<PlFr> qdc_to;
        Uid uid;
        /**
         * [kV or pu] voltage at the dc side
         */
        std::optional<PlFr> vm_dc;
    };

    /**
     * structure to hold reserve product solution data
     */
    struct Reserve {
        /**
         * additional reserve parameters currently not supported by CTM
         */
        nlohmann::json ext;
        /**
         * [MW or pu] shortfall on reserve product
         */
        PlFr shortfall;
        Uid uid;
    };

    using NumSteps = std::variant<std::vector<int64_t>, CtmSolutionSchema, int64_t>;

    /**
     * structure to hold shunt solution data
     */
    struct Shunt {
        /**
         * additional shunt parameters currently not supported by CTM
         */
        nlohmann::json ext;
        /**
         * number of energized steps of shunt section (lower bound is always 0)
         */
        NumSteps num_steps;
        Uid uid;
    };

    /**
     * structure to hold switch solution data
     */
    struct Switch {
        /**
         * additional switch parameters currently not supported by CTM
         */
        nlohmann::json ext;
        /**
         * [MW or pu] active power entering the switch at its from terminal
         */
        std::optional<PlFr> psw_fr;
        /**
         * [MVAr or pu] reactive power entering the switch at its from terminal
         */
        std::optional<PlFr> qsw_fr;
        /**
         * binary indicator of switch state; 0=>open, 1=>closed
         */
        InService state;
        Uid uid;
    };

    /**
     * structure to hold storage (battery) solution data
     */
    struct Storage {
        /**
         * [MW or pu] rate of charge
         */
        std::optional<Rg> charge;
        /**
         * [MW or pu] rate of discharge
         */
        std::optional<Rg> discharge;
        /**
         * [MWh or pu*h] state of charge
         */
        Rg energy;
        /**
         * additional storage parameters currently not supported by CTM
         */
        nlohmann::json ext;
        /**
         * [MW or pu] active power injection
         */
        PlFr ps;
        /**
         * [MW or pu] reactive power injection
         */
        std::optional<PlFr> qs;
        Uid uid;
    };

    /**
     * structure to hold 2-winding transformer solution data
     */
    struct Transformer {
        /**
         * additional 2-winding transformer parameters currently not supported by CTM
         */
        nlohmann::json ext;
        /**
         * [MW or pu] active power entering the transformer at its from terminal
         */
        std::optional<PlFr> pt_fr;
        /**
         * [MW or pu] active power entering the transformer at its to terminal
         */
        std::optional<PlFr> pt_to;
        /**
         * [MVAr or pu] reactive power entering the transformer at its from terminal
         */
        std::optional<PlFr> qt_fr;
        /**
         * [MVAr or pu] reactive power entering the transformer at its to terminal
         */
        std::optional<PlFr> qt_to;
        /**
         * [deg] angle phase shift
         */
        std::optional<PlFr> ta;
        /**
         * [-] tap ratio
         */
        std::optional<PlFr> tm;
        Uid uid;
    };

    /**
     * structure to hold persistent solution data
     */
    struct Solution {
        std::optional<std::vector<AcLine>> ac_line;
        std::vector<Bus> bus;
        std::vector<Gen> gen;
        /**
         * structure to hold global settings for parameters in the network
         */
        GlobalParams global_params;
        std::optional<std::vector<HvdcP2P>> hvdc_p2_p;
        std::optional<std::vector<Reserve>> reserve;
        std::optional<std::vector<Shunt>> shunt;
        std::optional<std::vector<Storage>> storage;
        std::optional<std::vector<Switch>> solution_switch;
        std::optional<std::vector<Transformer>> transformer;
    };

    using PathToFile = std::variant<std::vector<std::string>, std::string>;

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
        std::vector<Uid> uid;
        /**
         * array of time series values
         */
        std::optional<std::vector<std::vector<nlohmann::json>>> values;
    };

    /**
     * Common Transmission Model (CTM) Solution Schema v0.1
     */
    struct CtmSolution {
        /**
         * release version of CTM specification
         */
        std::string ctm_version;
        /**
         * structure to hold persistent solution data
         */
        Solution solution;
        /**
         * structure to contain all time variant data of the system/case. All time series are
         * synchronized to the same timestamps, which should should be stored using Unix time.
         * Structure is quasi-tabular, with uid, name, path_to_file, values, and ext being arrays in
         * the same order of said field. This is done in order to allow for better compression
         * (e.g., using HDF5) for the values field.
         */
        std::optional<TimeSeriesData> time_series_data;
    };
}

namespace quicktype {
void from_json(const json & j, CtmSolutionSchema & x);
void to_json(json & j, const CtmSolutionSchema & x);

void from_json(const json & j, AcLine & x);
void to_json(json & j, const AcLine & x);

void from_json(const json & j, Bus & x);
void to_json(json & j, const Bus & x);

void from_json(const json & j, ReserveProvision & x);
void to_json(json & j, const ReserveProvision & x);

void from_json(const json & j, Gen & x);
void to_json(json & j, const Gen & x);

void from_json(const json & j, GlobalParams & x);
void to_json(json & j, const GlobalParams & x);

void from_json(const json & j, HvdcP2P & x);
void to_json(json & j, const HvdcP2P & x);

void from_json(const json & j, Reserve & x);
void to_json(json & j, const Reserve & x);

void from_json(const json & j, Shunt & x);
void to_json(json & j, const Shunt & x);

void from_json(const json & j, Switch & x);
void to_json(json & j, const Switch & x);

void from_json(const json & j, Storage & x);
void to_json(json & j, const Storage & x);

void from_json(const json & j, Transformer & x);
void to_json(json & j, const Transformer & x);

void from_json(const json & j, Solution & x);
void to_json(json & j, const Solution & x);

void from_json(const json & j, TimeSeriesData & x);
void to_json(json & j, const TimeSeriesData & x);

void from_json(const json & j, CtmSolution & x);
void to_json(json & j, const CtmSolution & x);

void from_json(const json & j, UnitConvention & x);
void to_json(json & j, const UnitConvention & x);
}
namespace nlohmann {
template <>
struct adl_serializer<std::variant<int64_t, std::string>> {
    static void from_json(const json & j, std::variant<int64_t, std::string> & x);
    static void to_json(json & j, const std::variant<int64_t, std::string> & x);
};

template <>
struct adl_serializer<std::variant<quicktype::CtmSolutionSchema, double>> {
    static void from_json(const json & j, std::variant<quicktype::CtmSolutionSchema, double> & x);
    static void to_json(json & j, const std::variant<quicktype::CtmSolutionSchema, double> & x);
};

template <>
struct adl_serializer<std::variant<quicktype::CtmSolutionSchema, int64_t>> {
    static void from_json(const json & j, std::variant<quicktype::CtmSolutionSchema, int64_t> & x);
    static void to_json(json & j, const std::variant<quicktype::CtmSolutionSchema, int64_t> & x);
};

template <>
struct adl_serializer<std::variant<std::vector<int64_t>, quicktype::CtmSolutionSchema, int64_t>> {
    static void from_json(const json & j, std::variant<std::vector<int64_t>, quicktype::CtmSolutionSchema, int64_t> & x);
    static void to_json(json & j, const std::variant<std::vector<int64_t>, quicktype::CtmSolutionSchema, int64_t> & x);
};

template <>
struct adl_serializer<std::variant<std::vector<std::string>, std::string>> {
    static void from_json(const json & j, std::variant<std::vector<std::string>, std::string> & x);
    static void to_json(json & j, const std::variant<std::vector<std::string>, std::string> & x);
};
}
namespace quicktype {
    inline void from_json(const json & j, CtmSolutionSchema& x) {
        x.scale_factor = j.at("scale_factor").get<double>();
        x.uid = j.at("uid").get<Uid>();
    }

    inline void to_json(json & j, const CtmSolutionSchema & x) {
        j = json::object();
        j["scale_factor"] = x.scale_factor;
        j["uid"] = x.uid;
    }

    inline void from_json(const json & j, AcLine& x) {
        x.ext = get_untyped(j, "ext");
        x.pl_fr = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "pl_fr");
        x.pl_to = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "pl_to");
        x.ql_fr = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "ql_fr");
        x.ql_to = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "ql_to");
        x.uid = j.at("uid").get<Uid>();
    }

    inline void to_json(json & j, const AcLine & x) {
        j = json::object();
        j["ext"] = x.ext;
        j["pl_fr"] = x.pl_fr;
        j["pl_to"] = x.pl_to;
        j["ql_fr"] = x.ql_fr;
        j["ql_to"] = x.ql_to;
        j["uid"] = x.uid;
    }

    inline void from_json(const json & j, Bus& x) {
        x.ext = get_untyped(j, "ext");
        x.p_imbalance = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "p_imbalance");
        x.p_lambda = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "p_lambda");
        x.q_imbalance = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "q_imbalance");
        x.q_lambda = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "q_lambda");
        x.uid = j.at("uid").get<Uid>();
        x.va = j.at("va").get<PlFr>();
        x.vm = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "vm");
    }

    inline void to_json(json & j, const Bus & x) {
        j = json::object();
        j["ext"] = x.ext;
        j["p_imbalance"] = x.p_imbalance;
        j["p_lambda"] = x.p_lambda;
        j["q_imbalance"] = x.q_imbalance;
        j["q_lambda"] = x.q_lambda;
        j["uid"] = x.uid;
        j["va"] = x.va;
        j["vm"] = x.vm;
    }

    inline void from_json(const json & j, ReserveProvision& x) {
        x.rg = j.at("rg").get<Rg>();
        x.uid = j.at("uid").get<Uid>();
    }

    inline void to_json(json & j, const ReserveProvision & x) {
        j = json::object();
        j["rg"] = x.rg;
        j["uid"] = x.uid;
    }

    inline void from_json(const json & j, Gen& x) {
        x.ext = get_untyped(j, "ext");
        x.in_service = get_stack_optional<std::variant<CtmSolutionSchema, int64_t>>(j, "in_service");
        x.pg = j.at("pg").get<PlFr>();
        x.qg = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "qg");
        x.reserve_provision = get_stack_optional<std::vector<ReserveProvision>>(j, "reserve_provision");
        x.uid = j.at("uid").get<Uid>();
    }

    inline void to_json(json & j, const Gen & x) {
        j = json::object();
        j["ext"] = x.ext;
        j["in_service"] = x.in_service;
        j["pg"] = x.pg;
        j["qg"] = x.qg;
        j["reserve_provision"] = x.reserve_provision;
        j["uid"] = x.uid;
    }

    inline void from_json(const json & j, GlobalParams& x) {
        x.base_mva = get_stack_optional<double>(j, "base_mva");
        x.unit_convention = j.at("unit_convention").get<UnitConvention>();
    }

    inline void to_json(json & j, const GlobalParams & x) {
        j = json::object();
        j["base_mva"] = x.base_mva;
        j["unit_convention"] = x.unit_convention;
    }

    inline void from_json(const json & j, HvdcP2P& x) {
        x.ext = get_untyped(j, "ext");
        x.pdc_fr = j.at("pdc_fr").get<PlFr>();
        x.pdc_to = j.at("pdc_to").get<PlFr>();
        x.qdc_fr = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "qdc_fr");
        x.qdc_to = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "qdc_to");
        x.uid = j.at("uid").get<Uid>();
        x.vm_dc = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "vm_dc");
    }

    inline void to_json(json & j, const HvdcP2P & x) {
        j = json::object();
        j["ext"] = x.ext;
        j["pdc_fr"] = x.pdc_fr;
        j["pdc_to"] = x.pdc_to;
        j["qdc_fr"] = x.qdc_fr;
        j["qdc_to"] = x.qdc_to;
        j["uid"] = x.uid;
        j["vm_dc"] = x.vm_dc;
    }

    inline void from_json(const json & j, Reserve& x) {
        x.ext = get_untyped(j, "ext");
        x.shortfall = j.at("shortfall").get<PlFr>();
        x.uid = j.at("uid").get<Uid>();
    }

    inline void to_json(json & j, const Reserve & x) {
        j = json::object();
        j["ext"] = x.ext;
        j["shortfall"] = x.shortfall;
        j["uid"] = x.uid;
    }

    inline void from_json(const json & j, Shunt& x) {
        x.ext = get_untyped(j, "ext");
        x.num_steps = j.at("num_steps").get<NumSteps>();
        x.uid = j.at("uid").get<Uid>();
    }

    inline void to_json(json & j, const Shunt & x) {
        j = json::object();
        j["ext"] = x.ext;
        j["num_steps"] = x.num_steps;
        j["uid"] = x.uid;
    }

    inline void from_json(const json & j, Switch& x) {
        x.ext = get_untyped(j, "ext");
        x.psw_fr = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "psw_fr");
        x.qsw_fr = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "qsw_fr");
        x.state = j.at("state").get<InService>();
        x.uid = j.at("uid").get<Uid>();
    }

    inline void to_json(json & j, const Switch & x) {
        j = json::object();
        j["ext"] = x.ext;
        j["psw_fr"] = x.psw_fr;
        j["qsw_fr"] = x.qsw_fr;
        j["state"] = x.state;
        j["uid"] = x.uid;
    }

    inline void from_json(const json & j, Storage& x) {
        x.charge = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "charge");
        x.discharge = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "discharge");
        x.energy = j.at("energy").get<Rg>();
        x.ext = get_untyped(j, "ext");
        x.ps = j.at("ps").get<PlFr>();
        x.qs = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "qs");
        x.uid = j.at("uid").get<Uid>();
    }

    inline void to_json(json & j, const Storage & x) {
        j = json::object();
        j["charge"] = x.charge;
        j["discharge"] = x.discharge;
        j["energy"] = x.energy;
        j["ext"] = x.ext;
        j["ps"] = x.ps;
        j["qs"] = x.qs;
        j["uid"] = x.uid;
    }

    inline void from_json(const json & j, Transformer& x) {
        x.ext = get_untyped(j, "ext");
        x.pt_fr = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "pt_fr");
        x.pt_to = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "pt_to");
        x.qt_fr = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "qt_fr");
        x.qt_to = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "qt_to");
        x.ta = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "ta");
        x.tm = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "tm");
        x.uid = j.at("uid").get<Uid>();
    }

    inline void to_json(json & j, const Transformer & x) {
        j = json::object();
        j["ext"] = x.ext;
        j["pt_fr"] = x.pt_fr;
        j["pt_to"] = x.pt_to;
        j["qt_fr"] = x.qt_fr;
        j["qt_to"] = x.qt_to;
        j["ta"] = x.ta;
        j["tm"] = x.tm;
        j["uid"] = x.uid;
    }

    inline void from_json(const json & j, Solution& x) {
        x.ac_line = get_stack_optional<std::vector<AcLine>>(j, "ac_line");
        x.bus = j.at("bus").get<std::vector<Bus>>();
        x.gen = j.at("gen").get<std::vector<Gen>>();
        x.global_params = j.at("global_params").get<GlobalParams>();
        x.hvdc_p2_p = get_stack_optional<std::vector<HvdcP2P>>(j, "hvdc_p2p");
        x.reserve = get_stack_optional<std::vector<Reserve>>(j, "reserve");
        x.shunt = get_stack_optional<std::vector<Shunt>>(j, "shunt");
        x.storage = get_stack_optional<std::vector<Storage>>(j, "storage");
        x.solution_switch = get_stack_optional<std::vector<Switch>>(j, "switch");
        x.transformer = get_stack_optional<std::vector<Transformer>>(j, "transformer");
    }

    inline void to_json(json & j, const Solution & x) {
        j = json::object();
        j["ac_line"] = x.ac_line;
        j["bus"] = x.bus;
        j["gen"] = x.gen;
        j["global_params"] = x.global_params;
        j["hvdc_p2p"] = x.hvdc_p2_p;
        j["reserve"] = x.reserve;
        j["shunt"] = x.shunt;
        j["storage"] = x.storage;
        j["switch"] = x.solution_switch;
        j["transformer"] = x.transformer;
    }

    inline void from_json(const json & j, TimeSeriesData& x) {
        x.ext = get_stack_optional<std::vector<nlohmann::json>>(j, "ext");
        x.name = get_stack_optional<std::vector<std::string>>(j, "name");
        x.path_to_file = get_stack_optional<std::variant<std::vector<std::string>, std::string>>(j, "path_to_file");
        x.timestamp = get_stack_optional<std::vector<double>>(j, "timestamp");
        x.uid = j.at("uid").get<std::vector<Uid>>();
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

    inline void from_json(const json & j, CtmSolution& x) {
        x.ctm_version = j.at("ctm_version").get<std::string>();
        x.solution = j.at("solution").get<Solution>();
        x.time_series_data = get_stack_optional<TimeSeriesData>(j, "time_series_data");
    }

    inline void to_json(json & j, const CtmSolution & x) {
        j = json::object();
        j["ctm_version"] = x.ctm_version;
        j["solution"] = x.solution;
        j["time_series_data"] = x.time_series_data;
    }

    inline void from_json(const json & j, UnitConvention & x) {
        if (j == "NATURAL_UNITS") x = UnitConvention::NATURAL_UNITS;
        else if (j == "PER_UNIT_COMPONENT_BASE") x = UnitConvention::PER_UNIT_COMPONENT_BASE;
        else if (j == "PER_UNIT_SYSTEM_BASE") x = UnitConvention::PER_UNIT_SYSTEM_BASE;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const UnitConvention & x) {
        switch (x) {
            case UnitConvention::NATURAL_UNITS: j = "NATURAL_UNITS"; break;
            case UnitConvention::PER_UNIT_COMPONENT_BASE: j = "PER_UNIT_COMPONENT_BASE"; break;
            case UnitConvention::PER_UNIT_SYSTEM_BASE: j = "PER_UNIT_SYSTEM_BASE"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }
}
namespace nlohmann {
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

    inline void adl_serializer<std::variant<quicktype::CtmSolutionSchema, double>>::from_json(const json & j, std::variant<quicktype::CtmSolutionSchema, double> & x) {
        if (j.is_number())
            x = j.get<double>();
        else if (j.is_object())
            x = j.get<quicktype::CtmSolutionSchema>();
        else throw std::runtime_error("Could not deserialise!");
    }

    inline void adl_serializer<std::variant<quicktype::CtmSolutionSchema, double>>::to_json(json & j, const std::variant<quicktype::CtmSolutionSchema, double> & x) {
        switch (x.index()) {
            case 0:
                j = std::get<quicktype::CtmSolutionSchema>(x);
                break;
            case 1:
                j = std::get<double>(x);
                break;
            default: throw std::runtime_error("Input JSON does not conform to schema!");
        }
    }

    inline void adl_serializer<std::variant<quicktype::CtmSolutionSchema, int64_t>>::from_json(const json & j, std::variant<quicktype::CtmSolutionSchema, int64_t> & x) {
        if (j.is_number_integer())
            x = j.get<int64_t>();
        else if (j.is_object())
            x = j.get<quicktype::CtmSolutionSchema>();
        else throw std::runtime_error("Could not deserialise!");
    }

    inline void adl_serializer<std::variant<quicktype::CtmSolutionSchema, int64_t>>::to_json(json & j, const std::variant<quicktype::CtmSolutionSchema, int64_t> & x) {
        switch (x.index()) {
            case 0:
                j = std::get<quicktype::CtmSolutionSchema>(x);
                break;
            case 1:
                j = std::get<int64_t>(x);
                break;
            default: throw std::runtime_error("Input JSON does not conform to schema!");
        }
    }

    inline void adl_serializer<std::variant<std::vector<int64_t>, quicktype::CtmSolutionSchema, int64_t>>::from_json(const json & j, std::variant<std::vector<int64_t>, quicktype::CtmSolutionSchema, int64_t> & x) {
        if (j.is_number_integer())
            x = j.get<int64_t>();
        else if (j.is_object())
            x = j.get<quicktype::CtmSolutionSchema>();
        else if (j.is_array())
            x = j.get<std::vector<int64_t>>();
        else throw std::runtime_error("Could not deserialise!");
    }

    inline void adl_serializer<std::variant<std::vector<int64_t>, quicktype::CtmSolutionSchema, int64_t>>::to_json(json & j, const std::variant<std::vector<int64_t>, quicktype::CtmSolutionSchema, int64_t> & x) {
        switch (x.index()) {
            case 0:
                j = std::get<std::vector<int64_t>>(x);
                break;
            case 1:
                j = std::get<quicktype::CtmSolutionSchema>(x);
                break;
            case 2:
                j = std::get<int64_t>(x);
                break;
            default: throw std::runtime_error("Input JSON does not conform to schema!");
        }
    }

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
}
