# EPC files contain all the necessary information to run power flows

struct EPCData
    
    # members
    title::String
    comments::String
    solution_parameters::DataFrame
    datamaintainer::DataFrame
    substation::DataFrame
    bus::DataFrame
    branch::DataFrame
    transformer::DataFrame
    generator::DataFrame
    load::DataFrame
    shunt::DataFrame
    svd::DataFrame                 # controlled shunts *** NON-UNIFORM; PENDING ***
    area::DataFrame
    zone::DataFrame
    interface::DataFrame
    interface_branch::DataFrame
    dc_bus::DataFrame
    dc_line::DataFrame
    dc_converter::DataFrame
    vs_converter::DataFrame         # VS DC converters
    z_table::DataFrame              # transformer impedance adjustments
    gdc::DataFrame                  # unified power flow controllers
    transaction::DataFrame
    owner::DataFrame
    motor::DataFrame                # induction motors
    line::DataFrame
    qtable::DataFrame               # generator reactive capability curves
    ba::DataFrame                   # balancing authorities
    
    # reader from file
    function EPCData(filename::AbstractString)
        epc_dict = read_epc(filename)
        return new(epc_dict["title"], epc_dict["comments"], epc_dict["solution_parameters"],
                   get(epc_dict, "datamaintainer", DataFrame()),
                   get(epc_dict, "substation", DataFrame()),
                   get(epc_dict, "bus", DataFrame()),
                   get(epc_dict, "branch", DataFrame()), 
                   get(epc_dict, "transformer", DataFrame()),
                   get(epc_dict, "generator", DataFrame()),
                   get(epc_dict, "load", DataFrame()),
                   get(epc_dict, "shunt", DataFrame()),
                   get(epc_dict, "svd", DataFrame()),
                   get(epc_dict, "area", DataFrame()),
                   get(epc_dict, "zone", DataFrame()),
                   get(epc_dict, "interface", DataFrame()),
                   get(epc_dict, "interface_branch", DataFrame()),
                   get(epc_dict, "dc_bus", DataFrame()),
                   get(epc_dict, "dc_line", DataFrame()),
                   get(epc_dict, "dc_converter", DataFrame()),
                   get(epc_dict, "vs_converter", DataFrame()),
                   get(epc_dict, "z_table", DataFrame()),
                   get(epc_dict, "gdc", DataFrame()),
                   get(epc_dict, "transaction", DataFrame()),
                   get(epc_dict, "owner", DataFrame()),
                   get(epc_dict, "motor", DataFrame()),
                   get(epc_dict, "line", DataFrame()),
                   get(epc_dict, "qtable", DataFrame()),
                   get(epc_dict, "ba", DataFrame()))
    end
    
end

## function to process solution parameters

function process_solution_parameters(sol_params::Vector{String})
    splitted = split_w_quotes.(sol_params)
    out = DataFrame(Any[[v[1] for v in splitted],
                        [parse_value(v[2]) for v in splitted]],
                    Symbol[:Parameter, :Value])
    return out
end

## functions to get information from header

function get_keyword(header::AbstractString)
    keyword_last_pos = findfirst(" data ", header)[1] - 1
    return replace(strip(header[1:keyword_last_pos]), " " => "_")
end

get_num_records(header::AbstractString) = parse(Int64, match(r"\d+", header).match)

## load headers for uniform section parsing

EPC_UNIFORM_HEADERS = load_uniform_headers("PSLF/epc_uniform_headers.json")
    # NOTE: we add 'PSLF/' here as it necessary for parsing the JSON with this file being included
    # in the module file from one level up.
process_uniform_epc_section(section_keyword::AbstractString,
                            records::AbstractVector{PSLFRecord}) =
    process_uniform_section(section_keyword, records, EPC_UNIFORM_HEADERS)

## function to process controlled shunt data (svd)

function remove_zero_svd_blocks!(ni::Vector{Int}, bi::Vector{Float64})::Nothing
    @assert length(ni) == length(bi)
    length(ni) != 0 || return nothing
    rm_pos = Int[]
    for i = 1:length(ni)
        if ni[i] == 0 || bi[i] == 0.0
            push!(rm_pos, i)
        end
    end
    deleteat!(ni, rm_pos)
    deleteat!(bi, rm_pos)
    @assert length(ni) == length(bi)
    return nothing
end

struct SVDRecord
    bus::Int64
    name::String
    bkv::Float64
    id::String
    long_id::String
    st::Int64
    ty::Int64
    kreg_bus::Int64
    kreg_name::String
    kreg_bkv::Float64
    ar::Int64
    z::Int64
    g::Float64
    b::Float64
    bmin::Float64
    bmax::Float64
    vband::Float64
    befmin::Float64
    befmax::Float64
    d_in::Int64
    d_out::Int64
    proj_id::Int64
    nst::Int64
    o1::Int64
    p1::Float64
    o2::Int64
    p2::Float64
    o3::Int64
    p3::Float64
    o4::Int64
    p4::Float64
    ni::Union{Nothing, Vector{Int64}}
    bi::Union{Nothing, Vector{Float64}}
    xc::Union{Nothing, Float64}
    bminsh::Union{Nothing, Float64}
    bmaxsh::Union{Nothing, Float64}
    stsb::Union{Nothing, Int64}
    bminsb::Union{Nothing, Float64}
    bmaxsb::Union{Nothing, Float64}
    vrefmin::Union{Nothing, Float64}
    vrefmax::Union{Nothing, Float64}
    dvdb::Union{Nothing, Float64}
    function SVDRecord(epc_record::PSLFRecord)
        bus = epc_record.invocation[1]
        name = epc_record.invocation[2]
        bkv = epc_record.invocation[3]
        id = string(epc_record.invocation[4])
        long_id = string(epc_record.invocation[5])
        st = epc_record.fields[1]
        ty = epc_record.fields[2]
        kreg_bus = epc_record.fields[3]
        kreg_name = string(epc_record.fields[4])
        kreg_bkv = epc_record.fields[5]
        ar = epc_record.fields[6]
        z = epc_record.fields[7]
        g = epc_record.fields[8]
        b = epc_record.fields[9]
        bmin = epc_record.fields[10]
        bmax = epc_record.fields[11]
        vband = epc_record.fields[12]
        befmin = epc_record.fields[13]
        befmax = epc_record.fields[14]
        d_in = epc_record.fields[15]
        d_out = epc_record.fields[16]
        proj_id = epc_record.fields[17]
        nst = epc_record.fields[18]
        o1 = epc_record.fields[19]
        p1 = epc_record.fields[20]
        o2 = epc_record.fields[21]
        p2 = epc_record.fields[22]
        o3 = epc_record.fields[23]
        p3 = epc_record.fields[24]
        o4 = epc_record.fields[25]
        p4 = epc_record.fields[26]
        if ty == 0
            ni = nothing
            bi = nothing
            xc = nothing
            bminsh = nothing
            bmaxsh = nothing
            stsb = nothing
            bminsb = nothing
            bmaxsb = nothing
            vrefmin = nothing
            vrefmax = nothing
            dvdb = nothing
        elseif ty in (1, 2, 3, 4)
            ni = Int[epc_record.fields[27 + 2*i] for i = 0:9]
            bi = Float64[epc_record.fields[28 + 2*i] for i = 0:9]
            remove_zero_svd_blocks!(ni, bi)
            xc = nothing
            bminsh = nothing
            bmaxsh = nothing
            stsb = nothing
            bminsb = nothing
            bmaxsb = nothing
            vrefmin = epc_record.fields[47]
            vrefmax = epc_record.fields[48]
            dvdb = nothing
        elseif ty in (5, 6)
            ni = nothing
            bi = nothing
            xc = epc_record.fields[27]
            bminsh = epc_record.fields[28]
            bmaxsh = epc_record.fields[29]
            stsb = epc_record.fields[30]
            bminsb = epc_record.fields[31]
            bmaxsb = epc_record.fields[32]
            vrefmin = epc_record.fields[33]
            vrefmax = epc_record.fields[34]
            dvdb = epc_record.fields[35]
        elseif ty == 7
            ni = Int[epc_record.fields[27 + 2*i] for i = 0:9]
            bi = Float64[epc_record.fields[28 + 2*i] for i = 0:9]
            remove_zero_svd_blocks!(ni, bi)
            xc = epc_record.fields[47]
            bminsh = epc_record.fields[48]
            bmaxsh = epc_record.fields[49]
            stsb = epc_record.fields[50]
            bminsb = epc_record.fields[51]
            bmaxsb = epc_record.fields[52]
            vrefmin = epc_record.fields[53]
            vrefmax = epc_record.fields[54]
            dvdb = epc_record.fields[55]
        else
            error("unrecognized ty value ", ty)
        end
        return new(bus, name, bkv, id, long_id, st, ty, kreg_bus, kreg_name, kreg_bkv, ar, z, g, b,
                   bmin, bmax, vband, befmin, befmax, d_in, d_out, proj_id, nst, o1, p1, o2, p2, o3,
                   p3, o4, p4, ni, bi, xc, bminsh, bmaxsh, stsb, bminsb, bmaxsb, vrefmin, vrefmax,
                   dvdb)
    end
end


function process_epc_svd_section(records::AbstractVector{PSLFRecord})::DataFrame
    svd = SVDRecord.(records)
    svd_df = DataFrame(Any[[x.bus for x in svd], [x.name for x in svd], [x.bkv for x in svd],
                           [x.id for x in svd], [x.long_id for x in svd], [x.st for x in svd],
                           [x.ty for x in svd], [x.kreg_bus for x in svd],
                           [x.kreg_name for x in svd], [x.kreg_bkv for x in svd],
                           [x.ar for x in svd], [x.z for x in svd], [x.g for x in svd],
                           [x.b for x in svd], [x.bmin for x in svd], [x.bmax for x in svd],
                           [x.vband for x in svd], [x.befmin for x in svd], [x.befmax for x in svd],
                           [x.d_in for x in svd], [x.d_out for x in svd], [x.proj_id for x in svd],
                           [x.nst for x in svd], [x.o1 for x in svd], [x.p1 for x in svd],
                           [x.o2 for x in svd], [x.p2 for x in svd], [x.o3 for x in svd],
                           [x.p3 for x in svd], [x.o4 for x in svd], [x.p4 for x in svd],
                           [x.ni for x in svd], [x.bi for x in svd], [x.xc for x in svd],
                           [x.bminsh for x in svd], [x.bmaxsh for x in svd], [x.stsb for x in svd],
                           [x.bminsb for x in svd], [x.bmaxsb for x in svd],
                           [x.vrefmin for x in svd], [x.vrefmax for x in svd],
                           [x.dvdb for x in svd]],
                       Symbol[:bus, :name, :bkv, :id, :long_id, :st, :ty, :kreg_bus, :kreg_name,
                              :kreg_bkv, :ar, :z, :g, :b, :bmin, :bmax, :vband, :befmin, :befmax,
                              :d_in, :d_out, :proj_id, :nst, :o1, :p1, :o2, :p2, :o3, :p3, :o4, :p4,
                              :ni, :bi, :xc, :bminsh, :bmaxsh, :stsb, :bminsb, :bmaxsb, :vrefmin,
                              :vrefmax, :dvdb])
    return svd_df
end

## function to process generator reactive capability curves (qtable)

struct QDataRecord
    bus::Int64
    name::String
    bkv::Float64
    id::String
    long_id::String
    p::Vector{Float64}
    qmx::Vector{Float64}
    qmn::Vector{Float64}
    function QDataRecord(epc_record::PSLFRecord)
        bus = epc_record.invocation[1]
        name = epc_record.invocation[2]
        bkv = epc_record.invocation[3]
        id = string(epc_record.invocation[4])
        if length(epc_record.invocation) >= 5
            long_id = string(epc_record.invocation[5])
        else
            long_id = ""
        end
        if length(epc_record.fields) % 3 != 0
            error("PSLRecord should have a number of fields that is multiple of 3.")
        end
        n_p = length(epc_record.fields) ÷ 3
        p = Float64[]
        qmx = Float64[]
        qmn = Float64[]
        for i = 0:(n_p-1)
            if i > 0 && epc_record.fields[3*i + 1] < p[end]
                break
            end
            push!(p, epc_record.fields[3*i + 1])
            push!(qmx, epc_record.fields[3*i + 2])
            push!(qmn, epc_record.fields[3*i + 3])
        end
        if any(diff(p) .< 0)
            @show p
            error("p values not increasing for qtable record with name", name)
        end
        return new(bus, name, bkv, id, long_id, p, qmx, qmn)
    end
end

function process_epc_qtable_section(records::AbstractVector{PSLFRecord})::DataFrame
    qdata = QDataRecord.(records)
    qdata_df = DataFrame(Any[[x.bus for x in qdata],
                             [x.name for x in qdata],
                             [x.bkv for x in qdata],
                             [x.id for x in qdata],
                             [x.long_id for x in qdata],
                             [x.p for x in qdata],
                             [x.qmx for x in qdata],
                             [x.qmn for x in qdata]],
                         Symbol[:bus, :name, :bkv, :id, :long_id, :p, :qmx, :qmn])
    return qdata_df
end

## function to read EPC files

function read_epc(filename::AbstractString)
    
    # start time counter
    t_start = time_ns()
    println("Reading EPCData from ", filename, " ...")
    
    # create dictionary to fill with information in file before passing to object
    epc = Dict{String, Any}()
    
    # open file for reading
    f = open(filename, "r")
    
    ## read contents of EPC file into dictionary
    
    # title section
    if read_next_noncomment_line(f) != "title"
        error("EPC file should start with 'title' in the first (non-info) line")
    end
    
    epc["title"], _ = parse_until_match(f, r"^!$")
    epc["title"] = rstrip.(epc["title"])
    
    # comment section
    if read_next_noncomment_line(f) != "comments"
        error("EPC file should have 'comments' keyword following title section")
    end
    epc["comments"], _ = parse_until_match(f, r"^!$")
    epc["comments"] = rstrip.(epc["comments"])
    
    # solution parameter section
    if read_next_noncomment_line(f) != "solution parameters"
        error("EPC file should have 'solution parameters' keyword following comment section")
    end
    epc["solution_parameters"], _ = parse_until_match(f, r"^!$")
    epc["solution_parameters"] = process_solution_parameters(epc["solution_parameters"])
    
    # read data sections
    while !eof(f)
        l = read_next_noncomment_line(f)
        # check whether we are at the file 'end', break if so (no more data to read)
        is_file_end(l) && break
        # check whether we are at a section header, if not throw error
        is_section_header(l) || error("expected a section header but got: ", l)
        # get section keyword and number of records in it
        section_keyword = get_keyword(l)
        section_num_records = get_num_records(l)
        # parse section
        if haskey(epc, section_keyword)
            error("file contains repeated keyword '", section_keyword, "'.")
        end
        print("    Reading ", section_keyword, " section ... ")
        epc[section_keyword] = parse_next_section(f, section_num_records)
        @assert length(epc[section_keyword]) == section_num_records
        println("done. Read ", section_num_records, " records.")
    end
    
    # close file
    close(f)
    t_reading = (time_ns() - t_start)/1.0E9
    println("Done reading. EPCData read in ", round(t_reading, digits=3),
          " secs.\nProcessing into tables ...")
    
    ## process EPC records of each section into tables
    for (key, value) in epc
        print("    Processing ", key, " section ... ")
        if key == "solution_parameters"
            # nothing to do, already a data frame with names
            println("done")
            continue
        elseif key in ("title", "comments")
            # convert to string
            epc[key] = join(value, "\n")
        elseif key in keys(EPC_UNIFORM_HEADERS)
            # process uniform section
            epc[key] = process_uniform_epc_section(key, value)
        elseif key == "svd"
            # process controlled shunts
            epc["svd"] = process_epc_svd_section(value)
        elseif key == "qtable"
            # process generator reactive capability curves
            epc["qtable"] = process_epc_qtable_section(value)
        else
            error("unrecognized data section " * key * " in EPC file")
        end
        println("done")
    end
    
    # report reading time
    t_elapsed = (time_ns() - t_start)/1.0E9
    t_processing = t_elapsed - t_reading
    println("Done processing. EPCData processed in ", round(t_processing, digits=3),
             " secs.\nTotal EPC parsing time: ", round(t_elapsed, digits=3), " secs.")
    
    # return EPC dictionary
    return epc
    
end
