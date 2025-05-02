## function to strip IDs of extra stuff

clean_id(str::AbstractString) = strip(replace(str, "'" => ""))

## function to detect starts and ends points of sections

function isheadortail(l::AbstractString)::Bool
    if length(l) == 0
        return false
    elseif l[1] != '0' && l[1] != ' '
        return false
    elseif length(l) == 1 && l[1] == '0'
        return true
    elseif length(l) >= 2 && l[1:2] == "0 "
        return true
    elseif length(l) >= 3 && l[1:3] == " 0 "    # noncompliant but present in some ROP files
        return true
    else
        return false
    end
end

function sections(filename::AbstractString, casedata::Bool=true, returnheaders::Bool=false)
    sectionstarts = Int[]
    sectionends = Int[]
    if casedata
        append!(sectionstarts, [1,4])
        push!(sectionends, 3)
    else
        push!(sectionstarts, 1)
    end
    if returnheaders
        headers = String[]
        for i = 1:length(sectionstarts)
            push!(headers, "")
        end
    end
    f = open(filename, "r")
    nlines = 0
    for l in eachline(f)
        nlines += 1
        if length(l) == 0
            continue
        end
        if isheadortail(l)
            push!(sectionends, nlines-1)
            push!(sectionstarts, nlines+1)
            if returnheaders
                push!(headers, l)
            end
        elseif l[1] == 'Q' && length(l) == 1
            push!(sectionends, nlines-1)
        end
    end
    close(f)
    if length(sectionends) < length(sectionstarts)
        push!(sectionends, nlines)
    end
    if returnheaders
        return sectionstarts, sectionends, headers
    else
        return sectionstarts, sectionends
    end
end

## function to get index of next non-empty subsection

simplifystr(str::String) = lowercase(replace(str, " " => ""))

function getnextsectionidx(currentidx::Int, sectionstarts::AbstractVector{Int},
    sectionends::AbstractVector{Int}, headers::Union{AbstractVector{String}, Nothing}=nothing,
    keyword::Union{String, Nothing}=nothing)::Union{Int, Nothing}
    if headers != nothing
        keyword = simplifystr(keyword)
    end
    for idx = (currentidx+1):length(sectionstarts)
        if sectionends[idx] >= sectionstarts[idx] &&
            (headers == nothing || occursin(keyword, simplifystr(headers[idx])))
            return idx
        end
    end
    return nothing
end

## function to get RAW, ROP, INL, and CON filenames from directory

function instance_filenames(instancedir::String, maxnup::Int)
        extensions = ["raw", "rop", "inl", "con"]
        exfiles = String[]
        for ex in extensions
                nup = 0
                exfile = ""
                fdir = instancedir
                while nup < maxnup
                        exfile = joinpath(fdir, "case."*ex)
                        isfile(exfile) && break
                        nup += 1
                        fdir *= "/.."
                end
                nup == maxnup && error("case.", ex, " not found")
                push!(exfiles, exfile)
        end
        return tuple(exfiles...)
end
