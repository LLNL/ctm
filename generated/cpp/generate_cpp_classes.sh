#!/bin/sh

# requires quicktype: https://github.com/glideapps/quicktype

quicktype --lang c++ --src-lang schema --code-format with-struct --no-boost --out ctm_data.hpp ../../json_schemas/ctm_data_schema.json
quicktype --lang c++ --src-lang schema --code-format with-struct --no-boost --out ctm_solution.hpp ../../json_schemas/ctm_solution_schema.json
quicktype --lang c++ --src-lang schema --code-format with-struct --no-boost --out ctm_time_series.hpp ../../json_schemas/ctm_time_series_schema.json
