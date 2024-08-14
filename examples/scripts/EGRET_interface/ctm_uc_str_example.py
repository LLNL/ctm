#  ___________________________________________________________________________
# EGRET CTM extension example
#  ___________________________________________________________________________

## Example of solving a unit commitment problem from pglib-uc

from egret.models.unit_commitment import solve_unit_commitment
import ctminterface

## Create an Egret 'ModelData' object, which is just a lightweight
## wrapper around a python dictionary, from a CTM instance
print('Creating and solving RTS GML 2020-01-27.json (with transmission) ...')
with open('../../instances/unit_commitment_data/2020-01-27.json', 'r') as f:
    json_text = f.read()
md = ctminterface.create_ModelData(ctm_json_str=json_text)

## solve the unit commitment instance using solver cbc -- could use 'gurobi', 'cplex',
## or any valid Pyomo solver name, provided its available
md_sol = solve_unit_commitment(md, 'cbc', mipgap=0.01, timelimit=300, solver_tee=True)
print('Solved!')

## print the objective value to the screen
print('Objective value:', md_sol.data['system']['total_cost'])

## write the solution to an Egret *.json file
ctminterface.write_solution(md_sol, './2020-01-27_solution_from_text.json')
print('Wrote solution to 2020-01-27_solution_from_text.json')
