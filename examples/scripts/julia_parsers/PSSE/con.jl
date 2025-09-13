## function to read CON file
# CON file contains a single section describing all contingencies that can occur in the system
# file is read line-by-line and the result is returned as a data frame

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

function read_con(filename::AbstractString)::DataFrame
	
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
		if info[1] == "REMOVE"		# generator contingency
			push!(labels, conname)
			push!(ctypes, :Generator)
			push!(cons, GeneratorContingency(parse(Int, info[6]), info[3]))
		elseif info[1] == "OPEN"	# branch contingency
			push!(labels, conname)
			push!(ctypes, :Branch)
			push!(cons, TransmissionContingency(parse(Int, info[5]),
				parse(Int, info[8]), strip(info[10])))
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
	
	# put contingency data in data frame and return
	contingencies = DataFrame([labels, ctypes, cons], [:LABEL, :CTYPE, :CON])
	return contingencies
	
end
