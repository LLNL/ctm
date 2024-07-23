# Common Electric Power Transmission System Model JSON Schema Specification

The **C**ommon Electric Power **T**ransmission System **M**odel (CTM) is an intuitive, extensible,
language-agnostic, and range-validating spefication of electric power network components' parameter
names and units, and the relation between components, intended for use by the research community
developing new computational methods for power systems operations and simulation.

Power system datasets following the CTM specification can be read as dictionaries and manipulated in
that form in most languages, e.g., in Python we can check the first bus parameters using:

```python
import json
with open('path/to/ctm_instance.json', 'r') as f:
    ps = json.load(f)
print(ps["network"]["bus"][0])
```

whereas in [Julia](https://julialang.org/), the same operation can be done as:

```julia
using JSON
ps = JSON.parsefile("path/to/ctm_instance.json")
@show ps["network"]["bus"][0]
```

This standard data structure in CTM makes it easy to work in multiple power systems domains (e.g.,
economic operation, reliability assessment, electricity markets, stability assessment, etc.) without
requiring conversions between use-case-specific file formats and data structures with information
loss in the process.

This repository specifies CTM as a [JSON Schema](https://json-schema.org/), provides documentation,
derivated (code-generated) implementations of CTM, and example data and usage of the schema for
important use cases.

## Getting Started

The repository directories are organized as follows:

* `json_schemas`: contains the JSON Schema specification for CTM.
* `generated`: contains *class* definitions code-generated from the CTM JSON Schema. The purpose of
               these classes is to parse and manage CTM datasets in multiple programming languages.
* `documentation`: contains code-generated HTML documentation for the CTM JSON Schema.
* `examples`: contains example datasets in CTM JSON Schema compliant format and example scripts that
              use CTM for important use cases.

## Contributing and development roadmap

Contributing to CTM is easy: send us a [pull
request](https://help.github.com/articles/using-pull-requests/). When you send your request, make
`main` the destination branch.

CTM is a project currently under development. See our [development roadmap](DEVELOPMENT_ROADMAP.md)
for details and contact the authors below if you would like to get involved in planning and future
releases.

## Initiative History

The CTM authors started on this initiative in 2019, prompted by an uptick in new power system
modeling tools, each of which is using a different subset of the system parameters and specifying
its input on a different format. In 2023, this effort was reinvigorated by US DOE-sponsored projects
requiring multiple tools providing a common interface to access them, leading to the translation of
the initial specification to JSON and its expansion to cover use cases beyond power flow and optimal
power flow

## Authors

The CTM specification in this repository was written by Ignacio Aravena (LLNL,
aravenasolis1@llnl.gov), building on the initial CTM effort led by Carleton Coffrin (LANL), with
contributions from Clayton Barrows (NREL), Ray D. Zimmerman (Cornell), and others.

## License

CTM is distributed under the terms of both the MIT license. See [LICENSE](LICENSE) for
details. All new contributions must be made under the MIT license.

## Acknowledgments

This CTM specification has been developed with the support of the US Department of Energy (DOE),
Office of Electricty, Transmission Reliability and Resilience Program and the North American Energy
Resilience Model (NAERM).

LLNL-CODE-865934
