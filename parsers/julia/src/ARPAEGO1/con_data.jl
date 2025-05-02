# CON file contains a single section describing all contingencies that can occur in the system.

struct CONData
    
    # members
    contingencies::DataFrame
    
    # constructor from file
    CONData(filename::AbstractString) = new(read_con(filename))
    
end

## structure to hold a N-1 contingency event

abstract type Contingency end

struct GeneratorContingency <: Contingency
    Bus::Int
    Unit::String
end

struct TransmissionContingency <: Contingency
    FromBus::Int
    ToBus::Int
    Ckt::String
end

## function to read CON file
# file is read line-by-line and the result is returned as a data frame

function read_con(filename::AbstractString)::DataFrame
    
    # start time counter
    t_start = time_ns()
    print("Reading CONData from ", filename, " ...")
    
    # read contingency data
    f = open(filename, "r")
    labels = String[]
    ctypes = Symbol[]
    cons = Contingency[]
    emptycons = String[]
    while !eof(f)
        l = readline(f)
        if l == "END"
            break
        end
        if l[1:11] != "CONTINGENCY"
            error("expected contingency start line, found: ", l)
        end
        conname = split(l)[2]
        info = split(readline(f))
        if info[1] == "REMOVE"      # generator contingency
            push!(labels, conname)
            push!(ctypes, :Generator)
            push!(cons, GeneratorContingency(parse(Int, info[6]), clean_id(info[3])))
        elseif info[1] == "OPEN"    # branch contingency
            push!(labels, conname)
            push!(ctypes, :Branch)
            push!(cons, TransmissionContingency(parse(Int, info[5]),
                                                parse(Int, info[8]),
                                                clean_id(info[10])))
        elseif info[1] == "END"
            push!(emptycons, conname)
            continue
        else
            error("expected REMOVE, OPEN or END, found: ", info[1])
        end
        l = readline(f)
        if l != "END"
            error("expected contingency end line, found: ", l)
        end
    end
    if length(emptycons) > 0
        @warn(string("contingency registers ", emptycons, " are empty and they will be ignored."))
    end
    
    # put contingency data in data frame
    contingencies = DataFrame([labels, ctypes, cons], [:LABEL, :CTYPE, :CON])
    
    # report reading time
    t_end = time_ns()
    t_elapsed = (t_end - t_start)/1.0E9
    println(" done. CONData read in ", round(t_elapsed, digits=3), " secs.")
    
    # return contingency data frame
    return contingencies
    
end
