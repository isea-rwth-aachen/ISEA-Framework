Coolings     {#xmlcooling}
===================

Consecutively all coolings, that are available in the thermal model, are listed.
For every cooling the structure of particular XML-file is described and an example for the code is given.
<br/>

General
=========
Every cooling has a geometrical (position in space) and a physical part (parameters of the cooling power).
The geometrical part, which describes the position of the cooling, is given in a child-node of a <**CoolingBlocks**>-node.
The physical part is the <**Cooling**>-node included.
For both parts it is possible to define the type of cooling by setting the attribute "class", so that any geometry and parameter sets of the cooling can be combined with each other.

<br/>

Geometrical part
==========

<br/>

Prismatic Geometry
----------

__Objecttype: "CoolingPrismatic"__

XML Tag                  | Type                                | Explanation
-------------------------|-------------------------------------|----------------------------
Vertices                 | comma-separated doubles             | corner points of the polygon that builds the the base of the prism and lies parallel to the xy-plane, in global cartesian coordinates.
LowerZCoordinate         | double                              | z coordinate of the base of the prism
UpperZCoordinate         | double                              | z coordinate of the top of the prism
Cooling                  | [cooling](#physical)                | physical part (parametrization of the power) of the cooling

A prism, whose base lies parallel to the xy-plane and whose hight runs parallel to th z-axis.
\htmlinclude waterCooling_color.xml

Physical part {#physical}
==========

Neumann-boundary-value (constant heat flow density [W/m²] by the cooling)
----------

__Objecttype: "CoolingByConstantValue"__

XML Tag                  | Type                                | Explanation
-------------------------|-------------------------------------|----------------------------
Value                    | double                              | power of the cooling in W/m²

\htmlinclude cooling_by_constant_color.xml

Look-Up-table: heat flow density is dependant on the temperature of the boundary surface between cooling and thermal block.
----------

__Objecttype: "CoolingByLookUp"__

XML Tag                  | Type                                | Explanation
-------------------------|-------------------------------------|----------------------------
LookUpTable              | [parameter object](xmlobject.html)  | sets the cooling power in W/m²

Between the measurements points of the look-up-table the curve is interpolated and is smoothed slightly at the measurement points. The measurement points are temperatures given in °C.
\htmlinclude cooling_lookuptable_color.xml

thermal isolation
----------

__Objecttype: "CoolingByLookUp"__

No contact to the environment, so no heat transmission (adiabatic).
\htmlinclude cooling_isolation_color.xml


Dirichlet-boundary-condition (given boundary temperature)
----------

__Objecttype: "DirichletBoundaryCondition"__

XML Tag                  | Type                                | Explanation
-------------------------|-------------------------------------|----------------------------
SurfaceTemperature       | double                              | surface temperature in °C

The outer surface, on wich the cooling is connected to, is held on a constant temperature.
\htmlinclude cooling_dirichlet_boundary_condition_color.xml
