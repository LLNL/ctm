## function to read ROP file

function read_rop(filename::AbstractString)
	
    # if file is empty, return here
    if read(filename, String) == ""
        return DataFrame(), DataFrame(), DataFrame()
    end
	
    # find starting and ending point of each data section
	secstarts, secends, headers = sections(filename, false, true)
    
	# generator dispatch data
	idx = getnextsectionidx(1, secstarts, secends, headers, "Generator Dispatch")
	if secends[idx] >= secstarts[idx]
		generatordsp = CSV.read(filename, DataFrame,
			header=[:BUS,:GENID,:DISP,:DSPTBL],
			skipto=secstarts[idx], limit=secends[idx]-secstarts[idx]+1,
			quotechar='\'', types=Dict(1=>Int, 2=>String, 4=>Int), ntasks=1)
		generatordsp[!,:GENID] = clean_id.(generatordsp[!,:GENID])
	else
		generatordsp = DataFrame()
	end
	
	# active power dispatch tables
	idx = getnextsectionidx(idx, secstarts, secends, headers, "Active Power Dispatch")
	if secends[idx] >= secstarts[idx]
		activedsptables = CSV.read(filename, DataFrame,
			header=[:TBL,:PMAX,:PMIN,:FUELCOST,:CTYP,:STATUS,:CTBL],
			skipto=secstarts[idx], limit=secends[idx]-secstarts[idx]+1,
			quotechar='\'', types=Dict(1=>Int, 7=>Int), ntasks=1)
	else
		activedsptables = DataFrame()
	end
	
	# piecewise linear cost curves
	idx = getnextsectionidx(idx, secstarts, secends, headers, "Piece-wise Linear Cost")
	if secends[idx] >= secstarts[idx]
		costcurves = read_cost_curves(filename, secstarts[idx], secends[idx])
	else
		costcurves = DataFrame()
	end
	
	# return data frames with ROP data
	return generatordsp, activedsptables, costcurves 

end

# function to read piecewise linear cost functions

function read_cost_curves(filename::AbstractString, startline::Int, endline::Int)::DataFrame

	# collect cost curve information, row-by-row
	lbtl = Int[]
	label = String[]
	npairs = Int[]
	xi = Vector{Vector{Float64}}()
	yi = Vector{Vector{Float64}}()
	f = open(filename, "r")
	for i = 1:(startline-1)
		readline(f)
	end
	i = startline
	while i <= endline
		l = readline(f)
		header = strip.(split(l, ','))
		if length(header) != 3
			error("cost curve should start with a 3 field line, got: ", l)
		end
		push!(lbtl, parse(Int, header[1]))
		push!(label, replace(header[2], "'" => ""))
		push!(npairs, parse(Int, header[3]))
		x = Float64[]
		y = Float64[]
		for j = 1:npairs[end]
			xy = parse.(Float64, strip.(split(readline(f), ',')))
			if j == 1 || xy[1] > x[end]
				push!(x, xy[1])
				push!(y, xy[2])
			end
		end
		push!(xi, x)
		push!(yi, y)
		i += 1 + npairs[end]
	end
	close(f)
	
	# place cost curve information in a data frame and return
	costcurves = DataFrame([lbtl, label, npairs, xi, yi], [:LTBL,:LABEL,:NPAIRS,:Xi,:Yi])
	return costcurves
	
end
