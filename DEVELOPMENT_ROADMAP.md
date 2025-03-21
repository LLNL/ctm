## CTM Development Roadmap

* <del>`v0.1`: Initial release.</del>
    - [x] Quasi-stationary parameters, covering use cases from power flow up to medium-term
      reliability studies.
    - [x] JSON Schemas and Python Pydantic specifications.
    - [x] Reliability and unit commitment examples.
* `v0.2`: Additional features.
    - [x] C++ classes and Julia struct specifications.
    - [] Multiple-winding transformers.
    - [] Converters/parsers from common (text-based) power system formats.
* `v0.3`: Scalability features.
    - [] Dynamic model specifications for generators (e.g., machines and controllers).
    - [] Sequence model specifications for generators, transformers, and power lines.
    - [] Additional instances with dynamic and sequence data.
* `v0.4`: Scalability features.
    - [] Support for HDF5 Time Series specification.
