#!/bin/sh

# NOTE: must install JSON Schema for Humans from https://github.com/coveooss/json-schema-for-humans/ for this to run
#source ~/.local/python_environments/for_ctm/bin/activate

generate-schema-doc --config template_name=js_offline --config no_link_to_reused_ref ../json_schemas/ctm_data_schema.json ./ctm_data.html
generate-schema-doc --config template_name=js_offline --config no_link_to_reused_ref ../json_schemas/ctm_solution_schema.json ./ctm_solution.html
generate-schema-doc --config template_name=js_offline --config no_link_to_reused_ref ../json_schemas/ctm_time_series_schema.json ./ctm_time_series.html

#deactivate
