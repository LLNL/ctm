# generated by datamodel-codegen:
#   filename:  ctm_time_series_schema.json
#   timestamp: 2024-04-10T06:08:25+00:00

from __future__ import annotations

from typing import List, Optional, Union

from pydantic import BaseModel, Field, confloat, conint


class TimeSeriesData(BaseModel):
    timestamp: Optional[List[confloat(ge=0.0)]] = Field(
        None,
        description='[seconds] seconds since epoch (Unix time) for each instant for which time series values are provided',
    )
    uid: List[Union[conint(ge=0), str]] = Field(
        ..., description='array of uids of time series'
    )
    name: Optional[List[str]] = Field(None, description='array of names of time series')
    path_to_file: Optional[Union[str, List[str]]] = Field(
        None,
        description='path to file containing all time series information or a separate path for each time series',
    )
    values: Optional[List[List]] = Field(
        None, description='array of time series values'
    )
    ext: Optional[List] = Field(
        None,
        description='additional time series information not currently supported by CTM',
    )


class CommonTransmissionModelCtmTimeSeriesDataSchema(BaseModel):
    time_series_data: TimeSeriesData = Field(
        ...,
        description='structure to contain all time variant data of the system/case. All time series are synchronized to the same timestamps, which should should be stored using Unix time. Structure is quasi-tabular, with uid, name, path_to_file, values, and ext being arrays in the same order of said field. This is done in order to allow for better compression (e.g., using HDF5) for the values field.',
    )

from pydantic.tools import parse_obj_as
import json

def parse(filename):
    f = open(filename)
    json_dict = json.load(f)
    f.close()
    return parse_obj_as(Model, json_dict)

