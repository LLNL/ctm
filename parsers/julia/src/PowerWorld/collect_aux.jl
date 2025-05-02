function make_index_map(v::Vector{AUXTable})
    out = Dict{String, Vector{Int}}()
    for i = 1:length(v)
        if haskey(out, v[i].object_type)
            push!(out[v[i].object_type], i)
        else
            out[v[i].object_type] = Int[i]
        end
    end
    return out
end

all_elements_in(a::Vector, b::Vector) = all(x -> x in b, a)

function first_table_with_names(v::AbstractVector{AUXTable}, colnames::Vector{String})
    idx = findfirst(x -> all_elements_in(colnames, names(x.data)), v)
    if isnothing(idx)
        return nothing, nothing
    else
        return idx, deepcopy(v[idx].data)
    end
end

function get_with_missing(v::AbstractVector, indexes::AbstractVector)
    if any(isnothing.(indexes))
        return [if isnothing(e) missing else v[e] end for e in indexes]
    else
        return v[indexes]
    end
end

function collect_data_from_tables!(aux_tables::Vector{AUXTable},
                                   idx_map::Dict{String, Vector{Int}},
                                   data_name::String,
                                   primary_table_keys::Vector{String},
                                   object_id_fields::Union{Nothing, Vector{String}})
    
    # verify input
    length(aux_tables) != 0 || error("empty aux_tables")
    length(idx_map) != 0 || error("empty idx_map")
    length(primary_table_keys) != 0 || error("empty primary_table_keys")
    if !isnothing(object_id_fields)
        all_elements_in(object_id_fields, primary_table_keys) ||
            error("object_id_fields must be a subset of the primary keys")
    end
    
    # if key is not in idx_map, return empty DataFrame here
    haskey(idx_map, data_name) || return DataFrame()
    
    # collect relevant tables
    data_tables = view(aux_tables, idx_map[data_name])
    
    # get primary table
    primary_idx, primary_aux_table = first_table_with_names(data_tables, primary_table_keys)
    if isnothing(primary_idx)
        return DataFrame()
    end
    
    # identify and gather data from secondary tables
    secondary_idx = Int[]
    secondary_rm = Bool[]
    if !isnothing(object_id_fields)
        object_id = repeat([data_name], nrow(primary_aux_table))
        for oid in object_id_fields
            f = primary_aux_table[!, Symbol(oid)]
            if typeof(f[1]) <: Integer
                object_id .*= " '" .* string.(f) .* "'"
            else
                object_id .*= " '" .* strip.(f) .* "'"
            end
        end
        for i = 1:length(data_tables)
            # check if this is a secondary table
            i != primary_idx || continue
            oid_field_idx = findfirst(x -> x == :ObjectID, propertynames(data_tables[i].data))
            !isnothing(oid_field_idx) || continue
            push!(secondary_idx, i)
            # collect data
            pri_sec_idx = indexin(object_id, data_tables[i].data[!, :ObjectID])
            for j = 1:ncol(data_tables[i].data)
                jsymb = propertynames(data_tables[i].data)[j]
                if jsymb == :SUBDATA && :SUBDATA in propertynames(primary_aux_table)
                    # 'SUBDATA' must be appended (or key replaced) to existing 'SUBDATA' field
                    for k = 1:length(pri_sec_idx)
                        !isnothing(pri_sec_idx[k]) || continue
                        !isassigned(data_tables[i].data[!, :SUBDATA], pri_sec_idx[k]) || continue
                        if !isassigned(primary_aux_table[!, :SUBDATA], k)
                            primary_aux_table[k, :SUBDATA] =
                                data_tables[i].data[pri_sec_idx[k], :SUBDATA]
                        else
                            for (key, value) in data_tables[i].data[pri_sec_idx[k], :SUBDATA]
                                primary_aux_table[k, :SUBDATA][key] = value
                            end
                        end
                    end
                else
                    # for other types it is correct to override
                    primary_aux_table[!, jsymb] =
                        get_with_missing(data_tables[i].data[!, jsymb], pri_sec_idx)
                end
            end
            # remove collected data
            used_rows = unique(sort([e for e in pri_sec_idx if !isnothing(e)]))
            if length(used_rows) == nrow(data_tables[i].data)
                # if all rows have been removed, mark the table for deletion
                push!(secondary_rm, true)
            else
                # only partial rows used; remove rows and mark table to be kept
                delete!(data_tables[i].data, used_rows)
                push!(secondary_rm, false)
            end
        end
    end
    
    # empty used tables
    aux_tables[idx_map[data_name][primary_idx]].data = DataFrame()
    for i = 1:length(secondary_idx)
        if secondary_rm[i]
            aux_tables[idx_map[data_name][secondary_idx[i]]].data = DataFrame()
        end
    end
    
    # return constructed data 
    return primary_aux_table
    
end
