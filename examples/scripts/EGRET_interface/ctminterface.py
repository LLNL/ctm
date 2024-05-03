#  ___________________________________________________________________________
#
#  EGRET CTM JSON Extension
#  ___________________________________________________________________________

"""
This module provides supporting functions for parsing (and writing) CTM JSON files

.. todo::
    Documentation
"""

import os
import sys
sys.path.insert(0, os.path.abspath('../../../generated/pydantic'))

import ctmdata
import ctmsolution
import egret.data.model_data as md
import numpy as np

def create_ModelData(ctm_filename):
    """
    Parse JSON CTM input file into a ModelData object containing
    the model_data dictionary

    Parameters
    ----------
    ctm_filename : str
        Path and filename of the CTM input file you wish to load

    Returns
    -------
        ModelData
    """
    data = create_model_data_dict(ctm_filename)
    return md.ModelData(data)

def write_solution(model_data_wsol, ctm_sol_filename):
    """
    Extracts UC solution data from a solved ModelData object and 
    writes it to a file using the CTM solution format

    Parameters
    ----------
    model_data_wsol : ModelData
        Solved ModelData object from EGRET.
    ctm_sol_filename : str
        File name for writing the solution.
    """
    ctmsol = extract_uc_solution(model_data_wsol)
    ctmsolution.dump(ctmsol, ctm_sol_filename)

def ctmdata_2_model_data_dict(ctm):
    
    """
    Parse a CTM JSON file into a model_data dictionary

    Parameters
    ----------
    ctm : ctmdata
        CTM object containing instance information

    Returns
    -------
        dict : Returns a dictionary in the format required for the ModelData
               object.
    """
    
    # create the model data and create some shortcuts
    model_data = md.ModelData.empty_model_data_dict()
    system = model_data['system']
    elements = model_data['elements']
    
    # parse general (global) system data
    if ctm.network.global_params.unit_convention != ctmdata.UnitConvention.PER_UNIT_SYSTEM_BASE :
        raise Exception('Support only for unit_convention==PER_UNIT_SYSTEM_BASE')
    system['baseMVA'] = ctm.network.global_params.base_mva
    # system['reserve_requirement'] =                       ---PENDING; no reserves for now
    system['time_keys'] = list(range(1, len(ctm.time_series_data.timestamp) + 1))
    system['time_period_length_minutes'] = (ctm.time_series_data.timestamp[1] -
                                            ctm.time_series_data.timestamp[0])/60
                                            # NOTE: Egret does not support variable time resolution
    system["load_mismatch_cost"] = 10000            # no equivalent for this currently on CTM
    system["reserve_shortfall_cost"] = 1000         # no equivalent for this currently on CTM
    if ctm.network.global_params.bus_ref == None :
        system['reference_bus'] = ctm.network.bus[0].name
    else :
        system['reference_bus'] = str(ctm.network.global_params.bus_ref)
    system['reference_bus_angle'] = 0.0
    
    # parse data
    elements['area'] = ctm_name_only_to_md(ctm.network.area)
    elements['zone'] = ctm_name_only_to_md(ctm.network.zone)
    elements['bus'] = ctm_bus_to_md(ctm.network.bus, elements)
    elements['load'] = ctm_load_to_md(ctm.network.load, ctm.time_series_data,
                                      system, elements)
    # shunts are ignored in UC ...
    elements['generator'] = ctm_gen_to_md(ctm.network.gen, ctm.time_series_data,
                                          ctm.temporal_boundary.gen,
                                          system, elements)
    elements['branch'] = ctm_ac_line_to_md(ctm.network.ac_line, ctm.time_series_data,
                                           system, elements)
    elements['branch'].update(ctm_transformer_to_md(ctm.network.transformer, ctm.time_series_data,
                                                    system, elements))
    elements['dc_branch'] = ctm_hvdc_p2p_to_md(ctm.network.hvdc_p2p, ctm.time_series_data,
                                               system, elements)
    
    # return model data object created
    return model_data

def create_model_data_dict(ctm_filename):
    """
    Parse a CTM JSON file into a model_data dictionary

    Parameters
    ----------
    ctm_filename : str
        Path and filename of the CTM input file you wish to load

    Returns
    -------
        dict : Returns a dictionary in the format required for the ModelData
               object.
    """
    
    # read CTM data file
    # PENDING: reading time series in different files
    ctm = ctmdata.parse(ctm_filename)   # this uses the auto generated pydantic data classes for
                                        # validating the CTM JSON file
    
    # call conversion function and return
    data = ctmdata_2_model_data_dict(ctm)
    return md.ModelData(data)

def extract_uc_solution(model_data_wsol) :
    """
    Extracts UC solution data from a solved ModelData object onto
    a CTM solution object

    Parameters
    ----------
    model_data_wsol : ModelData
        Solved ModelData object from EGRET.

    Returns
    -------
        ctmsolution.CtmSolution
    """
    # create ctm dict
    ctm_sol_dict = {
                    'ctm_version': 'v0.1',
                    'solution': {
                                    'global_params': {
                                                'unit_convention': 'PER_UNIT_SYSTEM_BASE',
                                                'base_mva': model_data_wsol.data['system']['baseMVA']
                                            },
                                    'bus': [],
                                    'gen': [],
                                    'transformer': [],
                                    'hvdc_p2p': []
                                },
                    'time_series_data': {
                                    'uid': [],
                                    'values': []
                                }}
    # define function for scaling
    def mva2pu(value_or_arr):
        if isinstance(value_or_arr, float):
            return value_or_arr/model_data_wsol.data['system']['baseMVA']
        elif isinstance(value_or_arr, list):
            return [xi/model_data_wsol.data['system']['baseMVA'] for xi in value_or_arr]
        else:
            raise Exception('unexpected argument, nor float or list')
    
    # fill ctm dict with solution incrementally
    extract_uc_solution_bus(model_data_wsol.data['elements']['bus'],
                            ctm_sol_dict['solution']['bus'],
                            ctm_sol_dict['time_series_data'])
    extract_uc_solution_gen(model_data_wsol.data['elements']['generator'],
                            ctm_sol_dict['solution']['gen'],
                            ctm_sol_dict['time_series_data'], mva2pu)
    extract_uc_solution_transformer(model_data_wsol.data['elements']['branch'],
                                    ctm_sol_dict['solution']['transformer'],
                                    ctm_sol_dict['time_series_data'])
    extract_uc_solution_hvdc_p2p(model_data_wsol.data['elements']['dc_branch'],
                                 ctm_sol_dict['solution']['hvdc_p2p'],
                                 ctm_sol_dict['time_series_data'], mva2pu)
    # generate ctmsolution object (for validation) and return it
    ctm_sol = ctmsolution.CtmSolution(**ctm_sol_dict)
    return ctm_sol

###################################
# Auxiliary (uncommented) functions

def add_const_or_ts(arr, val, tsd) :
    if type(val) == float :
        arr += val
    else :
        idx = tsd.uid.index(val.uid)
        for i in range(len(arr)) :
            arr[i] += val.scale_factor * tsd.values[idx][i]

def ctm_name_only_to_md(ctm_array):
    out = {}
    if ctm_array == None:
        return out
    for elem in ctm_array:
        out[str(elem.uid)] = { 'in_service' : elem.status == 1,
                               'name' : elem.name }
    return out

def ctm_bus_to_md(ctm_bus, md_elems):
    if 'area' not in md_elems:
        raise Exception('bus parser can only be called after area parser')
    out = {}
    for elem in ctm_bus:
        # per CTM specs, if area is out, everything in it must be ignored
        elem_dict = { 'in_service' : ((elem.status == 1) and \
                                      md_elems['area'][str(elem.area)]['in_service']),
                      'base_kv' : elem.base_kv,
                      'area' : elem.area,
                      'zone' : elem.zone }
        out[str(elem.uid)] = elem_dict
    return out

def ctm_load_to_md(ctm_load, ctm_ts_data, md_system, md_elems):
    if 'bus' not in md_elems:
        raise Exception('load parser can only be called after bus parser')
    out = {}
    for elem in ctm_load:
        elem_dict = { 'in_service': ((elem.status == 1) and \
                                     md_elems['bus'][str(elem.bus)]['in_service']),
                      'bus': str(elem.bus)}
        p_ts = np.zeros(len(md_system['time_keys']))
        add_const_or_ts(p_ts, elem.pd, ctm_ts_data)
        if elem.pd_i != None :
            add_const_or_ts(p_ts, elem.pd_i, ctm_ts_data)
        if elem.pd_y != None :
            add_const_or_ts(p_ts, elem.pd_y, ctm_ts_data)
        p_ts *= md_system['baseMVA']
        elem_dict['p_load'] = { 'data_type' : 'time_series',
                                'values' : p_ts.tolist() }
        out[str(elem.uid)] = elem_dict
    return out

def get_generator_type(ctm_generator):
    if ctm_generator.primary_source in (ctmdata.PrimarySource.SOLAR,
                                        ctmdata.PrimarySource.WIND,
                                        ctmdata.PrimarySource.HYDRO):
        return 'renewable'
    else:
        return 'thermal'

def one_if_none(x):
    if x == None:
        return 1.0
    else:
        return x

def get_thermal_params(md_dict, elem, baseMVA):
    md_dict['fixed_commitment'] = 1 if bool(elem.service_required) else None
    md_dict['ramp_up_60min'] = elem.pg_delta_ub * baseMVA
    md_dict['ramp_down_60min'] = elem.pg_delta_lb * baseMVA
    md_dict['startup_capacity'] = elem.pg_lb * baseMVA
    md_dict['shutdown_capacity'] = elem.pg_lb * baseMVA
    md_dict['min_up_time'] = one_if_none(elem.in_service_time_lb)
    md_dict['min_down_time'] = one_if_none(elem.down_time_lb)
    # startup cost... this must be double checked with Ben Kenueven @ NREL
    if elem.startup_cost_hot != None and \
       elem.startup_time_hot != None and \
       elem.startup_cost_warm != None and \
       elem.startup_time_warm != None :
        su_cost = [(elem.down_time_lb, elem.startup_cost_hot),
                   (elem.startup_time_hot, elem.startup_cost_warm),
                   (elem.startup_time_warm, elem.startup_cost_cold)]
    else :
        su_cost = [(elem.down_time_lb, elem.startup_cost_cold)]
    md_dict['startup_cost'] = []
    for i in range(len(su_cost)):
        if len(md_dict['startup_cost']) == 0:
            md_dict['startup_cost'].append((md_dict['min_down_time'],
                                              su_cost[i][1]))
        elif su_cost[i][0] <= md_dict['startup_cost'][-1][0]:
            md_dict['startup_cost'][-1] = (md_dict['startup_cost'][-1][0],
                                             su_cost[i][1])
        # NOTE: we only check the next condition because EGRET does, but there is no physical reason for it
        elif ((su_cost[i][0] > md_dict['startup_cost'][-1][0]) and \
              (su_cost[i][1] > md_dict['startup_cost'][-1][1])): 
            md_dict['startup_cost'].append(su_cost[i])
    # operation cost
    if elem.cost_pg_model != ctmdata.CostPgModel.PIECEWISE_LINEAR :
        raise Exception("script currently only supports PIECEWISE_LINEAR cost specification")    
    md_dict['p_cost'] = { 'data_type' : 'cost_curve',
                            'cost_curve_type' : 'piecewise' }
    md_dict['p_cost']['values'] = list((elem.cost_pg_parameters.x[i] * baseMVA,
                                        elem.cost_pg_parameters.y[i])
                                       for i in range(len(elem.cost_pg_parameters.x)))

def get_ctm_limit(ctm_lim, ctm_ts_data, md_system):
    if ctm_lim == None :
        return None
    elif type(ctm_lim) == float :
        return ctm_lim * md_system['baseMVA']
    else :
        p_ts = np.zeros(len(md_system['time_keys']))
        add_const_or_ts(p_ts, ctm_lim, ctm_ts_data)
        p_ts *= md_system['baseMVA']
        return { 'data_type' : 'time_series', 'values' : p_ts.tolist() }

def get_gen_temporal_boundary(ctm_temporal_boundary_gen, gen_uid):
    obj_generator = (tbg for tbg in ctm_temporal_boundary_gen if tbg.uid == gen_uid)
    return next(obj_generator)

def fix_p_limits(p_min, p_max):
    p_inconsistent_idx = [i for i,(x,y) in enumerate(zip(p_min['values'], p_max['values'])) if x > y]
    for i in p_inconsistent_idx:
        p_min['values'][i] = 0.0
        p_max['values'][i] = 0.0

def show_inconsisten_p_limits_message(elem_dict):
    print('[warn] p_min > p_max for generator ' + elem_dict['name'] + '. Will assume offline at conflicting time intervals.')

def verify_and_fix_p_limits(elem_dict):
    if type(elem_dict['p_min']) == float and \
       type(elem_dict['p_max']) == float:
        if elem_dict['p_min'] > elem_dict['p_max']:
            show_inconsisten_p_limits_message(elem_dict)
            elem_dict['p_min'] = 0.0
            elem_dict['p_max'] = 0.0
    elif type(elem_dict['p_min']) == float and \
        type(elem_dict['p_max']) == dict:
        if any([elem_dict['p_min'] > p_max_t for p_max_t in elem_dict['p_max']['values']]):
            show_inconsisten_p_limits_message(elem_dict)
            elem_dict['p_min'] = { 'data_type' : 'time_series', 
                                   'values' :  [elem_dict['p_min']] * len(elem_dict['p_max']['values']) }
            fix_p_limits(elem_dict['p_min'], elem_dict['p_max'])
    elif type(elem_dict['p_min']) == dict and \
        type(elem_dict['p_max']) == float:
        if any([p_min_t > elem_dict['p_max'] for p_min_t in elem_dict['p_min']['values']]):
            show_inconsisten_p_limits_message(elem_dict)
            elem_dict['p_max'] = { 'data_type' : 'time_series', 
                                   'values' :  [elem_dict['p_max']] * len(elem_dict['p_min']['values']) }
            fix_p_limits(elem_dict['p_min'], elem_dict['p_max'])
    elif type(elem_dict['p_min']) == dict and \
        type(elem_dict['p_max']) == dict:
        if any([p_min_t > p_max_t for p_min_t, p_max_t in \
               zip(elem_dict['p_min']['values'], elem_dict['p_max']['values'])]):
            show_inconsisten_p_limits_message(elem_dict)
            fix_p_limits(elem_dict['p_min'], elem_dict['p_max'])
    else:
        raise Exception('unexpected types for p_min and p_max')

def ctm_gen_to_md(ctm_gen, ctm_ts_data, ctm_temporal_boundary_gen, md_system, md_elems):
    if 'bus' not in md_elems:
        raise Exception('gen parser can only be called after bus parser')
    out = {}
    tmp_boundary_uids = list((g.uid for g in ctm_temporal_boundary_gen))
    for elem in ctm_gen:
        if elem.status == 0 :
            continue
        elem_dict = { 'in_service' : ((elem.status == 1) and \
                                      md_elems['bus'][str(elem.bus)]['in_service']),
                      'name' : str(elem.name),
                      'bus' : str(elem.bus),
                      'fuel' : str(elem.primary_source.name),
                      'generator_type': get_generator_type(elem) }
        if elem_dict['generator_type'] == 'thermal':
            get_thermal_params(elem_dict, elem, md_system['baseMVA'])
        elem_dict['p_min'] = get_ctm_limit(elem.pg_lb, ctm_ts_data, md_system)
        elem_dict['p_max'] = get_ctm_limit(elem.pg_ub, ctm_ts_data, md_system)
        verify_and_fix_p_limits(elem_dict)
        elem_tb = get_gen_temporal_boundary(ctm_temporal_boundary_gen, elem.uid)
        elem_dict['initial_p_output'] = elem_tb.pg * md_system['baseMVA']
        if elem_dict['generator_type'] == 'renewable':
            elem_dict['initial_status'] = 1
        else:
            if elem_tb.in_service_time > 0 :
                elem_dict['initial_status'] = elem_tb.in_service_time
            else :
                elem_dict['initial_status'] = -1 * elem_tb.down_time
        out[str(elem.uid)] = elem_dict
    return out

def none_if_zero(x):
    if type(x) == float:
        if x == 0:
            return None
    return x

def get_common_branch_dict(elem, ctm_ts_data, md_system, md_elems):
    return { 'branch_type' : 'line',
             'in_service' : ((elem.status == 1) and \
                             md_elems['bus'][str(elem.bus_fr)]['in_service'] and \
                             md_elems['bus'][str(elem.bus_to)]['in_service']),
             'from_bus' : str(elem.bus_fr),
             'to_bus' : str(elem.bus_to),
             'resistance' : elem.r,
             'reactance' : elem.x,
             'rating_long_term' : \
                none_if_zero(get_ctm_limit(elem.sm_ub_a, ctm_ts_data, md_system)),
             'rating_short_term' : \
                none_if_zero(get_ctm_limit(elem.sm_ub_b, ctm_ts_data, md_system)),
             'rating_emergency' : \
                none_if_zero(get_ctm_limit(elem.sm_ub_c, ctm_ts_data, md_system)) }

def ctm_ac_line_to_md(ctm_ac_line, ctm_ts_data, md_system, md_elems):
    if 'bus' not in md_elems:
        raise Exception('ac_line parser can only be called after bus parser')
    out = {}
    for elem in ctm_ac_line :
        elem_dict = get_common_branch_dict(elem, ctm_ts_data, md_system, md_elems)
        elem_dict['charging_susceptance'] = elem.b_fr + elem.b_to
        elem_dict['angle_diff_min'] = elem.vad_lb
        elem_dict['angle_diff_max'] = elem.vad_ub
        out[str(elem.uid)] = elem_dict
    return out

def ctm_transformer_to_md(ctm_transformer, ctm_ts_data, md_system, md_elems):
    if 'bus' not in md_elems:
        raise Exception('transformer parser can only be called after bus parser')
    out = {}
    for elem in ctm_transformer:
        elem_dict = get_common_branch_dict(elem, ctm_ts_data, md_system, md_elems)
        elem_dict['charging_susceptance'] = elem.b
        elem_dict['angle_diff_min'] = None
        elem_dict['angle_diff_max'] = None
        elem_dict['transformer_tap_ratio'] = .5 * elem.tm_lb + .5 * elem.tm_ub
        # NOTE: phase shift should be optimized, but EGRET currently does not do that
        if elem.ta_lb == None :
            elem_dict['transformer_phase_shift'] = 0.
        else:
            elem_dict['transformer_phase_shift'] = 0.5 * elem.ta_lb + 0.5 * elem.ta_ub
        out[str(elem.uid)] = elem_dict
    return out

def ctm_hvdc_p2p_to_md(ctm_hvdc_p2p, ctm_ts_data, md_system, md_elems):
    if 'bus' not in md_elems:
        raise Exception('hvdc_p2p parser can only be called after bus parser')
    out = {}
    for elem in ctm_hvdc_p2p :
        elem_dict = { 'in_service' : ((elem.status == 1) and \
                                      md_elems['bus'][str(elem.bus_fr)]['in_service'] and \
                                      md_elems['bus'][str(elem.bus_to)]['in_service']),
                      'from_bus' : str(elem.bus_fr),
                      'to_bus' : str(elem.bus_to),
                      'rating_long_term' : get_ctm_limit(elem.pdc_fr_ub, ctm_ts_data, md_system) }
        elem_dict['rating_short_term'] = elem_dict['rating_long_term']
        elem_dict['rating_emergency'] = elem_dict['rating_long_term']
        # NOTE: EGRET does not currently support DC Lines with asymmetric capacities, eventhough
        # they are common in reality
        out[str(elem.uid)] = elem_dict
    return out


def no_filter(obj):
    return True

def no_scale(obj):
    return obj

def extract_uc_solution_elem(mddata, sol_dict, ts_dict,
                             md_value_keys, ctm_value_keys, scale_values,
                             md_ts_keys, ctm_ts_keys, scale_ts,
                             scale_func=no_scale,
                             filter_func=no_filter):
    if len(md_value_keys) != len(ctm_value_keys) or len(ctm_value_keys) != len(scale_values):
        raise Exception('inconsistent length for value fields')
    if len(md_ts_keys) != len(ctm_ts_keys) or len(ctm_ts_keys) != len(scale_ts):
        raise Exception('inconsistent length for time series fields')
    for k, v in mddata.items():
        if not filter_func(v):
            continue
        elem_dict = dict()
        elem_dict['uid'] = k
        for i in range(len(md_value_keys)):
            if not isinstance(mddata[k][md_value_keys[i]], (bool, str, int, float)):
                raise Exception('unexpected non primitive type')
            if scale_values[i]:
                elem_dict[ctm_value_keys[i]] = scale_func(mddata[k][md_value_keys[i]])
            else:
                elem_dict[ctm_value_keys[i]] = mddata[k][md_value_keys[i]]
        for i in range(len(md_ts_keys)):
            if not isinstance(mddata[k][md_ts_keys[i]], dict):
                raise Exception('unexpected non dict type')
            if mddata[k][md_ts_keys[i]]['data_type'] != 'time_series':
                raise Exception('got an unsupported dict type (not time_series)')
            ts_uid = 'TS' + str(len(ts_dict['uid'])) + '_' + k
            ts_dict['uid'].append(ts_uid)
            if scale_ts[i]:
                ts_dict['values'].append(scale_func(mddata[k][md_ts_keys[i]]['values']))
            else:
                ts_dict['values'].append(mddata[k][md_ts_keys[i]]['values'])
            elem_dict[ctm_ts_keys[i]] = {
                                            'uid' : ts_uid,
                                            'scale_factor' : 1.0
                                        }
        sol_dict.append(elem_dict)

def extract_uc_solution_bus(mddata, sol_dict, ts_dict):
    extract_uc_solution_elem(mddata, sol_dict, ts_dict,
                             [], [], [],
                             ['va'], ['va'], [False])

def filter_not_renewable(obj_dict):
    if not isinstance(obj_dict, dict):
        raise Exception('unexpected non dict type')
    return obj_dict['generator_type'] != 'renewable'

def filter_renewable(obj_dict):
    if not isinstance(obj_dict, dict):
        raise Exception('unexpected non dict type')
    return obj_dict['generator_type'] == 'renewable'
    
def extract_uc_solution_gen(mddata, sol_dict, ts_dict, scale_func=no_scale):
    extract_uc_solution_elem(mddata, sol_dict, ts_dict,
                             [], [], [],
                             ['commitment', 'pg'], ['in_service', 'pg'], [False, True],
                             scale_func,
                             filter_not_renewable)
    extract_uc_solution_elem(mddata, sol_dict, ts_dict,
                             ['in_service'], ['in_service'], [False],
                             ['pg'], ['pg'], [True],
                             scale_func,
                             filter_renewable)

def filter_transformers(obj_dict):
    if not isinstance(obj_dict, dict):
        raise Exception('unexpected non dict type')
    return obj_dict['branch_type'] == 'transformer'

def extract_uc_solution_transformer(mddata, sol_dict, ts_dict):
    extract_uc_solution_elem(mddata, sol_dict, ts_dict,
                             ['transformer_phase_shift'], ['ta'], [False],
                             [], [], [],
                             no_scale,
                             filter_transformers)

def extract_uc_solution_hvdc_p2p(mddata, sol_dict, ts_dict, scale_func=no_scale):
    extract_uc_solution_elem(mddata, sol_dict, ts_dict,
                             [], [], [],
                             ['pf', 'pf'],  ['pdc_fr', 'pdc_to'], [True, True],
                             scale_func)
    for i in range(len(ts_dict['values'][-1])):
        ts_dict['values'][-1][i] = -1.0 * ts_dict['values'][-1][i]   
