# function to read RAW file

function read_raw(filename::AbstractString)

	# find starting and ending point of each data section
	secstarts, secends = sections(filename, true)
	
	# get MVA base
	f = open(filename, "r")
    l = next_non_comment(f)
	close(f)
	MVAbase = parse(Float64, split(l, ',')[2]) 
	
	# read bus data
    print("Reading buses ... ")
	buses = CSV.read(filename, DataFrame,
		header=[:I,:NAME,:BASKV,:IDE,:AREA,:ZONE,:OWNER,:VM,:VA,:NVHI,:NVLO,:EVHI,:EVLO],
		skipto=secstarts[2], limit=secends[2]-secstarts[2]+1, delim=',',
		types=Dict(1=>Int, 4=>Int, 5=>Int, 8=>Float64, 9=>Float64, 10=>Float64, 11=>Float64,
			12=>Float64, 13=>Float64), ntasks=1, comment=RAWCOMMENT, quotechar='\'')
    println("done. Read ", size(buses, 1), " buses.")
	
	# read load data
	if secends[3] >= secstarts[3]
        print("Reading loads ... ")
		loads = CSV.read(filename, DataFrame,
			header=[:I,:ID,:STATUS,:AREA,:ZONE,:PL,:QL,:IP,:IQ,:YP,:YQ,:OWNER,:SCALE,:INTRPT],
			skipto=secstarts[3], limit=secends[3]-secstarts[3]+1, delim=',',
			types=Dict(1=>Int, 2=>String, 3=>Int, 6=>Float64, 7=>Float64), ntasks=1,
            comment=RAWCOMMENT)
		loads[!,:ID] = clean_id.(loads[!,:ID])
        println("done. Read ", size(loads, 1), " buses.")
	else
		loads = DataFrame()
	end
	
	# read fixed bus shunt data
	if secends[4] >= secstarts[4]
        print("Reading shunts ... ")
		fixedbusshunts = CSV.read(filename, DataFrame,
			header=[:I,:ID,:STATUS,:GL,:BL],
			skipto=secstarts[4], limit=secends[4]-secstarts[4]+1, delim=',',
			types=Dict(1=>Int, 2=>String, 3=>Int, 4=>Float64, 5=>Float64), ntasks=1,
            comment=RAWCOMMENT)
		fixedbusshunts[!,:ID] = clean_id.(fixedbusshunts[!,:ID])
        println("done. Read ", size(fixedbusshunts, 1), " shunts.")
	else
		fixedbusshunts = DataFrame()
	end
	
	# generator data
    print("Reading generators ... ")
	generators = CSV.read(filename, DataFrame,
		header=[:I,:ID,:PG,:QG,:QT,:QB,:VS,:IREG,:MBASE,:ZR,:ZX,:RT,:XT,:GTAP,:STAT,:RMPCT,
			:PT,:PB,:O1,:F1,:O2,:F2,:O3,:F3,:O4,:F4,:WMOD,:WPF],
		skipto=secstarts[5], limit=secends[5]-secstarts[5]+1, delim=',',
		types=Dict(1=>Int, 2=>String, 3=>Float64, 4=>Float64, 5=>Float64, 6=>Float64, 15=>Int,
			17=>Float64, 18=>Float64), ntasks=1, comment=RAWCOMMENT)
	generators[!,:ID] = clean_id.(generators[!,:ID])
    println("done. Read ", size(generators, 1), " generators.")

	# non-transformer branch data
	if secends[6] >= secstarts[6]
        print("Reading branches ... ")
		ntbranches = CSV.read(filename, DataFrame,
			header=[:I,:J,:CKT,:R,:X,:B,:RATEA,:RATEB,:RATEC,:GI,:BI,:GJ,:BJ,:ST,:MET,:LEN,
				:O1,:F1,:O2,:F2,:O3,:F3,:O4,:F4],
			skipto=secstarts[6], limit=secends[6]-secstarts[6]+1, delim=',',
			types=Dict(1=>Int, 2=>Int, 3=>String, 4=>Float64, 5=>Float64, 6=>Float64,
				7=>Float64, 9=>Float64, 14=>Int), ntasks=1, comment=RAWCOMMENT)
		ntbranches[!,:CKT] = clean_id.(ntbranches[!,:CKT])
        println("done. Read ", size(ntbranches, 1), " branches.")
	else
		ntbranches = DataFrame()
	end
	
	# transformer data
	if secends[7] >= secstarts[7]
        print("Reading transformers ... ")
		tbranches = read_transformer_data(filename, secstarts[7], secends[7])
        println("done. Read ", size(tbranches, 1), " transformers.")
	else
		tbranches = DataFrame()
	end
	
	# switched shunt data
	if secends[18] >= secstarts[18]
        print("Reading switched shunts ... ")
		switchedshunts = CSV.read(filename, DataFrame,
			header=[:I,:MODSW,:ADJM,:STAT,:VSWHI,:VSWLO,:SWREM,:RMPCT,:RMIDNT,:BINIT,
				:N1,:B1,:N2,:B2,:N3,:B3,:N4,:B4,:N5,:B5,:N6,:B6,:N7,:B7,:N8,:B8],
			skipto=secstarts[18], limit=secends[18]-secstarts[18]+1, delim=',',
			types=Dict(1=>Int, 4=>Int, 10=>Float64, 11=>Float64, 12=>Float64, 13=>Float64,
				14=>Float64, 15=>Float64, 16=>Float64, 17=>Float64, 18=>Float64,
				19=>Float64, 20=>Float64, 21=>Float64, 22=>Float64, 23=>Float64,
				24=>Float64, 25=>Float64, 26=>Float64), ntasks=1, comment=RAWCOMMENT,
                maxwarnings=0)
        println("done. Read ", size(switchedshunts, 1), " switched shunts.")
	else
		switchedshunts = DataFrame()
	end
	
	# return data frames with RAW data
	return MVAbase, buses, loads, fixedbusshunts, generators, ntbranches, tbranches, switchedshunts

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
			append!(row, strip.(split(l, ',')))
		end
        try
            rawinfo[k,:] = row
        catch e
            @show k
            @show i
            @show row
            throw(e)
        end
		k += 1
		i += 4
	end
	close(f)
	
	# form a data frame with the collected data and return
	transformers = DataFrame(rawinfo, [:I,:J,:K,:CKT,:CW,:CZ,:CM,:MAG1,:MAG2,
		:NMETR,:NAME,:STAT,:O1,:F1,:O2,:F2,:O3,:F3,:O4,:F4,:VECGRP,:R12,:X12,
		:SBASE12,:WINDV1,:NOMV1,:ANG1,:RATA1,:RATB1,:RATC1,:COD1,:CONT1,:RMA1,
		:RMI1,:VMA1,:VMI1,:NTP1,:TAB1,:CR1,:CX1,:CNXA1,:WINDV2,:NOMV2])
	colnames = names(transformers)
	intcols = [1:2;12]
	floatcols = [8:9;22:23;25;27:28;30;42]
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
