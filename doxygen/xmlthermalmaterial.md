Thermal materials     {#xmlthermalmaterial}
===================

All materials, which are used in the thermal model, are listed below.
The structure of the corresponding xml elements and a code snippet are given for each material.

<br/>

Abstract
=========

All material are listed in the <**ThermalMaterials**>-node, which can be found in the root node of the xml file (within the <**Configurations**>-node).
The attribute "cache='true'" has to be set for all materials.

<br/>

Thermal materials
==========

<br/>

Material with isotropic thermal conductivity
----------

__Object type: "Material"__

XML Tag                  | Type                                | Explanation
-------------------------|-------------------------------------|----------------------------
Density                  | double                              | density in kg/m³
SpecificCapacity         | double                              | specific heat capacity in J/(kg*K)
Conductivity             | double                              | thermal conductivity in W/(m*K)
Emissivity               | double                              | emissivity (0 to 1)

\htmlinclude thermalmaterial_color.xml

Material with anisotropic thermal conductivity in cartesian coordinates
----------

__Object type: "MaterialCartesian"__

XML Tag                  | Type                                | Explanation
-------------------------|-------------------------------------|----------------------------
Density                  | double                              | density in kg/m³
SpecificCapacity         | double                              | specific heat capacity in J/(kg*K)
ConductivityX            | double                              | thermal conductivity in x direction in W/(m*K)
ConductivityY            | double                              | thermal conductivity in y direction in W/(m*K)
ConductivityZ            | double                              | thermal conductivity in z direction in W/(m*K)
Emissivity               | double                              | emissivity (0 to 1)

\htmlinclude thermalmaterialcartesian_color.xml

<br/>


Material with anisotropic thermal conductivity in cylindrical coordinates
----------

__Objekttyp: "MaterialCylindric"__

XML Tag                  | Type                                | Explanation
-------------------------|-------------------------------------|----------------------------
Density                  | double                              | density in kg/m³
SpecificCapacity         | double                              | specific heat capacity in J/(kg*K)
ConductivityRho          | double                              | thermal conductivity in &rho; direction in W/(m*K)
ConductivityPhi          | double                              | thermal conductivity in &phi; direction in W/(m*K)
ConductivityZ            | double                              | thermal conductivity in z direction in W/(m*K)
Emissivity               | double                              | emissivity (0 to 1)

\htmlinclude thermalmaterialcylindric_color.xml

