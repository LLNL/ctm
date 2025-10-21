module JSONSchema2Struct

using Dates, JSON, StringCases

export jsonschema_to_structs

# from https://docs.julialang.org/en/v1/base/base/#Keywords
const RESERVED_WORDS =
    (
        "baremodule",
        "begin",
        "break",
        "catch",
        "const",
        "continue",
        "do",
        "else",
        "elseif",
        "end",
        "export",
        "false",
        "finally",
        "for",
        "function",
        "global",
        "if",
        "import",
        "let",
        "local",
        "macro",
        "module",
        "quote",
        "return",
        "struct",
        "true",
        "try",
        "using",
        "while"
    )

mutable struct ObjectDefinition
    name::String
    level::Int64
    parent::Int64
    members::Vector{String}
    types::Vector{String}
    defaults::Vector{String}
    req::Vector{Bool}
    is_def::Bool
    ObjectDefinition(name_::String, level_::Int64, parent_::Int64,
                     members_::Vector{String}, types_::Vector{String},
                     defaults_::Vector{String}, req_::Vector{Bool},
                     is_def_::Bool=false) = 
        new(name_, level_, parent_, members_, types_, defaults_, req_, is_def_)
end

mutable struct AliasDefinition
    name::String
    level::Int64
    parent::Int64
    type::String
    default::String
    is_def::Bool
    AliasDefinition(name_::String, level_::Int64, parent_::Int64,
                    type_::String, default_::String, is_def_::Bool=false) =
        new(name_, level_, parent_, type_, default_, is_def_)
end

mutable struct Pending
    name::String
    level::Int64
    parent::Int64
    ref::Base.RefValue{Dict{String, Any}}
    is_def::Bool
    Pending(name_::String, level_::Int64, parent_::Int64,
            ref_::Base.RefValue{Dict{String, Any}}, is_def_::Bool=false) =
        new(name_, level_, parent_, ref_, is_def_)
end

typefromname(name::String) = pascalcase(name) #* "Type"

function juliatype(jsontype::String)
    if jsontype == "integer"
        return "Int64"
    elseif jsontype == "number"
        return "Float64"
    elseif jsontype == "string"
        return "String"
    elseif jsontype == "bool"
        return "Bool"
    else
        error("unsupported JSON type " * jsontype)
    end
end

function juliadefault(jsontype::String)
    if jsontype == "integer"
        return "0"
    elseif jsontype == "number"
        return "0.0"
    elseif jsontype == "string"
        return "\"\""
    elseif jsontype == "bool"
        return "false"
    else
        error("unsupported JSON type " * jsontype)
    end
end



function parse_def(name::String, k::String, v::AbstractDict, index::Int64, d::AbstractDict)
    if haskey(v, "\$ref")
        v["\$ref"] == "#" && error("\$defs recursion not implemented.")
        error("\$def cross-referencing not allowed.")
    elseif !haskey(v, "type")
        return AliasDefinition(typefromname(k), 1, 0, "Any", "nothing", true), Pending[]
    elseif v["type"] in ("integer", "number", "string", "bool")
        return AliasDefinition(typefromname(k), 1, 0, juliatype(v["type"]),
                               juliadefault(v["type"]), true),
               Pending[]
    elseif v["type"] == "object"
        obj, pending = parse_object(typefromname(k), 1, 0, index, d["\$defs"][k])
        obj.is_def = true
        for p in pending
            p.is_def = true
        end
        return obj, pending
    elseif v["type"] == "array"
        typename, p = parse_array(k, "Vector{", 1, 0, v)
        p.is_def = true
        return AliasDefinition(typefromname(k), 1, 0, typename, typename * "()", true), Pending[p]
    else
        error("unsupported type ", v["type"])
    end
end

function parse_object(name::String, level::Int64,
                      parent::Int64, index::Int64, d::Dict)::Tuple{ObjectDefinition, Vector{Pending}}
    haskey(d, "type") && haskey(d, "properties") && 
        haskey(d, "required") || error("invalid schema for object")
    @assert d["type"] == "object"
    # process object properties
    od = ObjectDefinition(name, level, parent, String[], String[], String[], Bool[])
    pending = Pending[]
    for (k, v) in d["properties"]
        push!(od.members, k)
        if haskey(v, "\$ref")
            length(v) == 1 || error("\$ref can only reference one definition.")
            v["\$ref"][1:8] == "#/\$defs/" ||
                error("only references to subschemas currently supported.")
            push!(od.types, typefromname(v["\$ref"][9:end]))      # defs will be already processed at root
            push!(od.defaults, typefromname(v["\$ref"][9:end]) * "()")
        elseif haskey(v, "anyOf") || haskey(v, "oneOf") || haskey(v, "allOf")
            typename_kv, default_kv, pending_kv = parse_composition(k, level, index, v)
            push!(od.types, typename_kv)
            push!(od.defaults, default_kv)
            append!(pending, pending_kv)
        elseif !haskey(v, "type")
            push!(od.types, "Any")
            push!(od.defaults, "nothing")
        elseif v["type"] in ("integer", "number", "string", "bool")
            push!(od.types, juliatype(v["type"]))
            push!(od.defaults, juliadefault(v["type"]))
        elseif v["type"] == "object"
            push!(od.types, typefromname(k))
            push!(od.defaults, typefromname(k) * "()")
            push!(pending, Pending(typefromname(k), level+1, index, Ref(d["properties"][k])))
        elseif v["type"] == "array"
            typename_kv, pending_kv = parse_array(k, level, index, v)
            push!(od.types, typename_kv)
            push!(od.defaults, typename_kv * "()")
            append!(pending, pending_kv)
        else
            error("unsupported type ", v["type"])
        end
    end
    # check required
    od.req = zeros(Bool, length(od.members))
    reqidx = indexin(d["required"], od.members)
    #@show d["required"]
    #@show od.members
    for i in reqidx
        !isnothing(i) || error("required field not defined")
        od.req[i] = true
    end
    return od, pending
end

function parse_composition(name::String, level::Int64, parent::Int64, d::Dict,
                           obj_cnt::Union{Nothing, Int64}=nothing)
    haskey(d, "anyOf") || haskey(d, "oneOf") || haskey(d, "allOf") ||
        error("composition must have key \"anyOf\", \"oneOf\", or \"allOf\"")
    length(d) == 1 || error("composition must be of only one type")
    haskey(d, "anyOf") || error("only \"anyOf\" supported (no inference for type intersection)")
    typeof(d["anyOf"]) <: AbstractVector || error("composition must contain a vector of possible types")
    if length(d["anyOf"]) == 0
        return "Any", "nothing", Pending[]
    else
        typename = "Union{"
        default_value = ""
        pending = Pending[]
        passed_obj_cnt = !isnothing(obj_cnt)
        if isnothing(obj_cnt)
            obj_cnt = 0
        end
        for i = 1:length(d["anyOf"])
            v = d["anyOf"][i]
            if haskey(v, "\$ref")
                length(v) == 1 || error("\$ref can only reference one definition.")
                v["\$ref"][1:8] == "#/\$defs/" ||
                    error("only references to subschemas currently supported.")
                typename *= typefromname(v["\$ref"][9:end])         # refs will be already handled when we get here
                if i == 1
                    default_value = typefromname(v["\$ref"][9:end]) * "()"
                end
            elseif haskey(v, "anyOf") || haskey(v, "oneOf") || haskey(v, "allOf")
                typename_v, default_v, pending_v, obj_cnt = parse_composition(name, level, parent, v, obj_cnt)
                typename *= typename_v
                if i == 1
                    default_value = default_v
                end
                append!(pending, pending_v)
            elseif !haskey(v, "type")
                # Union{stuff, Any, stuff} = Any; no need to continue
                typename = "Any"
                default_value = nothing
                pending = Pending[]
                break
            elseif v["type"] in ("integer", "number", "string", "bool")
                typename *= juliatype(v["type"])
                if i == 1
                    default_value = juliadefault(v["type"])
                end
            elseif v["type"] == "object"
                obj_cnt += 1
                object_name = typefromname(name) * "Subtype" * string(obj_cnt)
                typename *= object_name
                if i == 1
                    default_value = object_name * "()"
                end
                push!(pending, Pending(object_name, level+1, parent, Ref(d["anyOf"][i])))
            elseif v["type"] == "array"
                typename_v, pending_v, obj_cnt = parse_array(name, level, parent, v, obj_cnt)
                typename *= typename_v
                if i == 1
                    default_value = typename_v * "()"
                end
                append!(pending, pending_v)
            else
                error("unsupported type ", v["type"])
            end
            if i < length(d["anyOf"])
                typename *= ", "
            else
                typename *= "}"
            end
        end
        if passed_obj_cnt
            return typename, default_value, pending, obj_cnt
        else
            return typename, default_value, pending
        end
    end
end

function parse_array(name::String, level::Int64, parent::Int64, d::Dict,
                     obj_cnt::Union{Nothing, Int64}=nothing)
    haskey(d, "type") || error("array must at least containt key \"type\"")
    @assert d["type"] == "array"
    if length(d) == 1
        return "Vector{Any}", Pending[]
    end
    haskey(d, "items") &&
        (haskey(d["items"], "\$ref") ||
         haskey(d["items"], "oneOf") || haskey(d["items"], "allOf") || haskey(d["items"], "anyOf") ||
         haskey(d["items"], "type")) ||
        error("invalid schema for array")
    eltypename = nothing
    pending = nothing
    if haskey(d["items"], "\$ref")
        length(d["items"]) == 1 || error("\$ref can only reference one definition.")
        d["items"]["\$ref"][1:8] == "#/\$defs/" ||
            error("only references to subschemas currently supported.")
        eltypename = typefromname(d["items"]["\$ref"][9:end])
        pending = Pending[]
    elseif haskey(d["items"], "anyOf") || haskey(d["items"], "oneOf") || haskey(d["items"], "allOf")
        if isnothing(obj_cnt)
            eltypename, _, pending = parse_composition(name, level, parent, d["items"])
        else
            eltypename, _, pending, obj_cnt = parse_composition(name, level, parent, d["items"], obj_cnt)
        end
    elseif d["items"]["type"] in ("integer", "number", "string", "bool")
        eltypename = juliatype(d["items"]["type"])
        pending = Pending[]
    elseif d["items"]["type"] == "object"
        if isnothing(obj_cnt)
            eltypename = typefromname(name)
        else
            obj_cnt += 1
            eltypename = typefromname(name) * "Subtype" * string(obj_cnt)
        end
        pending = Pending[Pending(eltypename, level + 1, parent, Ref(d["items"]))]
    elseif d["items"]["type"] == "array"
        if isnothing(obj_cnt)
            eltypename, pending = parse_array(name, level, parent, d["items"])
        else
            eltypename, pending, obj_cnt = parse_array(name, level, parent, d["items"], obj_cnt)
        end
    else
        error("unsupported type ", d["items"]["type"])
    end
    typename = "Vector{" * eltypename * "}"
    if isnothing(obj_cnt)
        return typename, pending
    else
        return typename, pending, obj_cnt
    end
end

function repeated_indicator(v::Vector)::Vector{Bool}
    perm = sortperm(v)
    repeated = Vector{Bool}(undef, length(v))
    for i = 1:length(v)
        if 1 < i && i < length(v)
            repeated[perm[i]] = v[perm[i]] == v[perm[i-1]] ||
                                v[perm[i]] == v[perm[i+1]]
        elseif i == 1
            repeated[perm[i]] = v[perm[i]] == v[perm[i+1]]
        else
            repeated[perm[i]] = v[perm[i]] == v[perm[i-1]]
        end
    end
    return repeated
end

function remove_nested_key!(d::Dict, k_to_remove)
    if haskey(d, k_to_remove)
        delete!(d, k_to_remove)
    end
    for (k, v) in d
        if typeof(v) <: Dict
            remove_nested_key!(v, k_to_remove)
        elseif typeof(v) <: Array
            for x in v
                if typeof(x) <: Dict
                    remove_nested_key!(x, k_to_remove)
                end
            end
        elseif typeof(v) <: Tuple
            new_v = Any[]
            for x in v
                if typeof(x) <: Dict
                    x = remove_nested_key!(x, k_to_remove)
                end
                push!(new_v, x)
            end
            d[k] = new_v
        end
    end
    return d
end

function parse_schema(name::String, d::Dict)::Vector{Union{ObjectDefinition, AliasDefinition}}
    # remove all descriptions and defaults from schema dictionary (THIS IS A HACK!)
    remove_nested_key!(d, "description")
    remove_nested_key!(d, "default")
    # allocate objects to hold parsed and non-parsed objects
    all_pending = Pending[Pending(pascalcase(name), 1, 0, Ref(d))]
    all_parsed = Union{ObjectDefinition, AliasDefinition}[]
    # parse $defs
    if haskey(d, "\$defs")
        for (k, v) in d["\$defs"]
            parsed, pending = parse_def(pascalcase(name), k, v, length(all_parsed) + 1, d)
            push!(all_parsed, parsed)
            append!(all_pending, pending)
        end
    end
    # collect all structs prototypes
    while length(all_pending) > 0
        p = pop!(all_pending)
        parsed, pending = parse_object(p.name, p.level, p.parent, length(all_parsed) + 1, p.ref.x)
        if p.is_def
            parsed.is_def = true
            for p in pending
                p.is_def = true
            end
        end
        push!(all_parsed, parsed)
        append!(all_pending, pending)
    end
    # combine names as necessary to ensure uniqueness
    make_names_unique!(all_parsed)
    # return
    return all_parsed
end

function find_name_in(name::AbstractString, type_expr::AbstractString)
    if type_expr[(end-1):end] == "()"
        type_expr = chop(type_expr, head=0, tail=2)
    end
    start_pos = 1
    while start_pos <= length(type_expr)
        m = match(r"{|}|,| ", type_expr, start_pos)
        if isnothing(m)
            end_pos = length(type_expr)
        else
            end_pos = m.offset - 1
        end
        if name == type_expr[start_pos:end_pos]
            return start_pos
        end
        end_pos != length(type_expr) || break
        m = match(r"[A-z]", type_expr, end_pos + 1)
        !isnothing(m) || break
        start_pos = m.offset
    end
    return nothing
end

function find_name_in(name::AbstractString, v::Vector{<:AbstractString})
    for i = 1:length(v)
        start_pos = find_name_in(name, v[i])
        if !isnothing(start_pos)
            return i, start_pos
        end
    end
    return nothing, nothing
end

insert_at_pos(insert_at::AbstractString, insert_what::AbstractString, pos::Int) = 
    insert_at[1:(pos-1)] * insert_what * insert_at[pos:end]

function make_names_unique!(objdefs::Vector{Union{ObjectDefinition, AliasDefinition}})
    # find order to prepend (hashed) parent name to (hashed) type for uniqueness
    names = collect(objdefs[i].name for i=1:length(objdefs))
    rep = repeated_indicator(names)
    names_rep = names[rep]
    levels_rep = collect(objdefs[i].level for i=1:length(objdefs))[rep]
    perm = sortperm(collect(zip(names_rep, levels_rep)), by=last)
    preprend_indexes = ((1:length(objdefs))[rep])[perm]
    # prepend parent names
    for i in preprend_indexes
        objdef = objdefs[i]
        parent = objdefs[objdefs[i].parent]
        ix, start_pos = find_name_in(objdef.name, parent.types)
        @assert !isnothing(ix)
        parent.types[ix] = insert_at_pos(parent.types[ix], parent.name, start_pos)
        start_pos = find_name_in(objdef.name, parent.defaults[ix])
        @assert !isnothing(start_pos)
        parent.defaults[ix] = insert_at_pos(parent.defaults[ix], parent.name, start_pos)
        objdef.name = parent.name * objdef.name
    end
    @assert length(unique(collect(objdefs[i].name
                                  for i=1:length(objdefs)))) == length(objdefs)
end

function member_varname(member_name::String)
    if member_name in RESERVED_WORDS
        return "var\"" * member_name * "\""
    else
        return member_name
    end
end

function print_def_julia(io::IO, od::ObjectDefinition)
    println(io, "Base.@kwdef mutable struct " * od.name)
    @assert length(od.members) == length(od.types) &&
            length(od.types) == length(od.req)
    for i = 1:length(od.members)
        varname = member_varname(od.members[i])
        if od.req[i]
            println(io, "    " * varname * "::" * od.types[i] * "=" * od.defaults[i])
        else
            println(io, "    " * varname * "::Union{Missing, " * od.types[i] * "}=missing")
        end
    end
    println(io, "end")
end

function print_def_julia(io::IO, ad::AliasDefinition)
    println(io, ad.name * " = " * ad.type)
end

function get_print_order(structsv::Vector{Union{ObjectDefinition, AliasDefinition}})::Vector{Int}
    levels = collect(structsv[i].level for i=1:length(structsv))
    maxlevel = maximum(levels)
    for i = 1:length(structsv)
        structsv[i].is_def || continue
        levels[i] += maxlevel + 1
    end
    return sortperm(levels, rev=true)
end

function print_defs_julia(io::IO, module_name::String,
                          structsv::Vector{Union{ObjectDefinition, AliasDefinition}},
                          top_level_name::String)
    
    # find reverse order permutation for dependent declarations
    perm = get_print_order(structsv)
    
    # print autogeneration comment
    println(io, "# autogenerated module using JSONSchema2Struct (author: I. Aravena, aravenasolis1@llnl.gov)")
    println(io, "# timestamp: ", now(), "\n")
 
    # print module header
    println(io, "module " * module_name * "\n")
    println(io, "import JSON3, StructTypes\n")
    
    # print object definitions
    for i = 1:length(structsv)
        print_def_julia(io, structsv[perm[i]])
        print(io, "\n")
    end
    
    # print StructTypes classifications (to have an automated parser from JSON3)
    for i = 1:length(structsv)
        if typeof(structsv[perm[i]]) <: AliasDefinition
            continue
        end
        println(io, "StructTypes.StructType(::Type{" * structsv[perm[i]].name * "}) = StructTypes.Mutable()")
    end
    print(io, "\n")
    
    # print parser from files (based on JSON3)
    println(io, "function parse(fname::String)::" * top_level_name)
    println(io, "    obj = open(fname, \"r\") do f")
    println(io, "              JSON3.read(f, " * top_level_name * ")")
    println(io, "          end")
    println(io, "    return obj")
    println(io, "end\n")
    
    # function to remove null fields (would cause validation failure with some validators
    println(io, "function remove_null_fields(fname::String)::Nothing")
    println(io, "    run(pipeline(`grep -vwE '(: null)' \$fname`, stdout=fname * \"_tmp\"))")
    println(io, "    in_io = open(fname * \"_tmp\", \"r\")")
    println(io, "    out_io = open(fname, \"w\")")
    println(io, "    prev_ln = \"\"")
    println(io, "    for ln in eachline(in_io)")
    println(io, "        if prev_ln == \"\"        # first line")
    println(io, "            prev_ln = ln")
    println(io, "        elseif rstrip(prev_ln)[end] == ',' && lstrip(ln)[1] == '}'")
    println(io, "            prev_ln = rstrip(prev_ln)[1:(end-1)]    # remove dangling comma")
    println(io, "            println(out_io, prev_ln)")
    println(io, "            prev_ln = ln")
    println(io, "        else                    # nothing to do")
    println(io, "            println(out_io, prev_ln)")
    println(io, "            prev_ln = ln")
    println(io, "        end")
    println(io, "    end")
    println(io, "    print(out_io, prev_ln)")
    println(io, "    close(in_io)")
    println(io, "    close(out_io)")
    println(io, "    rm(fname * \"_tmp\")")
    println(io, "end\n")
    
    # print printer (also based on JSON3)
    println(io, "function write(fname::String, x, omit_nulls::Bool=true)")
    println(io, "    open(fname, \"w\") do f")
    println(io, "        JSON3.pretty(f, x)")
    println(io, "    end")
    println(io, "    if omit_nulls")
    println(io, "        remove_null_fields(fname)")
    println(io, "    end")
    println(io, "end\n")
    
    # print constructors for primitive types
    println(io, "Int64() = Int64(0)")
    println(io, "Float64() = Float64(0)")
    println(io, "String() = \"\"")
    println(io, "Bool() = false\n")
    println(io, "Any() = nothing\n")
    
    # close module
    print(io, "end # module")
    
end

function jsonschema_to_structs(schema_fname::String, module_name::String;
                               path_to_module::String="", top_level_name::String="Root",
                               log::Bool=true)

    log && print("Generating structs for JSON schema at ", schema_fname, "...")
    
    # read schema
    schema = open(schema_fname) do f
               JSON.parse(f)
             end
    
    # collect all necessary structs
    obj_definitions = parse_schema(top_level_name, schema)
    
    # write module with all necessary structs for JSON3 reading
    open(joinpath(path_to_module, module_name * ".jl"), "w") do f
        print_defs_julia(f, module_name, obj_definitions, top_level_name)
    end
    
    log && println(" done. Julia structs written at ", joinpath(path_to_module, module_name * ".jl"))
        
end

end # module JSONSchema2Struct
