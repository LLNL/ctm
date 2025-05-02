# RAW file contains the following information:
# + System MVA base
# + Buses: id, area, voltage magnitude and angle, lb and ub for voltage in normal and emergency condtions
# + Loads: bus, id, status, active and reactive constant power
# + Fixed bus shunts: bus, id, status, active and reactive power at v=1pu
# + Generators: bus, id, real and reactive power output, lb and ub reactive power, status, lb and ub active power
# + Non-transformer branches: from bus, to bus, circuit, resistance, reactance, susceptance, normal and emergency ratings at v=1pu, status
# + Transformer branches (2-windings): 
# + Switched shunts: bus, status, initial susceptance, steps and susceptance per block
# !!! NOTE: Only RAW v33 supported !!!
mutable struct RAWData
    
    # members
    mva_base::Float64
    buses::DataFrame
    loads::DataFrame
    fixed_shunts::DataFrame
    generators::DataFrame
    branches::DataFrame
    transformers::DataFrame
    area_interchanges::DataFrame
    hvdc_p2p_branches::DataFrame
    volt_source_converters::DataFrame
    impedance_corrections::DataFrame
    hvdc_multi_terminal_branches::DataFrame
    multi_section_line_groups::DataFrame
    zones::DataFrame
    inter_area_transfers::DataFrame
    owners::DataFrame
    facts_control_devices::DataFrame
    switched_shunts::DataFrame
    gne_devices::DataFrame
    induction_machines::DataFrame
    
    # constructor from file
    RAWData(filename::AbstractString) = new(read_raw(filename)...)
    
end

## function to read RAW file

function read_raw(filename::AbstractString)
    
    # start time counter
    t_start = time_ns()
    print("Reading RAWData from ", filename, " ...")
    
    # find starting and ending point of each data section
    secstarts, secends = sections(filename, true)
    
    # get MVA base
    f = open(filename, "r")
    lnsplit = split(readline(f), ',')
    parse(Int, lnsplit[3]) == 33 || error("reader only compatible with RAW v33.")
    mva_base = parse(Float64, lnsplit[2]) 
    close(f)
    
    # read bus data
    buses = CSV.read(filename, DataFrame,
                     header=[:I,:NAME,:BASKV,:IDE,:AREA,:ZONE,:OWNER,:VM,:VA,
                             :NVHI,:NVLO,:EVHI,:EVLO],
                     skipto=secstarts[2], limit=secends[2]-secstarts[2]+1, quotechar='\'',
                     delim=',', types=Dict(1=>Int, 5=>Int, 8=>Float64, 9=>Float64, 10=>Float64,
                                           11=>Float64, 12=>Float64, 13=>Float64),
                     ntasks=1)
    
    # read load data
    if secends[3] >= secstarts[3]
        loads = CSV.read(filename, DataFrame,
                         header=[:I,:ID,:STATUS,:AREA,:ZONE,:PL,:QL,:IP,:IQ,:YP,:YQ,
                                 :OWNER,:SCALE,:INTRPT],
                         skipto=secstarts[3], limit=secends[3]-secstarts[3]+1,
                         quotechar='\'', delim=',',
                         types=Dict(1=>Int, 2=>String, 3=>Int, 6=>Float64, 7=>Float64),
                         ntasks=1)
        loads[!,:ID] = clean_id.(loads[!,:ID])
    else
        loads = DataFrame()
    end
    
    # read fixed bus shunt data
    if secends[4] >= secstarts[4]
        fixed_shunts = CSV.read(filename, DataFrame,
                                header=[:I,:ID,:STATUS,:GL,:BL],
                                skipto=secstarts[4], limit=secends[4]-secstarts[4]+1, 
                                quotechar='\'', delim=',',
                                types=Dict(1=>Int, 2=>String, 3=>Int, 4=>Float64, 5=>Float64),
                                ntasks=1)
        fixed_shunts[!,:ID] = clean_id.(fixed_shunts[!,:ID])
    else
        fixed_shunts = DataFrame()
    end
    
    # generator data
    generators = CSV.read(filename, DataFrame,
                          header=[:I,:ID,:PG,:QG,:QT,:QB,:VS,:IREG,:MBASE,
                                  :ZR,:ZX,:RT,:XT,:GTAP,:STAT,:RMPCT,
                                  :PT,:PB,:O1,:F1,:O2,:F2,:O3,:F3,:O4,:F4,:WMOD,:WPF],
                          skipto=secstarts[5], limit=secends[5]-secstarts[5]+1,
                          quotechar='\'', delim=',',
                          types=Dict(1=>Int, 2=>String, 3=>Float64, 4=>Float64, 5=>Float64, 
                                     6=>Float64, 15=>Int, 17=>Float64, 18=>Float64),
                          ntasks=1)
    generators[!,:ID] = clean_id.(generators[!,:ID])
    
    # non-transformer branch data
    if secends[6] >= secstarts[6]
        branches = CSV.read(filename, DataFrame,
                            header=[:I,:J,:CKT,:R,:X,:B,:RATEA,:RATEB,:RATEC,
                                    :GI,:BI,:GJ,:BJ,:ST,:MET,:LEN,
                                    :O1,:F1,:O2,:F2,:O3,:F3,:O4,:F4],
                            skipto=secstarts[6], limit=secends[6]-secstarts[6]+1,
                            quotechar='\'', delim=',',
                            types=Dict(1=>Int, 2=>Int, 3=>String, 4=>Float64, 5=>Float64, 6=>Float64,
                                       7=>Float64, 9=>Float64, 14=>Int),
                            ntasks=1)
        branches[!,:CKT] = clean_id.(branches[!,:CKT])
    else
        branches = DataFrame()
    end
    
    # transformer data
    if secends[7] >= secstarts[7]
        transformers = read_transformer_data(filename, secstarts[7], secends[7])
    else
        transformers = DataFrame()
    end
    
    # area interchange data
    if secends[8] >= secstarts[8]
        area_interchanges = CSV.read(filename, DataFrame,
                                     header=[:I, :ISW, :PDES, :PTOL, :ARNAME],
                                     skipto=secstarts[8], limit=secends[8]-secstarts[8]+1,
                                     quotechar='\'', delim=',',
                                     types=[Int, Int, Float64, Float64, String],
                                     ntasks=1)
    else
        area_interchanges = DataFrame()
    end
    
    # HVDC P2P data
    if secends[9] >= secstarts[9]
        error("two-terminal DC transmission line reader not implemented")
    else
        hvdc_p2p_branches = DataFrame()
    end
    
    # VSC DC transmission line data
    if secends[10] >= secstarts[10]
        error("voltage source converter reader not implemented")
    else
        volt_source_converters = DataFrame()
    end
    
    # impedance correction data
    if secends[11] >= secstarts[11]
        error("impedance correction reader not implemented")
    else
        impedance_corrections = DataFrame()
    end
    
    # HVDC multi-terminal branch data
    if secends[12] >= secstarts[12]
        error("HVDC multi-terminal reader not implemented")
    else
        hvdc_multi_terminal_branches = DataFrame()
    end
    
    # multi-section line data
    if secends[13] >= secstarts[13]
        error("multi-section line grouping reader not implemented")
    else
        multi_section_line_groups = DataFrame()
    end
    
    # zone data
    if secends[14] >= secstarts[14]
        zones = CSV.read(filename, DataFrame,
                         header=[:I, :ZONAME],
                         skipto=secstarts[14], limit=secends[14]-secstarts[14]+1,
                         quotechar='\'', delim=',',
                         types=[Int, String], ntasks=1)
    else
        zones = DataFrame()
    end
    
    # inter area transfer data
    if secends[15] >= secstarts[15]
        error("inter area transfer reader not implemented")
    else
        inter_area_transfers = DataFrame()
    end
    
    # owners data
    if secends[16] >= secstarts[16]
        owners = CSV.read(filename, DataFrame,
                          header=[:I, :OWNAME],
                          skipto=secstarts[14], limit=secends[14]-secstarts[14]+1,
                          quotechar='\'', delim=',',
                          types=[Int, String], ntasks=1)
    else
        owners = DataFrame()
    end
    
    # facts control data
    if secends[17] >= secstarts[17]
        error("facts control reader not implemented")
    else
        facts_control_devices = DataFrame()
    end
    
    # switched shunt data
    if secends[18] >= secstarts[18]
        switched_shunts = CSV.read(filename, DataFrame,
                                   header=[:I,:MODSW,:ADJM,:STAT,:VSWHI,:VSWLO,:SWREM,:RMPCT,
                                           :RMIDNT,:BINIT, :N1,:B1,:N2,:B2,:N3,:B3,:N4,:B4,
                                           :N5,:B5,:N6,:B6,:N7,:B7,:N8,:B8],
                                   skipto=secstarts[18], limit=secends[18]-secstarts[18]+1,
                                   quotechar='\'', delim=',',
                                   types=Dict(1=>Int, 4=>Int, 10=>Float64, 11=>Float64,
                                              12=>Float64, 13=>Float64, 14=>Float64,
                                              15=>Float64, 16=>Float64, 17=>Float64,
                                              18=>Float64, 19=>Float64, 20=>Float64,
                                              21=>Float64, 22=>Float64, 23=>Float64,
                                              24=>Float64, 25=>Float64, 26=>Float64),
                                   ntasks=1, silencewarnings=true)
    else
        switched_shunts = DataFrame()
    end
    
    # gne device data
    if secends[19] >= secstarts[19]
        error("gne device reader not implemented")
    else
        gne_devices = DataFrame()
    end
    
    # induction machine data
    if secends[19] >= secstarts[19]
        error("induction machine reader not implemented")
    else
        induction_machines = DataFrame()
    end
    
    # report reading time
    t_end = time_ns()
    t_elapsed = (t_end - t_start)/1.0E9
    println(" done. RAWData read in ", round(t_elapsed, digits=3), " secs.")
    
    # return parsed data frames
    return mva_base, buses, loads, fixed_shunts, generators, branches, transformers,
           area_interchanges, hvdc_p2p_branches, volt_source_converters, impedance_corrections,
           hvdc_multi_terminal_branches, multi_section_line_groups, zones, inter_area_transfers,
           owners, facts_control_devices, switched_shunts, gne_devices, induction_machines

end

## function to read transformer data

function read_transformer_data(filename::AbstractString, startline::Int, endline::Int)::DataFrame
    
    # check that lines number make sense
    if mod(endline - startline + 1, 4) != 0
        error("number of lines must be a multiple of 4")
    end
    
    # collect transformers information, row-by-row
    rawinfo = Array{String, 2}(undef, div(endline - startline + 1, 4), 43)
    f = open(filename, "r")
    for j = 1:(startline-1)
        readline(f)
    end
    i = startline
    k = 1
    while i <= endline
        row = String[]
        for j=1:4
            l = readline(f)
            append!(row, clean_id.(split(l, ',')))
        end
        rawinfo[k,:] = row
        k += 1
        i += 4
    end
    close(f)
    
    # form a data frame with the collected data and return
    transformers = DataFrame(rawinfo, [:I,:J,:K,:CKT,:CW,:CZ,:CM,:MAG1,:MAG2,:NMETR,:NAME,:STAT,:O1,
                                       :F1,:O2,:F2,:O3,:F3,:O4,:F4,:VECGRP,:R12,:X12, :SBASE12,
                                       :WINDV1,:NOMV1,:ANG1,:RATA1,:RATB1,:RATC1,:COD1,:CONT1,:RMA1,
                                       :RMI1,:VMA1,:VMI1,:NTP1,:TAB1,:CR1,:CX1,:CNXA1,:WINDV2,
                                       :NOMV2])
    colnames = names(transformers)
    intcols = [1:2;12]
    floatcols = [8:9;22:23;25;27:30;42]
    stringcols = [4,11]
    for col in intcols
        transformers[!,colnames[col]] =
            try
                parse.(Int, transformers[!,colnames[col]])
            catch
                error("failed to parse column ", col, " (", colnames[col], ") as Int.")
            end
    end
    for col in floatcols
        transformers[!,colnames[col]] =
            try
                parse.(Float64, transformers[!,colnames[col]])
            catch
                error("failed to parse column ", col, " (", colnames[col], ") as Float64.")
            end
    end
    for col in stringcols
        transformers[!,colnames[col]] = replace.(transformers[!,colnames[col]], Ref("'" => ""))
    end
    transformers[!,:CKT] = clean_id.(transformers[!,:CKT])
    return transformers
    
end
