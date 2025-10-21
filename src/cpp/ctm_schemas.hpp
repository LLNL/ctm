//  To parse this JSON data, first install
//
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     CtmData data = nlohmann::json::parse(jsonString);
//     CtmSolution data = nlohmann::json::parse(jsonString);
//     CtmTimeSeriesData data = nlohmann::json::parse(jsonString);

#pragma once

#include <optional>
#include <variant>
#include "json.hpp"

#include <unordered_map>

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

namespace ctm_schemas {
    using nlohmann::json;

    #ifndef NLOHMANN_UNTYPED_ctm_schemas_HELPER
    #define NLOHMANN_UNTYPED_ctm_schemas_HELPER
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

    #ifndef NLOHMANN_OPTIONAL_ctm_schemas_HELPER
    #define NLOHMANN_OPTIONAL_ctm_schemas_HELPER
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

    using BusFr = std::variant<int64_t, std::string>;

    /**
     * structure to hold a reference (possibly, to be scaled) to a time series
     */
    struct CmUbAClass {
        /**
         * [-] scale factor to be applied to the pointed-to time series to obtain this field's values
         */
        double scale_factor;
        /**
         * uid of time series (in time_series_data) this reference points to
         */
        BusFr uid;
    };

    using CmUbA = std::variant<CmUbAClass, double>;

    /**
     * structure to hold ac line data using concentrated (6-parameter circuit) PI model
     */
    struct NetworkAcLine {
        /**
         * [S or pu] shunt susceptance of line at from terminal
         */
        std::optional<double> b_fr;
        /**
         * [S or pu] shunt susceptance of line at to terminal
         */
        std::optional<double> b_to;
        /**
         * uid of bus at the from terminal of ac line
         */
        BusFr bus_fr;
        /**
         * uid of bus at the to terminal of ac line
         */
        BusFr bus_to;
        /**
         * [kA or pu] persistent current rating
         */
        std::optional<CmUbA> cm_ub_a;
        /**
         * [kA or pu] 4-hour current rating
         */
        std::optional<CmUbA> cm_ub_b;
        /**
         * [kA or pu] 15-minute current rating
         */
        std::optional<CmUbA> cm_ub_c;
        /**
         * additional ac line parameters currently not supported by CTM
         */
        nlohmann::json ext;
        /**
         * [S or pu] shunt conductance of line at from terminal
         */
        std::optional<double> g_fr;
        /**
         * [S or pu] shunt conductance of line at to terminal
         */
        std::optional<double> g_to;
        /**
         * line name
         */
        std::optional<std::string> name;
        /**
         * [MVA] nominal apparent power of ac line
         */
        std::optional<double> nominal_mva;
        /**
         * [hours] expected duration of persistent outage (time between outage and crews
         * re-energizing the branch)
         */
        std::optional<double> persistent_outage_duration;
        /**
         * [events/year] number of expected persistent outages per year (outages not cleared by
         * reconnectors)
         */
        std::optional<double> persistent_outage_rate;
        /**
         * [Ohm or pu] series resistance of line
         */
        double r;
        /**
         * [MVA or pu] persistent apparent power rating
         */
        std::optional<CmUbA> sm_ub_a;
        /**
         * [MVA or pu] 4-hour apparent power rating
         */
        std::optional<CmUbA> sm_ub_b;
        /**
         * [MVA or pu] 15-minute apparent power rating
         */
        std::optional<CmUbA> sm_ub_c;
        int64_t status;
        /**
         * [events/year] number of expected transient outages per year (outages cleared by
         * reconnectors)
         */
        std::optional<double> transient_outage_rate;
        BusFr uid;
        /**
         * [deg] voltage angle difference lower bound (stability)
         */
        std::optional<double> vad_lb;
        /**
         * [deg] voltage angle difference upper bound (stability)
         */
        std::optional<double> vad_ub;
        /**
         * [Ohm or pu] series impedance of line
         */
        double x;
    };

    /**
     * geographical subset of the electrical network with common Automatic Generation Control
     * (AGC) and responsible for its Area Control Error (ACE)
     */
    struct Area {
        /**
         * additional area parameters currently not supported by CTM
         */
        nlohmann::json ext;
        /**
         * area name
         */
        std::optional<std::string> name;
        /**
         * binary indicator of whether area should be included or omitted (if omitted all elements
         * within area should be omitted); 1=>included, 0=>omitted
         */
        int64_t status;
        BusFr uid;
    };

    enum class TypeEnum : int { PQ, PV, SLACK };

    using TypeUnion = std::variant<CmUbAClass, TypeEnum>;

    using VmLb = std::variant<CmUbAClass, double>;

    /**
     * structure to hold bus data
     */
    struct NetworkBus {
        /**
         * uid for area to which bus belongs to
         */
        std::optional<BusFr> area;
        /**
         * bus base (nominal) voltage
         */
        double base_kv;
        /**
         * additional bus parameters currently not supported by CTM
         */
        nlohmann::json ext;
        /**
         * bus name
         */
        std::optional<std::string> name;
        int64_t status;
        /**
         * bus type for power flow calculations (PV, PQ, or slack)
         */
        std::optional<TypeUnion> type;
        BusFr uid;
        /**
         * bus voltage lower bound
         */
        std::optional<VmLb> vm_lb;
        /**
         * bus voltage upper bound
         */
        std::optional<VmLb> vm_ub;
        /**
         * uid for zone to which bus belongs to
         */
        std::optional<BusFr> zone;
    };

    /**
     * type of generation cost model (i.e., function translating power/energy to money);
     * POLYNOMIAL => cost_pg_parameters is an array with n+1 coefficients <a_i> for f(x) = a_0 +
     * a_1 x^1 + ... + a_n x^n; PIECEWISE_LINEAR => cost_pg_parameters is a series of values
     * <x_i, f_i> and cost (f) should be interpolated linearly in between points; MARGINAL_COST
     * => cost_pg_parameters is a series of values <b_i, m_i>, where m_i is a marginal cost
     * ($/MWh or $/(pu*h)) and b_i is the amoung of power (MWh or pu*h) sold at marginal cost m_i
     */
    enum class CostPgModel : int { MARGINAL_COST, PIECEWISE_LINEAR, POLYNOMIAL };

    /**
     * pairs of data points saved as two vectors (of the same length)
     *
     * structure to hold a reference (possibly, to be scaled) to a time series
     */
    struct CostPgParametersClass {
        std::optional<std::vector<double>> x;
        std::optional<std::vector<double>> y;
        /**
         * [-] scale factor to be applied to the pointed-to time series to obtain this field's values
         */
        std::optional<double> scale_factor;
        /**
         * uid of time series (in time_series_data) this reference points to
         */
        std::optional<BusFr> uid;
    };

    using CostPgParameters = std::variant<std::vector<double>, CostPgParametersClass>;

    /**
     * primary energy source
     */
    enum class PrimarySource : int { BIOMASS, COAL, GAS, GEOTHERMAL, HYDRO, NUCLEAR, OIL, OTHER, SOLAR, WIND };

    /**
     * subtype of primary energy source; thermal classification taken from
     * https://www.eia.gov/survey/form/eia_923/instructions.pdf
     */
    enum class PrimarySourceSubtype : int { AG_BIPRODUCT, ANTRHC_BITMN_COAL, DISTILLATE_FUEL_OIL, GEOTHERMAL, HYDRO_DAM, HYDRO_PUMPED_STORAGE, HYDRO_RUN_OF_THE_RIVER, MUNICIPAL_WASTE, NATURAL_GAS, NUCLEAR, OTHER, OTHER_GAS, PETROLEUM_COKE, RESIDUAL_FUEL_OIL, SOLAR_CSP, SOLAR_PV, WASTE_COAL, WASTE_OIL, WIND_OFFSHORE, WIND_ONSHORE, WOOD_WASTE };

    using ServiceRequired = std::variant<CmUbAClass, int64_t>;

    /**
     * structure to hold generator data
     */
    struct NetworkGen {
        /**
         * uid of bus to which generator is connected to
         */
        BusFr bus;
        /**
         * type of generation cost model (i.e., function translating power/energy to money);
         * POLYNOMIAL => cost_pg_parameters is an array with n+1 coefficients <a_i> for f(x) = a_0 +
         * a_1 x^1 + ... + a_n x^n; PIECEWISE_LINEAR => cost_pg_parameters is a series of values
         * <x_i, f_i> and cost (f) should be interpolated linearly in between points; MARGINAL_COST
         * => cost_pg_parameters is a series of values <b_i, m_i>, where m_i is a marginal cost
         * ($/MWh or $/(pu*h)) and b_i is the amoung of power (MWh or pu*h) sold at marginal cost m_i
         */
        std::optional<CostPgModel> cost_pg_model;
        /**
         * parameters of generation cost function, can be time dependent
         */
        std::optional<CostPgParameters> cost_pg_parameters;
        /**
         * [h] minimim time the unit can be out of service (a.k.a., minimum down time)
         */
        std::optional<double> down_time_lb;
        /**
         * additional gen parameters currently not supported by CTM
         */
        nlohmann::json ext;
        /**
         * [-] fraction of time the generator is out of service because of forced outages (i.e.,
         * hours out of service---because of failures---during a year, divided by 8760)
         */
        std::optional<double> forced_outage_rate;
        /**
         * [h] minimim time the unit can be in service (a.k.a., minimum up time)
         */
        std::optional<double> in_service_time_lb;
        /**
         * [h] maximum time the unit can be in service (commitment == 1)
         */
        std::optional<double> in_service_time_ub;
        /**
         * [h] mean time to occurence of a failure; failures can be assumed to follow a Poisson
         * process
         */
        std::optional<double> mean_time_to_failure;
        /**
         * [h] mean time to repair a failure
         */
        std::optional<double> mean_time_to_repair;
        /**
         * generator name
         */
        std::optional<std::string> name;
        /**
         * [MVA] nominal apparent power of generator (nameplate capacity)
         */
        std::optional<double> nominal_mva;
        /**
         * [MW/h or pu/h] maximum active power decrease per hour
         */
        std::optional<double> pg_delta_lb;
        /**
         * [MW/h or pu/h] maximum active power increase per hour
         */
        std::optional<double> pg_delta_ub;
        /**
         * [MW or pu] lower bound of active power injection (rectangular operating zone)
         */
        std::optional<VmLb> pg_lb;
        /**
         * [MW or pu] upper bound of active power injection (rectangular operating zone)
         */
        std::optional<VmLb> pg_ub;
        /**
         * primary energy source
         */
        std::optional<PrimarySource> primary_source;
        /**
         * subtype of primary energy source; thermal classification taken from
         * https://www.eia.gov/survey/form/eia_923/instructions.pdf
         */
        std::optional<PrimarySourceSubtype> primary_source_subtype;
        /**
         * [MVAr or pu] lower bound of reactive power injection (rectangular operating zone)
         */
        std::optional<VmLb> qg_lb;
        /**
         * [MVAr or pu] upper bound of reactive power injection (rectangular operating zone)
         */
        std::optional<VmLb> qg_ub;
        /**
         * [-] fraction of time the generator is out of service because of scheduled maintenance
         * (i.e., hours out of service---because of scheduled maintenance---during a year, divided
         * by 8760)
         */
        std::optional<double> scheduled_maintenance_rate;
        /**
         * whether generator must be in service (e.g., nuclear power plant) or out of service (e.g.,
         * generator during maintenance or after an outage); 0 => no requirement, 1 => fixed in
         * service, 2 => fixed out of service
         */
        std::optional<ServiceRequired> service_required;
        /**
         * [$] cost of shutting down the unit
         */
        std::optional<VmLb> shutdown_cost;
        /**
         * [$] cost of starting the unit after being off > startup_time_warm hours
         */
        std::optional<VmLb> startup_cost_cold;
        /**
         * [$] cost of starting the unit after being off <= startup_time_hot hours
         */
        std::optional<VmLb> startup_cost_hot;
        /**
         * [$] cost of starting the unit after being off > startup_time_hot hours, but <=
         * startup_time_warm hours
         */
        std::optional<VmLb> startup_cost_warm;
        /**
         * [h] maximum time the unit can be off before a hot startup
         */
        std::optional<double> startup_time_hot;
        /**
         * [h] maximum time the unit can be off before a warm startup
         */
        std::optional<double> startup_time_warm;
        int64_t status;
        BusFr uid;
        /**
         * [kV or pu] target voltage magnitude of the bus that this generator connects to
         */
        std::optional<VmLb> vm_setpoint;
    };

    /**
     * units used for physical network parameters
     */
    enum class UnitConvention : int { NATURAL_UNITS, PER_UNIT_COMPONENT_BASE, PER_UNIT_SYSTEM_BASE };

    /**
     * structure to hold global settings for parameters in the network
     */
    struct NetworkGlobalParams {
        /**
         * [MVA] system-wide apparent power base
         */
        std::optional<double> base_mva;
        /**
         * UID of reference bus of the electrical network
         */
        std::optional<BusFr> bus_ref;
        /**
         * units used for physical network parameters
         */
        UnitConvention unit_convention;
    };

    /**
     * power conversion technology
     */
    enum class Technology : int { LCC, MMC, VSC };

    /**
     * structure to hold point-to-point hvdc line data
     */
    struct NetworkHvdcP2P {
        /**
         * [kV] base voltage at the dc side
         */
        std::optional<double> base_kv_dc;
        /**
         * uid of bus at the from terminal of hvdc line
         */
        BusFr bus_fr;
        /**
         * uid of bus at the to terminal of hvdc line
         */
        BusFr bus_to;
        /**
         * [kA or pu] ac persistent current rating, from terminal (if in pu, use from bus base_kv)
         */
        std::optional<CmUbA> cm_ub_fr;
        /**
         * [kA or pu] ac persistent current rating, to terminal (if in pu, use to bus base_kv)
         */
        std::optional<CmUbA> cm_ub_to;
        /**
         * additional hvdc point-to-point parameters currently not supported by CTM
         */
        nlohmann::json ext;
        /**
         * [MW or pu] standby loss
         */
        std::optional<double> loss_a;
        /**
         * [kV or pu] loss proportional to current magnitude (if in pu, base voltage corresponds to
         * base_kv_dc)
         */
        std::optional<double> loss_b;
        /**
         * [Ohm or pu] loss proportional to current magnitude squared (if in pu, base voltage
         * corresponds to base_kv_dc)
         */
        std::optional<double> loss_c;
        /**
         * HVDC line name
         */
        std::optional<std::string> name;
        /**
         * [MVA] nominal apparent power of hvdc line
         */
        std::optional<double> nominal_mva;
        /**
         * number of poles; 1 => monopole, 2 => bipole
         */
        std::optional<int64_t> p;
        /**
         * [MW or pu] minimum active power entering hvdc line at from bus
         */
        std::optional<VmLb> pdc_fr_lb;
        /**
         * [MW or pu] maximum active power entering hvdc line at from bus
         */
        std::optional<VmLb> pdc_fr_ub;
        /**
         * [MW or pu] minimum active power entering hvdc line at to bus
         */
        std::optional<VmLb> pdc_to_lb;
        /**
         * [MW or pu] maximum active power entering hvdc line at to bus
         */
        std::optional<VmLb> pdc_to_ub;
        /**
         * [hours] expected duration of persistent outage (time between outage and crews
         * re-energizing the branch)
         */
        std::optional<double> persistent_outage_duration;
        /**
         * [events/year] number of expected persistent outages per year (outages not cleared by
         * reconnectors)
         */
        std::optional<double> persistent_outage_rate;
        /**
         * [deg] only meaningful if technology == LCC; firing angle minimum
         */
        std::optional<double> phi_lb;
        /**
         * [deg] only meaningful if technology == LCC; firing angle maximum
         */
        std::optional<double> phi_ub;
        /**
         * [MVAr or pu] minimum reactive power entering hvdc line at from bus
         */
        std::optional<VmLb> qdc_fr_lb;
        /**
         * [MVAr or pu] maximum reactive power entering hvdc line at from bus
         */
        std::optional<VmLb> qdc_fr_ub;
        /**
         * [MVAr or pu] minimum reactive power entering hvdc line at to bus
         */
        std::optional<VmLb> qdc_to_lb;
        /**
         * [MW or pu] maximum active power entering hvdc line at to bus
         */
        std::optional<VmLb> qdc_to_ub;
        /**
         * [Ohm or pu] dc line resistance (if in pu, base voltage corresponds to base_kv_dc)
         */
        std::optional<double> r;
        /**
         * [MVA or pu] ac persistent apparent power rating
         */
        std::optional<CmUbA> sm_ub;
        int64_t status;
        /**
         * power conversion technology
         */
        std::optional<Technology> technology;
        /**
         * [events/year] number of expected transient outages per year (outages cleared by
         * reconnectors or other)
         */
        std::optional<double> transient_outage_rate;
        BusFr uid;
        /**
         * [kV or pu] minimum voltage at the dc side
         */
        std::optional<double> vm_dc_lb;
        /**
         * [kV or pu] maximum voltage at the dc side
         */
        std::optional<double> vm_dc_ub;
    };

    /**
     * structure to hold load (consumer) data using ZIP model
     */
    struct Load {
        /**
         * uid of bus to which load is connected to
         */
        BusFr bus;
        /**
         * additional bus parameters currently not supported by CTM
         */
        nlohmann::json ext;
        /**
         * load name
         */
        std::optional<std::string> name;
        /**
         * [MVA] nominal power of load
         */
        std::optional<double> nominal_mva;
        /**
         * active power demand
         */
        VmLb pd;
        /**
         * constant current active power demand at v_bus = 1.0 pu
         */
        std::optional<VmLb> pd_i;
        /**
         * constant impedance active power demand at v_bus = 1.0 pu
         */
        std::optional<VmLb> pd_y;
        /**
         * reactive power demand
         */
        VmLb qd;
        /**
         * constant current reactive power demand at v_bus = 1.0 pu
         */
        std::optional<VmLb> qd_i;
        /**
         * constant impedance reactive power demand at v_bus = 1.0 pu
         */
        std::optional<VmLb> qd_y;
        int64_t status;
        BusFr uid;
    };

    /**
     * structure to hold n-winding (n >= 3) transformer and phase shifter data using simplified
     * star model (2 circuit parameters per winding and 2 circuit parameters for magnetizing
     * branch between internal star node and neutral)
     */
    struct NetworkMultipleWindingTransformer {
        /**
         * [S or pu] shunt susceptance of transformer at internal star node (magnetizing branch)
         */
        double b;
        /**
         * array of uids of buses of transformer at terminal of winding=[1,2,...,num_windings]
         */
        std::vector<BusFr> bus_w;
        /**
         * [kA or pu] array of persistent current ratings of winding=[1,2,...,num_windings]
         */
        std::optional<std::vector<CmUbA>> cm_ub_a_w;
        /**
         * [kA or pu] array of 4-hour current ratings of winding=[1,2,...,num_windings]
         */
        std::optional<std::vector<CmUbA>> cm_ub_b_w;
        /**
         * [kA or pu] array of 15-minute current ratings of winding=[1,2,...,num_windings]
         */
        std::optional<std::vector<CmUbA>> cm_ub_c_w;
        /**
         * additional n-winding transformer parameters currently not supported by CTM
         */
        nlohmann::json ext;
        /**
         * [S or pu] shunt conductance of transformer at internal star node (magnetizing branch)
         */
        double g;
        /**
         * transformer name
         */
        std::optional<std::string> name;
        /**
         * [MVA] array of nominal apparent powers of winding=[1,2,...,num_windings] of transformer
         */
        std::optional<std::vector<double>> nominal_mva_w;
        /**
         * number of windings, greater or equal to 3 (for 2-winding transformers, use 'transformer'
         * object instead)
         */
        int64_t num_windings;
        /**
         * [hours] expected duration of persistent outage (time between outage and crews
         * re-energizing the branch)
         */
        std::optional<double> persistent_outage_duration;
        /**
         * [events/year] number of expected persistent outages per year (outages not cleared by
         * reconnectors)
         */
        std::optional<double> persistent_outage_rate;
        /**
         * [Ohm or pu] array of series resistances of winding=[1,2,...,num_windings] of transformer
         */
        std::vector<double> r_w;
        /**
         * [MVA or pu] array of persistent apparent power ratings of winding=[1,2,...,num_windings]
         */
        std::optional<std::vector<CmUbA>> sm_ub_a_w;
        /**
         * [MVA or pu] array of 4-hour apparent power ratings of winding=[1,2,...,num_windings]
         */
        std::optional<std::vector<CmUbA>> sm_ub_b_w;
        /**
         * [MVA or pu] array of 15-minute apparent power ratings of winding=[1,2,...,num_windings]
         */
        std::optional<std::vector<CmUbA>> sm_ub_c_w;
        int64_t status;
        /**
         * array of status of winding=[1,2,...,num_windings] (provided status=1, status_w[w]=0
         * indicates winding w is open, whereas status_w[w]=1 indicates winding is connected; if
         * status=0, all windings are assumed disconnected, regardless of values in status_w)
         */
        std::vector<int64_t> status_w;
        /**
         * [deg] array of minimum angle phase shifts (angle difference = va_w - va_star -
         * angle_shift) of winding=[1,2,...,num_windings]
         */
        std::optional<std::vector<double>> ta_lb_w;
        /**
         * array of number of discrete steps between ta_lb_w and ta_ub_w (including limit values) of
         * winding=[1,2,...,num_windings]
         */
        std::optional<std::vector<int64_t>> ta_steps_w;
        /**
         * [deg] array of maximum angle phase shifts (angle difference = va_w - va_star -
         * angle_shift) of winding=[1,2,...,num_windings]
         */
        std::optional<std::vector<double>> ta_ub_w;
        /**
         * [-] array of minimum tap ratios of winding=[1,2,...,num_windings] (1.0 correspond to
         * nominal ratio, inner_vm_w = vm_w * tap_value)
         */
        std::optional<std::vector<double>> tm_lb_w;
        /**
         * array of number of discrete steps between tm_lb_w and tm_ub_w (including limit values) of
         * winding=[1,2,...,num_windings]
         */
        std::optional<std::vector<int64_t>> tm_steps_w;
        /**
         * [-] array of maximum tap ratios of winding=[1,2,...,num_windings] (1.0 correspond to
         * nominal ratio, inner_vm_w = vm_w * tap_value)
         */
        std::optional<std::vector<double>> tm_ub_w;
        BusFr uid;
        /**
         * [Ohm or pu] array of series impedances of winding=[1,2,...,num_windings] of transformer
         */
        std::vector<double> x_w;
    };

    struct NetworkSwitch {
        /**
         * uid of bus at the from terminal of switch
         */
        BusFr bus_fr;
        /**
         * uid of bus at the to terminal of switch
         */
        BusFr bus_to;
        /**
         * [kA or pu] current limit
         */
        std::optional<double> cm_ub;
        /**
         * additional switch parameters currently not supported by CTM
         */
        nlohmann::json ext;
        /**
         * name of switch
         */
        std::optional<std::string> name;
        /**
         * [MVA] nominal apparent power of switch (nameplate capacity)
         */
        std::optional<double> nominal_mva;
        /**
         * [MVA or pu] apparent power flow limit
         */
        std::optional<double> sm_ub;
        int64_t status;
        BusFr uid;
    };

    enum class ReserveType : int { PRIMARY, SECONDARY, TERTIARY };

    /**
     * structure to hold reserve product and requirement data
     */
    struct NetworkReserve {
        /**
         * additional reserve parameters currently not supported by CTM
         */
        nlohmann::json ext;
        /**
         * name of reserve product
         */
        std::optional<std::string> name;
        /**
         * uid of generators contributing to this reserve
         */
        std::optional<std::vector<BusFr>> participants;
        /**
         * [MW or pu] downward active power required by this reserve
         */
        std::optional<CmUbA> pg_down;
        /**
         * [MW or pu] upward active power required by this reserve
         */
        std::optional<CmUbA> pg_up;
        ReserveType reserve_type;
        int64_t status;
        BusFr uid;
    };

    using Bs = std::variant<std::vector<double>, double>;

    using Gs = std::variant<std::vector<double>, double>;

    using NumStepsUbUnion = std::variant<std::vector<int64_t>, int64_t>;

    /**
     * structure to hold shunt data
     */
    struct NetworkShunt {
        /**
         * [MVAr or pu] reactive power demand at v_bus = 1.0 pu, per step of each shunt section
         */
        Bs bs;
        /**
         * uid of bus to which shunt is connected to
         */
        BusFr bus;
        /**
         * additional shunt parameters currently not supported by CTM
         */
        nlohmann::json ext;
        /**
         * [MW or pu] active power demand at v_bus = 1.0 pu, per step of each shunt section
         */
        Gs gs;
        /**
         * shunt name
         */
        std::optional<std::string> name;
        /**
         * [MVA] nominal apparent power of shunt (nameplate capacity)
         */
        std::optional<double> nominal_mva;
        /**
         * upper bound for number of energized steps of shunt section (lower bound is always 0)
         */
        NumStepsUbUnion num_steps_ub;
        int64_t status;
        BusFr uid;
    };

    using ChargeEfficiency = std::variant<CmUbAClass, double>;

    /**
     * structure to hold storage (battery) data
     */
    struct NetworkStorage {
        /**
         * uid of bus to which generator is connected to
         */
        BusFr bus;
        /**
         * [-] charge efficiency, in (0, 1]
         */
        ChargeEfficiency charge_efficiency;
        /**
         * [MW or pu] maximum rate of charge
         */
        std::optional<CmUbA> charge_ub;
        /**
         * [kA or pu] converter current output rating
         */
        std::optional<double> cm_ub;
        /**
         * [-] discharge efficiency, in (0, 1]
         */
        ChargeEfficiency discharge_efficiency;
        /**
         * [MW or pu] maximum rate of discharge
         */
        std::optional<CmUbA> discharge_ub;
        /**
         * [MWh or pu*h] maximum state of charge
         */
        std::optional<double> energy_ub;
        /**
         * additional storage parameters currently not supported by CTM
         */
        nlohmann::json ext;
        /**
         * storage name
         */
        std::optional<std::string> name;
        /**
         * [MVA] nominal apparent power of storage (nameplate capacity)
         */
        std::optional<double> nominal_mva;
        /**
         * [MW/h or pu/h] maximum active power decrease per hour
         */
        std::optional<double> ps_delta_lb;
        /**
         * [MW/h or pu/h] maximum active power increase per hour
         */
        std::optional<double> ps_delta_ub;
        /**
         * converter standby active power exogenous draw
         */
        double ps_ex;
        /**
         * converter standby reactive power exogenous draw
         */
        double qs_ex;
        /**
         * [MVAr or pu] minumum reactive power injection
         */
        std::optional<VmLb> qs_lb;
        /**
         * [MVAr or pu] maximum reactive power injection
         */
        std::optional<VmLb> qs_ub;
        /**
         * [MVA or pu] converter apparent power rating
         */
        std::optional<double> sm_ub;
        int64_t status;
        BusFr uid;
    };

    /**
     * structure to hold 2-winding transformer and phase shifter data using simplified
     * (4-parameter circuit) model
     */
    struct NetworkTransformer {
        /**
         * [S or pu] shunt susceptance of transformer at from terminal (magnetizing branch)
         */
        double b;
        /**
         * uid of bus at the from terminal of transformer
         */
        BusFr bus_fr;
        /**
         * uid of bus at the to terminal of transformer
         */
        BusFr bus_to;
        /**
         * [kA or pu] persistent current rating, referred to from side
         */
        std::optional<CmUbA> cm_ub_a;
        /**
         * [kA or pu] 4-hour current rating, referred to from side
         */
        std::optional<CmUbA> cm_ub_b;
        /**
         * [kA or pu] 15-minute current rating, referred to from side
         */
        std::optional<CmUbA> cm_ub_c;
        /**
         * additional transformer parameters currently not supported by CTM
         */
        nlohmann::json ext;
        /**
         * [S or pu] shunt conductance of transformer at from terminal (magnetizing branch)
         */
        double g;
        /**
         * transformer name
         */
        std::optional<std::string> name;
        /**
         * [MVA] nominal apparent power of transformer
         */
        std::optional<double> nominal_mva;
        /**
         * [hours] expected duration of persistent outage (time between outage and crews
         * re-energizing the branch)
         */
        std::optional<double> persistent_outage_duration;
        /**
         * [events/year] number of expected persistent outages per year (outages not cleared by
         * reconnectors)
         */
        std::optional<double> persistent_outage_rate;
        /**
         * [Ohm or pu] series resistance of line
         */
        double r;
        /**
         * [MVA or pu] persistent apparent power rating, referred to from side
         */
        std::optional<CmUbA> sm_ub_a;
        /**
         * [MVA or pu] 4-hour apparent power rating, referred to from side
         */
        std::optional<CmUbA> sm_ub_b;
        /**
         * [MVA or pu] 15-minute apparent power rating, referred to from side
         */
        std::optional<CmUbA> sm_ub_c;
        int64_t status;
        /**
         * [deg] minimum angle phase shift (angle difference = va_from - va_to - angle_shift)
         */
        std::optional<double> ta_lb;
        /**
         * number of discrete steps between ta_lb and ta_ub (including limit values)
         */
        std::optional<int64_t> ta_steps;
        /**
         * [deg] maximum angle phase shift (angle difference = va_from - va_to - angle_shift)
         */
        std::optional<double> ta_ub;
        /**
         * [-] minimum tap ratio (1.0 correspond to nominal ratio, inner_vm_from = vm_from *
         * tap_value)
         */
        std::optional<double> tm_lb;
        /**
         * number of discrete steps between tm_lb and tm_ub (including limit values)
         */
        std::optional<int64_t> tm_steps;
        /**
         * [-] maximum tap ratio (1.0 correspond to nominal ratio, inner_vm_from = vm_from *
         * tap_value)
         */
        std::optional<double> tm_ub;
        BusFr uid;
        /**
         * [Ohm or pu] series impedance of line
         */
        double x;
    };

    /**
     * geographical subset of the electrical network commonly associated with market purposes
     * (e.g., define sub-markets within a large interconnected system, defining different areas
     * for reserve products, etc.)
     */
    struct Zone {
        /**
         * additional zone parameters currently not supported by CTM
         */
        nlohmann::json ext;
        /**
         * zone name
         */
        std::optional<std::string> name;
        /**
         * binary indicator of whether zone should be included or omitted (if omitted all elements
         * within zone should be omitted); 1=>included, 0=>omitted
         */
        int64_t status;
        BusFr uid;
    };

    /**
     * structure to hold persistent network data
     */
    struct Network {
        std::optional<std::vector<NetworkAcLine>> ac_line;
        std::vector<Area> area;
        std::vector<NetworkBus> bus;
        std::vector<NetworkGen> gen;
        /**
         * structure to hold global settings for parameters in the network
         */
        NetworkGlobalParams global_params;
        std::optional<std::vector<NetworkHvdcP2P>> hvdc_p2_p;
        std::vector<Load> load;
        std::optional<std::vector<NetworkMultipleWindingTransformer>> multiple_winding_transformer;
        std::optional<std::vector<NetworkReserve>> reserve;
        std::optional<std::vector<NetworkShunt>> shunt;
        std::optional<std::vector<NetworkStorage>> storage;
        std::optional<std::vector<NetworkSwitch>> network_switch;
        std::optional<std::vector<NetworkTransformer>> transformer;
        std::optional<std::vector<Zone>> zone;
    };

    /**
     * structure to hold initial state of bus variables
     */
    struct TemporalBoundaryBus {
        /**
         * additional bus initial condition parameters currently not supported by CTM
         */
        nlohmann::json ext;
        /**
         * uid of bus this record refers to
         */
        BusFr uid;
        /**
         * [deg] initial voltage angle
         */
        double va;
        /**
         * [kV or pu] initial voltage magnitude
         */
        std::optional<double> vm;
    };

    /**
     * structure to hold initial state of generator variables
     */
    struct TemporalBoundaryGen {
        /**
         * [h] if in service, zero, else time the unit has been out of service
         */
        std::optional<double> down_time;
        /**
         * additional generator initial condition parameters currently not supported by CTM
         */
        nlohmann::json ext;
        /**
         * [h] if in service, time the unit has been in service, zero otherwise
         */
        std::optional<double> in_service_time;
        /**
         * [MW or pu] initial active power injection
         */
        double pg;
        /**
         * [MW or pu] initial reactive power injection
         */
        std::optional<double> qg;
        /**
         * uid of generator this record refers to
         */
        BusFr uid;
    };

    /**
     * structure to hold global parameters of temporal boundary
     */
    struct TemporalBoundaryGlobalParams {
        /**
         * [seconds] time elapsed since temporal_boundary conditions where present in the system
         */
        double time_elapsed;
    };

    /**
     * structure to hold initial state of hvdc point-to-point line variables
     */
    struct TemporalBoundaryHvdcP2P {
        /**
         * additional hvdc point-to-point line initial condition parameters currently not supported
         * by CTM
         */
        nlohmann::json ext;
        /**
         * [MW or pu] initial active power entering hvdc line at from bus
         */
        double pdc_fr;
        /**
         * [MW or pu] initial active power entering hvdc line at to bus
         */
        double pdc_to;
        /**
         * [MVAr or pu] initial reactive power entering hvdc line at from bus
         */
        std::optional<double> qdc_fr;
        /**
         * [MVAr or pu] initial reactive power entering hvdc line at to bus
         */
        std::optional<double> qdc_to;
        /**
         * uid of hvdc point-to-point this record refers to
         */
        BusFr uid;
        /**
         * [kV or pu] initial dc side voltage at from converter
         */
        std::optional<double> vm_dc_fr;
        /**
         * [kV or pu] initial dc side voltage at to converter
         */
        std::optional<double> vm_dc_to;
    };

    using TaW = std::optional<std::variant<std::vector<double>, bool, double, int64_t, std::map<std::string, nlohmann::json>, std::string>>;

    /**
     * structure to hold initial state of n-winding transformer variables
     */
    struct TemporalBoundaryMultipleWindingTransformer {
        /**
         * additional n-winding transformer initial condition parameters currently not supported by
         * CTM
         */
        nlohmann::json ext;
        /**
         * [deg] array of initial angle phase shifts for winding=[1,2,...,num_windings]
         */
        TaW ta_w;
        /**
         * [-] array of initial tap ratios for winding=[1,2,...,num_windings]
         */
        std::vector<double> tm_w;
        BusFr uid;
        /**
         * [deg] initial voltage angle of internal star node
         */
        double va_star_node;
        /**
         * [pu] initial voltage magnitude of internal star node
         */
        double vm_star_node;
    };

    /**
     * structure to hold initial state of shunt variables
     */
    struct TemporalBoundaryShunt {
        /**
         * additional shunt initial condition parameters currently not supported by CTM
         */
        nlohmann::json ext;
        /**
         * [-] number of initial energized steps per section
         */
        NumStepsUbUnion num_steps;
        /**
         * uid of shunt this record refers to
         */
        BusFr uid;
    };

    /**
     * structure to hold initial state of storage variables
     */
    struct TemporalBoundaryStorage {
        /**
         * [MWh or pu*h] initial state of charge
         */
        double energy;
        /**
         * additional storage initial condition parameters currently not supported by CTM
         */
        nlohmann::json ext;
        /**
         * [MW or pu] initial active power injection
         */
        std::optional<double> ps;
        /**
         * [MW or pu] initial reactive power injection
         */
        std::optional<double> qs;
        /**
         * uid of storage this record refers to
         */
        BusFr uid;
    };

    /**
     * structure to hold initial state of switch variables
     */
    struct TemporalBoundarySwitch {
        /**
         * additional switch initial condition parameters currently not supported by CTM
         */
        nlohmann::json ext;
        /**
         * [-] binary indicator of switch initial status; 0 => open, 1 => closed
         */
        int64_t state;
        /**
         * uid of switch this record refers to
         */
        BusFr uid;
    };

    /**
     * structure to hold initial state of transformer variables
     */
    struct TemporalBoundaryTransformer {
        /**
         * additional transformer initial condition parameters currently not supported by CTM
         */
        nlohmann::json ext;
        /**
         * [deg] initial angle phase shift
         */
        double ta;
        /**
         * [-] initial tap ratio
         */
        double tm;
        BusFr uid;
    };

    /**
     * structure to hold data on initial conditions of power system (state prior to start of
     * time series data)
     */
    struct TemporalBoundary {
        std::optional<std::vector<TemporalBoundaryBus>> bus;
        std::optional<std::vector<TemporalBoundaryGen>> gen;
        /**
         * structure to hold global parameters of temporal boundary
         */
        TemporalBoundaryGlobalParams global_params;
        std::optional<std::vector<TemporalBoundaryHvdcP2P>> hvdc_p2_p;
        std::optional<std::vector<TemporalBoundaryMultipleWindingTransformer>> multiple_winding_transformer;
        std::optional<std::vector<TemporalBoundaryShunt>> shunt;
        std::optional<std::vector<TemporalBoundaryStorage>> storage;
        std::optional<std::vector<TemporalBoundarySwitch>> temporal_boundary_switch;
        std::optional<std::vector<TemporalBoundaryTransformer>> transformer;
    };

    using PathToFile = std::variant<std::vector<std::string>, std::string>;

    /**
     * structure to contain all time variant data of the system/case. All time series are
     * synchronized to the same timestamps, which should should be stored using Unix time.
     * Structure is quasi-tabular, with uid, name, path_to_file, values, and ext being arrays in
     * the same order of said field. This is done in order to allow for better compression
     * (e.g., using HDF5) for the values field.
     */
    struct CtmDataTimeSeriesData {
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
        std::vector<BusFr> uid;
        /**
         * array of time series values
         */
        std::optional<std::vector<std::vector<nlohmann::json>>> values;
    };

    /**
     * Common Transmission Model (CTM) Data Schema v0.1
     */
    struct CtmData {
        /**
         * release version of CTM specification
         */
        std::string ctm_version;
        /**
         * structure to hold persistent network data
         */
        Network network;
        /**
         * structure to hold data on initial conditions of power system (state prior to start of
         * time series data)
         */
        TemporalBoundary temporal_boundary;
        /**
         * structure to contain all time variant data of the system/case. All time series are
         * synchronized to the same timestamps, which should should be stored using Unix time.
         * Structure is quasi-tabular, with uid, name, path_to_file, values, and ext being arrays in
         * the same order of said field. This is done in order to allow for better compression
         * (e.g., using HDF5) for the values field.
         */
        std::optional<CtmDataTimeSeriesData> time_series_data;
    };

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
        BusFr uid;
    };

    using PlFr = std::variant<CtmSolutionSchema, double>;

    /**
     * structure to hold switch solution data
     */
    struct SolutionAcLine {
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
        BusFr uid;
    };

    /**
     * structure to hold bus solution data
     */
    struct SolutionBus {
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
        BusFr uid;
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
        BusFr uid;
    };

    /**
     * structure to hold generator solution data
     */
    struct SolutionGen {
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
        BusFr uid;
    };

    /**
     * structure to hold global settings for parameters in the network
     */
    struct SolutionGlobalParams {
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
    struct SolutionHvdcP2P {
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
        BusFr uid;
        /**
         * [kV or pu] voltage at the dc side
         */
        std::optional<PlFr> vm_dc;
    };

    /**
     * structure to hold n-winding (n>=3) transformer solution data
     */
    struct SolutionMultipleWindingTransformer {
        /**
         * additional n-winding transformer parameters currently not supported by CTM
         */
        nlohmann::json ext;
        /**
         * [MW or pu] array of active power entering the transformer at the terminal corresponding
         * to winding=[1,2,...,num_windings]
         */
        std::optional<std::vector<PlFr>> pt_w;
        /**
         * [MVAr or pu] array of reactive power entering the transformer at the terminal
         * corresponding to winding=[1,2,...,num_windings]
         */
        std::optional<std::vector<PlFr>> qt_w;
        /**
         * [deg] array of angle phase shifts for winding=[1,2,...,num_windings]
         */
        std::optional<std::vector<PlFr>> ta_w;
        /**
         * [-] array of tap ratios for winding=[1,2,...,num_windings]
         */
        std::optional<std::vector<PlFr>> tm_w;
        BusFr uid;
        /**
         * [deg] voltage angle of internal star node
         */
        std::optional<PlFr> va_star_node;
        /**
         * [pu] voltage magnitude of internal star node
         */
        std::optional<PlFr> vm_star_node;
    };

    /**
     * structure to hold reserve product solution data
     */
    struct SolutionReserve {
        /**
         * additional reserve parameters currently not supported by CTM
         */
        nlohmann::json ext;
        /**
         * [MW or pu] shortfall on reserve product
         */
        PlFr shortfall;
        BusFr uid;
    };

    using PurpleNumSteps = std::variant<std::vector<int64_t>, CtmSolutionSchema, int64_t>;

    /**
     * structure to hold shunt solution data
     */
    struct SolutionShunt {
        /**
         * additional shunt parameters currently not supported by CTM
         */
        nlohmann::json ext;
        /**
         * number of energized steps of shunt section (lower bound is always 0)
         */
        PurpleNumSteps num_steps;
        BusFr uid;
    };

    /**
     * structure to hold switch solution data
     */
    struct SolutionSwitch {
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
        BusFr uid;
    };

    /**
     * structure to hold storage (battery) solution data
     */
    struct SolutionStorage {
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
        BusFr uid;
    };

    /**
     * structure to hold 2-winding transformer solution data
     */
    struct SolutionTransformer {
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
        BusFr uid;
    };

    /**
     * structure to hold persistent solution data
     */
    struct Solution {
        std::optional<std::vector<SolutionAcLine>> ac_line;
        std::vector<SolutionBus> bus;
        std::vector<SolutionGen> gen;
        /**
         * structure to hold global settings for parameters in the network
         */
        SolutionGlobalParams global_params;
        std::optional<std::vector<SolutionHvdcP2P>> hvdc_p2_p;
        std::optional<std::vector<SolutionMultipleWindingTransformer>> multiple_winding_transformer;
        std::optional<std::vector<SolutionReserve>> reserve;
        std::optional<std::vector<SolutionShunt>> shunt;
        std::optional<std::vector<SolutionStorage>> storage;
        std::optional<std::vector<SolutionSwitch>> solution_switch;
        std::optional<std::vector<SolutionTransformer>> transformer;
    };

    /**
     * structure to contain all time variant data of the system/case. All time series are
     * synchronized to the same timestamps, which should should be stored using Unix time.
     * Structure is quasi-tabular, with uid, name, path_to_file, values, and ext being arrays in
     * the same order of said field. This is done in order to allow for better compression
     * (e.g., using HDF5) for the values field.
     */
    struct CtmSolutionTimeSeriesData {
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
        std::vector<BusFr> uid;
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
        std::optional<CtmSolutionTimeSeriesData> time_series_data;
    };

    /**
     * structure to contain all time variant data of the system/case. All time series are
     * synchronized to the same timestamps, which should should be stored using Unix time.
     * Structure is quasi-tabular, with uid, name, path_to_file, values, and ext being arrays in
     * the same order of said field. This is done in order to allow for better compression
     * (e.g., using HDF5) for the values field.
     */
    struct CtmTimeSeriesDataTimeSeriesData {
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
        std::vector<BusFr> uid;
        /**
         * array of time series values
         */
        std::optional<std::vector<std::vector<nlohmann::json>>> values;
    };

    /**
     * Common Transmission Model (CTM) Time Series Data Schema v0.1
     */
    struct CtmTimeSeriesData {
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
        CtmTimeSeriesDataTimeSeriesData time_series_data;
    };
}

namespace ctm_schemas {
void from_json(const json & j, CmUbAClass & x);
void to_json(json & j, const CmUbAClass & x);

void from_json(const json & j, NetworkAcLine & x);
void to_json(json & j, const NetworkAcLine & x);

void from_json(const json & j, Area & x);
void to_json(json & j, const Area & x);

void from_json(const json & j, NetworkBus & x);
void to_json(json & j, const NetworkBus & x);

void from_json(const json & j, CostPgParametersClass & x);
void to_json(json & j, const CostPgParametersClass & x);

void from_json(const json & j, NetworkGen & x);
void to_json(json & j, const NetworkGen & x);

void from_json(const json & j, NetworkGlobalParams & x);
void to_json(json & j, const NetworkGlobalParams & x);

void from_json(const json & j, NetworkHvdcP2P & x);
void to_json(json & j, const NetworkHvdcP2P & x);

void from_json(const json & j, Load & x);
void to_json(json & j, const Load & x);

void from_json(const json & j, NetworkMultipleWindingTransformer & x);
void to_json(json & j, const NetworkMultipleWindingTransformer & x);

void from_json(const json & j, NetworkSwitch & x);
void to_json(json & j, const NetworkSwitch & x);

void from_json(const json & j, NetworkReserve & x);
void to_json(json & j, const NetworkReserve & x);

void from_json(const json & j, NetworkShunt & x);
void to_json(json & j, const NetworkShunt & x);

void from_json(const json & j, NetworkStorage & x);
void to_json(json & j, const NetworkStorage & x);

void from_json(const json & j, NetworkTransformer & x);
void to_json(json & j, const NetworkTransformer & x);

void from_json(const json & j, Zone & x);
void to_json(json & j, const Zone & x);

void from_json(const json & j, Network & x);
void to_json(json & j, const Network & x);

void from_json(const json & j, TemporalBoundaryBus & x);
void to_json(json & j, const TemporalBoundaryBus & x);

void from_json(const json & j, TemporalBoundaryGen & x);
void to_json(json & j, const TemporalBoundaryGen & x);

void from_json(const json & j, TemporalBoundaryGlobalParams & x);
void to_json(json & j, const TemporalBoundaryGlobalParams & x);

void from_json(const json & j, TemporalBoundaryHvdcP2P & x);
void to_json(json & j, const TemporalBoundaryHvdcP2P & x);

void from_json(const json & j, TemporalBoundaryMultipleWindingTransformer & x);
void to_json(json & j, const TemporalBoundaryMultipleWindingTransformer & x);

void from_json(const json & j, TemporalBoundaryShunt & x);
void to_json(json & j, const TemporalBoundaryShunt & x);

void from_json(const json & j, TemporalBoundaryStorage & x);
void to_json(json & j, const TemporalBoundaryStorage & x);

void from_json(const json & j, TemporalBoundarySwitch & x);
void to_json(json & j, const TemporalBoundarySwitch & x);

void from_json(const json & j, TemporalBoundaryTransformer & x);
void to_json(json & j, const TemporalBoundaryTransformer & x);

void from_json(const json & j, TemporalBoundary & x);
void to_json(json & j, const TemporalBoundary & x);

void from_json(const json & j, CtmDataTimeSeriesData & x);
void to_json(json & j, const CtmDataTimeSeriesData & x);

void from_json(const json & j, CtmData & x);
void to_json(json & j, const CtmData & x);

void from_json(const json & j, CtmSolutionSchema & x);
void to_json(json & j, const CtmSolutionSchema & x);

void from_json(const json & j, SolutionAcLine & x);
void to_json(json & j, const SolutionAcLine & x);

void from_json(const json & j, SolutionBus & x);
void to_json(json & j, const SolutionBus & x);

void from_json(const json & j, ReserveProvision & x);
void to_json(json & j, const ReserveProvision & x);

void from_json(const json & j, SolutionGen & x);
void to_json(json & j, const SolutionGen & x);

void from_json(const json & j, SolutionGlobalParams & x);
void to_json(json & j, const SolutionGlobalParams & x);

void from_json(const json & j, SolutionHvdcP2P & x);
void to_json(json & j, const SolutionHvdcP2P & x);

void from_json(const json & j, SolutionMultipleWindingTransformer & x);
void to_json(json & j, const SolutionMultipleWindingTransformer & x);

void from_json(const json & j, SolutionReserve & x);
void to_json(json & j, const SolutionReserve & x);

void from_json(const json & j, SolutionShunt & x);
void to_json(json & j, const SolutionShunt & x);

void from_json(const json & j, SolutionSwitch & x);
void to_json(json & j, const SolutionSwitch & x);

void from_json(const json & j, SolutionStorage & x);
void to_json(json & j, const SolutionStorage & x);

void from_json(const json & j, SolutionTransformer & x);
void to_json(json & j, const SolutionTransformer & x);

void from_json(const json & j, Solution & x);
void to_json(json & j, const Solution & x);

void from_json(const json & j, CtmSolutionTimeSeriesData & x);
void to_json(json & j, const CtmSolutionTimeSeriesData & x);

void from_json(const json & j, CtmSolution & x);
void to_json(json & j, const CtmSolution & x);

void from_json(const json & j, CtmTimeSeriesDataTimeSeriesData & x);
void to_json(json & j, const CtmTimeSeriesDataTimeSeriesData & x);

void from_json(const json & j, CtmTimeSeriesData & x);
void to_json(json & j, const CtmTimeSeriesData & x);

void from_json(const json & j, TypeEnum & x);
void to_json(json & j, const TypeEnum & x);

void from_json(const json & j, CostPgModel & x);
void to_json(json & j, const CostPgModel & x);

void from_json(const json & j, PrimarySource & x);
void to_json(json & j, const PrimarySource & x);

void from_json(const json & j, PrimarySourceSubtype & x);
void to_json(json & j, const PrimarySourceSubtype & x);

void from_json(const json & j, UnitConvention & x);
void to_json(json & j, const UnitConvention & x);

void from_json(const json & j, Technology & x);
void to_json(json & j, const Technology & x);

void from_json(const json & j, ReserveType & x);
void to_json(json & j, const ReserveType & x);
}
namespace nlohmann {
template <>
struct adl_serializer<std::variant<int64_t, std::string>> {
    static void from_json(const json & j, std::variant<int64_t, std::string> & x);
    static void to_json(json & j, const std::variant<int64_t, std::string> & x);
};

template <>
struct adl_serializer<std::variant<ctm_schemas::CmUbAClass, double>> {
    static void from_json(const json & j, std::variant<ctm_schemas::CmUbAClass, double> & x);
    static void to_json(json & j, const std::variant<ctm_schemas::CmUbAClass, double> & x);
};

template <>
struct adl_serializer<std::variant<ctm_schemas::CmUbAClass, ctm_schemas::TypeEnum>> {
    static void from_json(const json & j, std::variant<ctm_schemas::CmUbAClass, ctm_schemas::TypeEnum> & x);
    static void to_json(json & j, const std::variant<ctm_schemas::CmUbAClass, ctm_schemas::TypeEnum> & x);
};

template <>
struct adl_serializer<std::variant<std::vector<double>, ctm_schemas::CostPgParametersClass>> {
    static void from_json(const json & j, std::variant<std::vector<double>, ctm_schemas::CostPgParametersClass> & x);
    static void to_json(json & j, const std::variant<std::vector<double>, ctm_schemas::CostPgParametersClass> & x);
};

template <>
struct adl_serializer<std::variant<ctm_schemas::CmUbAClass, int64_t>> {
    static void from_json(const json & j, std::variant<ctm_schemas::CmUbAClass, int64_t> & x);
    static void to_json(json & j, const std::variant<ctm_schemas::CmUbAClass, int64_t> & x);
};

template <>
struct adl_serializer<std::variant<std::vector<double>, double>> {
    static void from_json(const json & j, std::variant<std::vector<double>, double> & x);
    static void to_json(json & j, const std::variant<std::vector<double>, double> & x);
};

template <>
struct adl_serializer<std::variant<std::vector<int64_t>, int64_t>> {
    static void from_json(const json & j, std::variant<std::vector<int64_t>, int64_t> & x);
    static void to_json(json & j, const std::variant<std::vector<int64_t>, int64_t> & x);
};

template <>
struct adl_serializer<std::variant<std::vector<double>, bool, double, int64_t, std::map<std::string, json>, std::string>> {
    static void from_json(const json & j, std::variant<std::vector<double>, bool, double, int64_t, std::map<std::string, json>, std::string> & x);
    static void to_json(json & j, const std::variant<std::vector<double>, bool, double, int64_t, std::map<std::string, json>, std::string> & x);
};

template <>
struct adl_serializer<std::variant<std::vector<std::string>, std::string>> {
    static void from_json(const json & j, std::variant<std::vector<std::string>, std::string> & x);
    static void to_json(json & j, const std::variant<std::vector<std::string>, std::string> & x);
};

template <>
struct adl_serializer<std::variant<ctm_schemas::CtmSolutionSchema, double>> {
    static void from_json(const json & j, std::variant<ctm_schemas::CtmSolutionSchema, double> & x);
    static void to_json(json & j, const std::variant<ctm_schemas::CtmSolutionSchema, double> & x);
};

template <>
struct adl_serializer<std::variant<ctm_schemas::CtmSolutionSchema, int64_t>> {
    static void from_json(const json & j, std::variant<ctm_schemas::CtmSolutionSchema, int64_t> & x);
    static void to_json(json & j, const std::variant<ctm_schemas::CtmSolutionSchema, int64_t> & x);
};

template <>
struct adl_serializer<std::variant<std::vector<int64_t>, ctm_schemas::CtmSolutionSchema, int64_t>> {
    static void from_json(const json & j, std::variant<std::vector<int64_t>, ctm_schemas::CtmSolutionSchema, int64_t> & x);
    static void to_json(json & j, const std::variant<std::vector<int64_t>, ctm_schemas::CtmSolutionSchema, int64_t> & x);
};
}
namespace ctm_schemas {
    inline void from_json(const json & j, CmUbAClass& x) {
        x.scale_factor = j.at("scale_factor").get<double>();
        x.uid = j.at("uid").get<BusFr>();
    }

    inline void to_json(json & j, const CmUbAClass & x) {
        j = json::object();
        j["scale_factor"] = x.scale_factor;
        j["uid"] = x.uid;
    }

    inline void from_json(const json & j, NetworkAcLine& x) {
        x.b_fr = get_stack_optional<double>(j, "b_fr");
        x.b_to = get_stack_optional<double>(j, "b_to");
        x.bus_fr = j.at("bus_fr").get<BusFr>();
        x.bus_to = j.at("bus_to").get<BusFr>();
        x.cm_ub_a = get_stack_optional<std::variant<CmUbAClass, double>>(j, "cm_ub_a");
        x.cm_ub_b = get_stack_optional<std::variant<CmUbAClass, double>>(j, "cm_ub_b");
        x.cm_ub_c = get_stack_optional<std::variant<CmUbAClass, double>>(j, "cm_ub_c");
        x.ext = get_untyped(j, "ext");
        x.g_fr = get_stack_optional<double>(j, "g_fr");
        x.g_to = get_stack_optional<double>(j, "g_to");
        x.name = get_stack_optional<std::string>(j, "name");
        x.nominal_mva = get_stack_optional<double>(j, "nominal_mva");
        x.persistent_outage_duration = get_stack_optional<double>(j, "persistent_outage_duration");
        x.persistent_outage_rate = get_stack_optional<double>(j, "persistent_outage_rate");
        x.r = j.at("r").get<double>();
        x.sm_ub_a = get_stack_optional<std::variant<CmUbAClass, double>>(j, "sm_ub_a");
        x.sm_ub_b = get_stack_optional<std::variant<CmUbAClass, double>>(j, "sm_ub_b");
        x.sm_ub_c = get_stack_optional<std::variant<CmUbAClass, double>>(j, "sm_ub_c");
        x.status = j.at("status").get<int64_t>();
        x.transient_outage_rate = get_stack_optional<double>(j, "transient_outage_rate");
        x.uid = j.at("uid").get<BusFr>();
        x.vad_lb = get_stack_optional<double>(j, "vad_lb");
        x.vad_ub = get_stack_optional<double>(j, "vad_ub");
        x.x = j.at("x").get<double>();
    }

    inline void to_json(json & j, const NetworkAcLine & x) {
        j = json::object();
        j["b_fr"] = x.b_fr;
        j["b_to"] = x.b_to;
        j["bus_fr"] = x.bus_fr;
        j["bus_to"] = x.bus_to;
        j["cm_ub_a"] = x.cm_ub_a;
        j["cm_ub_b"] = x.cm_ub_b;
        j["cm_ub_c"] = x.cm_ub_c;
        j["ext"] = x.ext;
        j["g_fr"] = x.g_fr;
        j["g_to"] = x.g_to;
        j["name"] = x.name;
        j["nominal_mva"] = x.nominal_mva;
        j["persistent_outage_duration"] = x.persistent_outage_duration;
        j["persistent_outage_rate"] = x.persistent_outage_rate;
        j["r"] = x.r;
        j["sm_ub_a"] = x.sm_ub_a;
        j["sm_ub_b"] = x.sm_ub_b;
        j["sm_ub_c"] = x.sm_ub_c;
        j["status"] = x.status;
        j["transient_outage_rate"] = x.transient_outage_rate;
        j["uid"] = x.uid;
        j["vad_lb"] = x.vad_lb;
        j["vad_ub"] = x.vad_ub;
        j["x"] = x.x;
    }

    inline void from_json(const json & j, Area& x) {
        x.ext = get_untyped(j, "ext");
        x.name = get_stack_optional<std::string>(j, "name");
        x.status = j.at("status").get<int64_t>();
        x.uid = j.at("uid").get<BusFr>();
    }

    inline void to_json(json & j, const Area & x) {
        j = json::object();
        j["ext"] = x.ext;
        j["name"] = x.name;
        j["status"] = x.status;
        j["uid"] = x.uid;
    }

    inline void from_json(const json & j, NetworkBus& x) {
        x.area = get_stack_optional<std::variant<int64_t, std::string>>(j, "area");
        x.base_kv = j.at("base_kv").get<double>();
        x.ext = get_untyped(j, "ext");
        x.name = get_stack_optional<std::string>(j, "name");
        x.status = j.at("status").get<int64_t>();
        x.type = get_stack_optional<std::variant<CmUbAClass, TypeEnum>>(j, "type");
        x.uid = j.at("uid").get<BusFr>();
        x.vm_lb = get_stack_optional<std::variant<CmUbAClass, double>>(j, "vm_lb");
        x.vm_ub = get_stack_optional<std::variant<CmUbAClass, double>>(j, "vm_ub");
        x.zone = get_stack_optional<std::variant<int64_t, std::string>>(j, "zone");
    }

    inline void to_json(json & j, const NetworkBus & x) {
        j = json::object();
        j["area"] = x.area;
        j["base_kv"] = x.base_kv;
        j["ext"] = x.ext;
        j["name"] = x.name;
        j["status"] = x.status;
        j["type"] = x.type;
        j["uid"] = x.uid;
        j["vm_lb"] = x.vm_lb;
        j["vm_ub"] = x.vm_ub;
        j["zone"] = x.zone;
    }

    inline void from_json(const json & j, CostPgParametersClass& x) {
        x.x = get_stack_optional<std::vector<double>>(j, "x");
        x.y = get_stack_optional<std::vector<double>>(j, "y");
        x.scale_factor = get_stack_optional<double>(j, "scale_factor");
        x.uid = get_stack_optional<std::variant<int64_t, std::string>>(j, "uid");
    }

    inline void to_json(json & j, const CostPgParametersClass & x) {
        j = json::object();
        j["x"] = x.x;
        j["y"] = x.y;
        j["scale_factor"] = x.scale_factor;
        j["uid"] = x.uid;
    }

    inline void from_json(const json & j, NetworkGen& x) {
        x.bus = j.at("bus").get<BusFr>();
        x.cost_pg_model = get_stack_optional<CostPgModel>(j, "cost_pg_model");
        x.cost_pg_parameters = get_stack_optional<std::variant<std::vector<double>, CostPgParametersClass>>(j, "cost_pg_parameters");
        x.down_time_lb = get_stack_optional<double>(j, "down_time_lb");
        x.ext = get_untyped(j, "ext");
        x.forced_outage_rate = get_stack_optional<double>(j, "forced_outage_rate");
        x.in_service_time_lb = get_stack_optional<double>(j, "in_service_time_lb");
        x.in_service_time_ub = get_stack_optional<double>(j, "in_service_time_ub");
        x.mean_time_to_failure = get_stack_optional<double>(j, "mean_time_to_failure");
        x.mean_time_to_repair = get_stack_optional<double>(j, "mean_time_to_repair");
        x.name = get_stack_optional<std::string>(j, "name");
        x.nominal_mva = get_stack_optional<double>(j, "nominal_mva");
        x.pg_delta_lb = get_stack_optional<double>(j, "pg_delta_lb");
        x.pg_delta_ub = get_stack_optional<double>(j, "pg_delta_ub");
        x.pg_lb = get_stack_optional<std::variant<CmUbAClass, double>>(j, "pg_lb");
        x.pg_ub = get_stack_optional<std::variant<CmUbAClass, double>>(j, "pg_ub");
        x.primary_source = get_stack_optional<PrimarySource>(j, "primary_source");
        x.primary_source_subtype = get_stack_optional<PrimarySourceSubtype>(j, "primary_source_subtype");
        x.qg_lb = get_stack_optional<std::variant<CmUbAClass, double>>(j, "qg_lb");
        x.qg_ub = get_stack_optional<std::variant<CmUbAClass, double>>(j, "qg_ub");
        x.scheduled_maintenance_rate = get_stack_optional<double>(j, "scheduled_maintenance_rate");
        x.service_required = get_stack_optional<std::variant<CmUbAClass, int64_t>>(j, "service_required");
        x.shutdown_cost = get_stack_optional<std::variant<CmUbAClass, double>>(j, "shutdown_cost");
        x.startup_cost_cold = get_stack_optional<std::variant<CmUbAClass, double>>(j, "startup_cost_cold");
        x.startup_cost_hot = get_stack_optional<std::variant<CmUbAClass, double>>(j, "startup_cost_hot");
        x.startup_cost_warm = get_stack_optional<std::variant<CmUbAClass, double>>(j, "startup_cost_warm");
        x.startup_time_hot = get_stack_optional<double>(j, "startup_time_hot");
        x.startup_time_warm = get_stack_optional<double>(j, "startup_time_warm");
        x.status = j.at("status").get<int64_t>();
        x.uid = j.at("uid").get<BusFr>();
        x.vm_setpoint = get_stack_optional<std::variant<CmUbAClass, double>>(j, "vm_setpoint");
    }

    inline void to_json(json & j, const NetworkGen & x) {
        j = json::object();
        j["bus"] = x.bus;
        j["cost_pg_model"] = x.cost_pg_model;
        j["cost_pg_parameters"] = x.cost_pg_parameters;
        j["down_time_lb"] = x.down_time_lb;
        j["ext"] = x.ext;
        j["forced_outage_rate"] = x.forced_outage_rate;
        j["in_service_time_lb"] = x.in_service_time_lb;
        j["in_service_time_ub"] = x.in_service_time_ub;
        j["mean_time_to_failure"] = x.mean_time_to_failure;
        j["mean_time_to_repair"] = x.mean_time_to_repair;
        j["name"] = x.name;
        j["nominal_mva"] = x.nominal_mva;
        j["pg_delta_lb"] = x.pg_delta_lb;
        j["pg_delta_ub"] = x.pg_delta_ub;
        j["pg_lb"] = x.pg_lb;
        j["pg_ub"] = x.pg_ub;
        j["primary_source"] = x.primary_source;
        j["primary_source_subtype"] = x.primary_source_subtype;
        j["qg_lb"] = x.qg_lb;
        j["qg_ub"] = x.qg_ub;
        j["scheduled_maintenance_rate"] = x.scheduled_maintenance_rate;
        j["service_required"] = x.service_required;
        j["shutdown_cost"] = x.shutdown_cost;
        j["startup_cost_cold"] = x.startup_cost_cold;
        j["startup_cost_hot"] = x.startup_cost_hot;
        j["startup_cost_warm"] = x.startup_cost_warm;
        j["startup_time_hot"] = x.startup_time_hot;
        j["startup_time_warm"] = x.startup_time_warm;
        j["status"] = x.status;
        j["uid"] = x.uid;
        j["vm_setpoint"] = x.vm_setpoint;
    }

    inline void from_json(const json & j, NetworkGlobalParams& x) {
        x.base_mva = get_stack_optional<double>(j, "base_mva");
        x.bus_ref = get_stack_optional<std::variant<int64_t, std::string>>(j, "bus_ref");
        x.unit_convention = j.at("unit_convention").get<UnitConvention>();
    }

    inline void to_json(json & j, const NetworkGlobalParams & x) {
        j = json::object();
        j["base_mva"] = x.base_mva;
        j["bus_ref"] = x.bus_ref;
        j["unit_convention"] = x.unit_convention;
    }

    inline void from_json(const json & j, NetworkHvdcP2P& x) {
        x.base_kv_dc = get_stack_optional<double>(j, "base_kv_dc");
        x.bus_fr = j.at("bus_fr").get<BusFr>();
        x.bus_to = j.at("bus_to").get<BusFr>();
        x.cm_ub_fr = get_stack_optional<std::variant<CmUbAClass, double>>(j, "cm_ub_fr");
        x.cm_ub_to = get_stack_optional<std::variant<CmUbAClass, double>>(j, "cm_ub_to");
        x.ext = get_untyped(j, "ext");
        x.loss_a = get_stack_optional<double>(j, "loss_a");
        x.loss_b = get_stack_optional<double>(j, "loss_b");
        x.loss_c = get_stack_optional<double>(j, "loss_c");
        x.name = get_stack_optional<std::string>(j, "name");
        x.nominal_mva = get_stack_optional<double>(j, "nominal_mva");
        x.p = get_stack_optional<int64_t>(j, "p");
        x.pdc_fr_lb = get_stack_optional<std::variant<CmUbAClass, double>>(j, "pdc_fr_lb");
        x.pdc_fr_ub = get_stack_optional<std::variant<CmUbAClass, double>>(j, "pdc_fr_ub");
        x.pdc_to_lb = get_stack_optional<std::variant<CmUbAClass, double>>(j, "pdc_to_lb");
        x.pdc_to_ub = get_stack_optional<std::variant<CmUbAClass, double>>(j, "pdc_to_ub");
        x.persistent_outage_duration = get_stack_optional<double>(j, "persistent_outage_duration");
        x.persistent_outage_rate = get_stack_optional<double>(j, "persistent_outage_rate");
        x.phi_lb = get_stack_optional<double>(j, "phi_lb");
        x.phi_ub = get_stack_optional<double>(j, "phi_ub");
        x.qdc_fr_lb = get_stack_optional<std::variant<CmUbAClass, double>>(j, "qdc_fr_lb");
        x.qdc_fr_ub = get_stack_optional<std::variant<CmUbAClass, double>>(j, "qdc_fr_ub");
        x.qdc_to_lb = get_stack_optional<std::variant<CmUbAClass, double>>(j, "qdc_to_lb");
        x.qdc_to_ub = get_stack_optional<std::variant<CmUbAClass, double>>(j, "qdc_to_ub");
        x.r = get_stack_optional<double>(j, "r");
        x.sm_ub = get_stack_optional<std::variant<CmUbAClass, double>>(j, "sm_ub");
        x.status = j.at("status").get<int64_t>();
        x.technology = get_stack_optional<Technology>(j, "technology");
        x.transient_outage_rate = get_stack_optional<double>(j, "transient_outage_rate");
        x.uid = j.at("uid").get<BusFr>();
        x.vm_dc_lb = get_stack_optional<double>(j, "vm_dc_lb");
        x.vm_dc_ub = get_stack_optional<double>(j, "vm_dc_ub");
    }

    inline void to_json(json & j, const NetworkHvdcP2P & x) {
        j = json::object();
        j["base_kv_dc"] = x.base_kv_dc;
        j["bus_fr"] = x.bus_fr;
        j["bus_to"] = x.bus_to;
        j["cm_ub_fr"] = x.cm_ub_fr;
        j["cm_ub_to"] = x.cm_ub_to;
        j["ext"] = x.ext;
        j["loss_a"] = x.loss_a;
        j["loss_b"] = x.loss_b;
        j["loss_c"] = x.loss_c;
        j["name"] = x.name;
        j["nominal_mva"] = x.nominal_mva;
        j["p"] = x.p;
        j["pdc_fr_lb"] = x.pdc_fr_lb;
        j["pdc_fr_ub"] = x.pdc_fr_ub;
        j["pdc_to_lb"] = x.pdc_to_lb;
        j["pdc_to_ub"] = x.pdc_to_ub;
        j["persistent_outage_duration"] = x.persistent_outage_duration;
        j["persistent_outage_rate"] = x.persistent_outage_rate;
        j["phi_lb"] = x.phi_lb;
        j["phi_ub"] = x.phi_ub;
        j["qdc_fr_lb"] = x.qdc_fr_lb;
        j["qdc_fr_ub"] = x.qdc_fr_ub;
        j["qdc_to_lb"] = x.qdc_to_lb;
        j["qdc_to_ub"] = x.qdc_to_ub;
        j["r"] = x.r;
        j["sm_ub"] = x.sm_ub;
        j["status"] = x.status;
        j["technology"] = x.technology;
        j["transient_outage_rate"] = x.transient_outage_rate;
        j["uid"] = x.uid;
        j["vm_dc_lb"] = x.vm_dc_lb;
        j["vm_dc_ub"] = x.vm_dc_ub;
    }

    inline void from_json(const json & j, Load& x) {
        x.bus = j.at("bus").get<BusFr>();
        x.ext = get_untyped(j, "ext");
        x.name = get_stack_optional<std::string>(j, "name");
        x.nominal_mva = get_stack_optional<double>(j, "nominal_mva");
        x.pd = j.at("pd").get<VmLb>();
        x.pd_i = get_stack_optional<std::variant<CmUbAClass, double>>(j, "pd_i");
        x.pd_y = get_stack_optional<std::variant<CmUbAClass, double>>(j, "pd_y");
        x.qd = j.at("qd").get<VmLb>();
        x.qd_i = get_stack_optional<std::variant<CmUbAClass, double>>(j, "qd_i");
        x.qd_y = get_stack_optional<std::variant<CmUbAClass, double>>(j, "qd_y");
        x.status = j.at("status").get<int64_t>();
        x.uid = j.at("uid").get<BusFr>();
    }

    inline void to_json(json & j, const Load & x) {
        j = json::object();
        j["bus"] = x.bus;
        j["ext"] = x.ext;
        j["name"] = x.name;
        j["nominal_mva"] = x.nominal_mva;
        j["pd"] = x.pd;
        j["pd_i"] = x.pd_i;
        j["pd_y"] = x.pd_y;
        j["qd"] = x.qd;
        j["qd_i"] = x.qd_i;
        j["qd_y"] = x.qd_y;
        j["status"] = x.status;
        j["uid"] = x.uid;
    }

    inline void from_json(const json & j, NetworkMultipleWindingTransformer& x) {
        x.b = j.at("b").get<double>();
        x.bus_w = j.at("bus_w").get<std::vector<BusFr>>();
        x.cm_ub_a_w = get_stack_optional<std::vector<CmUbA>>(j, "cm_ub_a_w");
        x.cm_ub_b_w = get_stack_optional<std::vector<CmUbA>>(j, "cm_ub_b_w");
        x.cm_ub_c_w = get_stack_optional<std::vector<CmUbA>>(j, "cm_ub_c_w");
        x.ext = get_untyped(j, "ext");
        x.g = j.at("g").get<double>();
        x.name = get_stack_optional<std::string>(j, "name");
        x.nominal_mva_w = get_stack_optional<std::vector<double>>(j, "nominal_mva_w");
        x.num_windings = j.at("num_windings").get<int64_t>();
        x.persistent_outage_duration = get_stack_optional<double>(j, "persistent_outage_duration");
        x.persistent_outage_rate = get_stack_optional<double>(j, "persistent_outage_rate");
        x.r_w = j.at("r_w").get<std::vector<double>>();
        x.sm_ub_a_w = get_stack_optional<std::vector<CmUbA>>(j, "sm_ub_a_w");
        x.sm_ub_b_w = get_stack_optional<std::vector<CmUbA>>(j, "sm_ub_b_w");
        x.sm_ub_c_w = get_stack_optional<std::vector<CmUbA>>(j, "sm_ub_c_w");
        x.status = j.at("status").get<int64_t>();
        x.status_w = j.at("status_w").get<std::vector<int64_t>>();
        x.ta_lb_w = get_stack_optional<std::vector<double>>(j, "ta_lb_w");
        x.ta_steps_w = get_stack_optional<std::vector<int64_t>>(j, "ta_steps_w");
        x.ta_ub_w = get_stack_optional<std::vector<double>>(j, "ta_ub_w");
        x.tm_lb_w = get_stack_optional<std::vector<double>>(j, "tm_lb_w");
        x.tm_steps_w = get_stack_optional<std::vector<int64_t>>(j, "tm_steps_w");
        x.tm_ub_w = get_stack_optional<std::vector<double>>(j, "tm_ub_w");
        x.uid = j.at("uid").get<BusFr>();
        x.x_w = j.at("x_w").get<std::vector<double>>();
    }

    inline void to_json(json & j, const NetworkMultipleWindingTransformer & x) {
        j = json::object();
        j["b"] = x.b;
        j["bus_w"] = x.bus_w;
        j["cm_ub_a_w"] = x.cm_ub_a_w;
        j["cm_ub_b_w"] = x.cm_ub_b_w;
        j["cm_ub_c_w"] = x.cm_ub_c_w;
        j["ext"] = x.ext;
        j["g"] = x.g;
        j["name"] = x.name;
        j["nominal_mva_w"] = x.nominal_mva_w;
        j["num_windings"] = x.num_windings;
        j["persistent_outage_duration"] = x.persistent_outage_duration;
        j["persistent_outage_rate"] = x.persistent_outage_rate;
        j["r_w"] = x.r_w;
        j["sm_ub_a_w"] = x.sm_ub_a_w;
        j["sm_ub_b_w"] = x.sm_ub_b_w;
        j["sm_ub_c_w"] = x.sm_ub_c_w;
        j["status"] = x.status;
        j["status_w"] = x.status_w;
        j["ta_lb_w"] = x.ta_lb_w;
        j["ta_steps_w"] = x.ta_steps_w;
        j["ta_ub_w"] = x.ta_ub_w;
        j["tm_lb_w"] = x.tm_lb_w;
        j["tm_steps_w"] = x.tm_steps_w;
        j["tm_ub_w"] = x.tm_ub_w;
        j["uid"] = x.uid;
        j["x_w"] = x.x_w;
    }

    inline void from_json(const json & j, NetworkSwitch& x) {
        x.bus_fr = j.at("bus_fr").get<BusFr>();
        x.bus_to = j.at("bus_to").get<BusFr>();
        x.cm_ub = get_stack_optional<double>(j, "cm_ub");
        x.ext = get_untyped(j, "ext");
        x.name = get_stack_optional<std::string>(j, "name");
        x.nominal_mva = get_stack_optional<double>(j, "nominal_mva");
        x.sm_ub = get_stack_optional<double>(j, "sm_ub");
        x.status = j.at("status").get<int64_t>();
        x.uid = j.at("uid").get<BusFr>();
    }

    inline void to_json(json & j, const NetworkSwitch & x) {
        j = json::object();
        j["bus_fr"] = x.bus_fr;
        j["bus_to"] = x.bus_to;
        j["cm_ub"] = x.cm_ub;
        j["ext"] = x.ext;
        j["name"] = x.name;
        j["nominal_mva"] = x.nominal_mva;
        j["sm_ub"] = x.sm_ub;
        j["status"] = x.status;
        j["uid"] = x.uid;
    }

    inline void from_json(const json & j, NetworkReserve& x) {
        x.ext = get_untyped(j, "ext");
        x.name = get_stack_optional<std::string>(j, "name");
        x.participants = get_stack_optional<std::vector<BusFr>>(j, "participants");
        x.pg_down = get_stack_optional<std::variant<CmUbAClass, double>>(j, "pg_down");
        x.pg_up = get_stack_optional<std::variant<CmUbAClass, double>>(j, "pg_up");
        x.reserve_type = j.at("reserve_type").get<ReserveType>();
        x.status = j.at("status").get<int64_t>();
        x.uid = j.at("uid").get<BusFr>();
    }

    inline void to_json(json & j, const NetworkReserve & x) {
        j = json::object();
        j["ext"] = x.ext;
        j["name"] = x.name;
        j["participants"] = x.participants;
        j["pg_down"] = x.pg_down;
        j["pg_up"] = x.pg_up;
        j["reserve_type"] = x.reserve_type;
        j["status"] = x.status;
        j["uid"] = x.uid;
    }

    inline void from_json(const json & j, NetworkShunt& x) {
        x.bs = j.at("bs").get<Bs>();
        x.bus = j.at("bus").get<BusFr>();
        x.ext = get_untyped(j, "ext");
        x.gs = j.at("gs").get<Gs>();
        x.name = get_stack_optional<std::string>(j, "name");
        x.nominal_mva = get_stack_optional<double>(j, "nominal_mva");
        x.num_steps_ub = j.at("num_steps_ub").get<NumStepsUbUnion>();
        x.status = j.at("status").get<int64_t>();
        x.uid = j.at("uid").get<BusFr>();
    }

    inline void to_json(json & j, const NetworkShunt & x) {
        j = json::object();
        j["bs"] = x.bs;
        j["bus"] = x.bus;
        j["ext"] = x.ext;
        j["gs"] = x.gs;
        j["name"] = x.name;
        j["nominal_mva"] = x.nominal_mva;
        j["num_steps_ub"] = x.num_steps_ub;
        j["status"] = x.status;
        j["uid"] = x.uid;
    }

    inline void from_json(const json & j, NetworkStorage& x) {
        x.bus = j.at("bus").get<BusFr>();
        x.charge_efficiency = j.at("charge_efficiency").get<ChargeEfficiency>();
        x.charge_ub = get_stack_optional<std::variant<CmUbAClass, double>>(j, "charge_ub");
        x.cm_ub = get_stack_optional<double>(j, "cm_ub");
        x.discharge_efficiency = j.at("discharge_efficiency").get<ChargeEfficiency>();
        x.discharge_ub = get_stack_optional<std::variant<CmUbAClass, double>>(j, "discharge_ub");
        x.energy_ub = get_stack_optional<double>(j, "energy_ub");
        x.ext = get_untyped(j, "ext");
        x.name = get_stack_optional<std::string>(j, "name");
        x.nominal_mva = get_stack_optional<double>(j, "nominal_mva");
        x.ps_delta_lb = get_stack_optional<double>(j, "ps_delta_lb");
        x.ps_delta_ub = get_stack_optional<double>(j, "ps_delta_ub");
        x.ps_ex = j.at("ps_ex").get<double>();
        x.qs_ex = j.at("qs_ex").get<double>();
        x.qs_lb = get_stack_optional<std::variant<CmUbAClass, double>>(j, "qs_lb");
        x.qs_ub = get_stack_optional<std::variant<CmUbAClass, double>>(j, "qs_ub");
        x.sm_ub = get_stack_optional<double>(j, "sm_ub");
        x.status = j.at("status").get<int64_t>();
        x.uid = j.at("uid").get<BusFr>();
    }

    inline void to_json(json & j, const NetworkStorage & x) {
        j = json::object();
        j["bus"] = x.bus;
        j["charge_efficiency"] = x.charge_efficiency;
        j["charge_ub"] = x.charge_ub;
        j["cm_ub"] = x.cm_ub;
        j["discharge_efficiency"] = x.discharge_efficiency;
        j["discharge_ub"] = x.discharge_ub;
        j["energy_ub"] = x.energy_ub;
        j["ext"] = x.ext;
        j["name"] = x.name;
        j["nominal_mva"] = x.nominal_mva;
        j["ps_delta_lb"] = x.ps_delta_lb;
        j["ps_delta_ub"] = x.ps_delta_ub;
        j["ps_ex"] = x.ps_ex;
        j["qs_ex"] = x.qs_ex;
        j["qs_lb"] = x.qs_lb;
        j["qs_ub"] = x.qs_ub;
        j["sm_ub"] = x.sm_ub;
        j["status"] = x.status;
        j["uid"] = x.uid;
    }

    inline void from_json(const json & j, NetworkTransformer& x) {
        x.b = j.at("b").get<double>();
        x.bus_fr = j.at("bus_fr").get<BusFr>();
        x.bus_to = j.at("bus_to").get<BusFr>();
        x.cm_ub_a = get_stack_optional<std::variant<CmUbAClass, double>>(j, "cm_ub_a");
        x.cm_ub_b = get_stack_optional<std::variant<CmUbAClass, double>>(j, "cm_ub_b");
        x.cm_ub_c = get_stack_optional<std::variant<CmUbAClass, double>>(j, "cm_ub_c");
        x.ext = get_untyped(j, "ext");
        x.g = j.at("g").get<double>();
        x.name = get_stack_optional<std::string>(j, "name");
        x.nominal_mva = get_stack_optional<double>(j, "nominal_mva");
        x.persistent_outage_duration = get_stack_optional<double>(j, "persistent_outage_duration");
        x.persistent_outage_rate = get_stack_optional<double>(j, "persistent_outage_rate");
        x.r = j.at("r").get<double>();
        x.sm_ub_a = get_stack_optional<std::variant<CmUbAClass, double>>(j, "sm_ub_a");
        x.sm_ub_b = get_stack_optional<std::variant<CmUbAClass, double>>(j, "sm_ub_b");
        x.sm_ub_c = get_stack_optional<std::variant<CmUbAClass, double>>(j, "sm_ub_c");
        x.status = j.at("status").get<int64_t>();
        x.ta_lb = get_stack_optional<double>(j, "ta_lb");
        x.ta_steps = get_stack_optional<int64_t>(j, "ta_steps");
        x.ta_ub = get_stack_optional<double>(j, "ta_ub");
        x.tm_lb = get_stack_optional<double>(j, "tm_lb");
        x.tm_steps = get_stack_optional<int64_t>(j, "tm_steps");
        x.tm_ub = get_stack_optional<double>(j, "tm_ub");
        x.uid = j.at("uid").get<BusFr>();
        x.x = j.at("x").get<double>();
    }

    inline void to_json(json & j, const NetworkTransformer & x) {
        j = json::object();
        j["b"] = x.b;
        j["bus_fr"] = x.bus_fr;
        j["bus_to"] = x.bus_to;
        j["cm_ub_a"] = x.cm_ub_a;
        j["cm_ub_b"] = x.cm_ub_b;
        j["cm_ub_c"] = x.cm_ub_c;
        j["ext"] = x.ext;
        j["g"] = x.g;
        j["name"] = x.name;
        j["nominal_mva"] = x.nominal_mva;
        j["persistent_outage_duration"] = x.persistent_outage_duration;
        j["persistent_outage_rate"] = x.persistent_outage_rate;
        j["r"] = x.r;
        j["sm_ub_a"] = x.sm_ub_a;
        j["sm_ub_b"] = x.sm_ub_b;
        j["sm_ub_c"] = x.sm_ub_c;
        j["status"] = x.status;
        j["ta_lb"] = x.ta_lb;
        j["ta_steps"] = x.ta_steps;
        j["ta_ub"] = x.ta_ub;
        j["tm_lb"] = x.tm_lb;
        j["tm_steps"] = x.tm_steps;
        j["tm_ub"] = x.tm_ub;
        j["uid"] = x.uid;
        j["x"] = x.x;
    }

    inline void from_json(const json & j, Zone& x) {
        x.ext = get_untyped(j, "ext");
        x.name = get_stack_optional<std::string>(j, "name");
        x.status = j.at("status").get<int64_t>();
        x.uid = j.at("uid").get<BusFr>();
    }

    inline void to_json(json & j, const Zone & x) {
        j = json::object();
        j["ext"] = x.ext;
        j["name"] = x.name;
        j["status"] = x.status;
        j["uid"] = x.uid;
    }

    inline void from_json(const json & j, Network& x) {
        x.ac_line = get_stack_optional<std::vector<NetworkAcLine>>(j, "ac_line");
        x.area = j.at("area").get<std::vector<Area>>();
        x.bus = j.at("bus").get<std::vector<NetworkBus>>();
        x.gen = j.at("gen").get<std::vector<NetworkGen>>();
        x.global_params = j.at("global_params").get<NetworkGlobalParams>();
        x.hvdc_p2_p = get_stack_optional<std::vector<NetworkHvdcP2P>>(j, "hvdc_p2p");
        x.load = j.at("load").get<std::vector<Load>>();
        x.multiple_winding_transformer = get_stack_optional<std::vector<NetworkMultipleWindingTransformer>>(j, "multiple_winding_transformer");
        x.reserve = get_stack_optional<std::vector<NetworkReserve>>(j, "reserve");
        x.shunt = get_stack_optional<std::vector<NetworkShunt>>(j, "shunt");
        x.storage = get_stack_optional<std::vector<NetworkStorage>>(j, "storage");
        x.network_switch = get_stack_optional<std::vector<NetworkSwitch>>(j, "switch");
        x.transformer = get_stack_optional<std::vector<NetworkTransformer>>(j, "transformer");
        x.zone = get_stack_optional<std::vector<Zone>>(j, "zone");
    }

    inline void to_json(json & j, const Network & x) {
        j = json::object();
        j["ac_line"] = x.ac_line;
        j["area"] = x.area;
        j["bus"] = x.bus;
        j["gen"] = x.gen;
        j["global_params"] = x.global_params;
        j["hvdc_p2p"] = x.hvdc_p2_p;
        j["load"] = x.load;
        j["multiple_winding_transformer"] = x.multiple_winding_transformer;
        j["reserve"] = x.reserve;
        j["shunt"] = x.shunt;
        j["storage"] = x.storage;
        j["switch"] = x.network_switch;
        j["transformer"] = x.transformer;
        j["zone"] = x.zone;
    }

    inline void from_json(const json & j, TemporalBoundaryBus& x) {
        x.ext = get_untyped(j, "ext");
        x.uid = j.at("uid").get<BusFr>();
        x.va = j.at("va").get<double>();
        x.vm = get_stack_optional<double>(j, "vm");
    }

    inline void to_json(json & j, const TemporalBoundaryBus & x) {
        j = json::object();
        j["ext"] = x.ext;
        j["uid"] = x.uid;
        j["va"] = x.va;
        j["vm"] = x.vm;
    }

    inline void from_json(const json & j, TemporalBoundaryGen& x) {
        x.down_time = get_stack_optional<double>(j, "down_time");
        x.ext = get_untyped(j, "ext");
        x.in_service_time = get_stack_optional<double>(j, "in_service_time");
        x.pg = j.at("pg").get<double>();
        x.qg = get_stack_optional<double>(j, "qg");
        x.uid = j.at("uid").get<BusFr>();
    }

    inline void to_json(json & j, const TemporalBoundaryGen & x) {
        j = json::object();
        j["down_time"] = x.down_time;
        j["ext"] = x.ext;
        j["in_service_time"] = x.in_service_time;
        j["pg"] = x.pg;
        j["qg"] = x.qg;
        j["uid"] = x.uid;
    }

    inline void from_json(const json & j, TemporalBoundaryGlobalParams& x) {
        x.time_elapsed = j.at("time_elapsed").get<double>();
    }

    inline void to_json(json & j, const TemporalBoundaryGlobalParams & x) {
        j = json::object();
        j["time_elapsed"] = x.time_elapsed;
    }

    inline void from_json(const json & j, TemporalBoundaryHvdcP2P& x) {
        x.ext = get_untyped(j, "ext");
        x.pdc_fr = j.at("pdc_fr").get<double>();
        x.pdc_to = j.at("pdc_to").get<double>();
        x.qdc_fr = get_stack_optional<double>(j, "qdc_fr");
        x.qdc_to = get_stack_optional<double>(j, "qdc_to");
        x.uid = j.at("uid").get<BusFr>();
        x.vm_dc_fr = get_stack_optional<double>(j, "vm_dc_fr");
        x.vm_dc_to = get_stack_optional<double>(j, "vm_dc_to");
    }

    inline void to_json(json & j, const TemporalBoundaryHvdcP2P & x) {
        j = json::object();
        j["ext"] = x.ext;
        j["pdc_fr"] = x.pdc_fr;
        j["pdc_to"] = x.pdc_to;
        j["qdc_fr"] = x.qdc_fr;
        j["qdc_to"] = x.qdc_to;
        j["uid"] = x.uid;
        j["vm_dc_fr"] = x.vm_dc_fr;
        j["vm_dc_to"] = x.vm_dc_to;
    }

    inline void from_json(const json & j, TemporalBoundaryMultipleWindingTransformer& x) {
        x.ext = get_untyped(j, "ext");
        x.ta_w = get_stack_optional<std::variant<std::vector<double>, bool, double, int64_t, std::map<std::string, nlohmann::json>, std::string>>(j, "ta_w");
        x.tm_w = j.at("tm_w").get<std::vector<double>>();
        x.uid = j.at("uid").get<BusFr>();
        x.va_star_node = j.at("va_star_node").get<double>();
        x.vm_star_node = j.at("vm_star_node").get<double>();
    }

    inline void to_json(json & j, const TemporalBoundaryMultipleWindingTransformer & x) {
        j = json::object();
        j["ext"] = x.ext;
        j["ta_w"] = x.ta_w;
        j["tm_w"] = x.tm_w;
        j["uid"] = x.uid;
        j["va_star_node"] = x.va_star_node;
        j["vm_star_node"] = x.vm_star_node;
    }

    inline void from_json(const json & j, TemporalBoundaryShunt& x) {
        x.ext = get_untyped(j, "ext");
        x.num_steps = j.at("num_steps").get<NumStepsUbUnion>();
        x.uid = j.at("uid").get<BusFr>();
    }

    inline void to_json(json & j, const TemporalBoundaryShunt & x) {
        j = json::object();
        j["ext"] = x.ext;
        j["num_steps"] = x.num_steps;
        j["uid"] = x.uid;
    }

    inline void from_json(const json & j, TemporalBoundaryStorage& x) {
        x.energy = j.at("energy").get<double>();
        x.ext = get_untyped(j, "ext");
        x.ps = get_stack_optional<double>(j, "ps");
        x.qs = get_stack_optional<double>(j, "qs");
        x.uid = j.at("uid").get<BusFr>();
    }

    inline void to_json(json & j, const TemporalBoundaryStorage & x) {
        j = json::object();
        j["energy"] = x.energy;
        j["ext"] = x.ext;
        j["ps"] = x.ps;
        j["qs"] = x.qs;
        j["uid"] = x.uid;
    }

    inline void from_json(const json & j, TemporalBoundarySwitch& x) {
        x.ext = get_untyped(j, "ext");
        x.state = j.at("state").get<int64_t>();
        x.uid = j.at("uid").get<BusFr>();
    }

    inline void to_json(json & j, const TemporalBoundarySwitch & x) {
        j = json::object();
        j["ext"] = x.ext;
        j["state"] = x.state;
        j["uid"] = x.uid;
    }

    inline void from_json(const json & j, TemporalBoundaryTransformer& x) {
        x.ext = get_untyped(j, "ext");
        x.ta = j.at("ta").get<double>();
        x.tm = j.at("tm").get<double>();
        x.uid = j.at("uid").get<BusFr>();
    }

    inline void to_json(json & j, const TemporalBoundaryTransformer & x) {
        j = json::object();
        j["ext"] = x.ext;
        j["ta"] = x.ta;
        j["tm"] = x.tm;
        j["uid"] = x.uid;
    }

    inline void from_json(const json & j, TemporalBoundary& x) {
        x.bus = get_stack_optional<std::vector<TemporalBoundaryBus>>(j, "bus");
        x.gen = get_stack_optional<std::vector<TemporalBoundaryGen>>(j, "gen");
        x.global_params = j.at("global_params").get<TemporalBoundaryGlobalParams>();
        x.hvdc_p2_p = get_stack_optional<std::vector<TemporalBoundaryHvdcP2P>>(j, "hvdc_p2p");
        x.multiple_winding_transformer = get_stack_optional<std::vector<TemporalBoundaryMultipleWindingTransformer>>(j, "multiple_winding_transformer");
        x.shunt = get_stack_optional<std::vector<TemporalBoundaryShunt>>(j, "shunt");
        x.storage = get_stack_optional<std::vector<TemporalBoundaryStorage>>(j, "storage");
        x.temporal_boundary_switch = get_stack_optional<std::vector<TemporalBoundarySwitch>>(j, "switch");
        x.transformer = get_stack_optional<std::vector<TemporalBoundaryTransformer>>(j, "transformer");
    }

    inline void to_json(json & j, const TemporalBoundary & x) {
        j = json::object();
        j["bus"] = x.bus;
        j["gen"] = x.gen;
        j["global_params"] = x.global_params;
        j["hvdc_p2p"] = x.hvdc_p2_p;
        j["multiple_winding_transformer"] = x.multiple_winding_transformer;
        j["shunt"] = x.shunt;
        j["storage"] = x.storage;
        j["switch"] = x.temporal_boundary_switch;
        j["transformer"] = x.transformer;
    }

    inline void from_json(const json & j, CtmDataTimeSeriesData& x) {
        x.ext = get_stack_optional<std::vector<nlohmann::json>>(j, "ext");
        x.name = get_stack_optional<std::vector<std::string>>(j, "name");
        x.path_to_file = get_stack_optional<std::variant<std::vector<std::string>, std::string>>(j, "path_to_file");
        x.timestamp = get_stack_optional<std::vector<double>>(j, "timestamp");
        x.uid = j.at("uid").get<std::vector<BusFr>>();
        x.values = get_stack_optional<std::vector<std::vector<nlohmann::json>>>(j, "values");
    }

    inline void to_json(json & j, const CtmDataTimeSeriesData & x) {
        j = json::object();
        j["ext"] = x.ext;
        j["name"] = x.name;
        j["path_to_file"] = x.path_to_file;
        j["timestamp"] = x.timestamp;
        j["uid"] = x.uid;
        j["values"] = x.values;
    }

    inline void from_json(const json & j, CtmData& x) {
        x.ctm_version = j.at("ctm_version").get<std::string>();
        x.network = j.at("network").get<Network>();
        x.temporal_boundary = j.at("temporal_boundary").get<TemporalBoundary>();
        x.time_series_data = get_stack_optional<CtmDataTimeSeriesData>(j, "time_series_data");
    }

    inline void to_json(json & j, const CtmData & x) {
        j = json::object();
        j["ctm_version"] = x.ctm_version;
        j["network"] = x.network;
        j["temporal_boundary"] = x.temporal_boundary;
        j["time_series_data"] = x.time_series_data;
    }

    inline void from_json(const json & j, CtmSolutionSchema& x) {
        x.scale_factor = j.at("scale_factor").get<double>();
        x.uid = j.at("uid").get<BusFr>();
    }

    inline void to_json(json & j, const CtmSolutionSchema & x) {
        j = json::object();
        j["scale_factor"] = x.scale_factor;
        j["uid"] = x.uid;
    }

    inline void from_json(const json & j, SolutionAcLine& x) {
        x.ext = get_untyped(j, "ext");
        x.pl_fr = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "pl_fr");
        x.pl_to = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "pl_to");
        x.ql_fr = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "ql_fr");
        x.ql_to = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "ql_to");
        x.uid = j.at("uid").get<BusFr>();
    }

    inline void to_json(json & j, const SolutionAcLine & x) {
        j = json::object();
        j["ext"] = x.ext;
        j["pl_fr"] = x.pl_fr;
        j["pl_to"] = x.pl_to;
        j["ql_fr"] = x.ql_fr;
        j["ql_to"] = x.ql_to;
        j["uid"] = x.uid;
    }

    inline void from_json(const json & j, SolutionBus& x) {
        x.ext = get_untyped(j, "ext");
        x.p_imbalance = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "p_imbalance");
        x.p_lambda = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "p_lambda");
        x.q_imbalance = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "q_imbalance");
        x.q_lambda = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "q_lambda");
        x.uid = j.at("uid").get<BusFr>();
        x.va = j.at("va").get<PlFr>();
        x.vm = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "vm");
    }

    inline void to_json(json & j, const SolutionBus & x) {
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
        x.uid = j.at("uid").get<BusFr>();
    }

    inline void to_json(json & j, const ReserveProvision & x) {
        j = json::object();
        j["rg"] = x.rg;
        j["uid"] = x.uid;
    }

    inline void from_json(const json & j, SolutionGen& x) {
        x.ext = get_untyped(j, "ext");
        x.in_service = get_stack_optional<std::variant<CtmSolutionSchema, int64_t>>(j, "in_service");
        x.pg = j.at("pg").get<PlFr>();
        x.qg = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "qg");
        x.reserve_provision = get_stack_optional<std::vector<ReserveProvision>>(j, "reserve_provision");
        x.uid = j.at("uid").get<BusFr>();
    }

    inline void to_json(json & j, const SolutionGen & x) {
        j = json::object();
        j["ext"] = x.ext;
        j["in_service"] = x.in_service;
        j["pg"] = x.pg;
        j["qg"] = x.qg;
        j["reserve_provision"] = x.reserve_provision;
        j["uid"] = x.uid;
    }

    inline void from_json(const json & j, SolutionGlobalParams& x) {
        x.base_mva = get_stack_optional<double>(j, "base_mva");
        x.unit_convention = j.at("unit_convention").get<UnitConvention>();
    }

    inline void to_json(json & j, const SolutionGlobalParams & x) {
        j = json::object();
        j["base_mva"] = x.base_mva;
        j["unit_convention"] = x.unit_convention;
    }

    inline void from_json(const json & j, SolutionHvdcP2P& x) {
        x.ext = get_untyped(j, "ext");
        x.pdc_fr = j.at("pdc_fr").get<PlFr>();
        x.pdc_to = j.at("pdc_to").get<PlFr>();
        x.qdc_fr = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "qdc_fr");
        x.qdc_to = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "qdc_to");
        x.uid = j.at("uid").get<BusFr>();
        x.vm_dc = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "vm_dc");
    }

    inline void to_json(json & j, const SolutionHvdcP2P & x) {
        j = json::object();
        j["ext"] = x.ext;
        j["pdc_fr"] = x.pdc_fr;
        j["pdc_to"] = x.pdc_to;
        j["qdc_fr"] = x.qdc_fr;
        j["qdc_to"] = x.qdc_to;
        j["uid"] = x.uid;
        j["vm_dc"] = x.vm_dc;
    }

    inline void from_json(const json & j, SolutionMultipleWindingTransformer& x) {
        x.ext = get_untyped(j, "ext");
        x.pt_w = get_stack_optional<std::vector<PlFr>>(j, "pt_w");
        x.qt_w = get_stack_optional<std::vector<PlFr>>(j, "qt_w");
        x.ta_w = get_stack_optional<std::vector<PlFr>>(j, "ta_w");
        x.tm_w = get_stack_optional<std::vector<PlFr>>(j, "tm_w");
        x.uid = j.at("uid").get<BusFr>();
        x.va_star_node = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "va_star_node");
        x.vm_star_node = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "vm_star_node");
    }

    inline void to_json(json & j, const SolutionMultipleWindingTransformer & x) {
        j = json::object();
        j["ext"] = x.ext;
        j["pt_w"] = x.pt_w;
        j["qt_w"] = x.qt_w;
        j["ta_w"] = x.ta_w;
        j["tm_w"] = x.tm_w;
        j["uid"] = x.uid;
        j["va_star_node"] = x.va_star_node;
        j["vm_star_node"] = x.vm_star_node;
    }

    inline void from_json(const json & j, SolutionReserve& x) {
        x.ext = get_untyped(j, "ext");
        x.shortfall = j.at("shortfall").get<PlFr>();
        x.uid = j.at("uid").get<BusFr>();
    }

    inline void to_json(json & j, const SolutionReserve & x) {
        j = json::object();
        j["ext"] = x.ext;
        j["shortfall"] = x.shortfall;
        j["uid"] = x.uid;
    }

    inline void from_json(const json & j, SolutionShunt& x) {
        x.ext = get_untyped(j, "ext");
        x.num_steps = j.at("num_steps").get<PurpleNumSteps>();
        x.uid = j.at("uid").get<BusFr>();
    }

    inline void to_json(json & j, const SolutionShunt & x) {
        j = json::object();
        j["ext"] = x.ext;
        j["num_steps"] = x.num_steps;
        j["uid"] = x.uid;
    }

    inline void from_json(const json & j, SolutionSwitch& x) {
        x.ext = get_untyped(j, "ext");
        x.psw_fr = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "psw_fr");
        x.qsw_fr = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "qsw_fr");
        x.state = j.at("state").get<InService>();
        x.uid = j.at("uid").get<BusFr>();
    }

    inline void to_json(json & j, const SolutionSwitch & x) {
        j = json::object();
        j["ext"] = x.ext;
        j["psw_fr"] = x.psw_fr;
        j["qsw_fr"] = x.qsw_fr;
        j["state"] = x.state;
        j["uid"] = x.uid;
    }

    inline void from_json(const json & j, SolutionStorage& x) {
        x.charge = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "charge");
        x.discharge = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "discharge");
        x.energy = j.at("energy").get<Rg>();
        x.ext = get_untyped(j, "ext");
        x.ps = j.at("ps").get<PlFr>();
        x.qs = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "qs");
        x.uid = j.at("uid").get<BusFr>();
    }

    inline void to_json(json & j, const SolutionStorage & x) {
        j = json::object();
        j["charge"] = x.charge;
        j["discharge"] = x.discharge;
        j["energy"] = x.energy;
        j["ext"] = x.ext;
        j["ps"] = x.ps;
        j["qs"] = x.qs;
        j["uid"] = x.uid;
    }

    inline void from_json(const json & j, SolutionTransformer& x) {
        x.ext = get_untyped(j, "ext");
        x.pt_fr = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "pt_fr");
        x.pt_to = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "pt_to");
        x.qt_fr = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "qt_fr");
        x.qt_to = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "qt_to");
        x.ta = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "ta");
        x.tm = get_stack_optional<std::variant<CtmSolutionSchema, double>>(j, "tm");
        x.uid = j.at("uid").get<BusFr>();
    }

    inline void to_json(json & j, const SolutionTransformer & x) {
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
        x.ac_line = get_stack_optional<std::vector<SolutionAcLine>>(j, "ac_line");
        x.bus = j.at("bus").get<std::vector<SolutionBus>>();
        x.gen = j.at("gen").get<std::vector<SolutionGen>>();
        x.global_params = j.at("global_params").get<SolutionGlobalParams>();
        x.hvdc_p2_p = get_stack_optional<std::vector<SolutionHvdcP2P>>(j, "hvdc_p2p");
        x.multiple_winding_transformer = get_stack_optional<std::vector<SolutionMultipleWindingTransformer>>(j, "multiple_winding_transformer");
        x.reserve = get_stack_optional<std::vector<SolutionReserve>>(j, "reserve");
        x.shunt = get_stack_optional<std::vector<SolutionShunt>>(j, "shunt");
        x.storage = get_stack_optional<std::vector<SolutionStorage>>(j, "storage");
        x.solution_switch = get_stack_optional<std::vector<SolutionSwitch>>(j, "switch");
        x.transformer = get_stack_optional<std::vector<SolutionTransformer>>(j, "transformer");
    }

    inline void to_json(json & j, const Solution & x) {
        j = json::object();
        j["ac_line"] = x.ac_line;
        j["bus"] = x.bus;
        j["gen"] = x.gen;
        j["global_params"] = x.global_params;
        j["hvdc_p2p"] = x.hvdc_p2_p;
        j["multiple_winding_transformer"] = x.multiple_winding_transformer;
        j["reserve"] = x.reserve;
        j["shunt"] = x.shunt;
        j["storage"] = x.storage;
        j["switch"] = x.solution_switch;
        j["transformer"] = x.transformer;
    }

    inline void from_json(const json & j, CtmSolutionTimeSeriesData& x) {
        x.ext = get_stack_optional<std::vector<nlohmann::json>>(j, "ext");
        x.name = get_stack_optional<std::vector<std::string>>(j, "name");
        x.path_to_file = get_stack_optional<std::variant<std::vector<std::string>, std::string>>(j, "path_to_file");
        x.timestamp = get_stack_optional<std::vector<double>>(j, "timestamp");
        x.uid = j.at("uid").get<std::vector<BusFr>>();
        x.values = get_stack_optional<std::vector<std::vector<nlohmann::json>>>(j, "values");
    }

    inline void to_json(json & j, const CtmSolutionTimeSeriesData & x) {
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
        x.time_series_data = get_stack_optional<CtmSolutionTimeSeriesData>(j, "time_series_data");
    }

    inline void to_json(json & j, const CtmSolution & x) {
        j = json::object();
        j["ctm_version"] = x.ctm_version;
        j["solution"] = x.solution;
        j["time_series_data"] = x.time_series_data;
    }

    inline void from_json(const json & j, CtmTimeSeriesDataTimeSeriesData& x) {
        x.ext = get_stack_optional<std::vector<nlohmann::json>>(j, "ext");
        x.name = get_stack_optional<std::vector<std::string>>(j, "name");
        x.path_to_file = get_stack_optional<std::variant<std::vector<std::string>, std::string>>(j, "path_to_file");
        x.timestamp = get_stack_optional<std::vector<double>>(j, "timestamp");
        x.uid = j.at("uid").get<std::vector<BusFr>>();
        x.values = get_stack_optional<std::vector<std::vector<nlohmann::json>>>(j, "values");
    }

    inline void to_json(json & j, const CtmTimeSeriesDataTimeSeriesData & x) {
        j = json::object();
        j["ext"] = x.ext;
        j["name"] = x.name;
        j["path_to_file"] = x.path_to_file;
        j["timestamp"] = x.timestamp;
        j["uid"] = x.uid;
        j["values"] = x.values;
    }

    inline void from_json(const json & j, CtmTimeSeriesData& x) {
        x.ctm_version = j.at("ctm_version").get<std::string>();
        x.time_series_data = j.at("time_series_data").get<CtmTimeSeriesDataTimeSeriesData>();
    }

    inline void to_json(json & j, const CtmTimeSeriesData & x) {
        j = json::object();
        j["ctm_version"] = x.ctm_version;
        j["time_series_data"] = x.time_series_data;
    }

    inline void from_json(const json & j, TypeEnum & x) {
        if (j == "PQ") x = TypeEnum::PQ;
        else if (j == "PV") x = TypeEnum::PV;
        else if (j == "slack") x = TypeEnum::SLACK;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const TypeEnum & x) {
        switch (x) {
            case TypeEnum::PQ: j = "PQ"; break;
            case TypeEnum::PV: j = "PV"; break;
            case TypeEnum::SLACK: j = "slack"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, CostPgModel & x) {
        if (j == "MARGINAL_COST") x = CostPgModel::MARGINAL_COST;
        else if (j == "PIECEWISE_LINEAR") x = CostPgModel::PIECEWISE_LINEAR;
        else if (j == "POLYNOMIAL") x = CostPgModel::POLYNOMIAL;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const CostPgModel & x) {
        switch (x) {
            case CostPgModel::MARGINAL_COST: j = "MARGINAL_COST"; break;
            case CostPgModel::PIECEWISE_LINEAR: j = "PIECEWISE_LINEAR"; break;
            case CostPgModel::POLYNOMIAL: j = "POLYNOMIAL"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, PrimarySource & x) {
        if (j == "BIOMASS") x = PrimarySource::BIOMASS;
        else if (j == "COAL") x = PrimarySource::COAL;
        else if (j == "GAS") x = PrimarySource::GAS;
        else if (j == "GEOTHERMAL") x = PrimarySource::GEOTHERMAL;
        else if (j == "HYDRO") x = PrimarySource::HYDRO;
        else if (j == "NUCLEAR") x = PrimarySource::NUCLEAR;
        else if (j == "OIL") x = PrimarySource::OIL;
        else if (j == "OTHER") x = PrimarySource::OTHER;
        else if (j == "SOLAR") x = PrimarySource::SOLAR;
        else if (j == "WIND") x = PrimarySource::WIND;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const PrimarySource & x) {
        switch (x) {
            case PrimarySource::BIOMASS: j = "BIOMASS"; break;
            case PrimarySource::COAL: j = "COAL"; break;
            case PrimarySource::GAS: j = "GAS"; break;
            case PrimarySource::GEOTHERMAL: j = "GEOTHERMAL"; break;
            case PrimarySource::HYDRO: j = "HYDRO"; break;
            case PrimarySource::NUCLEAR: j = "NUCLEAR"; break;
            case PrimarySource::OIL: j = "OIL"; break;
            case PrimarySource::OTHER: j = "OTHER"; break;
            case PrimarySource::SOLAR: j = "SOLAR"; break;
            case PrimarySource::WIND: j = "WIND"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, PrimarySourceSubtype & x) {
        static std::unordered_map<std::string, PrimarySourceSubtype> enumValues {
            {"AG_BIPRODUCT", PrimarySourceSubtype::AG_BIPRODUCT},
            {"ANTRHC_BITMN_COAL", PrimarySourceSubtype::ANTRHC_BITMN_COAL},
            {"DISTILLATE_FUEL_OIL", PrimarySourceSubtype::DISTILLATE_FUEL_OIL},
            {"GEOTHERMAL", PrimarySourceSubtype::GEOTHERMAL},
            {"HYDRO_DAM", PrimarySourceSubtype::HYDRO_DAM},
            {"HYDRO_PUMPED_STORAGE", PrimarySourceSubtype::HYDRO_PUMPED_STORAGE},
            {"HYDRO_RUN_OF_THE_RIVER", PrimarySourceSubtype::HYDRO_RUN_OF_THE_RIVER},
            {"MUNICIPAL_WASTE", PrimarySourceSubtype::MUNICIPAL_WASTE},
            {"NATURAL_GAS", PrimarySourceSubtype::NATURAL_GAS},
            {"NUCLEAR", PrimarySourceSubtype::NUCLEAR},
            {"OTHER", PrimarySourceSubtype::OTHER},
            {"OTHER_GAS", PrimarySourceSubtype::OTHER_GAS},
            {"PETROLEUM_COKE", PrimarySourceSubtype::PETROLEUM_COKE},
            {"RESIDUAL_FUEL_OIL", PrimarySourceSubtype::RESIDUAL_FUEL_OIL},
            {"SOLAR_CSP", PrimarySourceSubtype::SOLAR_CSP},
            {"SOLAR_PV", PrimarySourceSubtype::SOLAR_PV},
            {"WASTE_COAL", PrimarySourceSubtype::WASTE_COAL},
            {"WASTE_OIL", PrimarySourceSubtype::WASTE_OIL},
            {"WIND_OFFSHORE", PrimarySourceSubtype::WIND_OFFSHORE},
            {"WIND_ONSHORE", PrimarySourceSubtype::WIND_ONSHORE},
            {"WOOD_WASTE", PrimarySourceSubtype::WOOD_WASTE},
        };
        auto iter = enumValues.find(j.get<std::string>());
        if (iter != enumValues.end()) {
            x = iter->second;
        }
    }

    inline void to_json(json & j, const PrimarySourceSubtype & x) {
        switch (x) {
            case PrimarySourceSubtype::AG_BIPRODUCT: j = "AG_BIPRODUCT"; break;
            case PrimarySourceSubtype::ANTRHC_BITMN_COAL: j = "ANTRHC_BITMN_COAL"; break;
            case PrimarySourceSubtype::DISTILLATE_FUEL_OIL: j = "DISTILLATE_FUEL_OIL"; break;
            case PrimarySourceSubtype::GEOTHERMAL: j = "GEOTHERMAL"; break;
            case PrimarySourceSubtype::HYDRO_DAM: j = "HYDRO_DAM"; break;
            case PrimarySourceSubtype::HYDRO_PUMPED_STORAGE: j = "HYDRO_PUMPED_STORAGE"; break;
            case PrimarySourceSubtype::HYDRO_RUN_OF_THE_RIVER: j = "HYDRO_RUN_OF_THE_RIVER"; break;
            case PrimarySourceSubtype::MUNICIPAL_WASTE: j = "MUNICIPAL_WASTE"; break;
            case PrimarySourceSubtype::NATURAL_GAS: j = "NATURAL_GAS"; break;
            case PrimarySourceSubtype::NUCLEAR: j = "NUCLEAR"; break;
            case PrimarySourceSubtype::OTHER: j = "OTHER"; break;
            case PrimarySourceSubtype::OTHER_GAS: j = "OTHER_GAS"; break;
            case PrimarySourceSubtype::PETROLEUM_COKE: j = "PETROLEUM_COKE"; break;
            case PrimarySourceSubtype::RESIDUAL_FUEL_OIL: j = "RESIDUAL_FUEL_OIL"; break;
            case PrimarySourceSubtype::SOLAR_CSP: j = "SOLAR_CSP"; break;
            case PrimarySourceSubtype::SOLAR_PV: j = "SOLAR_PV"; break;
            case PrimarySourceSubtype::WASTE_COAL: j = "WASTE_COAL"; break;
            case PrimarySourceSubtype::WASTE_OIL: j = "WASTE_OIL"; break;
            case PrimarySourceSubtype::WIND_OFFSHORE: j = "WIND_OFFSHORE"; break;
            case PrimarySourceSubtype::WIND_ONSHORE: j = "WIND_ONSHORE"; break;
            case PrimarySourceSubtype::WOOD_WASTE: j = "WOOD_WASTE"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
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

    inline void from_json(const json & j, Technology & x) {
        if (j == "LCC") x = Technology::LCC;
        else if (j == "MMC") x = Technology::MMC;
        else if (j == "VSC") x = Technology::VSC;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const Technology & x) {
        switch (x) {
            case Technology::LCC: j = "LCC"; break;
            case Technology::MMC: j = "MMC"; break;
            case Technology::VSC: j = "VSC"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, ReserveType & x) {
        if (j == "PRIMARY") x = ReserveType::PRIMARY;
        else if (j == "SECONDARY") x = ReserveType::SECONDARY;
        else if (j == "TERTIARY") x = ReserveType::TERTIARY;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const ReserveType & x) {
        switch (x) {
            case ReserveType::PRIMARY: j = "PRIMARY"; break;
            case ReserveType::SECONDARY: j = "SECONDARY"; break;
            case ReserveType::TERTIARY: j = "TERTIARY"; break;
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

    inline void adl_serializer<std::variant<ctm_schemas::CmUbAClass, double>>::from_json(const json & j, std::variant<ctm_schemas::CmUbAClass, double> & x) {
        if (j.is_number())
            x = j.get<double>();
        else if (j.is_object())
            x = j.get<ctm_schemas::CmUbAClass>();
        else throw std::runtime_error("Could not deserialise!");
    }

    inline void adl_serializer<std::variant<ctm_schemas::CmUbAClass, double>>::to_json(json & j, const std::variant<ctm_schemas::CmUbAClass, double> & x) {
        switch (x.index()) {
            case 0:
                j = std::get<ctm_schemas::CmUbAClass>(x);
                break;
            case 1:
                j = std::get<double>(x);
                break;
            default: throw std::runtime_error("Input JSON does not conform to schema!");
        }
    }

    inline void adl_serializer<std::variant<ctm_schemas::CmUbAClass, ctm_schemas::TypeEnum>>::from_json(const json & j, std::variant<ctm_schemas::CmUbAClass, ctm_schemas::TypeEnum> & x) {
        if (j.is_object())
            x = j.get<ctm_schemas::CmUbAClass>();
        else if (j.is_string())
            x = j.get<ctm_schemas::TypeEnum>();
        else throw std::runtime_error("Could not deserialise!");
    }

    inline void adl_serializer<std::variant<ctm_schemas::CmUbAClass, ctm_schemas::TypeEnum>>::to_json(json & j, const std::variant<ctm_schemas::CmUbAClass, ctm_schemas::TypeEnum> & x) {
        switch (x.index()) {
            case 0:
                j = std::get<ctm_schemas::CmUbAClass>(x);
                break;
            case 1:
                j = std::get<ctm_schemas::TypeEnum>(x);
                break;
            default: throw std::runtime_error("Input JSON does not conform to schema!");
        }
    }

    inline void adl_serializer<std::variant<std::vector<double>, ctm_schemas::CostPgParametersClass>>::from_json(const json & j, std::variant<std::vector<double>, ctm_schemas::CostPgParametersClass> & x) {
        if (j.is_object())
            x = j.get<ctm_schemas::CostPgParametersClass>();
        else if (j.is_array())
            x = j.get<std::vector<double>>();
        else throw std::runtime_error("Could not deserialise!");
    }

    inline void adl_serializer<std::variant<std::vector<double>, ctm_schemas::CostPgParametersClass>>::to_json(json & j, const std::variant<std::vector<double>, ctm_schemas::CostPgParametersClass> & x) {
        switch (x.index()) {
            case 0:
                j = std::get<std::vector<double>>(x);
                break;
            case 1:
                j = std::get<ctm_schemas::CostPgParametersClass>(x);
                break;
            default: throw std::runtime_error("Input JSON does not conform to schema!");
        }
    }

    inline void adl_serializer<std::variant<ctm_schemas::CmUbAClass, int64_t>>::from_json(const json & j, std::variant<ctm_schemas::CmUbAClass, int64_t> & x) {
        if (j.is_number_integer())
            x = j.get<int64_t>();
        else if (j.is_object())
            x = j.get<ctm_schemas::CmUbAClass>();
        else throw std::runtime_error("Could not deserialise!");
    }

    inline void adl_serializer<std::variant<ctm_schemas::CmUbAClass, int64_t>>::to_json(json & j, const std::variant<ctm_schemas::CmUbAClass, int64_t> & x) {
        switch (x.index()) {
            case 0:
                j = std::get<ctm_schemas::CmUbAClass>(x);
                break;
            case 1:
                j = std::get<int64_t>(x);
                break;
            default: throw std::runtime_error("Input JSON does not conform to schema!");
        }
    }

    inline void adl_serializer<std::variant<std::vector<double>, double>>::from_json(const json & j, std::variant<std::vector<double>, double> & x) {
        if (j.is_number())
            x = j.get<double>();
        else if (j.is_array())
            x = j.get<std::vector<double>>();
        else throw std::runtime_error("Could not deserialise!");
    }

    inline void adl_serializer<std::variant<std::vector<double>, double>>::to_json(json & j, const std::variant<std::vector<double>, double> & x) {
        switch (x.index()) {
            case 0:
                j = std::get<std::vector<double>>(x);
                break;
            case 1:
                j = std::get<double>(x);
                break;
            default: throw std::runtime_error("Input JSON does not conform to schema!");
        }
    }

    inline void adl_serializer<std::variant<std::vector<int64_t>, int64_t>>::from_json(const json & j, std::variant<std::vector<int64_t>, int64_t> & x) {
        if (j.is_number_integer())
            x = j.get<int64_t>();
        else if (j.is_array())
            x = j.get<std::vector<int64_t>>();
        else throw std::runtime_error("Could not deserialise!");
    }

    inline void adl_serializer<std::variant<std::vector<int64_t>, int64_t>>::to_json(json & j, const std::variant<std::vector<int64_t>, int64_t> & x) {
        switch (x.index()) {
            case 0:
                j = std::get<std::vector<int64_t>>(x);
                break;
            case 1:
                j = std::get<int64_t>(x);
                break;
            default: throw std::runtime_error("Input JSON does not conform to schema!");
        }
    }

    inline void adl_serializer<std::variant<std::vector<double>, bool, double, int64_t, std::map<std::string, json>, std::string>>::from_json(const json & j, std::variant<std::vector<double>, bool, double, int64_t, std::map<std::string, json>, std::string> & x) {
        if (j.is_boolean())
            x = j.get<bool>();
        else if (j.is_number_integer())
            x = j.get<int64_t>();
        else if (j.is_number())
            x = j.get<double>();
        else if (j.is_string())
            x = j.get<std::string>();
        else if (j.is_object())
            x = j.get<std::map<std::string, json>>();
        else if (j.is_array())
            x = j.get<std::vector<double>>();
        else throw std::runtime_error("Could not deserialise!");
    }

    inline void adl_serializer<std::variant<std::vector<double>, bool, double, int64_t, std::map<std::string, json>, std::string>>::to_json(json & j, const std::variant<std::vector<double>, bool, double, int64_t, std::map<std::string, json>, std::string> & x) {
        switch (x.index()) {
            case 0:
                j = std::get<std::vector<double>>(x);
                break;
            case 1:
                j = std::get<bool>(x);
                break;
            case 2:
                j = std::get<double>(x);
                break;
            case 3:
                j = std::get<int64_t>(x);
                break;
            case 4:
                j = std::get<std::map<std::string, json>>(x);
                break;
            case 5:
                j = std::get<std::string>(x);
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

    inline void adl_serializer<std::variant<ctm_schemas::CtmSolutionSchema, double>>::from_json(const json & j, std::variant<ctm_schemas::CtmSolutionSchema, double> & x) {
        if (j.is_number())
            x = j.get<double>();
        else if (j.is_object())
            x = j.get<ctm_schemas::CtmSolutionSchema>();
        else throw std::runtime_error("Could not deserialise!");
    }

    inline void adl_serializer<std::variant<ctm_schemas::CtmSolutionSchema, double>>::to_json(json & j, const std::variant<ctm_schemas::CtmSolutionSchema, double> & x) {
        switch (x.index()) {
            case 0:
                j = std::get<ctm_schemas::CtmSolutionSchema>(x);
                break;
            case 1:
                j = std::get<double>(x);
                break;
            default: throw std::runtime_error("Input JSON does not conform to schema!");
        }
    }

    inline void adl_serializer<std::variant<ctm_schemas::CtmSolutionSchema, int64_t>>::from_json(const json & j, std::variant<ctm_schemas::CtmSolutionSchema, int64_t> & x) {
        if (j.is_number_integer())
            x = j.get<int64_t>();
        else if (j.is_object())
            x = j.get<ctm_schemas::CtmSolutionSchema>();
        else throw std::runtime_error("Could not deserialise!");
    }

    inline void adl_serializer<std::variant<ctm_schemas::CtmSolutionSchema, int64_t>>::to_json(json & j, const std::variant<ctm_schemas::CtmSolutionSchema, int64_t> & x) {
        switch (x.index()) {
            case 0:
                j = std::get<ctm_schemas::CtmSolutionSchema>(x);
                break;
            case 1:
                j = std::get<int64_t>(x);
                break;
            default: throw std::runtime_error("Input JSON does not conform to schema!");
        }
    }

    inline void adl_serializer<std::variant<std::vector<int64_t>, ctm_schemas::CtmSolutionSchema, int64_t>>::from_json(const json & j, std::variant<std::vector<int64_t>, ctm_schemas::CtmSolutionSchema, int64_t> & x) {
        if (j.is_number_integer())
            x = j.get<int64_t>();
        else if (j.is_object())
            x = j.get<ctm_schemas::CtmSolutionSchema>();
        else if (j.is_array())
            x = j.get<std::vector<int64_t>>();
        else throw std::runtime_error("Could not deserialise!");
    }

    inline void adl_serializer<std::variant<std::vector<int64_t>, ctm_schemas::CtmSolutionSchema, int64_t>>::to_json(json & j, const std::variant<std::vector<int64_t>, ctm_schemas::CtmSolutionSchema, int64_t> & x) {
        switch (x.index()) {
            case 0:
                j = std::get<std::vector<int64_t>>(x);
                break;
            case 1:
                j = std::get<ctm_schemas::CtmSolutionSchema>(x);
                break;
            case 2:
                j = std::get<int64_t>(x);
                break;
            default: throw std::runtime_error("Input JSON does not conform to schema!");
        }
    }
}
