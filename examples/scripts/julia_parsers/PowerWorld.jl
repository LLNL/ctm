module PowerWorld

using DataFrames, CSV, JSON

export AUXData

include("PowerWorld/read_aux.jl")
include("PowerWorld/collect_aux.jl")

# TO DO: enforce types for each column in tables, similar to what is done for EPC

struct AUXData
    
    # members                                   AUX keys
    area::DataFrame                             # Area
    balancing_authority::DataFrame              # BalancingAuthority
    branch_line::DataFrame                      # Branch (as Branch but with BranchDeviceType == "Line")
    branch_transformer::DataFrame               # Branch (as Branch but with BranchDeviceType == "Transformer")
    bus::DataFrame                              # Bus
    custom_field_description::DataFrame         # CustomFieldDescription
    data_maintainer::DataFrame                  # DataMaintainer
    dc_transmission_line::DataFrame             # DCTransmissionLine
    gen::DataFrame                              # Gen
    injection_group::DataFrame                  # InjectionGroup
    interface_element::DataFrame                # InterfaceElement
    interface::DataFrame                        # Interface
    limit_monitoring_options_value::DataFrame   # Limit_Monitoring_Options_Value
    limit_set::DataFrame                        # LimitSet
    line_shunt::DataFrame                       # LineShunt
    load::DataFrame                             # Load
    load_model_group::DataFrame                 # LoadModelGroup
    multi_terminal_dc_bus::DataFrame            # MTDCBus
    multi_terminal_dc_converter::DataFrame      # MTDCConverter
    multi_terminal_dc_record::DataFrame         # MTDCRecord
    multi_terminal_dc_transimssion_line::DataFrame  # MTDCTransmissionLine
    multi_section_line::DataFrame               # MultiSectionLine
    owner::DataFrame                            # Owner
    part_point::DataFrame                       # PartPoint
    powerworld_case_information::DataFrame      # PWCaseInformation
    reactive_capability::DataFrame              # ReactiveCapability
    shunt_svc::DataFrame                        # Shunt (as Shunt but with field SVCstsv or SVCControlling)
    shunt_switched::DataFrame                   # Shunt (as Shunt but with field BlockNumberStep1)
    substation::DataFrame                       # Substation
    three_winding_transformer::DataFrame        # 3WXFormer
    xf_correction::DataFrame                    # XFCorrection
    zone::DataFrame                             # Zone
    ext::Vector{AUXTable}                       # anything not above will be saved here
    
    # constructor from individual files
    function AUXData(aux_fname::AbstractString)
        
        # read aux data into vector of tables
        aux_tables = read_aux_file(aux_fname)
        
        # create shorthand function for collecting data
        idx_map = make_index_map(aux_tables)
        collect_data!(data_name, primary_table_keys, object_id_fields=nothing) =
            collect_data_from_tables!(aux_tables, idx_map, data_name, primary_table_keys, object_id_fields)
         
        # collect all fields to create object
        area_ = collect_data!("Area", ["Number", "Name", "SuperArea"], ["Number"])
        balancing_authority_ = collect_data!("BalancingAuthority", ["Number", "Name", "AGC"], ["Number"])
        branch_line_ = collect_data!("Branch", ["BusNumFrom", "BusNumTo", "Circuit",
                                                "BranchDeviceType", "ConsolidateAllow"],
                                     ["BusNumFrom", "BusNumTo", "Circuit"])
        branch_transformer_ = collect_data!("Branch", ["BusNumFrom", "BusNumTo", "Circuit",
                                                       "BranchDeviceType", "RegBusNum"],
                                            ["BusNumFrom", "BusNumTo", "Circuit"])
        bus_ = collect_data!("Bus", ["Number", "Name", "NomkV"], ["Number"])
        custom_field_description_ = collect_data!("CustomFieldDescription", ["ObjectType", "CustomType"])
        data_maintainer_ = collect_data!("DataMaintainer", ["Name", "Contact"])
        dc_transmission_line_ = collect_data!("DCTransmissionLine",
                                              ["BusNumRect", "BusNumInv", "Circuit", "Name"],
                                              ["BusNumRect", "BusNumInv", "Circuit"])
        gen_ = collect_data!("Gen", ["BusNum", "ID", "Status", "MWMax", "MWMin"], ["BusNum", "ID"])
        injection_group_ = collect_data!("InjectionGroup", ["Name"], ["Name"])
        interface_element_ = collect_data!("InterfaceElement", ["InterfaceName", "Element"])
        interface_ = collect_data!("Interface", ["Number", "Name", "MonDirection"], ["Name"])
        limit_monitoring_options_value_ = collect_data!("Limit_Monitoring_Options_Value", ["Option"])
        limit_set_ = collect_data!("LimitSet", ["Name", "Disabled"])
        line_shunt_ = collect_data!("LineShunt", ["BusNumFrom", "BusNumTo", "Circuit", "ID", "BusNumLoc"],
                                    ["BusNumFrom", "BusNumTo", "Circuit", "ID"])
        load_ = collect_data!("Load", ["BusNum", "ID", "SMW", "SMvar"], ["BusNum", "ID"])
        load_model_group_ = collect_data!("LoadModelGroup", ["Name", "LongName"], ["Name"])
        multi_terminal_dc_bus_ = collect_data!("MTDCBus", ["Number", "MTDCNumber", "Name"])
        multi_terminal_dc_converter_ = collect_data!("MTDCConverter",
                                                     ["BusNumAC", "BusNumDC", "MTDCNumber", "NumBridges"],
                                                     ["BusNumAC", "BusNumDC", "MTDCNumber"])
        multi_terminal_dc_record_ = collect_data!("MTDCRecord", ["Number", "Name"], ["Number"])
        multi_terminal_dc_transimssion_line_ =
            collect_data!("MTDCTransmissionLine", ["BusNumFrom", "BusNumTo", "Circuit", "MTDCNumber"])
        multi_section_line_ = collect_data!("MultiSectionLine",
                                            ["BusNumFrom", "BusNumTo", "Circuit", "AllowMixedStatus"])
        owner_ = collect_data!("Owner", ["Number", "Name"], ["Number"])
        part_point_ = collect_data!("PartPoint", ["GroupName"])
        powerworld_case_information_ = collect_data!("PWCaseInformation", ["Selected"], String[])
        reactive_capability_ = collect_data!("ReactiveCapability", ["BusNum", "ID"])
        shunt_svc_ = collect_data!("Shunt", ["BusNum", "ID", "SVCstsv", "SVCControlling"],
                                   ["BusNum", "ID"])
        shunt_switched_ = collect_data!("Shunt", ["BusNum", "ID", "BlockNumberStep1"], ["BusNum", "ID"])
        substation_ = collect_data!("Substation", ["Number", "Name"], ["Number"])
        three_winding_transformer_ =
            collect_data!("3WXFormer", ["BusIDPri", "BusIDSec", "BusIDTer", "Circuit", "BusIDStar"],
                          ["BusIDPri", "BusIDSec", "BusIDTer", "Circuit"])
        xf_correction_ = collect_data!("XFCorrection", ["Number", "Name"])
        zone_ = collect_data!("Zone", ["Number", "Name"], ["Number"])
        
        # leave the remaining tables in ext_
        ext_ = AUXTable[]
        for i = 1:length(aux_tables)
            if nrow(aux_tables[i].data) > 0
                push!(ext_, aux_tables[i])
            end
        end
        
        # construct and return object, entries not found are set with 0 empty data frames
        return new(area_, balancing_authority_, branch_line_, branch_transformer_, bus_,
                   custom_field_description_, data_maintainer_, dc_transmission_line_, gen_,
                   injection_group_, interface_element_, interface_, limit_monitoring_options_value_,
                   limit_set_, line_shunt_, load_, load_model_group_, multi_terminal_dc_bus_,
                   multi_terminal_dc_converter_, multi_terminal_dc_record_,
                   multi_terminal_dc_transimssion_line_, multi_section_line_, owner_,
                   part_point_, powerworld_case_information_, reactive_capability_, shunt_svc_,
                   shunt_switched_, substation_, three_winding_transformer_, xf_correction_, 
                   zone_, ext_)
    
    end
    
end

# end-of-module end
end
