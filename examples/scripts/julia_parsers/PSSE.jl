module PSSE
println("NOTE: PSSE format parser only works for a subset of features for files in v33.")

using DataFrames, CSV

include("PSSE/utils.jl")
include("PSSE/raw.jl")
include("PSSE/rop.jl")
include("PSSE/con.jl")

struct RAWData
    MVAbase::Float64                        # System MVA base
    buses::DataFrame                        # Buses: id, area, voltage magnitude and angle,
                                            #        lb and ub for voltage in normal and 
                                            #        emergency condtions
    loads::DataFrame                        # Loads: bus, id, status, active and reactive constant power
    fixed_bus_shunts::DataFrame             # Fixed bus shunts: bus, id, status, active and 
                                            #                   reactive power at v=1pu
    generators::DataFrame                   # Generators: bus, id, real and reactive power output, 
                                            #             lb and ub reactive power, status, lb and 
                                            #             ub active power
    non_transformer_branches::DataFrame     # Non-transformer branches: from bus, to bus, circuit, 
                                            #                           resistance, reactance, 
                                            #                           susceptance, normal and 
                                            #                           emergency ratings at v=1pu, 
                                            #                           status
    transformer_branches::DataFrame         # Transformer branches: see PSSE/raw.jl for details
    switched_shunts::DataFrame              # Switched shunts: bus, status, initial susceptance, 
                                            #                  steps and susceptance per block
    
    function RAWData(filename::AbstractString)
        return new(read_raw(filename)...)
    end
end

struct ROPData
    generator_dispatch::DataFrame           # Generator dispatch: correspondance between generators 
                                            #                     and dispatch tables
    active_dispatch_tables::DataFrame       # Active power dispatch tables: correspondance between 
                                            #                               dispatch tables and cost
                                            #                               curves
    cost_curves::DataFrame                  # Cost curves: piecewise linear cost curves, described as 
                                            #              pairs (production_i, cost_i)
    
    function ROPData(filename::AbstractString)
        return new(read_rop(filename)...)
    end
end

struct CONData
    contingencies::DataFrame
    
    function CONData(filename::AbstractString)
        return new(read_con(filename))
    end
end

end # module
