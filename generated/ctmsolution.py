# generated by datamodel-codegen:
#   filename:  ctm_solution_schema.json
#   timestamp: 2024-02-17T17:48:34+00:00

from __future__ import annotations

from enum import Enum
from typing import Any, List, Optional, Union

from pydantic import (
    BaseModel,
    Field,
    PositiveFloat,
    PositiveInt,
    RootModel,
    confloat,
    conint,
)


class UnitConvention(Enum):
    NATURAL_UNITS = 'NATURAL_UNITS'
    PER_UNIT_COMPONENT_BASE = 'PER_UNIT_COMPONENT_BASE'
    PER_UNIT_SYSTEM_BASE = 'PER_UNIT_SYSTEM_BASE'


class PositiveInteger(RootModel[PositiveInt]):
    root: PositiveInt


class TimeSeriesReference(BaseModel):
    uid: Union[conint(ge=0), str]
    scale_factor: float


class Global(BaseModel):
    unit_convention: UnitConvention
    base_mva: Optional[PositiveFloat] = None


class Bu(BaseModel):
    uid: Union[conint(ge=0), str]
    vm: Optional[Union[TimeSeriesReference, PositiveFloat]] = None
    va: Union[float, TimeSeriesReference]
    p_imbalance: Optional[Union[float, TimeSeriesReference]] = None
    q_imbalance: Optional[Union[float, TimeSeriesReference]] = None
    p_lambda: Optional[Union[float, TimeSeriesReference]] = None
    q_lambda: Optional[Union[float, TimeSeriesReference]] = None
    source_uid: Optional[Any] = None


class Shunt(BaseModel):
    uid: Union[conint(ge=0), str]
    num_steps: Union[List[conint(ge=0)], TimeSeriesReference, conint(ge=0)]
    source_uid: Optional[Any] = None


class Gen(BaseModel):
    uid: Union[conint(ge=0), str]
    pg: Union[float, TimeSeriesReference]
    qg: Optional[Union[float, TimeSeriesReference]] = None
    in_service: Optional[Union[TimeSeriesReference, conint(ge=0, le=1)]] = None
    source_uid: Optional[Any] = None


class Storage(BaseModel):
    uid: Union[conint(ge=0), str]
    ps: Union[float, TimeSeriesReference]
    qs: Optional[Union[float, TimeSeriesReference]] = None
    energy: Union[TimeSeriesReference, confloat(ge=0.0)]
    charge: Optional[Union[TimeSeriesReference, confloat(ge=0.0)]] = None
    discharge: Optional[Union[TimeSeriesReference, confloat(ge=0.0)]] = None
    source_uid: Optional[Any] = None


class Switch(BaseModel):
    uid: Union[conint(ge=0), str]
    psw_fr: Optional[Union[float, TimeSeriesReference]] = None
    qsw_fr: Optional[Union[float, TimeSeriesReference]] = None
    state: Union[TimeSeriesReference, conint(ge=0, le=1)]
    source_uid: Optional[Any] = None


class AcLine(BaseModel):
    uid: Union[conint(ge=0), str]
    pl_fr: Optional[Union[float, TimeSeriesReference]] = None
    ql_fr: Optional[Union[float, TimeSeriesReference]] = None
    pl_to: Optional[Union[float, TimeSeriesReference]] = None
    ql_to: Optional[Union[float, TimeSeriesReference]] = None
    source_uid: Optional[Any] = None


class Transformer(BaseModel):
    uid: Union[conint(ge=0), str]
    tm: Optional[Union[float, TimeSeriesReference]] = None
    ta: Optional[Union[float, TimeSeriesReference]] = None
    pt_fr: Optional[Union[float, TimeSeriesReference]] = None
    qt_fr: Optional[Union[float, TimeSeriesReference]] = None
    pt_to: Optional[Union[float, TimeSeriesReference]] = None
    qt_to: Optional[Union[float, TimeSeriesReference]] = None
    source_uid: Optional[Any] = None


class HvdcP2p(BaseModel):
    uid: Union[conint(ge=0), str]
    vm_dc: Optional[Union[TimeSeriesReference, PositiveFloat]] = None
    pdc_fr: Union[float, TimeSeriesReference]
    qdc_fr: Optional[Union[float, TimeSeriesReference]] = None
    pdc_to: Union[float, TimeSeriesReference]
    qdc_to: Optional[Union[float, TimeSeriesReference]] = None
    source_uid: Optional[Any] = None


class Solution(BaseModel):
    global_: Global = Field(..., alias='global')
    bus: List[Bu]
    shunt: Optional[List[Shunt]] = None
    gen: List[Gen]
    storage: Optional[List[Storage]] = None
    switch: Optional[List[Switch]] = None
    ac_line: Optional[List[AcLine]] = None
    transformer: Optional[List[Transformer]] = None
    hvdc_p2p: List[HvdcP2p]


class TimeSeriesData(BaseModel):
    uid: List[Union[conint(ge=0), str]]
    name: Optional[List[str]] = None
    source_uid: Optional[List] = None
    path_to_file: Optional[Union[str, List[str]]] = None
    timestamp: Optional[List[conint(ge=0)]] = None
    values: Optional[List[List]] = None


class Model(BaseModel):
    solution: Solution
    time_series_data: Optional[TimeSeriesData] = None

from pydantic.tools import parse_obj_as
import json

def parse(filename):
    f = open(filename)
    json_dict = json.load(f)
    f.close()
    return parse_obj_as(Model, json_dict)
