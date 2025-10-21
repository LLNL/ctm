#!/bin/sh

# requires quicktype: https://github.com/glideapps/quicktype

quicktype --lang c++ --src-lang schema --code-format with-struct --no-boost --namespace ctm_schemas --out ctm_schemas.hpp ../../json_schemas/ctm_data_schema.json ../../json_schemas/ctm_solution_schema.json ../../json_schemas/ctm_time_series_schema.json
