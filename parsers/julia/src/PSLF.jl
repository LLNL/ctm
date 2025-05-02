module PSLF

using DataFrames, CSV, JSON

export EPCData, PSLFData

include("PSLF/utils.jl")
include("PSLF/epc_data.jl")

struct PSLFData
    
    # members
    epc_data::EPCData
    
    # constructor from individual files
    PSLFData(epc_fname::AbstractString) = new(EPCData(epc_fname))
    
end

end
