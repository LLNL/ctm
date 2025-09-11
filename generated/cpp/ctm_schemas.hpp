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

    using BusRefElement = std::variant<int64_t, std::string>;

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
        BusRefElement uid;
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
        BusRefElement bus_fr;
        /**
         * uid of bus at the to terminal of ac line
         */
        BusRefElement bus_to;
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
        BusRefElement uid;
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
        BusRefElement uid;
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
        std::optional<BusRefElement> area;
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
        BusRefElement uid;
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
        std::optional<BusRefElement> zone;
    };

    /**
     * class of dynamic model, e.g., machine, excitation, etc.
     */
    enum class Category : int { EXCITER, MACHINE, PRIME_MOVER, STABILIZER, WIND_TURBINE };

    using DynamicAndControlGen = std::variant<std::vector<BusRefElement>, int64_t, std::string>;

    using KA = std::variant<double, int64_t>;

    /**
     * type of dynamic model in this record
     */
    enum class Model : int { ESAC1_A, ESAC6_A, ESDC1_A, ESDC2_A, ESST4_B, EXAC1, EXAC2, EXPIC1, GAST, GENROU, GENSAL, GGOV1, HYGOV, IEEEG1, IEEEST, IEEET1, REEC_A, REGC_A, REPC_A, REPC_B, SCRX, SEXS, TGOV1, WTGA_A, WTGP_A, WTGQ_A, WTGT_A };

    /**
     * type of monitored branch
     */
    enum class MonitoredBranchType : int { AC_LINE, HVDC_P2_P, TRASFORMER };

    using Bs = std::variant<std::vector<double>, double>;

    /**
     * genrou model record; see
     * https://www.powerworld.com/WebHelp/Content/TransientModels_HTML/Machine%20Model%20GENROU.htm
     *
     * gensal model record; see
     * https://www.powerworld.com/WebHelp/Content/TransientModels_HTML/Machine%20Model%20GENSAL.htm
     *
     * esac1a model record; see
     * https://www.powerworld.com/WebHelp/Content/TransientModels_HTML/Exciter%20ESAC1A.htm
     *
     * esac6a model record; see
     * https://www.powerworld.com/WebHelp/Content/TransientModels_HTML/Exciter%20ESAC6A.htm
     *
     * esdc1a or esdc2a model record; esdc1a:
     * https://www.powerworld.com/WebHelp/Content/TransientModels_HTML/Exciter%20ESDC1A.htm;
     * esdc2a:
     * https://www.powerworld.com/WebHelp/Content/TransientModels_HTML/Exciter%20ESDC2A.htm
     *
     * esst4b model record; see
     * https://www.powerworld.com/WebHelp/Content/TransientModels_HTML/Exciter%20ESST4B.htm
     *
     * exac1 model record; see
     * https://www.powerworld.com/WebHelp/Content/TransientModels_HTML/Exciter%20EXAC1.htm
     *
     * exac2 model record; see
     * https://www.powerworld.com/WebHelp/Content/TransientModels_HTML/Exciter%20EXAC2.htm
     *
     * expic1 model record; see
     * https://www.powerworld.com/WebHelp/Content/TransientModels_HTML/Exciter%20EXPIC1.htm
     *
     * ieeet1 model record; see
     * https://www.powerworld.com/WebHelp/Content/TransientModels_HTML/Exciter%20IEEET1.htm
     *
     * scrx model record; see
     * https://www.powerworld.com/WebHelp/Content/TransientModels_HTML/Exciter%20SCRX.htm
     *
     * sexs model record; see
     * https://www.powerworld.com/WebHelp/Content/TransientModels_HTML/Exciter%20SEXS_GE.htm
     *
     * gast model record; see
     * https://www.powerworld.com/WebHelp/Content/TransientModels_HTML/Governor%20GAST_GE.htm
     *
     * ggov1 model record; see
     * https://www.powerworld.com/WebHelp/Content/TransientModels_HTML/Governor%20GGOV1%20and%20GGOV1D.htm
     *
     * hygov model record; see
     * https://www.powerworld.com/WebHelp/Content/TransientModels_HTML/Governor%20HYGOV%20and%20HYGOVD.htm
     *
     * ieeeg1 model record; see
     * https://www.powerworld.com/WebHelp/Content/TransientModels_HTML/Governor%20IEEEG1,%20IEEEG1D%20and%20IEEEG1_GE.htm
     *
     * tgov1 model record; see
     * https://www.powerworld.com/WebHelp/Content/TransientModels_HTML/Governor%20TGOV1%20and%20TGOV1D.htm
     *
     * ieeest model record; see
     * https://www.powerworld.com/WebHelp/Content/TransientModels_HTML/Stabilizer%20IEEEST.htm
     *
     * wtga_a model record; see
     * https://www.powerworld.com/WebHelp/Content/TransientModels_HTML/Aerodynamic%20Model%20WTGAR_A.htm
     *
     * wtgp_a model record; see
     * https://www.powerworld.com/WebHelp/Content/TransientModels_HTML/Stabilizer%20WTGPT_A.htm
     * (WTPTA1)
     *
     * wtgq_a model record; see
     * https://www.powerworld.com/WebHelp/Content/TransientModels_HTML/Pref%20Controller%20WTGTRQ_A.htm
     * (WTGTQ_A)
     *
     * wtgt_a model record; see
     * https://www.powerworld.com/WebHelp/Content/TransientModels_HTML/Governor%20WTGT_A.htm
     *
     * regc_a model record; see
     * https://www.powerworld.com/WebHelp/Content/TransientModels_HTML/Machine%20Model%20REGC_A.htm
     *
     * reec_a model record; see
     * https://www.powerworld.com/WebHelp/Content/TransientModels_HTML/Exciter%20REEC_A.htm
     *
     * repc_a model record; see
     * https://www.powerworld.com/WebHelp/Content/TransientModels_HTML/Plant%20Controller%20REPC_A.htm
     *
     * repc_b model record; see
     * https://www.powerworld.com/WebHelp/Content/TransientModels_HTML/Plant%20Controller%20REPC_B.htm
     */
    struct DynamicAndControl {
        /**
         * class of dynamic model, e.g., machine, excitation, etc.
         */
        Category category;
        /**
         * [p.u.] Damping factor
         */
        std::optional<double> d;
        std::optional<DynamicAndControlGen> gen;
        /**
         * [second] Inertia constant
         */
        std::optional<double> h;
        /**
         * [p.u.] Stator leakage reactance
         */
        std::optional<double> l_1;
        /**
         * [p.u.] D-axis synchronous reactance
         */
        std::optional<double> l_d;
        /**
         * [p.u.] D-axis transient reactance
         */
        std::optional<double> l_pd;
        /**
         * [p.u.] D-axis sub-transient reactance
         */
        std::optional<double> l_ppd;
        /**
         * [p.u.] Q-axis transient reactance
         */
        std::optional<double> l_pq;
        /**
         * [p.u.] Q-axis synchronous reactance
         */
        std::optional<double> l_q;
        /**
         * type of dynamic model in this record
         */
        Model model;
        /**
         * [p.u.] Stator resistance
         */
        std::optional<double> r_a;
        /**
         * [p.u.] Compounding resistance for voltage control
         */
        std::optional<double> r_comp;
        /**
         * [none] Saturation factor at 1 pu flux
         */
        std::optional<double> s_1;
        /**
         * [none] Saturation factor at 1.2 pu flux
         */
        std::optional<double> s_12;
        /**
         * [second] D-axis transient rotor time constant
         */
        std::optional<double> t_pdo;
        /**
         * [second] D-axis sub-transient rotor time constant
         */
        std::optional<double> t_ppdo;
        /**
         * [second] Q-axis sub-transient rotor time constant
         */
        std::optional<double> t_ppqo;
        /**
         * [second] Q-axis transient rotor time constant
         */
        std::optional<double> t_pqo;
        BusRefElement uid;
        /**
         * [p.u.] Compounding reactance for voltage control
         */
        std::optional<double> x_comp;
        /**
         * [none] Field voltage value, 1
         *
         * [p.u.] Field voltage value, 1
         */
        std::optional<double> e_1;
        /**
         * [none] Field voltage value, 2
         *
         * [p.u.] Field voltage value, 2
         */
        std::optional<double> e_2;
        /**
         * [none] AVR gain (> 0.)
         *
         * [none] Gain (> 0.)
         *
         * [p.u.] Gain
         *
         * [none] Voltage regulator gain (> 0)
         *
         * [none] PI controller gain
         *
         * [none] Governor gain
         *
         * [none] Acceleration limiter Gain
         *
         * [none] Aerodynamic gain factor
         */
        std::optional<KA> k_a;
        /**
         * [p.u.] Rectifier regulation factor
         *
         * [p.u.] Exciter regulation factor
         *
         * [none] PI controller gain (>0, if Tc >0.)
         *
         * [p.u.] Droop gain
         */
        std::optional<double> k_c;
        /**
         * [p.u.] Exciter internal reactance
         */
        std::optional<double> k_d;
        /**
         * [p.u.] Exciter field resistance constant
         *
         * [p.u.] Exciter field resistance line slope margin
         *
         * [none] Exciter constant
         */
        std::optional<double> k_e;
        /**
         * [p.u.] Rate feedback gain
         *
         * [none] Rate feedback gain
         */
        std::optional<double> k_f;
        /**
         * [none] Saturation factor at E1
         */
        std::optional<double> s_e1;
        /**
         * [none] Saturation factor at E2
         */
        std::optional<double> s_e2;
        /**
         * [none] If = 1, multiply output (Efd) by generator speed.
         *
         * [none] If not 0, multiply output (Efd) by generator speed
         *
         * [none] If=1, multiply output (Efd) by generator speed
         */
        std::optional<double> spdmlt;
        /**
         * [second] AVR time constant (> 0.)
         *
         * [second] Time constant
         *
         * [second] Time constant (> 0.)
         *
         * [second] AVR time constant
         *
         * [second] Voltage regulator time constant (> 0.)
         *
         * [second] Time constant (> 0)
         *
         * [second] Voltage regulator time constant (> 0)
         *
         * [second] Acceleration limiter time constant (> 0.)
         */
        std::optional<double> t_a;
        /**
         * [second] TGR lag time constant
         *
         * [second] Time constant
         *
         * [second] Lag time constant
         *
         * [second] Lead-lag denominator time constant
         *
         * [second] Denominator time constant of lag-lead block
         *
         * [second] Turbine lag time constant (> 0.)
         */
        std::optional<double> t_b;
        /**
         * [second] TGR lead time constant
         *
         * [second] Lead time constant
         *
         * [second] Lead-lag numerator time constant
         *
         * [second] Time constant
         *
         * [second] PI controller phase lead time constant
         *
         * [second] Turbine lead time constant
         */
        std::optional<double> t_c;
        /**
         * [second] Exciter time constant (> 0.)
         *
         * [second] Exciter time constant
         *
         * [second] Exciter field time constant (> 0.)
         *
         * [second] Time constant of gain block (>0)
         */
        std::optional<double> t_e;
        /**
         * [second] Rate feedback time constant (> 0.)
         *
         * [second] Rate feedback time constant
         *
         * [second] Filter time constant
         */
        std::optional<double> t_f;
        /**
         * [second] Filter time constant
         *
         * [second] Voltage transducer time constant
         *
         * [second] Transducer time constant
         *
         * [second] Washout time constant
         */
        std::optional<double> t_r;
        /**
         * [p.u.] Maximum AVR output
         *
         * [p.u.] Maximum controller element output
         *
         * [p.u.] Maximum control element output
         */
        std::optional<double> va_max;
        /**
         * [p.u.] Minimum AVR output
         *
         * [p.u.] Minimum controller element output
         *
         * [p.u.] Minimum control element output
         */
        std::optional<double> va_min;
        /**
         * [p.u.] Maximum exciter control signal
         *
         * [p.u.] Maximum controller output
         *
         * [p.u.] Maximum AVR output
         *
         * [p.u.] Voltage regulator maximum output
         *
         * [p.u.] Voltage regulator maximum limit
         *
         * [p.u.] Maximum control element output
         */
        std::optional<double> vr_max;
        /**
         * [p.u.] Minimum exciter control signal
         *
         * [p.u.] Minimum controller output
         *
         * [p.u.] Minimum AVR output
         *
         * [p.u.] Voltage regulator minimum output
         *
         * [p.u.] Voltage regulator minimum limit
         *
         * [p.u.] Minimum control element output
         */
        std::optional<double> vr_min;
        /**
         * [p.u.] Exciter field current limiter gain
         *
         * [p.u.] Exciter field current feedback gain
         */
        std::optional<double> k_h;
        /**
         * [second] Field current limiter time constant (> 0.)
         */
        std::optional<double> t_h;
        /**
         * [second] Field current limiter time constant
         */
        std::optional<double> t_j;
        /**
         * [second] Lag time constant
         */
        std::optional<double> t_k;
        /**
         * [p.u.] Exciter field current limit reference
         */
        std::optional<double> vfe_lim;
        /**
         * [p.u.] Maximum field current limiter signal
         */
        std::optional<double> vh_max;
        /**
         * [none] If not 0, apply lower limit of 0. to exciter output
         */
        std::optional<double> exclim;
        /**
         * [none] UEL input: if < 2, HV gate; if = 2, add to error signal
         */
        std::optional<double> uelin;
        /**
         * [degree] Phase angle (theta_p) of potential source
         */
        std::optional<double> ang_p;
        /**
         * [p.u.] Inner loop feedback gain
         */
        std::optional<double> k_g;
        /**
         * [p.u.] Current source gain
         *
         * [p.u.] Integral gain
         */
        std::optional<double> k_i;
        /**
         * [p.u.] Integral gain of inner loop regulator
         */
        std::optional<double> k_im;
        /**
         * [p.u.] AVR Integral Gain
         */
        std::optional<double> k_ir;
        /**
         * [p.u.] Potential source gain
         *
         * [p.u.] Potential source gain (> 0.)
         *
         * [p.u.] Proportional gain
         */
        std::optional<double> k_p;
        /**
         * [p.u.] Prop. gain of inner loop regulator
         */
        std::optional<double> k_pm;
        /**
         * [p.u.] AVR proportional Gain
         */
        std::optional<double> k_pr;
        /**
         * [p.u.] Maximum excitation voltage
         */
        std::optional<double> vb_max;
        /**
         * [p.u.] Maximum inner loop feedback gain
         */
        std::optional<double> vg_max;
        /**
         * [p.u.] Maximum inner loop regulator output
         */
        std::optional<double> vm_max;
        /**
         * [p.u.] Minimum inner loop regulator output
         */
        std::optional<double> vm_min;
        /**
         * [p.u.] P-bar leakage reactance
         */
        std::optional<double> x_l;
        /**
         * [none] Exciter field current controller gain
         */
        std::optional<double> k_b;
        /**
         * [p.u.] Exciter field current limiter gain
         */
        std::optional<double> k_l;
        /**
         * [p.u.] Maximum exciter field current
         */
        std::optional<double> v_lr;
        /**
         * [p.u.] Exciter maximum limit
         *
         * [p.u.] Field voltage clipping limit
         */
        std::optional<double> efd_max;
        /**
         * [p.u.] Exciter minimum limit
         *
         * [p.u.] Field voltage clipping limit
         */
        std::optional<double> efd_min;
        /**
         * [second] PI controller time constant
         */
        std::optional<double> t_a1;
        /**
         * [second] Voltage regulator time constant
         */
        std::optional<double> t_a2;
        /**
         * [second] Lead time constant
         */
        std::optional<double> t_a3;
        /**
         * [second] Lag time constant
         */
        std::optional<double> t_a4;
        /**
         * [second] Rate feedback time constant
         */
        std::optional<double> t_f1;
        /**
         * [second] Rate feedback lag time constant
         */
        std::optional<double> t_f2;
        /**
         * [p.u.] PI maximum limit
         */
        std::optional<double> v_r1;
        /**
         * [p.u.] PI minimum limit
         */
        std::optional<double> v_r2;
        /**
         * [none] Required entry of zero
         */
        std::optional<double> s_pare;
        /**
         * [none] Power source switch: 1 = fixed ac voltage; 0 = generator terminal voltage
         */
        std::optional<int64_t> c_switch;
        /**
         * [p.u.] Maximum field voltage output
         *
         * [p.u.] Maximum error limit
         */
        std::optional<double> e_max;
        /**
         * [p.u.] Minimum field voltage output
         *
         * [p.u.] Minimum error limit
         */
        std::optional<double> e_min;
        /**
         * [none] Gain (>0)
         *
         * [p.u.] Governor gain (reciprocal of droop)
         */
        std::optional<double> k;
        /**
         * [none] Rc/Rfd - ratio of field discharge resistance to field winding resistance
         */
        std::optional<double> r_crfd;
        /**
         * [none] Ta/Tb - gain reduction ratio of lag-lead element
         */
        std::optional<double> t_atb;
        /**
         * [none] Turbine power time constant numerator scale factor
         */
        std::optional<double> a;
        /**
         * [none] Turbine power time constant denominator scale factor
         */
        std::optional<double> b;
        /**
         * [p.u.] Turbine damping coefficient
         *
         * [p.u.] Turbine damping factor
         */
        std::optional<double> d_turb;
        /**
         * [Hz] Intentional deadband width
         *
         * [p.u.] Deadband in voltage error
         */
        std::optional<double> db1;
        /**
         * [MW] Unintentional deadband
         *
         * [p.u.] Deadband in voltage error
         */
        std::optional<double> db2;
        /**
         * [Hz] Intentional db hysteresis
         */
        std::optional<double> eps;
        /**
         * [p.u.] Fuel flow at zero power output
         */
        std::optional<double> f_idle;
        /**
         * [p.u. gv] Nonlinear gain point 1
         */
        std::optional<double> g_v1;
        /**
         * [p.u. gv] Nonlinear gain point 2
         */
        std::optional<double> g_v2;
        /**
         * [p.u. gv] Nonlinear gain point 3
         */
        std::optional<double> g_v3;
        /**
         * [p.u. gv] Nonlinear gain point 4
         */
        std::optional<double> g_v4;
        /**
         * [p.u. gv] Nonlinear gain point 5
         */
        std::optional<double> g_v5;
        /**
         * [p.u. gv] Nonlinear gain point 6
         */
        std::optional<double> g_v6;
        /**
         * [none] Temperature limiter gain
         */
        std::optional<double> k_t;
        /**
         * [none] Ambient temperature load limit
         */
        std::optional<double> l_max;
        /**
         * [p.u.] Valve position change allowed at fast rate
         */
        std::optional<double> l_oadinc;
        /**
         * [p.u./second] Maximum long term fuel valve opening rate
         */
        std::optional<double> lt_rate;
        /**
         * [MW] Turbine Rating
         */
        std::optional<double> mw_cap;
        /**
         * [p.u. power] Nonlinear gain point 1
         */
        std::optional<double> p_gv1;
        /**
         * [p.u. power] Nonlinear gain point 2
         */
        std::optional<double> p_gv2;
        /**
         * [p.u. power] Nonlinear gain point 3
         */
        std::optional<double> p_gv3;
        /**
         * [p.u. power] Nonlinear gain point 4
         */
        std::optional<double> p_gv4;
        /**
         * [p.u. power] Nonlinear gain point 5
         */
        std::optional<double> p_gv5;
        /**
         * [p.u. power] Nonlinear gain point 6
         */
        std::optional<double> p_gv6;
        /**
         * [p.u.] Permanent droop
         */
        std::optional<double> r;
        /**
         * [p.u./second] Maximum fuel valve opening rate
         */
        std::optional<double> r_max;
        /**
         * [second] Governor mechanism time constant
         *
         * [second] Governor lag time constant
         *
         * [second] Steam bowl time constant
         *
         * [second] Lead/lag time constant
         */
        std::optional<double> t_1;
        /**
         * [second] Turbine power time constant
         *
         * [second] Governor lead time constant
         *
         * [second] Numerator time constant of T2, T3 block
         *
         * [second] Lead/lag time constant
         */
        std::optional<double> t_2;
        /**
         * [second] Turbine exhaust temperature time constant
         *
         * [second] Valve positioner time constant
         *
         * [second] Reheater time constant
         *
         * [second] Lead/lag time constant
         */
        std::optional<double> t_3;
        /**
         * [second] Governor lead time constant
         *
         * [second] Inlet piping/steam bowl time constant
         *
         * [second] Lead/lag time constant
         */
        std::optional<double> t_4;
        /**
         * [second] Governor lag time constant
         *
         * [second] Time constant of second boiler pass
         *
         * [second] Washout numerator time constant
         */
        std::optional<double> t_5;
        /**
         * [second] Valve position averaging time constant
         */
        std::optional<double> t_ltr;
        /**
         * [p.u. of mw_cap] Maximum turbine power
         *
         * [none] Maximum valve position limit
         *
         * [p.u. of mwcap] Maximum valve position
         *
         * [p.u.] Voltage control maximum limit
         */
        std::optional<double> v_max;
        /**
         * [p.u. of mw_cap] Minimum turbine power
         *
         * [none] Minimum valve position limit
         *
         * [p.u. of mwcap] Minimum valve position
         *
         * [p.u.] Voltage control minimum limit
         */
        std::optional<double> v_min;
        /**
         * [p.u./second] Acceleration limiter setpoint
         */
        std::optional<double> aset;
        /**
         * [p.u.] Speed sensitivity coefficient
         */
        std::optional<double> d_m;
        /**
         * [none] Speed governor dead band
         */
        std::optional<double> db;
        /**
         * [none] Switch for fuel source characteristic. Values: =0 for fuel flow independent of
         * speed; =1 fuel flow proportional to speed.
         */
        std::optional<int64_t> f_lag;
        /**
         * [none] Governor derivative gain
         */
        std::optional<double> k_dgov;
        /**
         * [none] Governor integral gain
         */
        std::optional<double> k_igov;
        /**
         * [none] Load limiter integral gain for PI controller
         */
        std::optional<double> k_iload;
        /**
         * [none] Power controller (reset) gain
         */
        std::optional<double> k_imw;
        /**
         * [none] Governor proportional gain
         */
        std::optional<double> k_pgov;
        /**
         * [none] Load limiter proportional gain for PI controller
         */
        std::optional<double> k_pload;
        /**
         * [none] Turbine gain (> 0.)
         */
        std::optional<double> k_trub;
        /**
         * [p.u.] Load limiter reference value
         */
        std::optional<double> l_dref;
        /**
         * [none] Maximum value for speed error signal
         */
        std::optional<double> max_err;
        /**
         * [none] Minimum value for speed error signal
         */
        std::optional<double> min_err;
        /**
         * [MW] Power controller setpoint
         */
        std::optional<double> p_mwset;
        /**
         * [p.u.] Permanent droop
         */
        std::optional<double> dynamic_and_control_r;
        /**
         * [p.u./second] Minimum valve opening rate
         */
        std::optional<double> r_close;
        /**
         * [none] Maximum rate of load limit decrease
         */
        std::optional<double> r_down;
        /**
         * [p.u./second] Maximum valve opening rate
         */
        std::optional<double> r_open;
        /**
         * [none] Feedback signal for droop. Values: =1 selected electrical power;  =0 none
         * (isochronous governor); =-1 fuel valve stroke (true stroke); =-2 governor output (
         * requested stroke)
         */
        std::optional<int64_t> r_select;
        /**
         * [none] Maximum rate of load limit increase
         */
        std::optional<double> r_up;
        /**
         * [second] Actuator time constant
         */
        std::optional<double> t_act;
        /**
         * [second] Governor derivative controller time constant
         */
        std::optional<double> t_dgov;
        /**
         * [second] Transport lag time constant for diesel engine
         */
        std::optional<double> t_eng;
        /**
         * [second] Load Limiter time constant (> 0.)
         */
        std::optional<double> t_fload;
        /**
         * [second] Electrical power transducer time constant (> 0.)
         */
        std::optional<double> t_pelec;
        /**
         * [second] Temperature detection lead time constant
         */
        std::optional<double> t_sa;
        /**
         * [second] Temperature detection lag time constant
         */
        std::optional<double> t_sb;
        /**
         * [p.u.] No load fuel flow
         */
        std::optional<double> wfnl;
        /**
         * [p.u.] Turbine gain
         */
        std::optional<double> a_t;
        /**
         * [p.u.] Kaplan blade servo point 0
         */
        std::optional<double> b_gv0;
        /**
         * [p.u.] Kaplan blade servo point 1
         */
        std::optional<double> b_gv1;
        /**
         * [p.u.] Kaplan blade servo point 2
         */
        std::optional<double> b_gv2;
        /**
         * [p.u.] Kaplan blade servo point 3
         */
        std::optional<double> b_gv3;
        /**
         * [p.u.] Kaplan blade servo point 4
         */
        std::optional<double> b_gv4;
        /**
         * [p.u.] Kaplan blade servo point 5
         */
        std::optional<double> b_gv5;
        /**
         * [none] Maximum blade adjustment factor
         */
        std::optional<double> b_max;
        /**
         * [p.u. of mwcap] Maximum gate opening
         */
        std::optional<double> g_max;
        /**
         * [p.u. of mwcap] Minimum gate opening
         */
        std::optional<double> g_min;
        /**
         * [p.u.] Head available at dam
         */
        std::optional<double> hdam;
        /**
         * [p.u.] No-load flow at nominal head
         */
        std::optional<double> qnl;
        /**
         * [p.u.] Permanent droop (R)
         */
        std::optional<double> r_perm;
        /**
         * [p.u.] Temporary droop (r)
         */
        std::optional<double> r_temp;
        /**
         * [second] Blade servo time constant
         */
        std::optional<double> t_blade;
        /**
         * [second] Gate servo time constant
         */
        std::optional<double> t_g;
        /**
         * [second] Lead time constant
         */
        std::optional<double> t_n;
        /**
         * [second] Lag time constant
         */
        std::optional<double> t_np;
        /**
         * [second] Water inertia time constant
         *
         * [second] Time delay for each controlled device in gen
         */
        std::optional<Bs> t_w;
        /**
         * [N/A] NOT USED
         */
        std::optional<int64_t> ttrip;
        /**
         * [p.u./second] Maximum gate velocity
         */
        std::optional<double> v_elm;
        /**
         * [none] Fraction of hp shaft power after first boiler pass
         */
        std::optional<double> k_1;
        /**
         * [none] Fraction of lp shaft power after first boiler pass
         */
        std::optional<double> k_2;
        /**
         * [none] Fraction of hp shaft power after second boiler pass
         */
        std::optional<double> k_3;
        /**
         * [none] Fraction of lp shaft power after second boiler pass
         */
        std::optional<double> k_4;
        /**
         * [none] Fraction of hp shaft power after third boiler pass
         */
        std::optional<double> k_5;
        /**
         * [none] Fraction of lp shaft power after third boiler pass
         */
        std::optional<double> k_6;
        /**
         * [none] Fraction of hp shaft power after fourth boiler pass
         */
        std::optional<double> k_7;
        /**
         * [none] Fraction of lp shaft power after fourth boiler pass
         */
        std::optional<double> k_8;
        /**
         * [p.u. of mwcap] Maximum valve opening
         *
         * [p.u.] Maximum power reference
         *
         * [p.u.] Maximum power
         */
        std::optional<double> p_max;
        /**
         * [p.u. of mwcap] Minimum valve opening
         *
         * [p.u.] Minimum power reference
         *
         * [p.u.] Minimum power
         */
        std::optional<double> p_min;
        /**
         * [second] Time constant of third boiler pass
         *
         * [second] Washout denominator time constant
         */
        std::optional<double> t_6;
        /**
         * [second] Time constant of fourth boiler pass
         */
        std::optional<double> t_7;
        /**
         * [p.u./second] Maximum valve closing velocity (< 0.)
         */
        std::optional<double> u_c;
        /**
         * [p.u./second] Maximum valve opening velocity
         */
        std::optional<double> u_o;
        /**
         * [p.u.] Turbine damping coefficient
         */
        std::optional<double> d_t;
        /**
         * [none] Notch filter parameters
         */
        std::optional<double> a_1;
        /**
         * [none] Notch filter parameters
         */
        std::optional<double> a_2;
        /**
         * [none] Notch filter parameters
         */
        std::optional<double> a_3;
        /**
         * [none] Notch filter parameters
         */
        std::optional<double> a_4;
        /**
         * [none] Notch filter parameters
         */
        std::optional<double> a_5;
        /**
         * [none] Notch filter parameters
         */
        std::optional<double> a_6;
        /**
         * [none] Input signal code
         */
        std::optional<double> j;
        /**
         * [none] Remote signal bus number
         */
        std::optional<int64_t> dynamic_and_control_k;
        /**
         * [none] Stabilizer gain
         */
        std::optional<double> k_s;
        /**
         * [p.u.] Maximum stabilizer output
         */
        std::optional<double> ls_max;
        /**
         * [p.u.] Minimum stabilizer output
         */
        std::optional<double> ls_min;
        /**
         * [second] Time delay
         */
        std::optional<double> t_delay;
        /**
         * [p.u.] Stabilizer input cutoff threshold
         */
        std::optional<double> v_cl;
        /**
         * [p.u.] Stabilizer input cutoff threshold
         */
        std::optional<double> v_cu;
        /**
         * [MVA] MVA base
         */
        std::optional<double> mvab;
        /**
         * [degree] Initial blade pitch angle
         */
        std::optional<double> theta_0;
        /**
         * [p.u./p.u.] Proportional gain
         */
        std::optional<double> k_cc;
        /**
         * [p.u./p.u./second] Pitch compensation integral gain
         */
        std::optional<double> k_ic;
        /**
         * [p.u./p.u./second] Pitch controller integral gain
         */
        std::optional<double> k_iw;
        /**
         * [p.u./p.u.] Pitch compensation proportional gain
         */
        std::optional<double> k_pc;
        /**
         * [p.u./p.u.] Pitch controller proportional gain
         */
        std::optional<double> k_pw;
        /**
         * [degree] Maximum pitch angle limit
         */
        std::optional<double> pi_max;
        /**
         * [degree] Minimum pitch angle limit
         */
        std::optional<double> pi_min;
        /**
         * [degree/second] Minimum pitch angle rate
         */
        std::optional<double> pi_ratmn;
        /**
         * [degree/second] Maximum pitch angle rate
         */
        std::optional<double> pi_ratmx;
        /**
         * [second] Pitch time
         */
        std::optional<double> t_pi;
        /**
         * [p.u./p.u./second] Integral gain
         */
        std::optional<double> k_ip;
        /**
         * [p.u./p.u.] Proportional gain
         */
        std::optional<double> k_pp;
        /**
         * [p.u.] User defined point
         */
        std::optional<double> p1;
        /**
         * [p.u.] User defined point
         */
        std::optional<double> p2;
        /**
         * [p.u.] User defined point
         */
        std::optional<double> p3;
        /**
         * [p.u.] User defined point
         */
        std::optional<double> p4;
        /**
         * [p.u.] User defined point
         */
        std::optional<double> spd1;
        /**
         * [p.u.] User defined point
         */
        std::optional<double> spd2;
        /**
         * [p.u.] User defined point
         */
        std::optional<double> spd3;
        /**
         * [p.u.] User defined point
         */
        std::optional<double> spd4;
        /**
         * [none] Flag to specify PI controller input
         */
        std::optional<int64_t> t_flag;
        /**
         * [second] Power measurement lag time constant
         */
        std::optional<double> t_p;
        /**
         * [p.u.] Maximum torque
         */
        std::optional<double> te_max;
        /**
         * [p.u.] Minimum torque
         */
        std::optional<double> te_min;
        /**
         * [p.u.] Maximum torque
         */
        std::optional<double> tw_ref;
        /**
         * [p.u.] Damping coefficient
         */
        std::optional<double> d_shaft;
        /**
         * [MW-sec/MVA] Generator inertia
         */
        std::optional<double> h_g;
        /**
         * [MW-sec/MVA] Turbine inertia
         */
        std::optional<double> h_t;
        /**
         * [p.u.] Stiffness constant
         */
        std::optional<double> k_shaft;
        /**
         * [p.u.] Initial speed
         */
        std::optional<double> w_o;
        /**
         * [p.u.] Acceleration factor used in the high voltage reactive power logic
         */
        std::optional<double> accel;
        /**
         * [p.u.] LVPL characteristic breakpoint
         */
        std::optional<double> brkpt;
        /**
         * [p.u./second] Upward rate limit on reactive current command
         */
        std::optional<double> iqr_max;
        /**
         * [p.u./second] Downward rate limit on reactive current command
         */
        std::optional<double> iqr_min;
        /**
         * [p.u.] LVPL breakpoint
         */
        std::optional<double> lvpl1;
        /**
         * [none] Connect (1) / disconnect (0) Low Volt. Power Logic switch
         */
        std::optional<int64_t> lvpl_sw;
        /**
         * [p.u.] Low voltage point for low voltage active power logic
         */
        std::optional<double> lvpnt0;
        /**
         * [p.u.] High voltage point for low voltage active power logic
         */
        std::optional<double> lvpnt1;
        /**
         * [p.u.] Limit in the high voltage reactive power logic
         *
         * [p.u.] Reactive power minimum limit
         *
         * [p.u.] Minimum Q control output
         *
         * [p.u.] Minimum Q PI control (<0) output
         */
        std::optional<double> q_min;
        /**
         * [p.u.] LVPL ramp rate of recovery limit
         */
        std::optional<double> rrpwr;
        /**
         * [second] Voltage measurement time constant
         *
         * [second] Voltage or reactive power transducer time constant
         */
        std::optional<double> tfltr;
        /**
         * [second] Time constant
         */
        std::optional<double> tg;
        /**
         * [p.u.] Generator effective reactance
         */
        std::optional<double> xe;
        /**
         * [p.u.] LVPL characteristic zero crossing
         */
        std::optional<double> zerox;
        /**
         * [p.u./second] Up ramp rate on power reference
         */
        std::optional<double> dp_max;
        /**
         * [p.u./second] Down ramp rate on power reference
         */
        std::optional<double> dp_min;
        /**
         * [p.u.] Maximum allowable total current limit
         */
        std::optional<double> i_max;
        /**
         * [p.u.] User defined current used to define VDL2 function
         */
        std::optional<double> ip1;
        /**
         * [p.u.] User defined current used to define VDL2 function
         */
        std::optional<double> ip2;
        /**
         * [p.u.] User defined current used to define VDL2 function
         */
        std::optional<double> ip3;
        /**
         * [p.u.] User defined current used to define VDL2 function
         */
        std::optional<double> ip4;
        /**
         * [p.u.] User defined current used to define VDL1 function
         */
        std::optional<double> iq1;
        /**
         * [p.u.] User defined current used to define VDL1 function
         */
        std::optional<double> iq2;
        /**
         * [p.u.] User defined current used to define VDL1 function
         */
        std::optional<double> iq3;
        /**
         * [p.u.] User defined current used to define VDL1 function
         */
        std::optional<double> iq4;
        /**
         * [p.u.] Value at which Iqinj is held for textbf{thld} seconds following a voltage dip if
         * thld > 0
         */
        std::optional<double> iqfrz;
        /**
         * [p.u.] Maximum limit of reactive current injection (textbf{iqinj})
         */
        std::optional<double> iqh1;
        /**
         * [p.u.] Maximum limit of reactive current injection (textbf{iqinj})
         */
        std::optional<double> iql1;
        /**
         * [p.u.] Integral gain
         */
        std::optional<double> kqi;
        /**
         * [p.u.] Proportional gain
         */
        std::optional<double> kqp;
        /**
         * [p.u./p.u.] Reactive current injection gain during voltage dip (and overvoltage)
         * conditions
         */
        std::optional<double> kqv;
        /**
         * [p.u.] Integral gain
         */
        std::optional<double> kvi;
        /**
         * [p.u.] Proportional gain
         */
        std::optional<double> kvp;
        /**
         * [none] Power reference flag: = 1 : reference is Pref*speed (Do not use with Type 3 WTG);
         * = 0 : reference is Pref
         */
        std::optional<int64_t> p_flag;
        /**
         * [none] Power factor flag: = 1 : Power factor control; = 0 : Q control
         */
        std::optional<int64_t> pf_flag;
        /**
         * [none] Flag for P or Q priority selection on current limit: = 1 : P priority ; = 0 : Q
         * priority
         */
        std::optional<int64_t> pq_flag;
        /**
         * [none] Reactive power control flag: = 1 : Voltage/Q control; = 0 : Constant power factor
         * or Q Control
         */
        std::optional<int64_t> q_flag;
        /**
         * [p.u.] Reactive power maximum limit
         *
         * [p.u.] Maximum Q control output
         *
         * [p.u.] Maximum Q PI control (>0) output
         */
        std::optional<double> q_max;
        /**
         * [second] Time constant
         */
        std::optional<double> t_pord;
        /**
         * [second] Time delay associated with the computation of iqinj and with the operation of
         * switch SW
         */
        std::optional<double> thld;
        /**
         * [none] The active current command (Ipcmd) is held for thld2 seconds after voltage_dip
         * returns to zero
         */
        std::optional<double> thld2;
        /**
         * [second] Time constant
         */
        std::optional<double> tiq;
        /**
         * [second] Electrical power transducer time constant
         *
         * [second] Lag time constant on Pgen measurement
         */
        std::optional<double> tp;
        /**
         * [second] Transducer time constant
         */
        std::optional<double> trv;
        /**
         * [p.u.] V_term < v_dip activates the current injection logic
         */
        std::optional<double> v_dip;
        /**
         * [none] Voltage control flag: = 1 : Q Control; = 0 : Voltage control
         */
        std::optional<int64_t> v_flag;
        /**
         * [p.u.] Reference voltage
         */
        std::optional<double> v_ref0;
        /**
         * [p.u.] User-defined reference on the inner-loop voltage control (default value is zero)
         */
        std::optional<double> v_ref1;
        /**
         * [p.u.] V_term < v_up activates the current injection logic
         */
        std::optional<double> v_up;
        /**
         * [p.u.] User defined voltage used to define VDL2 function
         */
        std::optional<double> vp1;
        /**
         * [p.u.] User defined voltage used to define VDL2 function
         */
        std::optional<double> vp2;
        /**
         * [p.u.] User defined voltage used to define VDL2 function
         */
        std::optional<double> vp3;
        /**
         * [p.u.] User defined voltage used to define VDL2 function
         */
        std::optional<double> vp4;
        /**
         * [p.u.] User defined voltage used to define VDL1 function
         */
        std::optional<double> vq1;
        /**
         * [p.u.] User defined voltage used to define VDL1 function
         */
        std::optional<double> vq2;
        /**
         * [p.u.] User defined voltage used to define VDL1 function
         */
        std::optional<double> vq3;
        /**
         * [p.u.] User defined voltage used to define VDL1 function
         */
        std::optional<double> vq4;
        /**
         * [p.u.] Deadband
         */
        std::optional<double> dbd;
        /**
         * [p.u.] Downside droop
         */
        std::optional<double> ddn;
        /**
         * [p.u.] Upside droop
         */
        std::optional<double> dup;
        /**
         * [p.u.] Deadband downside
         */
        std::optional<double> fdbd1;
        /**
         * [p.u.] Deadband upside
         */
        std::optional<double> fdbd2;
        /**
         * [p.u.] Maximum error limit
         */
        std::optional<double> fe_max;
        /**
         * [p.u.] Minimum error limit
         */
        std::optional<double> fe_min;
        /**
         * [none] Pref output flag
         */
        std::optional<int64_t> frqflg;
        /**
         * [p.u.] Integral gain for power control
         */
        std::optional<double> kig;
        /**
         * [p.u.] Proportional gain for power control
         */
        std::optional<double> kpg;
        std::optional<BusRefElement> monitored_branch;
        /**
         * type of monitored branch
         */
        std::optional<MonitoredBranchType> monitored_branch_type;
        std::optional<BusRefElement> monitored_bus;
        /**
         * [none] TS
         */
        std::optional<int64_t> outflag;
        /**
         * [none] Per unit flag. If puflag = 0 then Pbranch and Qbranch are in p.u. system base;
         * else Pbranch and Qbranch are in p.u. on model base (mvab)
         */
        std::optional<int64_t> puflag;
        /**
         * [p.u.] Line drop compensation resistance
         */
        std::optional<double> r_c;
        /**
         * [none] = 1 : Voltage control; = 0: Reactive control
         */
        std::optional<int64_t> refflg;
        /**
         * [second] Lag time constant on Pref feedback
         */
        std::optional<double> t_lag;
        /**
         * [second] Lead time constant
         */
        std::optional<double> tft;
        /**
         * [second] Lag time constant
         */
        std::optional<double> tfv;
        /**
         * [none] Flag for selection of droop (=0), or line drop compensation (=1)
         */
        std::optional<int64_t> vcmpflg;
        /**
         * [p.u.] If Vreg < vfrz, then state s2 is frozen
         */
        std::optional<double> vfrz;
        /**
         * [p.u.] Line drop compensation reactance
         */
        std::optional<double> x_c;
        /**
         * [none] Reactive path gain for each controlled device in gen
         */
        std::optional<std::vector<double>> k_w;
        /**
         * [none] Real path gain for each controlled device in gen
         */
        std::optional<std::vector<double>> k_z;
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
        std::optional<BusRefElement> uid;
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
        BusRefElement bus;
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
        BusRefElement uid;
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
        std::optional<BusRefElement> bus_ref;
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
        BusRefElement bus_fr;
        /**
         * uid of bus at the to terminal of hvdc line
         */
        BusRefElement bus_to;
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
        BusRefElement uid;
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
        BusRefElement bus;
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
        BusRefElement uid;
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
        BusRefElement bus_fr;
        /**
         * uid of bus at the to terminal of switch
         */
        BusRefElement bus_to;
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
        BusRefElement uid;
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
        std::optional<std::vector<BusRefElement>> participants;
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
        BusRefElement uid;
    };

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
        BusRefElement bus;
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
        BusRefElement uid;
    };

    using ChargeEfficiency = std::variant<CmUbAClass, double>;

    /**
     * structure to hold storage (battery) data
     */
    struct NetworkStorage {
        /**
         * uid of bus to which generator is connected to
         */
        BusRefElement bus;
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
        BusRefElement uid;
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
        BusRefElement bus_fr;
        /**
         * uid of bus at the to terminal of transformer
         */
        BusRefElement bus_to;
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
        BusRefElement uid;
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
        BusRefElement uid;
    };

    /**
     * structure to hold persistent network data
     */
    struct Network {
        std::optional<std::vector<NetworkAcLine>> ac_line;
        std::vector<Area> area;
        std::vector<NetworkBus> bus;
        std::optional<std::vector<DynamicAndControl>> dynamic_and_control;
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
        BusRefElement uid;
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
        BusRefElement uid;
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
        BusRefElement uid;
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
        BusRefElement uid;
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
        BusRefElement uid;
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
        BusRefElement uid;
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
        BusRefElement uid;
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
        std::vector<BusRefElement> uid;
        /**
         * array of time series values
         */
        std::optional<std::vector<std::vector<nlohmann::json>>> values;
    };

    /**
     * Common Transmission Model (CTM) Data Schema v0.2
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
        BusRefElement uid;
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
        BusRefElement uid;
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
        BusRefElement uid;
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
        BusRefElement uid;
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
        BusRefElement uid;
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
        BusRefElement uid;
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
        BusRefElement uid;
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
        BusRefElement uid;
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
        BusRefElement uid;
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
        BusRefElement uid;
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
        BusRefElement uid;
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
        std::vector<BusRefElement> uid;
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
        std::vector<BusRefElement> uid;
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

void from_json(const json & j, DynamicAndControl & x);
void to_json(json & j, const DynamicAndControl & x);

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

void from_json(const json & j, Category & x);
void to_json(json & j, const Category & x);

void from_json(const json & j, Model & x);
void to_json(json & j, const Model & x);

void from_json(const json & j, MonitoredBranchType & x);
void to_json(json & j, const MonitoredBranchType & x);

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
struct adl_serializer<std::variant<std::vector<ctm_schemas::BusRefElement>, int64_t, std::string>> {
    static void from_json(const json & j, std::variant<std::vector<ctm_schemas::BusRefElement>, int64_t, std::string> & x);
    static void to_json(json & j, const std::variant<std::vector<ctm_schemas::BusRefElement>, int64_t, std::string> & x);
};

template <>
struct adl_serializer<std::variant<double, int64_t>> {
    static void from_json(const json & j, std::variant<double, int64_t> & x);
    static void to_json(json & j, const std::variant<double, int64_t> & x);
};

template <>
struct adl_serializer<std::variant<std::vector<double>, double>> {
    static void from_json(const json & j, std::variant<std::vector<double>, double> & x);
    static void to_json(json & j, const std::variant<std::vector<double>, double> & x);
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
        x.uid = j.at("uid").get<BusRefElement>();
    }

    inline void to_json(json & j, const CmUbAClass & x) {
        j = json::object();
        j["scale_factor"] = x.scale_factor;
        j["uid"] = x.uid;
    }

    inline void from_json(const json & j, NetworkAcLine& x) {
        x.b_fr = get_stack_optional<double>(j, "b_fr");
        x.b_to = get_stack_optional<double>(j, "b_to");
        x.bus_fr = j.at("bus_fr").get<BusRefElement>();
        x.bus_to = j.at("bus_to").get<BusRefElement>();
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
        x.uid = j.at("uid").get<BusRefElement>();
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
        x.uid = j.at("uid").get<BusRefElement>();
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
        x.uid = j.at("uid").get<BusRefElement>();
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

    inline void from_json(const json & j, DynamicAndControl& x) {
        x.category = j.at("category").get<Category>();
        x.d = get_stack_optional<double>(j, "D");
        x.gen = get_stack_optional<std::variant<std::vector<BusRefElement>, int64_t, std::string>>(j, "gen");
        x.h = get_stack_optional<double>(j, "H");
        x.l_1 = get_stack_optional<double>(j, "l_1");
        x.l_d = get_stack_optional<double>(j, "l_d");
        x.l_pd = get_stack_optional<double>(j, "l_pd");
        x.l_ppd = get_stack_optional<double>(j, "l_ppd");
        x.l_pq = get_stack_optional<double>(j, "l_pq");
        x.l_q = get_stack_optional<double>(j, "l_q");
        x.model = j.at("model").get<Model>();
        x.r_a = get_stack_optional<double>(j, "r_a");
        x.r_comp = get_stack_optional<double>(j, "r_comp");
        x.s_1 = get_stack_optional<double>(j, "s_1");
        x.s_12 = get_stack_optional<double>(j, "s_12");
        x.t_pdo = get_stack_optional<double>(j, "t_pdo");
        x.t_ppdo = get_stack_optional<double>(j, "t_ppdo");
        x.t_ppqo = get_stack_optional<double>(j, "t_ppqo");
        x.t_pqo = get_stack_optional<double>(j, "t_pqo");
        x.uid = j.at("uid").get<BusRefElement>();
        x.x_comp = get_stack_optional<double>(j, "x_comp");
        x.e_1 = get_stack_optional<double>(j, "e_1");
        x.e_2 = get_stack_optional<double>(j, "e_2");
        x.k_a = get_stack_optional<std::variant<double, int64_t>>(j, "k_a");
        x.k_c = get_stack_optional<double>(j, "k_c");
        x.k_d = get_stack_optional<double>(j, "k_d");
        x.k_e = get_stack_optional<double>(j, "k_e");
        x.k_f = get_stack_optional<double>(j, "k_f");
        x.s_e1 = get_stack_optional<double>(j, "s_e1");
        x.s_e2 = get_stack_optional<double>(j, "s_e2");
        x.spdmlt = get_stack_optional<double>(j, "spdmlt");
        x.t_a = get_stack_optional<double>(j, "t_a");
        x.t_b = get_stack_optional<double>(j, "t_b");
        x.t_c = get_stack_optional<double>(j, "t_c");
        x.t_e = get_stack_optional<double>(j, "t_e");
        x.t_f = get_stack_optional<double>(j, "t_f");
        x.t_r = get_stack_optional<double>(j, "t_r");
        x.va_max = get_stack_optional<double>(j, "va_max");
        x.va_min = get_stack_optional<double>(j, "va_min");
        x.vr_max = get_stack_optional<double>(j, "vr_max");
        x.vr_min = get_stack_optional<double>(j, "vr_min");
        x.k_h = get_stack_optional<double>(j, "k_h");
        x.t_h = get_stack_optional<double>(j, "t_h");
        x.t_j = get_stack_optional<double>(j, "t_j");
        x.t_k = get_stack_optional<double>(j, "t_k");
        x.vfe_lim = get_stack_optional<double>(j, "vfe_lim");
        x.vh_max = get_stack_optional<double>(j, "vh_max");
        x.exclim = get_stack_optional<double>(j, "exclim");
        x.uelin = get_stack_optional<double>(j, "uelin");
        x.ang_p = get_stack_optional<double>(j, "ang_p");
        x.k_g = get_stack_optional<double>(j, "k_g");
        x.k_i = get_stack_optional<double>(j, "k_i");
        x.k_im = get_stack_optional<double>(j, "k_im");
        x.k_ir = get_stack_optional<double>(j, "k_ir");
        x.k_p = get_stack_optional<double>(j, "k_p");
        x.k_pm = get_stack_optional<double>(j, "k_pm");
        x.k_pr = get_stack_optional<double>(j, "k_pr");
        x.vb_max = get_stack_optional<double>(j, "vb_max");
        x.vg_max = get_stack_optional<double>(j, "vg_max");
        x.vm_max = get_stack_optional<double>(j, "vm_max");
        x.vm_min = get_stack_optional<double>(j, "vm_min");
        x.x_l = get_stack_optional<double>(j, "x_l");
        x.k_b = get_stack_optional<double>(j, "k_b");
        x.k_l = get_stack_optional<double>(j, "k_l");
        x.v_lr = get_stack_optional<double>(j, "v_lr");
        x.efd_max = get_stack_optional<double>(j, "efd_max");
        x.efd_min = get_stack_optional<double>(j, "efd_min");
        x.t_a1 = get_stack_optional<double>(j, "t_a1");
        x.t_a2 = get_stack_optional<double>(j, "t_a2");
        x.t_a3 = get_stack_optional<double>(j, "t_a3");
        x.t_a4 = get_stack_optional<double>(j, "t_a4");
        x.t_f1 = get_stack_optional<double>(j, "t_f1");
        x.t_f2 = get_stack_optional<double>(j, "t_f2");
        x.v_r1 = get_stack_optional<double>(j, "v_r1");
        x.v_r2 = get_stack_optional<double>(j, "v_r2");
        x.s_pare = get_stack_optional<double>(j, "s_pare");
        x.c_switch = get_stack_optional<int64_t>(j, "c_switch");
        x.e_max = get_stack_optional<double>(j, "e_max");
        x.e_min = get_stack_optional<double>(j, "e_min");
        x.k = get_stack_optional<double>(j, "K");
        x.r_crfd = get_stack_optional<double>(j, "r_crfd");
        x.t_atb = get_stack_optional<double>(j, "t_atb");
        x.a = get_stack_optional<double>(j, "a");
        x.b = get_stack_optional<double>(j, "b");
        x.d_turb = get_stack_optional<double>(j, "d_turb");
        x.db1 = get_stack_optional<double>(j, "db1");
        x.db2 = get_stack_optional<double>(j, "db2");
        x.eps = get_stack_optional<double>(j, "eps");
        x.f_idle = get_stack_optional<double>(j, "f_idle");
        x.g_v1 = get_stack_optional<double>(j, "g_v1");
        x.g_v2 = get_stack_optional<double>(j, "g_v2");
        x.g_v3 = get_stack_optional<double>(j, "g_v3");
        x.g_v4 = get_stack_optional<double>(j, "g_v4");
        x.g_v5 = get_stack_optional<double>(j, "g_v5");
        x.g_v6 = get_stack_optional<double>(j, "g_v6");
        x.k_t = get_stack_optional<double>(j, "k_t");
        x.l_max = get_stack_optional<double>(j, "l_max");
        x.l_oadinc = get_stack_optional<double>(j, "l_oadinc");
        x.lt_rate = get_stack_optional<double>(j, "lt_rate");
        x.mw_cap = get_stack_optional<double>(j, "mw_cap");
        x.p_gv1 = get_stack_optional<double>(j, "p_gv1");
        x.p_gv2 = get_stack_optional<double>(j, "p_gv2");
        x.p_gv3 = get_stack_optional<double>(j, "p_gv3");
        x.p_gv4 = get_stack_optional<double>(j, "p_gv4");
        x.p_gv5 = get_stack_optional<double>(j, "p_gv5");
        x.p_gv6 = get_stack_optional<double>(j, "p_gv6");
        x.r = get_stack_optional<double>(j, "R");
        x.r_max = get_stack_optional<double>(j, "r_max");
        x.t_1 = get_stack_optional<double>(j, "t_1");
        x.t_2 = get_stack_optional<double>(j, "t_2");
        x.t_3 = get_stack_optional<double>(j, "t_3");
        x.t_4 = get_stack_optional<double>(j, "t_4");
        x.t_5 = get_stack_optional<double>(j, "t_5");
        x.t_ltr = get_stack_optional<double>(j, "t_ltr");
        x.v_max = get_stack_optional<double>(j, "v_max");
        x.v_min = get_stack_optional<double>(j, "v_min");
        x.aset = get_stack_optional<double>(j, "aset");
        x.d_m = get_stack_optional<double>(j, "d_m");
        x.db = get_stack_optional<double>(j, "db");
        x.f_lag = get_stack_optional<int64_t>(j, "f_lag");
        x.k_dgov = get_stack_optional<double>(j, "k_dgov");
        x.k_igov = get_stack_optional<double>(j, "k_igov");
        x.k_iload = get_stack_optional<double>(j, "k_iload");
        x.k_imw = get_stack_optional<double>(j, "k_imw");
        x.k_pgov = get_stack_optional<double>(j, "k_pgov");
        x.k_pload = get_stack_optional<double>(j, "k_pload");
        x.k_trub = get_stack_optional<double>(j, "k_trub");
        x.l_dref = get_stack_optional<double>(j, "l_dref");
        x.max_err = get_stack_optional<double>(j, "max_err");
        x.min_err = get_stack_optional<double>(j, "min_err");
        x.p_mwset = get_stack_optional<double>(j, "p_mwset");
        x.dynamic_and_control_r = get_stack_optional<double>(j, "r");
        x.r_close = get_stack_optional<double>(j, "r_close");
        x.r_down = get_stack_optional<double>(j, "r_down");
        x.r_open = get_stack_optional<double>(j, "r_open");
        x.r_select = get_stack_optional<int64_t>(j, "r_select");
        x.r_up = get_stack_optional<double>(j, "r_up");
        x.t_act = get_stack_optional<double>(j, "t_act");
        x.t_dgov = get_stack_optional<double>(j, "t_dgov");
        x.t_eng = get_stack_optional<double>(j, "t_eng");
        x.t_fload = get_stack_optional<double>(j, "t_fload");
        x.t_pelec = get_stack_optional<double>(j, "t_pelec");
        x.t_sa = get_stack_optional<double>(j, "t_sa");
        x.t_sb = get_stack_optional<double>(j, "t_sb");
        x.wfnl = get_stack_optional<double>(j, "wfnl");
        x.a_t = get_stack_optional<double>(j, "a_t");
        x.b_gv0 = get_stack_optional<double>(j, "b_gv0");
        x.b_gv1 = get_stack_optional<double>(j, "b_gv1");
        x.b_gv2 = get_stack_optional<double>(j, "b_gv2");
        x.b_gv3 = get_stack_optional<double>(j, "b_gv3");
        x.b_gv4 = get_stack_optional<double>(j, "b_gv4");
        x.b_gv5 = get_stack_optional<double>(j, "b_gv5");
        x.b_max = get_stack_optional<double>(j, "b_max");
        x.g_max = get_stack_optional<double>(j, "g_max");
        x.g_min = get_stack_optional<double>(j, "g_min");
        x.hdam = get_stack_optional<double>(j, "hdam");
        x.qnl = get_stack_optional<double>(j, "qnl");
        x.r_perm = get_stack_optional<double>(j, "r_perm");
        x.r_temp = get_stack_optional<double>(j, "r_temp");
        x.t_blade = get_stack_optional<double>(j, "t_blade");
        x.t_g = get_stack_optional<double>(j, "t_g");
        x.t_n = get_stack_optional<double>(j, "t_n");
        x.t_np = get_stack_optional<double>(j, "t_np");
        x.t_w = get_stack_optional<std::variant<std::vector<double>, double>>(j, "t_w");
        x.ttrip = get_stack_optional<int64_t>(j, "ttrip");
        x.v_elm = get_stack_optional<double>(j, "v_elm");
        x.k_1 = get_stack_optional<double>(j, "k_1");
        x.k_2 = get_stack_optional<double>(j, "k_2");
        x.k_3 = get_stack_optional<double>(j, "k_3");
        x.k_4 = get_stack_optional<double>(j, "k_4");
        x.k_5 = get_stack_optional<double>(j, "k_5");
        x.k_6 = get_stack_optional<double>(j, "k_6");
        x.k_7 = get_stack_optional<double>(j, "k_7");
        x.k_8 = get_stack_optional<double>(j, "k_8");
        x.p_max = get_stack_optional<double>(j, "p_max");
        x.p_min = get_stack_optional<double>(j, "p_min");
        x.t_6 = get_stack_optional<double>(j, "t_6");
        x.t_7 = get_stack_optional<double>(j, "t_7");
        x.u_c = get_stack_optional<double>(j, "u_c");
        x.u_o = get_stack_optional<double>(j, "u_o");
        x.d_t = get_stack_optional<double>(j, "d_t");
        x.a_1 = get_stack_optional<double>(j, "a_1");
        x.a_2 = get_stack_optional<double>(j, "a_2");
        x.a_3 = get_stack_optional<double>(j, "a_3");
        x.a_4 = get_stack_optional<double>(j, "a_4");
        x.a_5 = get_stack_optional<double>(j, "a_5");
        x.a_6 = get_stack_optional<double>(j, "a_6");
        x.j = get_stack_optional<double>(j, "j");
        x.dynamic_and_control_k = get_stack_optional<int64_t>(j, "k");
        x.k_s = get_stack_optional<double>(j, "k_s");
        x.ls_max = get_stack_optional<double>(j, "ls_max");
        x.ls_min = get_stack_optional<double>(j, "ls_min");
        x.t_delay = get_stack_optional<double>(j, "t_delay");
        x.v_cl = get_stack_optional<double>(j, "v_cl");
        x.v_cu = get_stack_optional<double>(j, "v_cu");
        x.mvab = get_stack_optional<double>(j, "mvab");
        x.theta_0 = get_stack_optional<double>(j, "theta_0");
        x.k_cc = get_stack_optional<double>(j, "k_cc");
        x.k_ic = get_stack_optional<double>(j, "k_ic");
        x.k_iw = get_stack_optional<double>(j, "k_iw");
        x.k_pc = get_stack_optional<double>(j, "k_pc");
        x.k_pw = get_stack_optional<double>(j, "k_pw");
        x.pi_max = get_stack_optional<double>(j, "pi_max");
        x.pi_min = get_stack_optional<double>(j, "pi_min");
        x.pi_ratmn = get_stack_optional<double>(j, "pi_ratmn");
        x.pi_ratmx = get_stack_optional<double>(j, "pi_ratmx");
        x.t_pi = get_stack_optional<double>(j, "t_pi");
        x.k_ip = get_stack_optional<double>(j, "k_ip");
        x.k_pp = get_stack_optional<double>(j, "k_pp");
        x.p1 = get_stack_optional<double>(j, "p1");
        x.p2 = get_stack_optional<double>(j, "p2");
        x.p3 = get_stack_optional<double>(j, "p3");
        x.p4 = get_stack_optional<double>(j, "p4");
        x.spd1 = get_stack_optional<double>(j, "spd1");
        x.spd2 = get_stack_optional<double>(j, "spd2");
        x.spd3 = get_stack_optional<double>(j, "spd3");
        x.spd4 = get_stack_optional<double>(j, "spd4");
        x.t_flag = get_stack_optional<int64_t>(j, "t_flag");
        x.t_p = get_stack_optional<double>(j, "t_p");
        x.te_max = get_stack_optional<double>(j, "te_max");
        x.te_min = get_stack_optional<double>(j, "te_min");
        x.tw_ref = get_stack_optional<double>(j, "tw_ref");
        x.d_shaft = get_stack_optional<double>(j, "d_shaft");
        x.h_g = get_stack_optional<double>(j, "h_g");
        x.h_t = get_stack_optional<double>(j, "h_t");
        x.k_shaft = get_stack_optional<double>(j, "k_shaft");
        x.w_o = get_stack_optional<double>(j, "w_o");
        x.accel = get_stack_optional<double>(j, "accel");
        x.brkpt = get_stack_optional<double>(j, "brkpt");
        x.iqr_max = get_stack_optional<double>(j, "iqr_max");
        x.iqr_min = get_stack_optional<double>(j, "iqr_min");
        x.lvpl1 = get_stack_optional<double>(j, "lvpl1");
        x.lvpl_sw = get_stack_optional<int64_t>(j, "lvpl_sw");
        x.lvpnt0 = get_stack_optional<double>(j, "lvpnt0");
        x.lvpnt1 = get_stack_optional<double>(j, "lvpnt1");
        x.q_min = get_stack_optional<double>(j, "q_min");
        x.rrpwr = get_stack_optional<double>(j, "rrpwr");
        x.tfltr = get_stack_optional<double>(j, "tfltr");
        x.tg = get_stack_optional<double>(j, "tg");
        x.xe = get_stack_optional<double>(j, "xe");
        x.zerox = get_stack_optional<double>(j, "zerox");
        x.dp_max = get_stack_optional<double>(j, "dp_max");
        x.dp_min = get_stack_optional<double>(j, "dp_min");
        x.i_max = get_stack_optional<double>(j, "i_max");
        x.ip1 = get_stack_optional<double>(j, "ip1");
        x.ip2 = get_stack_optional<double>(j, "ip2");
        x.ip3 = get_stack_optional<double>(j, "ip3");
        x.ip4 = get_stack_optional<double>(j, "ip4");
        x.iq1 = get_stack_optional<double>(j, "iq1");
        x.iq2 = get_stack_optional<double>(j, "iq2");
        x.iq3 = get_stack_optional<double>(j, "iq3");
        x.iq4 = get_stack_optional<double>(j, "iq4");
        x.iqfrz = get_stack_optional<double>(j, "iqfrz");
        x.iqh1 = get_stack_optional<double>(j, "iqh1");
        x.iql1 = get_stack_optional<double>(j, "iql1");
        x.kqi = get_stack_optional<double>(j, "kqi");
        x.kqp = get_stack_optional<double>(j, "kqp");
        x.kqv = get_stack_optional<double>(j, "kqv");
        x.kvi = get_stack_optional<double>(j, "kvi");
        x.kvp = get_stack_optional<double>(j, "kvp");
        x.p_flag = get_stack_optional<int64_t>(j, "p_flag");
        x.pf_flag = get_stack_optional<int64_t>(j, "pf_flag");
        x.pq_flag = get_stack_optional<int64_t>(j, "pq_flag");
        x.q_flag = get_stack_optional<int64_t>(j, "q_flag");
        x.q_max = get_stack_optional<double>(j, "q_max");
        x.t_pord = get_stack_optional<double>(j, "t_pord");
        x.thld = get_stack_optional<double>(j, "thld");
        x.thld2 = get_stack_optional<double>(j, "thld2");
        x.tiq = get_stack_optional<double>(j, "tiq");
        x.tp = get_stack_optional<double>(j, "tp");
        x.trv = get_stack_optional<double>(j, "trv");
        x.v_dip = get_stack_optional<double>(j, "v_dip");
        x.v_flag = get_stack_optional<int64_t>(j, "v_flag");
        x.v_ref0 = get_stack_optional<double>(j, "v_ref0");
        x.v_ref1 = get_stack_optional<double>(j, "v_ref1");
        x.v_up = get_stack_optional<double>(j, "v_up");
        x.vp1 = get_stack_optional<double>(j, "vp1");
        x.vp2 = get_stack_optional<double>(j, "vp2");
        x.vp3 = get_stack_optional<double>(j, "vp3");
        x.vp4 = get_stack_optional<double>(j, "vp4");
        x.vq1 = get_stack_optional<double>(j, "vq1");
        x.vq2 = get_stack_optional<double>(j, "vq2");
        x.vq3 = get_stack_optional<double>(j, "vq3");
        x.vq4 = get_stack_optional<double>(j, "vq4");
        x.dbd = get_stack_optional<double>(j, "dbd");
        x.ddn = get_stack_optional<double>(j, "ddn");
        x.dup = get_stack_optional<double>(j, "dup");
        x.fdbd1 = get_stack_optional<double>(j, "fdbd1");
        x.fdbd2 = get_stack_optional<double>(j, "fdbd2");
        x.fe_max = get_stack_optional<double>(j, "fe_max");
        x.fe_min = get_stack_optional<double>(j, "fe_min");
        x.frqflg = get_stack_optional<int64_t>(j, "frqflg");
        x.kig = get_stack_optional<double>(j, "kig");
        x.kpg = get_stack_optional<double>(j, "kpg");
        x.monitored_branch = get_stack_optional<std::variant<int64_t, std::string>>(j, "monitored_branch");
        x.monitored_branch_type = get_stack_optional<MonitoredBranchType>(j, "monitored_branch_type");
        x.monitored_bus = get_stack_optional<std::variant<int64_t, std::string>>(j, "monitored_bus");
        x.outflag = get_stack_optional<int64_t>(j, "outflag");
        x.puflag = get_stack_optional<int64_t>(j, "puflag");
        x.r_c = get_stack_optional<double>(j, "r_c");
        x.refflg = get_stack_optional<int64_t>(j, "refflg");
        x.t_lag = get_stack_optional<double>(j, "t_lag");
        x.tft = get_stack_optional<double>(j, "tft");
        x.tfv = get_stack_optional<double>(j, "tfv");
        x.vcmpflg = get_stack_optional<int64_t>(j, "vcmpflg");
        x.vfrz = get_stack_optional<double>(j, "vfrz");
        x.x_c = get_stack_optional<double>(j, "x_c");
        x.k_w = get_stack_optional<std::vector<double>>(j, "k_w");
        x.k_z = get_stack_optional<std::vector<double>>(j, "k_z");
    }

    inline void to_json(json & j, const DynamicAndControl & x) {
        j = json::object();
        j["category"] = x.category;
        j["D"] = x.d;
        j["gen"] = x.gen;
        j["H"] = x.h;
        j["l_1"] = x.l_1;
        j["l_d"] = x.l_d;
        j["l_pd"] = x.l_pd;
        j["l_ppd"] = x.l_ppd;
        j["l_pq"] = x.l_pq;
        j["l_q"] = x.l_q;
        j["model"] = x.model;
        j["r_a"] = x.r_a;
        j["r_comp"] = x.r_comp;
        j["s_1"] = x.s_1;
        j["s_12"] = x.s_12;
        j["t_pdo"] = x.t_pdo;
        j["t_ppdo"] = x.t_ppdo;
        j["t_ppqo"] = x.t_ppqo;
        j["t_pqo"] = x.t_pqo;
        j["uid"] = x.uid;
        j["x_comp"] = x.x_comp;
        j["e_1"] = x.e_1;
        j["e_2"] = x.e_2;
        j["k_a"] = x.k_a;
        j["k_c"] = x.k_c;
        j["k_d"] = x.k_d;
        j["k_e"] = x.k_e;
        j["k_f"] = x.k_f;
        j["s_e1"] = x.s_e1;
        j["s_e2"] = x.s_e2;
        j["spdmlt"] = x.spdmlt;
        j["t_a"] = x.t_a;
        j["t_b"] = x.t_b;
        j["t_c"] = x.t_c;
        j["t_e"] = x.t_e;
        j["t_f"] = x.t_f;
        j["t_r"] = x.t_r;
        j["va_max"] = x.va_max;
        j["va_min"] = x.va_min;
        j["vr_max"] = x.vr_max;
        j["vr_min"] = x.vr_min;
        j["k_h"] = x.k_h;
        j["t_h"] = x.t_h;
        j["t_j"] = x.t_j;
        j["t_k"] = x.t_k;
        j["vfe_lim"] = x.vfe_lim;
        j["vh_max"] = x.vh_max;
        j["exclim"] = x.exclim;
        j["uelin"] = x.uelin;
        j["ang_p"] = x.ang_p;
        j["k_g"] = x.k_g;
        j["k_i"] = x.k_i;
        j["k_im"] = x.k_im;
        j["k_ir"] = x.k_ir;
        j["k_p"] = x.k_p;
        j["k_pm"] = x.k_pm;
        j["k_pr"] = x.k_pr;
        j["vb_max"] = x.vb_max;
        j["vg_max"] = x.vg_max;
        j["vm_max"] = x.vm_max;
        j["vm_min"] = x.vm_min;
        j["x_l"] = x.x_l;
        j["k_b"] = x.k_b;
        j["k_l"] = x.k_l;
        j["v_lr"] = x.v_lr;
        j["efd_max"] = x.efd_max;
        j["efd_min"] = x.efd_min;
        j["t_a1"] = x.t_a1;
        j["t_a2"] = x.t_a2;
        j["t_a3"] = x.t_a3;
        j["t_a4"] = x.t_a4;
        j["t_f1"] = x.t_f1;
        j["t_f2"] = x.t_f2;
        j["v_r1"] = x.v_r1;
        j["v_r2"] = x.v_r2;
        j["s_pare"] = x.s_pare;
        j["c_switch"] = x.c_switch;
        j["e_max"] = x.e_max;
        j["e_min"] = x.e_min;
        j["K"] = x.k;
        j["r_crfd"] = x.r_crfd;
        j["t_atb"] = x.t_atb;
        j["a"] = x.a;
        j["b"] = x.b;
        j["d_turb"] = x.d_turb;
        j["db1"] = x.db1;
        j["db2"] = x.db2;
        j["eps"] = x.eps;
        j["f_idle"] = x.f_idle;
        j["g_v1"] = x.g_v1;
        j["g_v2"] = x.g_v2;
        j["g_v3"] = x.g_v3;
        j["g_v4"] = x.g_v4;
        j["g_v5"] = x.g_v5;
        j["g_v6"] = x.g_v6;
        j["k_t"] = x.k_t;
        j["l_max"] = x.l_max;
        j["l_oadinc"] = x.l_oadinc;
        j["lt_rate"] = x.lt_rate;
        j["mw_cap"] = x.mw_cap;
        j["p_gv1"] = x.p_gv1;
        j["p_gv2"] = x.p_gv2;
        j["p_gv3"] = x.p_gv3;
        j["p_gv4"] = x.p_gv4;
        j["p_gv5"] = x.p_gv5;
        j["p_gv6"] = x.p_gv6;
        j["R"] = x.r;
        j["r_max"] = x.r_max;
        j["t_1"] = x.t_1;
        j["t_2"] = x.t_2;
        j["t_3"] = x.t_3;
        j["t_4"] = x.t_4;
        j["t_5"] = x.t_5;
        j["t_ltr"] = x.t_ltr;
        j["v_max"] = x.v_max;
        j["v_min"] = x.v_min;
        j["aset"] = x.aset;
        j["d_m"] = x.d_m;
        j["db"] = x.db;
        j["f_lag"] = x.f_lag;
        j["k_dgov"] = x.k_dgov;
        j["k_igov"] = x.k_igov;
        j["k_iload"] = x.k_iload;
        j["k_imw"] = x.k_imw;
        j["k_pgov"] = x.k_pgov;
        j["k_pload"] = x.k_pload;
        j["k_trub"] = x.k_trub;
        j["l_dref"] = x.l_dref;
        j["max_err"] = x.max_err;
        j["min_err"] = x.min_err;
        j["p_mwset"] = x.p_mwset;
        j["r"] = x.dynamic_and_control_r;
        j["r_close"] = x.r_close;
        j["r_down"] = x.r_down;
        j["r_open"] = x.r_open;
        j["r_select"] = x.r_select;
        j["r_up"] = x.r_up;
        j["t_act"] = x.t_act;
        j["t_dgov"] = x.t_dgov;
        j["t_eng"] = x.t_eng;
        j["t_fload"] = x.t_fload;
        j["t_pelec"] = x.t_pelec;
        j["t_sa"] = x.t_sa;
        j["t_sb"] = x.t_sb;
        j["wfnl"] = x.wfnl;
        j["a_t"] = x.a_t;
        j["b_gv0"] = x.b_gv0;
        j["b_gv1"] = x.b_gv1;
        j["b_gv2"] = x.b_gv2;
        j["b_gv3"] = x.b_gv3;
        j["b_gv4"] = x.b_gv4;
        j["b_gv5"] = x.b_gv5;
        j["b_max"] = x.b_max;
        j["g_max"] = x.g_max;
        j["g_min"] = x.g_min;
        j["hdam"] = x.hdam;
        j["qnl"] = x.qnl;
        j["r_perm"] = x.r_perm;
        j["r_temp"] = x.r_temp;
        j["t_blade"] = x.t_blade;
        j["t_g"] = x.t_g;
        j["t_n"] = x.t_n;
        j["t_np"] = x.t_np;
        j["t_w"] = x.t_w;
        j["ttrip"] = x.ttrip;
        j["v_elm"] = x.v_elm;
        j["k_1"] = x.k_1;
        j["k_2"] = x.k_2;
        j["k_3"] = x.k_3;
        j["k_4"] = x.k_4;
        j["k_5"] = x.k_5;
        j["k_6"] = x.k_6;
        j["k_7"] = x.k_7;
        j["k_8"] = x.k_8;
        j["p_max"] = x.p_max;
        j["p_min"] = x.p_min;
        j["t_6"] = x.t_6;
        j["t_7"] = x.t_7;
        j["u_c"] = x.u_c;
        j["u_o"] = x.u_o;
        j["d_t"] = x.d_t;
        j["a_1"] = x.a_1;
        j["a_2"] = x.a_2;
        j["a_3"] = x.a_3;
        j["a_4"] = x.a_4;
        j["a_5"] = x.a_5;
        j["a_6"] = x.a_6;
        j["j"] = x.j;
        j["k"] = x.dynamic_and_control_k;
        j["k_s"] = x.k_s;
        j["ls_max"] = x.ls_max;
        j["ls_min"] = x.ls_min;
        j["t_delay"] = x.t_delay;
        j["v_cl"] = x.v_cl;
        j["v_cu"] = x.v_cu;
        j["mvab"] = x.mvab;
        j["theta_0"] = x.theta_0;
        j["k_cc"] = x.k_cc;
        j["k_ic"] = x.k_ic;
        j["k_iw"] = x.k_iw;
        j["k_pc"] = x.k_pc;
        j["k_pw"] = x.k_pw;
        j["pi_max"] = x.pi_max;
        j["pi_min"] = x.pi_min;
        j["pi_ratmn"] = x.pi_ratmn;
        j["pi_ratmx"] = x.pi_ratmx;
        j["t_pi"] = x.t_pi;
        j["k_ip"] = x.k_ip;
        j["k_pp"] = x.k_pp;
        j["p1"] = x.p1;
        j["p2"] = x.p2;
        j["p3"] = x.p3;
        j["p4"] = x.p4;
        j["spd1"] = x.spd1;
        j["spd2"] = x.spd2;
        j["spd3"] = x.spd3;
        j["spd4"] = x.spd4;
        j["t_flag"] = x.t_flag;
        j["t_p"] = x.t_p;
        j["te_max"] = x.te_max;
        j["te_min"] = x.te_min;
        j["tw_ref"] = x.tw_ref;
        j["d_shaft"] = x.d_shaft;
        j["h_g"] = x.h_g;
        j["h_t"] = x.h_t;
        j["k_shaft"] = x.k_shaft;
        j["w_o"] = x.w_o;
        j["accel"] = x.accel;
        j["brkpt"] = x.brkpt;
        j["iqr_max"] = x.iqr_max;
        j["iqr_min"] = x.iqr_min;
        j["lvpl1"] = x.lvpl1;
        j["lvpl_sw"] = x.lvpl_sw;
        j["lvpnt0"] = x.lvpnt0;
        j["lvpnt1"] = x.lvpnt1;
        j["q_min"] = x.q_min;
        j["rrpwr"] = x.rrpwr;
        j["tfltr"] = x.tfltr;
        j["tg"] = x.tg;
        j["xe"] = x.xe;
        j["zerox"] = x.zerox;
        j["dp_max"] = x.dp_max;
        j["dp_min"] = x.dp_min;
        j["i_max"] = x.i_max;
        j["ip1"] = x.ip1;
        j["ip2"] = x.ip2;
        j["ip3"] = x.ip3;
        j["ip4"] = x.ip4;
        j["iq1"] = x.iq1;
        j["iq2"] = x.iq2;
        j["iq3"] = x.iq3;
        j["iq4"] = x.iq4;
        j["iqfrz"] = x.iqfrz;
        j["iqh1"] = x.iqh1;
        j["iql1"] = x.iql1;
        j["kqi"] = x.kqi;
        j["kqp"] = x.kqp;
        j["kqv"] = x.kqv;
        j["kvi"] = x.kvi;
        j["kvp"] = x.kvp;
        j["p_flag"] = x.p_flag;
        j["pf_flag"] = x.pf_flag;
        j["pq_flag"] = x.pq_flag;
        j["q_flag"] = x.q_flag;
        j["q_max"] = x.q_max;
        j["t_pord"] = x.t_pord;
        j["thld"] = x.thld;
        j["thld2"] = x.thld2;
        j["tiq"] = x.tiq;
        j["tp"] = x.tp;
        j["trv"] = x.trv;
        j["v_dip"] = x.v_dip;
        j["v_flag"] = x.v_flag;
        j["v_ref0"] = x.v_ref0;
        j["v_ref1"] = x.v_ref1;
        j["v_up"] = x.v_up;
        j["vp1"] = x.vp1;
        j["vp2"] = x.vp2;
        j["vp3"] = x.vp3;
        j["vp4"] = x.vp4;
        j["vq1"] = x.vq1;
        j["vq2"] = x.vq2;
        j["vq3"] = x.vq3;
        j["vq4"] = x.vq4;
        j["dbd"] = x.dbd;
        j["ddn"] = x.ddn;
        j["dup"] = x.dup;
        j["fdbd1"] = x.fdbd1;
        j["fdbd2"] = x.fdbd2;
        j["fe_max"] = x.fe_max;
        j["fe_min"] = x.fe_min;
        j["frqflg"] = x.frqflg;
        j["kig"] = x.kig;
        j["kpg"] = x.kpg;
        j["monitored_branch"] = x.monitored_branch;
        j["monitored_branch_type"] = x.monitored_branch_type;
        j["monitored_bus"] = x.monitored_bus;
        j["outflag"] = x.outflag;
        j["puflag"] = x.puflag;
        j["r_c"] = x.r_c;
        j["refflg"] = x.refflg;
        j["t_lag"] = x.t_lag;
        j["tft"] = x.tft;
        j["tfv"] = x.tfv;
        j["vcmpflg"] = x.vcmpflg;
        j["vfrz"] = x.vfrz;
        j["x_c"] = x.x_c;
        j["k_w"] = x.k_w;
        j["k_z"] = x.k_z;
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
        x.bus = j.at("bus").get<BusRefElement>();
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
        x.uid = j.at("uid").get<BusRefElement>();
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
        x.bus_fr = j.at("bus_fr").get<BusRefElement>();
        x.bus_to = j.at("bus_to").get<BusRefElement>();
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
        x.uid = j.at("uid").get<BusRefElement>();
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
        x.bus = j.at("bus").get<BusRefElement>();
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
        x.uid = j.at("uid").get<BusRefElement>();
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
        x.bus_fr = j.at("bus_fr").get<BusRefElement>();
        x.bus_to = j.at("bus_to").get<BusRefElement>();
        x.cm_ub = get_stack_optional<double>(j, "cm_ub");
        x.ext = get_untyped(j, "ext");
        x.name = get_stack_optional<std::string>(j, "name");
        x.nominal_mva = get_stack_optional<double>(j, "nominal_mva");
        x.sm_ub = get_stack_optional<double>(j, "sm_ub");
        x.status = j.at("status").get<int64_t>();
        x.uid = j.at("uid").get<BusRefElement>();
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
        x.participants = get_stack_optional<std::vector<BusRefElement>>(j, "participants");
        x.pg_down = get_stack_optional<std::variant<CmUbAClass, double>>(j, "pg_down");
        x.pg_up = get_stack_optional<std::variant<CmUbAClass, double>>(j, "pg_up");
        x.reserve_type = j.at("reserve_type").get<ReserveType>();
        x.status = j.at("status").get<int64_t>();
        x.uid = j.at("uid").get<BusRefElement>();
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
        x.bus = j.at("bus").get<BusRefElement>();
        x.ext = get_untyped(j, "ext");
        x.gs = j.at("gs").get<Gs>();
        x.name = get_stack_optional<std::string>(j, "name");
        x.nominal_mva = get_stack_optional<double>(j, "nominal_mva");
        x.num_steps_ub = j.at("num_steps_ub").get<NumStepsUbUnion>();
        x.status = j.at("status").get<int64_t>();
        x.uid = j.at("uid").get<BusRefElement>();
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
        x.bus = j.at("bus").get<BusRefElement>();
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
        x.uid = j.at("uid").get<BusRefElement>();
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
        x.bus_fr = j.at("bus_fr").get<BusRefElement>();
        x.bus_to = j.at("bus_to").get<BusRefElement>();
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
        x.uid = j.at("uid").get<BusRefElement>();
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
        x.uid = j.at("uid").get<BusRefElement>();
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
        x.dynamic_and_control = get_stack_optional<std::vector<DynamicAndControl>>(j, "dynamic_and_control");
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
        j["dynamic_and_control"] = x.dynamic_and_control;
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
        x.uid = j.at("uid").get<BusRefElement>();
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
        x.uid = j.at("uid").get<BusRefElement>();
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
        x.uid = j.at("uid").get<BusRefElement>();
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
        x.uid = j.at("uid").get<BusRefElement>();
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
        x.uid = j.at("uid").get<BusRefElement>();
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
        x.uid = j.at("uid").get<BusRefElement>();
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
        x.uid = j.at("uid").get<BusRefElement>();
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
        x.uid = j.at("uid").get<std::vector<BusRefElement>>();
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
        x.uid = j.at("uid").get<BusRefElement>();
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
        x.uid = j.at("uid").get<BusRefElement>();
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
        x.uid = j.at("uid").get<BusRefElement>();
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
        x.uid = j.at("uid").get<BusRefElement>();
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
        x.uid = j.at("uid").get<BusRefElement>();
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
        x.uid = j.at("uid").get<BusRefElement>();
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
        x.uid = j.at("uid").get<BusRefElement>();
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
        x.uid = j.at("uid").get<BusRefElement>();
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
        x.uid = j.at("uid").get<BusRefElement>();
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
        x.uid = j.at("uid").get<BusRefElement>();
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
        x.uid = j.at("uid").get<BusRefElement>();
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
        x.uid = j.at("uid").get<std::vector<BusRefElement>>();
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
        x.uid = j.at("uid").get<std::vector<BusRefElement>>();
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

    inline void from_json(const json & j, Category & x) {
        if (j == "EXCITER") x = Category::EXCITER;
        else if (j == "MACHINE") x = Category::MACHINE;
        else if (j == "PRIME_MOVER") x = Category::PRIME_MOVER;
        else if (j == "STABILIZER") x = Category::STABILIZER;
        else if (j == "WIND_TURBINE") x = Category::WIND_TURBINE;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const Category & x) {
        switch (x) {
            case Category::EXCITER: j = "EXCITER"; break;
            case Category::MACHINE: j = "MACHINE"; break;
            case Category::PRIME_MOVER: j = "PRIME_MOVER"; break;
            case Category::STABILIZER: j = "STABILIZER"; break;
            case Category::WIND_TURBINE: j = "WIND_TURBINE"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, Model & x) {
        static std::unordered_map<std::string, Model> enumValues {
            {"ESAC1A", Model::ESAC1_A},
            {"ESAC6A", Model::ESAC6_A},
            {"ESDC1A", Model::ESDC1_A},
            {"ESDC2A", Model::ESDC2_A},
            {"ESST4B", Model::ESST4_B},
            {"EXAC1", Model::EXAC1},
            {"EXAC2", Model::EXAC2},
            {"EXPIC1", Model::EXPIC1},
            {"GAST", Model::GAST},
            {"GENROU", Model::GENROU},
            {"GENSAL", Model::GENSAL},
            {"GGOV1", Model::GGOV1},
            {"HYGOV", Model::HYGOV},
            {"IEEEG1", Model::IEEEG1},
            {"IEEEST", Model::IEEEST},
            {"IEEET1", Model::IEEET1},
            {"REEC_A", Model::REEC_A},
            {"REGC_A", Model::REGC_A},
            {"REPC_A", Model::REPC_A},
            {"REPC_B", Model::REPC_B},
            {"SCRX", Model::SCRX},
            {"SEXS", Model::SEXS},
            {"TGOV1", Model::TGOV1},
            {"WTGA_A", Model::WTGA_A},
            {"WTGP_A", Model::WTGP_A},
            {"WTGQ_A", Model::WTGQ_A},
            {"WTGT_A", Model::WTGT_A},
        };
        auto iter = enumValues.find(j.get<std::string>());
        if (iter != enumValues.end()) {
            x = iter->second;
        }
    }

    inline void to_json(json & j, const Model & x) {
        switch (x) {
            case Model::ESAC1_A: j = "ESAC1A"; break;
            case Model::ESAC6_A: j = "ESAC6A"; break;
            case Model::ESDC1_A: j = "ESDC1A"; break;
            case Model::ESDC2_A: j = "ESDC2A"; break;
            case Model::ESST4_B: j = "ESST4B"; break;
            case Model::EXAC1: j = "EXAC1"; break;
            case Model::EXAC2: j = "EXAC2"; break;
            case Model::EXPIC1: j = "EXPIC1"; break;
            case Model::GAST: j = "GAST"; break;
            case Model::GENROU: j = "GENROU"; break;
            case Model::GENSAL: j = "GENSAL"; break;
            case Model::GGOV1: j = "GGOV1"; break;
            case Model::HYGOV: j = "HYGOV"; break;
            case Model::IEEEG1: j = "IEEEG1"; break;
            case Model::IEEEST: j = "IEEEST"; break;
            case Model::IEEET1: j = "IEEET1"; break;
            case Model::REEC_A: j = "REEC_A"; break;
            case Model::REGC_A: j = "REGC_A"; break;
            case Model::REPC_A: j = "REPC_A"; break;
            case Model::REPC_B: j = "REPC_B"; break;
            case Model::SCRX: j = "SCRX"; break;
            case Model::SEXS: j = "SEXS"; break;
            case Model::TGOV1: j = "TGOV1"; break;
            case Model::WTGA_A: j = "WTGA_A"; break;
            case Model::WTGP_A: j = "WTGP_A"; break;
            case Model::WTGQ_A: j = "WTGQ_A"; break;
            case Model::WTGT_A: j = "WTGT_A"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, MonitoredBranchType & x) {
        if (j == "AC_LINE") x = MonitoredBranchType::AC_LINE;
        else if (j == "HVDC_P2P") x = MonitoredBranchType::HVDC_P2_P;
        else if (j == "TRASFORMER") x = MonitoredBranchType::TRASFORMER;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const MonitoredBranchType & x) {
        switch (x) {
            case MonitoredBranchType::AC_LINE: j = "AC_LINE"; break;
            case MonitoredBranchType::HVDC_P2_P: j = "HVDC_P2P"; break;
            case MonitoredBranchType::TRASFORMER: j = "TRASFORMER"; break;
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

    inline void adl_serializer<std::variant<std::vector<ctm_schemas::BusRefElement>, int64_t, std::string>>::from_json(const json & j, std::variant<std::vector<ctm_schemas::BusRefElement>, int64_t, std::string> & x) {
        if (j.is_number_integer())
            x = j.get<int64_t>();
        else if (j.is_string())
            x = j.get<std::string>();
        else if (j.is_array())
            x = j.get<std::vector<ctm_schemas::BusRefElement>>();
        else throw std::runtime_error("Could not deserialise!");
    }

    inline void adl_serializer<std::variant<std::vector<ctm_schemas::BusRefElement>, int64_t, std::string>>::to_json(json & j, const std::variant<std::vector<ctm_schemas::BusRefElement>, int64_t, std::string> & x) {
        switch (x.index()) {
            case 0:
                j = std::get<std::vector<ctm_schemas::BusRefElement>>(x);
                break;
            case 1:
                j = std::get<int64_t>(x);
                break;
            case 2:
                j = std::get<std::string>(x);
                break;
            default: throw std::runtime_error("Input JSON does not conform to schema!");
        }
    }

    inline void adl_serializer<std::variant<double, int64_t>>::from_json(const json & j, std::variant<double, int64_t> & x) {
        if (j.is_number_integer())
            x = j.get<int64_t>();
        else if (j.is_number())
            x = j.get<double>();
        else throw std::runtime_error("Could not deserialise!");
    }

    inline void adl_serializer<std::variant<double, int64_t>>::to_json(json & j, const std::variant<double, int64_t> & x) {
        switch (x.index()) {
            case 0:
                j = std::get<double>(x);
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
