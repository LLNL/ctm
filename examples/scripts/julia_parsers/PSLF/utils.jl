## function to split string by spaces respecting double quotes
split_w_quotes(str::AbstractString) =
    map(m -> strip(m.match, '"'), eachmatch(r"\"[^\"]*\"|\S+", str))

## function to detect whether a string is actually an integer number
is_string_an_int(str::AbstractString) = occursin(r"^[-+]?[0-9]+$", str)

## function to detect whether a string is actually a floating point number
is_string_a_float(str::AbstractString) = occursin(r"^[-+]?([0-9]*[.])[0-9]+([eE][-+]?\d+)?$", str)

## function to detect whether a line is a section header
is_section_header(str::AbstractString) = occursin(r"[^ ]\s+data\s+\[\s*\d*\]\s+", str)

## function to detect whether we have reach the end of data in a file
is_file_end(str::AbstractString) = str == "end"

## function to automatically parse value as float, int, or string
function parse_value(str::AbstractString)
    if is_string_a_float(str)
        return parse(Float64, str)
    elseif is_string_an_int(str)
        return parse(Int64, str)
    else
        return strip(str)
    end
end

## function to read next non-comment line
function read_next_noncomment_line(io::IO)::Union{String, Symbol}
    while !eof(io)
        l = readline(io)
        if l[1] != '#'
            return l
        end
    end
    return :EOF
end

## structure to hold PSLF-like record
struct PSLFRecord
    invocation::Vector
    fields::Vector
    PSLFRecord(invocation_::AbstractVector{<:AbstractString},
              fields_::AbstractVector{<:AbstractString}) =
              new(parse_value.(invocation_), parse_value.(fields_))
end

## function to parse an PSLF-like record
function parse_next_record(io::IO)::PSLFRecord
    # read first line; invocation and fields
    l = strip(replace(read_next_noncomment_line(io), "\xa0" => " "))
    s = split_w_quotes(l)
    ix = findfirst(x -> strip(x) == ":", s)
    if isnothing(ix)
        invocation = String[]
        fields = s
    else
        invocation = s[1:(ix-1)]
        fields = s[(ix+1):end]
    end
    # read further lines if necessary
    while l[end] == '/'
        pop!(fields)         # remove last field, corresponding to closing
        l = read_next_noncomment_line(io)
        if l == :EOF
            error("PSLF file terminated in the middle of a record.")
        end
        additional_fields = split_w_quotes(l)
        append!(fields, additional_fields)
    end
    # return PSLF record
    return PSLFRecord(invocation, fields)
end

## function to parse an PSLF-like section
function parse_next_section(io::IO, n_records::Int)::Vector{PSLFRecord}
    section_records = Vector{PSLFRecord}(undef, n_records)
    for i = 1:n_records
        section_records[i] = parse_next_record(io)
    end
    return section_records
end

## function to parse from io until end pattern is met
function parse_until_match(io::IO, regex::Regex)::Tuple{Vector{String}, Union{String, Symbol}}
    out = String[]
    while true
        l = read_next_noncomment_line(io)
        if occursin(regex, l)
            return out, l
        end
        push!(out, l)
        if eof(io)
            return out, :EOF
        end
    end
end

## function to detect whether vector of PSLFRecord is uniform
is_vector_of_records_uniform(v::AbstractVector{PSLFRecord}) = 
    length(unique([length(x.invocation) for x in v])) == 1 &&
    length(unique([length(x.fields) for x in v])) == 1 

## function convert uniform vector of PSLFRecord into DataFrame
function uniform_records_to_dataframe(v::AbstractVector{PSLFRecord})::DataFrame
    is_vector_of_records_uniform(v) || error("cannot convert non-uniform vector of records to DataFrame")
    # if there are not records, return here
    if length(v) == 0
        return DataFrame()
    end
    # construct column names vector
    len_invocation = length(v[1].invocation)
    len_fields = length(v[1].fields)
    column_names = [Symbol("invocation_" * string(i)) for i in 1:len_invocation]
    append!(column_names, [Symbol("field_" * string(i)) for i in 1:len_fields])
    # construct vectors of data by transposition
    data = Vector[]
    for i in 1:len_invocation
        data_i = [v[j].invocation[i] for j in 1:length(v)]
        push!(data, data_i)
    end
    for i in 1:len_fields
        data_i = [v[j].fields[i] for j in 1:length(v)]
        push!(data, data_i)
    end
    # construct and return DataFrame
    return DataFrame(data, column_names)
end

## functions to parse header files for uniform sections

function get_header_type_vector(type_string_vector::AbstractVector)::Vector{DataType}
    out = DataType[]
    sizehint!(out, length(type_string_vector))
    for type_string in type_string_vector
        if type_string == "int"
            push!(out, Int64)
        elseif type_string == "float"
            push!(out, Float64)
        elseif type_string == "string"
            push!(out, String)
        else
            error("unrecognized type string ", type_string)
        end
    end
    return out
end

function load_uniform_headers(filename::AbstractString)
    out = JSON.parsefile(filename)
    for (k, v) in out
        out[k]["invocation"] = Symbol.(v["invocation"])
        out[k]["invocation_types"] = get_header_type_vector(v["invocation_types"])
        out[k]["fields"] = Symbol.(v["fields"])
        out[k]["fields_types"] = get_header_type_vector(v["fields_types"])
    end
    return out
end

## generic function to process uniform section

function process_uniform_section(section_keyword::AbstractString,
                                 records::AbstractVector{PSLFRecord},
                                 uniform_headers::Dict{String, Any})::DataFrame
    # check wether this section is known to be uniform and data is uniform
    @assert section_keyword in keys(uniform_headers)
    is_vector_of_records_uniform(records) ||
        error("unexpected non-uniform data for key ", section_keyword)
    # if there are no records, we return here
    if length(records) == 0
        return DataFrame()
    end
    # shorthand for current header
    header = uniform_headers[section_keyword]
    # collect lengths
    len_invocation_header = length(header["invocation"])
    len_fields_header = length(header["fields"])
    len_invocation_records = length(records[1].invocation)
    len_fields_records = length(records[1].fields)
    # convert records into data frame
    df = uniform_records_to_dataframe(records)
    # change names of columns
    invocation_colnames = Symbol.(names(df)[1:len_invocation_records])
    if len_invocation_header > 0 && len_invocation_records > 0
        n_changes = min(len_invocation_header, len_invocation_records)
        invocation_colnames[1:n_changes] .= header["invocation"][1:n_changes]
    end
    fields_colnames =
        Symbol.(names(df)[(len_invocation_records+1):(len_invocation_records+len_fields_records)])
    if len_fields_header > 0 && len_fields_records > 0
        n_changes = min(len_fields_header, len_fields_records)
        fields_colnames[1:n_changes] .= header["fields"][1:n_changes]
    end
    rename!(df, [invocation_colnames; fields_colnames])
    # change types of columns
    for j = 1:min(len_invocation_header, len_invocation_records)
        coltype = header["invocation_types"][j]
        if coltype == String
            df[!,j] = convert(Vector{String}, string.(df[!,j]))
        else
            df[!,j] = convert(Vector{coltype}, df[!,j])
        end
    end
    for k = 1:min(len_fields_header, len_fields_records)
        coltype = header["fields_types"][k]
        j = len_invocation_records + k
        if coltype == String
            df[!,j] = string.(df[!,j])
        else
            df[!,j] = convert(Vector{coltype}, df[!,j])
        end
    end
    # return data frame with processed data
    return df
end
