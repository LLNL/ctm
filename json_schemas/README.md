##  CTM JSON Schema Documentation

This folder contains JSON Schemas specifying CTM. These JSON Schemas correspond to:
* `ctm_data_schema.json`: Specification for power system data for a use case (e.g., power flow, unit
                          commitment). It contains specifications for most power system components,
                          such as buses, generators, and branches, among others.
* `ctm_solution_schema.json`: Specification for results of a power system use case. It supports the
                              typical results of quasi-stationary short- to mid-term simulations.
* `ctm_time_series_schema.json`: Specification for standalone time series data for power systems.
                                 These time series can be called from a CTM Data or a CTM Solution
                                 file, allowing for future scalable solutions to deal with large
                                 time series datasets (e.g., combining the schema with HDF5).
