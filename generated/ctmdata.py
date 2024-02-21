# generated by datamodel-codegen:
#   filename:  ctm_data_schema.json
#   timestamp: 2024-02-17T17:48:33+00:00

from __future__ import annotations

from enum import Enum
from typing import Any, List, Optional, Union

from pydantic import BaseModel, Field, PositiveFloat, PositiveInt, confloat, conint


class UnitConvention(Enum):
    NATURAL_UNITS = 'NATURAL_UNITS'
    PER_UNIT_COMPONENT_BASE = 'PER_UNIT_COMPONENT_BASE'
    PER_UNIT_SYSTEM_BASE = 'PER_UNIT_SYSTEM_BASE'


class PrimarySource(Enum):
    COAL = 'COAL'
    OIL = 'OIL'
    GAS = 'GAS'
    NUCLEAR = 'NUCLEAR'
    BIOMASS = 'BIOMASS'
    GEOTHERMAL = 'GEOTHERMAL'
    SOLAR = 'SOLAR'
    WIND = 'WIND'
    HYDRO = 'HYDRO'
    OTHER = 'OTHER'


class PrimarySourceSubtype(Enum):
    ANTRHC_BITMN_COAL = 'ANTRHC_BITMN_COAL'
    WASTE_COAL = 'WASTE_COAL'
    DISTILLATE_FUEL_OIL = 'DISTILLATE_FUEL_OIL'
    WASTE_OIL = 'WASTE_OIL'
    PETROLEUM_COKE = 'PETROLEUM_COKE'
    RESIDUAL_FUEL_OIL = 'RESIDUAL_FUEL_OIL'
    NATURAL_GAS = 'NATURAL_GAS'
    OTHER_GAS = 'OTHER_GAS'
    NUCLEAR = 'NUCLEAR'
    AG_BIPRODUCT = 'AG_BIPRODUCT'
    MUNICIPAL_WASTE = 'MUNICIPAL_WASTE'
    WOOD_WASTE = 'WOOD_WASTE'
    GEOTHERMAL = 'GEOTHERMAL'
    SOLAR_PV = 'SOLAR_PV'
    SOLAR_CSP = 'SOLAR_CSP'
    WIND_ONSHORE = 'WIND_ONSHORE'
    WIND_OFFSHORE = 'WIND_OFFSHORE'
    HYDRO_RUN_OF_THE_RIVER = 'HYDRO_RUN_OF_THE_RIVER'
    HYDRO_DAM = 'HYDRO_DAM'
    HYDRO_PUMPED_STORAGE = 'HYDRO_PUMPED_STORAGE'
    OTHER = 'OTHER'


class TimeSeriesReference(BaseModel):
    uid: Union[conint(ge=0), str]
    scale_factor: float


class Global(BaseModel):
    unit_convention: UnitConvention
    base_mva: Optional[PositiveFloat] = None
    bus_ref: Optional[List[conint(ge=0)]] = None


class Area(BaseModel):
    uid: Union[conint(ge=0), str]
    status: Union[TimeSeriesReference, conint(ge=0, le=1)]
    name: Optional[str] = None
    source_uid: Optional[Any] = None


class Zone(BaseModel):
    uid: Union[conint(ge=0), str]
    status: Union[TimeSeriesReference, conint(ge=0, le=1)]
    name: Optional[str] = None
    source_uid: Optional[Any] = None


class Bu(BaseModel):
    uid: Union[conint(ge=0), str]
    status: Union[TimeSeriesReference, conint(ge=0, le=1)]
    name: Optional[str] = None
    source_uid: Optional[Any] = None
    base_kv: PositiveFloat
    type: Optional[Union[conint(ge=1, le=3), TimeSeriesReference]] = None
    vm_lb: Optional[Union[TimeSeriesReference, PositiveFloat]] = None
    vm_ub: Optional[Union[TimeSeriesReference, PositiveFloat]] = None
    area: Optional[Union[conint(ge=0), str]] = None
    zone: Optional[Union[conint(ge=0), str]] = None


class Load(BaseModel):
    uid: Union[conint(ge=0), str]
    status: Union[TimeSeriesReference, conint(ge=0, le=1)]
    name: Optional[str] = None
    source_uid: Optional[Any] = None
    nominal_mva: Optional[PositiveFloat] = None
    bus: Union[conint(ge=0), str]
    pd: Union[float, TimeSeriesReference]
    qd: Union[float, TimeSeriesReference]
    pd_i: Union[float, TimeSeriesReference]
    qd_i: Union[float, TimeSeriesReference]
    pd_y: Union[float, TimeSeriesReference]
    qd_y: Union[float, TimeSeriesReference]


class Shunt(BaseModel):
    uid: Union[conint(ge=0), str]
    status: Union[TimeSeriesReference, conint(ge=0, le=1)]
    name: Optional[str] = None
    source_uid: Optional[Any] = None
    nominal_mva: Optional[PositiveFloat] = None
    bus: Union[conint(ge=0), str]
    num_steps_ub: Union[List[conint(ge=0)], conint(ge=0)]
    gs: Union[List[confloat(ge=0.0)], confloat(ge=0.0)]
    bs: Union[float, List[float]]


class Gen(BaseModel):
    uid: Union[conint(ge=0), str]
    status: Union[TimeSeriesReference, conint(ge=0, le=1)]
    name: Optional[str] = None
    source_uid: Optional[Any] = None
    nominal_mva: Optional[PositiveFloat] = None
    bus: Union[conint(ge=0), str]
    vm_setpoint: Optional[Union[TimeSeriesReference, PositiveFloat]] = None
    pg_lb: Optional[Union[float, TimeSeriesReference]] = None
    pg_ub: Optional[Union[float, TimeSeriesReference]] = None
    qg_lb: Optional[Union[float, TimeSeriesReference]] = None
    qg_ub: Optional[Union[float, TimeSeriesReference]] = None
    primary_source: Optional[PrimarySource] = None
    primary_source_subtype: Optional[PrimarySourceSubtype] = None
    cost_pg_model: Optional[conint(ge=1, le=2)] = None
    cost_pg_parameters: Optional[Union[List[float], TimeSeriesReference]] = None
    startup_cost_hot: Optional[Union[float, TimeSeriesReference]] = None
    startup_cost_warm: Optional[Union[float, TimeSeriesReference]] = None
    startup_cost_cold: Optional[Union[float, TimeSeriesReference]] = None
    startup_time_hot: Optional[confloat(ge=0.0)] = None
    startup_time_warm: Optional[confloat(ge=0.0)] = None
    startup_time_cold: Optional[confloat(ge=0.0)] = None
    in_service_time_ub: Optional[confloat(ge=0.0)] = None
    in_service_time_lb: Optional[confloat(ge=0.0)] = None
    down_time_lb: Optional[confloat(ge=0.0)] = None
    pg_delta_ub: Optional[confloat(ge=0.0)] = None
    pg_delta_lb: Optional[confloat(ge=0.0)] = None
    service_required: Optional[Union[conint(ge=0, le=2), TimeSeriesReference]] = None
    shutdown_cost: Optional[Union[float, TimeSeriesReference]] = None


class Storage(BaseModel):
    uid: Union[conint(ge=0), str]
    status: Union[TimeSeriesReference, conint(ge=0, le=1)]
    name: Optional[str] = None
    source_uid: Optional[Any] = None
    nominal_mva: Optional[PositiveFloat] = None
    bus: Union[conint(ge=0), str]
    charge_efficiency: Union[TimeSeriesReference, confloat(ge=0.0)]
    discharge_efficiency: Union[TimeSeriesReference, confloat(ge=0.0)]
    ps_ex: float
    qs_ex: float
    energy_ub: Optional[confloat(ge=0.0)] = None
    charge_ub: Optional[Union[TimeSeriesReference, confloat(ge=0.0)]] = None
    discharge_ub: Optional[Union[TimeSeriesReference, confloat(ge=0.0)]] = None
    sm_ub: Optional[confloat(ge=0.0)] = None
    cm_ub: Optional[confloat(ge=0.0)] = None
    qs_lb: Optional[Union[float, TimeSeriesReference]] = None
    qs_ub: Optional[Union[float, TimeSeriesReference]] = None
    ps_delta_ub: Optional[confloat(ge=0.0)] = None
    ps_delta_lb: Optional[confloat(ge=0.0)] = None


class Switch(BaseModel):
    uid: Union[conint(ge=0), str]
    status: Union[TimeSeriesReference, conint(ge=0, le=1)]
    name: Optional[str] = None
    source_uid: Optional[Any] = None
    nominal_mva: Optional[PositiveFloat] = None
    bus_fr: Union[conint(ge=0), str]
    bus_to: Union[conint(ge=0), str]
    sm_ub: Optional[confloat(ge=0.0)] = None
    cm_ub: Optional[confloat(ge=0.0)] = None


class AcLine(BaseModel):
    uid: Union[conint(ge=0), str]
    status: Union[TimeSeriesReference, conint(ge=0, le=1)]
    name: Optional[str] = None
    source_uid: Optional[Any] = None
    nominal_mva: Optional[PositiveFloat] = None
    bus_fr: Union[conint(ge=0), str]
    bus_to: Union[conint(ge=0), str]
    r: float
    x: float
    g_fr: float
    b_fr: float
    g_to: float
    b_to: float
    sm_ub_a: Optional[Union[TimeSeriesReference, confloat(ge=0.0)]] = None
    sm_ub_b: Optional[Union[TimeSeriesReference, confloat(ge=0.0)]] = None
    sm_ub_c: Optional[Union[TimeSeriesReference, confloat(ge=0.0)]] = None
    cm_ub_a: Optional[Union[TimeSeriesReference, confloat(ge=0.0)]] = None
    cm_ub_b: Optional[Union[TimeSeriesReference, confloat(ge=0.0)]] = None
    cm_ub_c: Optional[Union[TimeSeriesReference, confloat(ge=0.0)]] = None
    vad_lb: Optional[float] = None
    vad_ub: Optional[float] = None


class Transformer(BaseModel):
    uid: Union[conint(ge=0), str]
    status: Union[TimeSeriesReference, conint(ge=0, le=1)]
    name: Optional[str] = None
    source_uid: Optional[Any] = None
    nominal_mva: Optional[PositiveFloat] = None
    bus_fr: Union[conint(ge=0), str]
    bus_to: Union[conint(ge=0), str]
    r: float
    x: float
    g: float
    b: float
    tm_lb: Optional[PositiveFloat] = None
    tm_ub: Optional[PositiveFloat] = None
    tm_steps: Optional[PositiveInt] = None
    ta_lb: Optional[float] = None
    ta_ub: Optional[float] = None
    ta_steps: Optional[PositiveInt] = None
    sm_ub_a: Optional[Union[TimeSeriesReference, confloat(ge=0.0)]] = None
    sm_ub_b: Optional[Union[TimeSeriesReference, confloat(ge=0.0)]] = None
    sm_ub_c: Optional[Union[TimeSeriesReference, confloat(ge=0.0)]] = None
    cm_ub_a: Optional[Union[TimeSeriesReference, confloat(ge=0.0)]] = None
    cm_ub_b: Optional[Union[TimeSeriesReference, confloat(ge=0.0)]] = None
    cm_ub_c: Optional[Union[TimeSeriesReference, confloat(ge=0.0)]] = None


class HvdcP2p(BaseModel):
    uid: Union[conint(ge=0), str]
    status: Union[TimeSeriesReference, conint(ge=0, le=1)]
    name: Optional[str] = None
    source_uid: Optional[Any] = None
    nominal_mva: Optional[PositiveFloat] = None
    bus_fr: Union[conint(ge=0), str]
    bus_to: Union[conint(ge=0), str]
    base_kv_dc: Optional[PositiveFloat] = None
    vm_dc_lb: Optional[PositiveFloat] = None
    vm_dc_ub: Optional[PositiveFloat] = None
    pdc_fr_lb: Optional[Union[float, TimeSeriesReference]] = None
    qdc_fr_lb: Optional[Union[float, TimeSeriesReference]] = None
    pdc_fr_ub: Optional[Union[float, TimeSeriesReference]] = None
    qdc_fr_ub: Optional[Union[float, TimeSeriesReference]] = None
    pdc_to_lb: Optional[Union[float, TimeSeriesReference]] = None
    qdc_to_lb: Optional[Union[float, TimeSeriesReference]] = None
    pdc_to_ub: Optional[Union[float, TimeSeriesReference]] = None
    qdc_to_ub: Optional[Union[float, TimeSeriesReference]] = None
    r: confloat(ge=0.0)
    p: conint(ge=1, le=2)
    technology: conint(ge=1, le=3)
    loss_a: confloat(ge=0.0)
    loss_b: confloat(ge=0.0)
    loss_c: confloat(ge=0.0)
    sm_ub: Optional[Union[TimeSeriesReference, confloat(ge=0.0)]] = None
    cm_ub: Optional[Union[TimeSeriesReference, confloat(ge=0.0)]] = None
    phi_lb: Optional[float] = None
    phi_ub: Optional[float] = None


class Reserve(BaseModel):
    uid: Union[conint(ge=0), str]
    status: Union[TimeSeriesReference, conint(ge=0, le=1)]
    name: Optional[str] = None
    source_uid: Optional[Any] = None
    reserve_type: conint(ge=1, le=3)
    participants: Optional[List[Union[conint(ge=0), str]]] = None
    pg_lb: Optional[Union[TimeSeriesReference, confloat(ge=0.0)]] = None
    pg_ub: Optional[Union[TimeSeriesReference, confloat(ge=0.0)]] = None


class Network(BaseModel):
    global_: Global = Field(..., alias='global')
    area: List[Area]
    zone: List[Zone]
    bus: List[Bu]
    load: List[Load]
    shunt: List[Shunt]
    gen: List[Gen]
    storage: List[Storage]
    switch: List[Switch]
    ac_line: List[AcLine]
    transformer: List[Transformer]
    hvdc_p2p: List[HvdcP2p]
    reserve: List[Reserve]


class Global1(BaseModel):
    time_elapsed: confloat(ge=0.0)


class Gen1(BaseModel):
    uid: Union[conint(ge=0), str]
    source_uid: Optional[Any] = None
    pg_prev: float
    in_service_time_prev: confloat(ge=0.0)
    down_time_prev: confloat(ge=0.0)


class Storage1(BaseModel):
    uid: Union[conint(ge=0), str]
    source_uid: Optional[Any] = None
    energy_prev: confloat(ge=0.0)


class TemporalBoundary(BaseModel):
    global_: Global1 = Field(..., alias='global')
    gen: List[Gen1]
    storage: List[Storage1]


class TimeSeriesData(BaseModel):
    uid: List[Union[conint(ge=0), str]]
    name: Optional[List[str]] = None
    source_uid: Optional[List] = None
    path_to_file: Optional[Union[str, List[str]]] = None
    timestamp: Optional[List[conint(ge=0)]] = None
    values: Optional[List[List]] = None


class Model(BaseModel):
    network: Network
    temporal_boundary: TemporalBoundary
    time_series_data: Optional[TimeSeriesData] = None

from pydantic.tools import parse_obj_as
import json

def parse(filename):
    f = open(filename)
    json_dict = json.load(f)
    f.close()
    return parse_obj_as(Model, json_dict)

