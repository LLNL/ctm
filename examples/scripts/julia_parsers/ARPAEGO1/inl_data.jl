# INL file contains a single section describing governor response
# (generator contingency re-dispatch factor)

struct INLData
    
    # members
    generator_redispatch::DataFrame
    
    # constructor from file
    INLData(filename::AbstractString) = new(read_inl(filename))
    
end

## function to read INL file

function read_inl(filename::AbstractString)::DataFrame
    
    # start time counter
    t_start = time_ns()
    print("Reading INLData from ", filename, " ...")
    
    # find starting and ending point of each data section (unique in this case)
    secstarts, secends = sections(filename, false)

    # read governor response data
    if secends[1] >= secstarts[1]
        generator_redispatch = CSV.read(filename, DataFrame,
                                        header=[:I,:ID,:H,:PMAX,:PMIN,:R,:D],
                                        skipto=secstarts[1], limit=secends[1]-secstarts[1]+1,
                                        types=Dict(1=>Int, 2=>String, 6=>Float64), ntasks=1)
        generator_redispatch[!,:ID] = clean_id.(generator_redispatch[!,:ID])
    else
        generator_redispatch = DataFrame()
    end
    
    # report reading time
    t_end = time_ns()
    t_elapsed = (t_end - t_start)/1.0E9
    println(" done. INLData read in ", round(t_elapsed, digits=3), " secs.")
    
    # return governor response data
    return generator_redispatch
end
