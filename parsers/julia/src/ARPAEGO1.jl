module ARPAEGO1

using DataFrames, CSV

export RAWData, ROPData, INLData, CONData, GO1Data

include("ARPAEGO1/utils.jl")
include("ARPAEGO1/raw_data.jl")
include("ARPAEGO1/rop_data.jl")
include("ARPAEGO1/inl_data.jl")
include("ARPAEGO1/con_data.jl")

struct GO1Data
    
    # members
    raw_data::RAWData
    rop_data::ROPData
    inl_data::Union{INLData, Nothing}
    con_data::Union{CONData, Nothing}
    
    # constructor from individual files
    GO1Data(raw_fname::AbstractString, rop_fname::AbstractString,
            inl_fname::AbstractString, con_fname::AbstractString) = 
        new(RAWData(raw_fname), ROPData(rop_fname), INLData(inl_fname), CONData(con_fname))
    
    # constructor from directory to RAW file
    GO1Data(raw_directory::AbstractString, maxnup::Int=3) =
        GO1Data(instance_filenames(raw_directory, maxnup)...)
    
    # cosntructor from RAW and ROP only (ACOPF data without contingencies)
    GO1Data(raw_fname::AbstractString, rop_fname::AbstractString) =
        new(RAWData(raw_fname), ROPData(rop_fname), nothing, nothing)
    
end

function Base.show(io::IO, go1_instance::GO1Data)
    n_buses = nrow(go1_instance.raw_data.buses)
    n_branches = nrow(go1_instance.raw_data.branches)
    n_branches += nrow(go1_instance.raw_data.transformers)
    n_generators = nrow(go1_instance.raw_data.generators)
    if isnothing(go1_instance.con_data)
        print(io, "System formed by ", n_buses, " buses, ", n_branches, " branches, and ", n_generators,
                  " generators.")
    else
        n_contingencies = nrow(go1_instance.con_data.contingencies)
        print(io, "System formed by ", n_buses, " buses, ", n_branches, " branches, and ", n_generators,
                  " generators, with ", n_contingencies, " possible N-1 contingencies.")
    end
    return nothing
end

end
