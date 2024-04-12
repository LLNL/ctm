# Common Electric Power Transmission System Model (CTM)

Repository for maintaining CTM [JSON Schema](https://json-schema.org/). The OPEN COG Grid project
will release all coherent data and software tools using this format specification, as it present
multiple advantages over existing data formats, both open and closed.

## Contents

* `ctm_tex`: contains the current LaTeX specification for CTM; this is the original CTM data model
  created by Carletton Coffrin (LANL) and other collaborators.
* `ctm_json`: contains the current JSON specification for CTM.
* `generated`: contains *class* definitions code generated from the JSON CTM schema. The purpose of
  these classes is to parse and manage CTM datasets in multiple programming languages.
* `examples`: contains example datasets in CTM JSON format.

## Task to complete before sending for code review

* <del>Add additional parameters for PF/OPF to RTS-GMLC system</del>
* <del>Add RTS unit commitment cases with solutions from EGRET.</del>
* **Port and update EGRET interface.**
* <del>Create HTTP documentation using: https://coveooss.github.io/json-schema-for-humans/</del>
* Create MD files with explanations in each folder.

These tasks need to be completed by 2024-04-12. Then, write report for IEEE.

## Author
Ignacio Aravena, aravenasolis1@llnl.gov.
