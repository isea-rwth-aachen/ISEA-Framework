Thermal blocks     {#xmlthermalblock}
===================

All thermal blocks that are used in the thermal model are listed in the section below.
The structure of the corresponding xml element and a code example is given for each block.

Abstract
=========

![Possible geometrical blocks](all_cells.png)
Each thermal block can be specified by the attribute "class".
Each thermal block can posses the xml tag <**Heat**>.
The value indicated therein indicates the power loss in Watt.
If the power loss is set at another point in the simulation (e.g. in the electrical model or as input of the Simulink S-function), this value will be ignored.

<br/>

Base types
==========

<br/>

Rectangular thermal block
----------
![Rectangular thermal block](quadratic.png)

__Object type: "RectangularBlock"__

XML Tag                  | Type                                | Explanation
------------------------ |-------------------------------------|----------------------------
Material                 | [material](xmlthermalmaterial.html) | material of the thermal block
Dimensions               | comma-separated doubles             | size of the thermal block in x, y, and z direction in meters
SizeX                    | integer                             | discretization of the thermal block in finite volumes in x direction
SizeY                    | integer                             | discretization of the thermal block in finite volumes in y direction
SizeZ                    | integer                             | discretization of the thermal block in finite volumes in z direction
InitialTemperature       | double                              | initial temperature of the thermal block in °C
ElectricalDiscretization | double                              | discretization of the electrical [cell elements](xmlelectrical.html) within the thermal block. These have to fit into the thermal discretization in each dimension (e.g z-dimension: SizeZ modulo z == 0). If  ElectricalDiscretization is not used, a single cell element will be addressed to this block. If one of the attributes x, y or z is missing, 1 will be used as the default value.
- Attribute x            | integer                             | electrical discretization in x direction
- Attribute y            | integer                             | electrical discretization in x direction
- Attribute z            | integer                             | electrical discretization in x direction

\htmlinclude xmlrectangular_color.xml

Cylindrical cell surrounded by a rectangular block of fill material
----------

__Object type: "QuadraticCellBlock"__

XML Tag                  | Type                                | Explanation
------------------------ |-------------------------------------|----------------------------
CellMaterial             | [material](xmlthermalmaterial.html) | material of the cell
FillMaterial             | [material](xmlthermalmaterial.html) | material of the block surrounding the cell
Length                   | double                              | length of the quadratic base area in meters
RadiusCell               | double                              | radius of the cell in meters. The cell is placed centrally within the thermal block.
Height                   | double                              | height (in z direction) of the thermal block in meters
CellRhoDivision          | integer                             | discretization of the battery cell in &rho; direction
PhiDivision              | integer                             | discretization of the thermal block in &phi; direction
HeightDivision           | integer                             | discretization of the thermal block in z direction
InitialTemperature       | double                              | initial temperature of the thermal block in °C
ArcPolygonization        | double                              | suggested route for the polygonization of the existing arcs and circles
ElectricalDiscretization | double                              | discretization of the electrical [cell elements](xmlelectrical.html) within the thermal block. These have to fit into the thermal discretization in each dimension (e.g z-dimension: SizeZ modulo z == 0). If  ElectricalDiscretization is not used, a single cell element will be addressed to this block. If one of the attributes phi or z is missing, 1 will be used as the default value.
- Attribute phi          | integer                             | electrical discretization in phi direction
- Attribute z            | integer                             | electrical discretization in x direction

\htmlinclude xmlquadratic_color.xml

Supercap cell with two radial layers surrounded by fill material within a rectangular block
----------

![Supercap with two radial layers](cell_cyl.png)

__Object type: "Supercap"__

XML Tag                  | Type                                | Explanation
------------------------ |-------------------------------------|----------------------------
InnerCellMaterial        | [material](xmlthermalmaterial.html) | material of the inner layer of the cell
OuterCellMaterial        | [material](xmlthermalmaterial.html) | material of the outer layer of the cell
FillMaterial             | [material](xmlthermalmaterial.html) | material of the block surrounding the cell
Length                   | double                              | length of the quadratic base area in meters
InnerRadiusCell          | double                              | radius of the boundary between the inner and outer layer in meters
OuterRadiusCell          | double                              | radius of the supercap in meters. The cell is placed centrally within the thermal block.
Height                   | double                              | height (in z direction) of the thermal block in meters
PhiDivision              | integer                             | discretization of the thermal block in &phi; direction
HeightDivision           | integer                             | discretization of the thermal block in z direction
InitialTemperature       | double                              | initial temperature of the thermal block in °C
ArcPolygonization        | double                              | suggested route for the polygonization of the existing arcs and circles
ElectricalDiscretization | double                              | discretization of the electrical [cell elements](xmlelectrical.html) within the thermal block. These have to fit into the thermal discretization in each dimension (e.g z-dimension: SizeZ modulo z == 0). If  ElectricalDiscretization is not used, a single cell element will be addressed to this block. If one of the attributes phi or z is missing, 1 will be used as the default value.
- Attribute phi          | integer                             | electrical discretization in phi direction
- Attribute z            | integer                             | electrical discretization in x direction

\htmlinclude xmlsupercap_color.xml

Cylindrical cell surrounded by a hexagonal prismatic block of fill material
----------

![Hexagonal prismatic block](cell_hex.png)

__Object type: "HexagonalCellBlock"__

XML Tag                  | Type                                | Explanation
------------------------ |-------------------------------------|----------------------------
CellMaterial             | [material](xmlthermalmaterial.html) | material of the cell
FillMaterial             | [material](xmlthermalmaterial.html) | material of the block surrounding the cell
Length                   | double                              | edge length of the regular hexagonal base area of the thermal block in meters
RadiusCell               | double                              | radius of the cell in meters. The cell is placed centrally within the thermal block.
Height                   | double                              | height (in z direction) of the thermal block in meters
CellRhoDivision          | integer                             | discretization of the battery cell in &rho; direction
PhiDivision              | integer                             | discretization of the thermal block in &phi; direction
HeightDivision           | integer                             | discretization of the thermal block in z direction
InitialTemperature       | double                              | initial temperature of the thermal block in °C
ArcPolygonization        | double                              | suggested route for the polygonization of the existing arcs and circles
ElectricalDiscretization | double                              | discretization of the electrical [cell elements](xmlelectrical.html) within the thermal block. These have to fit into the thermal discretization in each dimension (e.g z-dimension: SizeZ modulo z == 0). If  ElectricalDiscretization is not used, a single cell element will be addressed to this block. If one of the attributes phi or z is missing, 1 will be used as the default value.
- Attribute phi          | integer                             | electrical discretization in phi direction
- Attribute z            | integer                             | electrical discretization in x direction

\htmlinclude xmlhexagonal_color.xml

Triangular prismatic thermal block
----------
![Triangular prismatic thermal block](tri.png)

__Object type: "TriangularPrismBlock"__

XML Tag                  | Type                                | Explanation
------------------------ |-------------------------------------|----------------------------
Material                 | [material](xmlthermalmaterial.html) | material of the prism
Vertices                 | comma-separated doubles             | vertices of the triangle that forms the base of the thermal block in xy coordinates.
Height                   | double                              | height (in z direction) of the thermal block in meters
HeightDivision           | integer                             | discretization of the thermal block in z direction
InitialTemperature       | double                              | initial temperature of the thermal block in °C
ElectricalDiscretization | double                              | discretization of the electrical [cell elements](xmlelectrical.html) within the thermal block. These have to fit into the thermal discretization in each dimension (e.g z-dimension: SizeZ modulo z == 0). If  ElectricalDiscretization is not used, a single cell element will be addressed to this block. If the attribute z is missing, 1 will be used as the default value.
- Attribute z            | integer                             | electrical discretization in x direction

\htmlinclude xmltri_color.xml

