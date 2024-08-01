# generated by datamodel-codegen:
#   filename:  ctm_solution_schema.json
#   timestamp: 2024-08-01T07:51:47+00:00

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
    root: PositiveInt = Field(..., description='positive integer number')


class TimeSeriesReference(BaseModel):
    uid: Union[conint(ge=0), str] = Field(
        ...,
        description='uid of time series (in time_series_data) this reference points to',
    )
    scale_factor: float = Field(
        ...,
        description="[-] scale factor to be applied to the pointed-to time series to obtain this field's values",
    )


class GlobalParams(BaseModel):
    unit_convention: UnitConvention = Field(
        ..., description='units used for physical network parameters'
    )
    base_mva: Optional[PositiveFloat] = Field(
        100.0, description='[MVA] system-wide apparent power base'
    )


class Bu(BaseModel):
    uid: Union[conint(ge=0), str] = Field(..., description="object's Unique IDentifier")
    vm: Optional[Union[TimeSeriesReference, PositiveFloat]] = Field(
        None, description='[kV or pu] voltage magnitude'
    )
    va: Union[float, TimeSeriesReference] = Field(
        ..., description='[deg] voltage magnitude'
    )
    p_imbalance: Optional[Union[float, TimeSeriesReference]] = Field(
        None,
        description='[MW or pu] signed power imbalance; positive indicates active load loss',
    )
    q_imbalance: Optional[Union[float, TimeSeriesReference]] = Field(
        None,
        description='[MVAr or pu] signed power imbalance; positive indicates reactive load loss',
    )
    p_lambda: Optional[Union[float, TimeSeriesReference]] = Field(
        None, description='[$/MW or $/pu] dual of active power balance constraints'
    )
    q_lambda: Optional[Union[float, TimeSeriesReference]] = Field(
        None, description='[$/MVAr or $/pu] dual of reactive power balance constraints'
    )
    ext: Optional[Any] = Field(
        None, description='additional bus parameters currently not supported by CTM'
    )


class Shunt(BaseModel):
    uid: Union[conint(ge=0), str] = Field(..., description="object's Unique IDentifier")
    num_steps: Union[List[conint(ge=0)], TimeSeriesReference, conint(ge=0)] = Field(
        ...,
        description='number of energized steps of shunt section (lower bound is always 0)',
    )
    ext: Optional[Any] = Field(
        None, description='additional shunt parameters currently not supported by CTM'
    )


class ReserveProvision(BaseModel):
    uid: Union[conint(ge=0), str] = Field(
        ..., description='uid of reserve product rg contributes to'
    )
    rg: Union[TimeSeriesReference, confloat(ge=0.0)] = Field(
        ..., description='[MW or pu] contribution to reserve'
    )


class Gen(BaseModel):
    uid: Union[conint(ge=0), str] = Field(..., description="object's Unique IDentifier")
    pg: Union[float, TimeSeriesReference] = Field(
        ..., description='[MW or pu] active power injection'
    )
    qg: Optional[Union[float, TimeSeriesReference]] = Field(
        None, description='[MVAr or pu] reactive power injection'
    )
    in_service: Optional[Union[TimeSeriesReference, conint(ge=0, le=1)]] = Field(
        None,
        description='commitment binary indicator; 0=>unit is turned off, 1=>unit is online',
    )
    reserve_provision: Optional[List[ReserveProvision]] = None
    ext: Optional[Any] = Field(
        None,
        description='additional generator parameters currently not supported by CTM',
    )


class Storage(BaseModel):
    uid: Union[conint(ge=0), str] = Field(..., description="object's Unique IDentifier")
    ps: Union[float, TimeSeriesReference] = Field(
        ..., description='[MW or pu] active power injection'
    )
    qs: Optional[Union[float, TimeSeriesReference]] = Field(
        None, description='[MW or pu] reactive power injection'
    )
    energy: Union[TimeSeriesReference, confloat(ge=0.0)] = Field(
        ..., description='[MWh or pu*h] state of charge'
    )
    charge: Optional[Union[TimeSeriesReference, confloat(ge=0.0)]] = Field(
        None, description='[MW or pu] rate of charge'
    )
    discharge: Optional[Union[TimeSeriesReference, confloat(ge=0.0)]] = Field(
        None, description='[MW or pu] rate of discharge'
    )
    ext: Optional[Any] = Field(
        None, description='additional storage parameters currently not supported by CTM'
    )


class Switch(BaseModel):
    uid: Union[conint(ge=0), str] = Field(..., description="object's Unique IDentifier")
    psw_fr: Optional[Union[float, TimeSeriesReference]] = Field(
        None,
        description='[MW or pu] active power entering the switch at its from terminal',
    )
    qsw_fr: Optional[Union[float, TimeSeriesReference]] = Field(
        None,
        description='[MVAr or pu] reactive power entering the switch at its from terminal',
    )
    state: Union[TimeSeriesReference, conint(ge=0, le=1)] = Field(
        ..., description='binary indicator of switch state; 0=>open, 1=>closed'
    )
    ext: Optional[Any] = Field(
        None, description='additional switch parameters currently not supported by CTM'
    )


class AcLine(BaseModel):
    uid: Union[conint(ge=0), str] = Field(..., description="object's Unique IDentifier")
    pl_fr: Optional[Union[float, TimeSeriesReference]] = Field(
        None,
        description='[MW or pu] active power entering the ac line at its from terminal',
    )
    ql_fr: Optional[Union[float, TimeSeriesReference]] = Field(
        None,
        description='[MVAr or pu] reactive power entering the ac line at its from terminal',
    )
    pl_to: Optional[Union[float, TimeSeriesReference]] = Field(
        None,
        description='[MVAr or pu] active power entering the ac line at its from terminal',
    )
    ql_to: Optional[Union[float, TimeSeriesReference]] = Field(
        None,
        description='[MVAr or pu] reactive power entering the ac line at its to terminal',
    )
    ext: Optional[Any] = Field(
        None, description='additional switch parameters currently not supported by CTM'
    )


class Transformer(BaseModel):
    uid: Union[conint(ge=0), str] = Field(..., description="object's Unique IDentifier")
    tm: Optional[Union[float, TimeSeriesReference]] = Field(
        None, description='[-] tap ratio'
    )
    ta: Optional[Union[float, TimeSeriesReference]] = Field(
        None, description='[deg] angle phase shift'
    )
    pt_fr: Optional[Union[float, TimeSeriesReference]] = Field(
        None,
        description='[MW or pu] active power entering the transformer at its from terminal',
    )
    qt_fr: Optional[Union[float, TimeSeriesReference]] = Field(
        None,
        description='[MVAr or pu] reactive power entering the transformer at its from terminal',
    )
    pt_to: Optional[Union[float, TimeSeriesReference]] = Field(
        None,
        description='[MW or pu] active power entering the transformer at its to terminal',
    )
    qt_to: Optional[Union[float, TimeSeriesReference]] = Field(
        None,
        description='[MVAr or pu] reactive power entering the transformer at its to terminal',
    )
    ext: Optional[Any] = Field(
        None,
        description='additional 2-winding transformer parameters currently not supported by CTM',
    )


class HvdcP2p(BaseModel):
    uid: Union[conint(ge=0), str] = Field(..., description="object's Unique IDentifier")
    vm_dc: Optional[Union[TimeSeriesReference, PositiveFloat]] = Field(
        None, description='[kV or pu] voltage at the dc side'
    )
    pdc_fr: Union[float, TimeSeriesReference] = Field(
        ...,
        description='[MW or pu] active power entering the hvdc line at its from terminal',
    )
    qdc_fr: Optional[Union[float, TimeSeriesReference]] = Field(
        None,
        description='[MVAr or pu] reactive power entering the hvdc line at its from terminal',
    )
    pdc_to: Union[float, TimeSeriesReference] = Field(
        ...,
        description='[MW or pu] active power entering the hvdc line at its to terminal',
    )
    qdc_to: Optional[Union[float, TimeSeriesReference]] = Field(
        None,
        description='[MVAr or pu] reactive power entering the hvdc line at its to terminal',
    )
    ext: Optional[Any] = Field(
        None,
        description='additional hvdc point-to-point parameters currently not supported by CTM',
    )


class Reserve(BaseModel):
    uid: Union[conint(ge=0), str] = Field(..., description="object's Unique IDentifier")
    shortfall: Union[float, TimeSeriesReference] = Field(
        ..., description='[MW or pu] shortfall on reserve product'
    )
    ext: Optional[Any] = Field(
        None, description='additional reserve parameters currently not supported by CTM'
    )


class Solution(BaseModel):
    global_params: GlobalParams = Field(
        ...,
        description='structure to hold global settings for parameters in the network',
    )
    bus: List[Bu]
    shunt: Optional[List[Shunt]] = None
    gen: List[Gen]
    storage: Optional[List[Storage]] = None
    switch: Optional[List[Switch]] = None
    ac_line: Optional[List[AcLine]] = None
    transformer: Optional[List[Transformer]] = None
    hvdc_p2p: Optional[List[HvdcP2p]] = None
    reserve: Optional[List[Reserve]] = None


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


class CtmSolution(BaseModel):
    ctm_version: str = Field(..., description='release version of CTM specification')
    solution: Solution = Field(
        ..., description='structure to hold persistent solution data'
    )
    time_series_data: Optional[TimeSeriesData] = Field(
        None,
        description='structure to contain all time variant data of the system/case. All time series are synchronized to the same timestamps, which should should be stored using Unix time. Structure is quasi-tabular, with uid, name, path_to_file, values, and ext being arrays in the same order of said field. This is done in order to allow for better compression (e.g., using HDF5) for the values field.',
    )

from pydantic.tools import parse_obj_as
import json

def parse(filename):
    f = open(filename, 'r')
    json_dict = json.load(f)
    f.close()
    return parse_obj_as(CtmSolution, json_dict)

def dump(instance, filename):
    f = open(filename, 'w')
    f.write(instance.model_dump_json(indent=4, exclude_unset=True, exclude_none=True))
    f.close()

