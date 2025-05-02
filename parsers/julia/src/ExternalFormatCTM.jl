module ExternalFormatCTMParser

# load external modules
using JSON, JSON3

# load (code-generated) CTMData module
Base.include(@__MODULE__, "../../../generated/julia/CTMData.jl")
using .CTMData

# load submodules to read specific formats
Base.include(@__MODULE__, "ARPAEGO1.jl")        # parser for ARPA-E GO Competition Challenge 1 (https://gocompetition.energy.gov/sites/default/files/SCOPF_Problem_Formulation__Challenge_1_20190412.pdf)
using .ARPAEGO1
Base.include(@__MODULE__, "PSLF.jl")            # parser for PSLF EPC format
using .PSLF
Base.include(@__MODULE__, "PowerWorld.jl")      # parser for PowerWorld Simulator Auxiliary format (https://www.powerworld.com/WebHelp/Content/Other_Documents/Auxiliary-File-Format.pdf)
using .PowerWorld

# function to turn CTMData dict to struct
function CTMData_struct_from_CTMData_dict(ctmdata_dict::Dict)::CTMData.Root
    buff = IOBuffer()
    JSON.print(buff, ctmdata_dict)
    seekstart(buff)
    out = JSON3.read(buff, CTMData.Root)
    close(buff)
    return out
end

## functions here will first generate a CTMData dictionary to then convert it to a CTMData object

# function to read from AUX
function CTMData_dict_from_AUX(fname::AbstractString)::Dict
    
    # read AUX data
    aux_data = PowerWorld.AUXData(fname)
    
    # create and fill dictionary
    ctmdata_dict = Dict{Symbol, Any}()
    
    # return dictionary
    return ctmdata_dict
    
end

from_AUX(fname::AbstractString) = CTMData_struct_from_CTMData_dict(CTMData_dict_from_AUX(fname))

# end-of-module end
end
