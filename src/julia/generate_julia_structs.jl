using Pkg; Pkg.activate("JSONSchema2Struct"); Pkg.instantiate()
using JSONSchema2Struct

jsonschema_to_structs("../../json_schemas/ctm_data_schema.json", "CTMData")
jsonschema_to_structs("../../json_schemas/ctm_time_series_schema.json", "CTMTimeSeries")
jsonschema_to_structs("../../json_schemas/ctm_solution_schema.json", "CTMSolution")
