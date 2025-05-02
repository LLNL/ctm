## function to parse an aux file into a vector of AUXTable

mutable struct AUXTable
    object_type::String
    data::DataFrame
end

function read_aux_file(aux_fname::AbstractString)::Vector{AUXTable}
    
    # consume aux file into memory
    v_aux_sec = consume_aux_file(aux_fname)
    
    # make each AUXSection into an AUXTable (parallelized)
    v_aux_table = Vector{AUXTable}(undef, length(v_aux_sec))
    Threads.@threads for i = 1:length(v_aux_sec)
        v_aux_table[i] = AUXTable(v_aux_sec[i].object_type, parse_section(v_aux_sec[i]))
    end
    
    # return tables
    return v_aux_table
end

##  function to capture section starts, headers, and ends

struct AUXSection
    object_type::String
    field_list::Vector{String}
    contents::IOBuffer
    has_subdata::Bool
    function AUXSection(object_type_::String, field_list_::Vector{String},
                        contents_::IOBuffer, has_subdata_::Bool)
        return new(object_type_, field_list_, contents_, has_subdata_)
    end
    function AUXSection(header::AbstractString, contents_::IOBuffer, has_subdata_::Bool)
        object_type_ = strip(match(r"^[A-Z3][^ ]* ", header).match)
        if object_type_ == "DATA"
            error("Unsupported legacy AUX header.")
        end
        return new(object_type_,
                   strip.(split(match(r"\(.*\)", replace(header, "\"" => "")).match[2:(end-1)], ",")),
                   contents_, has_subdata_)
    end
end

function readline_no_comments(io::IO, comment::String="//")
    l = readline(io) 
    m = match(r"^[^(//)]*//", l)
    if isnothing(m)
        return l
    else
        return m.match[1:(end-2)]
    end
end

function read_until_next_match!(io::IO, regex::Regex,
                                out_buff::Union{IOBuffer, Nothing})
    while !eof(io)
        l = readline_no_comments(io)
        if !isnothing(match(regex, l))
            return l
        end
        if !isnothing(out_buff)
            write(out_buff, l, "\n")
        end
    end
    return :EOF
end

read_until_next_header_start!(io::IO) =
    read_until_next_match!(io, r"^[A-Z3][^ ]* \(", nothing)
read_until_section_end_or_subdata!(io::IO, out_buff::IOBuffer) = 
    read_until_next_match!(io, r"^[ ]*<SUBDATA|^}", out_buff)

function consume_aux_file(aux_fname::AbstractString)::Vector{AUXSection}
    out = AUXSection[]
    io = open(aux_fname, "r")
    while !eof(io)
        header = read_until_next_header_start!(io)
        if header == :EOF
            break
        end
        while true
            if eof(io)
                error("AUX file terminated unexpectedly mid header.")
            end
            l = readline_no_comments(io)
            if !isnothing(match(r"^{", l))
                break
            end
            header *= strip(l)
        end
        contents = IOBuffer()
        has_subdata = false
        while true
            if eof(io)
                error("AUX file terminated unexpectedly mid data list.")
            end
            l = read_until_section_end_or_subdata!(io, contents)
            if !isnothing(match(r"^[ ]*<SUBDATA", l))
                write(contents, l, "\n")
                has_subdata = true
            else
                @assert !isnothing(match(r"^}", l))
                break
            end
        end
        push!(out, AUXSection(header, contents, has_subdata))
    end
    close(io)
    return out
end

## function to parse an AUXSection into a DataFrame

function parse_section(aux_sec::AUXSection)::DataFrame
    if !aux_sec.has_subdata
        # return tabular section here
        return parse_tabular_section(aux_sec)
    end
    # collect all tabular data in a second AUXSection object and subdata in array of dicts
    subdata = Dict{Int, Dict}()
    tab_sec = AUXSection(aux_sec.object_type, aux_sec.field_list, IOBuffer(), false)
    row_counter = 0
    seekstart(aux_sec.contents)
    while !eof(aux_sec.contents)
        l = readline(aux_sec.contents)
        if isnothing(match(r"^[ ]*<SUBDATA", l))
            row_counter += 1
            write(tab_sec.contents, l, "\n")
            continue
        else
            if isnothing(match(r"^[ ]*<SUBDATA [^ ]+>[ ]*$", l))
                error("malformed subdata start: "* l)
            end
            if !haskey(subdata, row_counter)
                subdata[row_counter] = Dict{String, Any}()
            end
            subobject_type = chop(strip(l), head=9, tail=1)
            if haskey(subdata[row_counter], subobject_type)
                error("multiple subdata blocks of equal type for the same object")
            end
            subdata_io = IOBuffer()
            while true
                if eof(aux_sec.contents)
                    error("section ended unexpectedly mid SUBDATA block")
                end
                l = readline(aux_sec.contents)
                if !isnothing(match(r"^[ ]*</SUBDATA>[ ]*$", l))
                    break
                end
                if lstrip(l) != ""
                    write(subdata_io, lstrip(l), "\n")
                end
            end
            subdata[row_counter][subobject_type] = parse_subdata(subobject_type, subdata_io)
        end
    end
    # parse tabular data into data frame
    out = parse_tabular_section(tab_sec)
    # add subdata in addition column
    out[!, :SUBDATA] = Vector{Dict}(undef, nrow(out))
    for (i, sd) in subdata
        out[i, :SUBDATA] = sd
    end
    # return parsed section
    return out
end

# function to parse subdata

const TABULAR_SUBDATA_HEADERS =
    Dict{String, Vector{String}}("BidCurve" => ["MW", "Price_USD_per_MWhr"],
                                 "GICMagLatScalarPoint" => ["Geomagnetic_Latitude", "Scaling_Value"],
                                 "ReactiveCapability" => ["MW", "MinMVAR", "MaxMVAR"])

function parse_subdata(subobject_type::AbstractString, subdata_io::IOBuffer)
    if haskey(TABULAR_SUBDATA_HEADERS, subobject_type)
        seekstart(subdata_io)
        return CSV.read(subdata_io, DataFrame,
                        header=TABULAR_SUBDATA_HEADERS[subobject_type],
                        types=Float64, delim=' ', ignorerepeated=true, ntasks=1)
    else
        return chop(String(take!(subdata_io)), tail=1)
    end
end

# function to parse tabular section

function parse_tabular_section(aux_sec::AUXSection)::DataFrame
    @assert !aux_sec.has_subdata
    seekstart(aux_sec.contents)
    df = CSV.read(aux_sec.contents, DataFrame, header=0, delim=' ',
                  ignorerepeated=true, ntasks=1)
    rename!(df, Symbol.(aux_sec.field_list))
    return df
end
