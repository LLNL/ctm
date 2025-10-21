#!/bin/sh

# NOTE: must install datamodel-code-generator from https://github.com/koxudaxi/datamodel-code-generator for this to run
#source ~/.local/python_environments/for_ctm/bin/activate

echo "CTM Data Schema"
datamodel-codegen --collapse-root-models --disable-appending-item-suffix --input ../../json_schemas/ctm_data_schema.json --input-file-type jsonschema --output src/ctm/ctmdata.py --output-model-type pydantic_v2.BaseModel
echo "CTM Solution Schema"
datamodel-codegen --collapse-root-models --disable-appending-item-suffix --input ../../json_schemas/ctm_solution_schema.json --input-file-type jsonschema --output src/ctm/ctmsolution.py --output-model-type pydantic_v2.BaseModel
echo "CTM Time Series Schema"
datamodel-codegen --collapse-root-models --disable-appending-item-suffix --input ../../json_schemas/ctm_time_series_schema.json --input-file-type jsonschema --output src/ctm/ctmtimeseries.py --output-model-type pydantic_v2.BaseModel
#deactivate

for i in "ctmdata.py CtmData" "ctmsolution.py CtmSolution" "ctmtimeseries.py CtmTimeSeriesData"
do
  F_M=( $i )
  F=${F_M[0]}
  M=${F_M[1]}
  echo "" >> $F
  echo "from pydantic.tools import parse_obj_as" >> $F
  echo "import json" >> $F
  echo "" >> $F
  echo "def parse(filename):" >> $F
  echo "    f = open(filename, 'r')" >> $F
  echo "    json_dict = json.load(f)" >> $F
  echo "    f.close()" >> $F
  echo "    return parse_obj_as($M, json_dict)" >> $F
  echo "" >> $F
  echo "def dump(instance, filename):" >> $F
  echo "    f = open(filename, 'w')" >> $F
  echo "    f.write(instance.model_dump_json(indent=4, exclude_unset=True, exclude_none=True))" >> $F
  echo "    f.close()" >> $F
  echo "" >> $F
done
