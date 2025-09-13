# function to strip IDs of extra stuff

clean_id(str::AbstractString) = strip(replace(str, "'" => ""))

# function to detect starts and ends points of sections

RAWCOMMENT = "@!"

function iscomment(l::AbstractString)::Bool
    return startswith(l, RAWCOMMENT)
end

function next_non_comment(f)
    l = readline(f)
    while iscomment(l)
        l = readline(f)
    end
    return l
end

function isheadortail(l::AbstractString)::Bool
	if length(l) == 0
		return false
	elseif l[1] != '0' && l[1] != ' '
		return false
	elseif length(l) == 1 && l[1] == '0'
		return true
	elseif length(l) >= 2 && l[1:2] == "0 "
		return true
	elseif length(l) >= 3 && l[1:3] == " 0 "	# noncompliant but present in some ROP files
		return true
	else
		return false
	end
end

function sections(filename::AbstractString, casedata::Bool=true, returnheaders::Bool=false)
	f = open(filename, "r")
	sectionstarts = Int[]
	sectionends = Int[]
    l = readline(f)
	if casedata
		append!(sectionstarts, [1 + iscomment(l), 4])
		push!(sectionends, 3 + iscomment(l))
	else
		push!(sectionstarts, 1 + iscomment(l))
	end
	if returnheaders
		headers = String[]
		for i = 1:length(sectionstarts)
			push!(headers, "")
		end
	end
	nlines = 1
	while !eof(f)
        l = readline(f)
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

# function to get index of next non-empty subsection

function getnextsectionidx(currentidx::Int, sectionstarts::AbstractVector{Int},
	sectionends::AbstractVector{Int}, headers::Union{AbstractVector{String}, Nothing}=nothing,
	keyword::Union{String, Nothing}=nothing)::Union{Int, Nothing}
    simplify_str(str::String) = lowercase(replace(str, " " => ""))
	if headers != nothing
		keyword = simplify_str(keyword)
	end
	for idx = (currentidx+1):length(sectionstarts)
		if sectionends[idx] >= sectionstarts[idx] &&
			(headers == nothing || occursin(keyword, simplify_str(headers[idx])))
			return idx
		end
	end
	return nothing
end
