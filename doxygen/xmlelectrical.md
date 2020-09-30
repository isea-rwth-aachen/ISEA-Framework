Electrical Objects     {#xmlelectrical}
===================

This page lists all types of electrical objects that can be used to build a network, including the structure of the XML elements.

General
=========

Each XML element that describes an electrical object has the attribute "class". This attribute is used to determine the object type. Additionally, the attribute "observale" can be set to true. This causes current, voltage, dissipation power and (if applicable) soc to be calculated and added to the simulation output.

Basic types
==========

Resistance
----------
A resistance is added by setting the "class" attribute to **OhmicResistance**.

XML Tag                                 | Unit | Type                     | Explanation                                                                                 
----------------------------------------|------|--------------------------|--------------------------------------------------------------------------------
Object                                  | Ohm  | [Object](xmlobject.html) | Value of the resistance

Definition of a resistance with constant value:
\htmlinclude ohmic_color.xml

Capacity
----------
A resistance is added by setting the "class" attribute to **Capacity**.

XML Tag                                 | Unit  | Type                     | Explanation                                                                                 
----------------------------------------|-------|--------------------------|--------------------------------------------------------------------------------
Object                                  | Farad | [Object](xmlobject.html) | Value of the capacitor

Definition of a capacity with constant value:
\htmlinclude capacity_color.xml


Voltage Source
----------
A voltage source is added by setting the "class" attribute to **VoltageSource**.

XML Tag                                 | Unit  | Type                     | Explanation                                                                                 
----------------------------------------|-------|--------------------------|--------------------------------------------------------------------------------
Object                                  | V     | [Object](xmlobject.html) | Value of the voltage source

Definition of a voltage source with constant value:
\htmlinclude voltage_color.xml

Parallel Connection
----------
A parallel connection of elements is added by setting the "class" attribute to **ParallelTwoPort**.

XML Tag                                 | Type             | Default value | Explanation                                                                                 
----------------------------------------|------------------|---------------|--------------------------------------------------------------------------------
Children                                | List of twoports |               | List of child elements that are connected in parallel
- Attribute count                       | int > 0          | 1             | Number of times each child is added

Definition of a parallel connection of a capacitor and a resistor:
\htmlinclude parallel_color.xml

Serial Connection
----------
A serial connection of elements is added by setting the "class" attribute to **SerialTwoPort**.

XML Tag                                 | Type             | Default value | Explanation                                         
----------------------------------------|------------------|---------------|-----------------------------------------------------
Children                                | List of twoports |               | List of child elements that are connected in series 
- Attribute count                       | int > 0          | 1             | Number of times each child is added                 

Definition of a serial connection of a capacitor and a resistor:
\htmlinclude serial_color.xml

More complex elements
======================

Zarc Element
----------
A zarc element is added by setting the "class" attribute to **ZarcElement**.
It describes an element with an impedance value of 
\f[
\underline{Z}_{Zarc} = \frac{R}{1 + (j\omega\tau)^\phi}
\f]

XML Tag                                 | Unit | Type                     | Explanation                                                                                 
----------------------------------------|------|--------------------------|--------------------------------------------------------------------------------
LookupTau                               | s    | [Object](xmlobject.html) | Value of tau
LookupOhmicResistance                   | Ohm  | [Object](xmlobject.html) | Value of the resistance R
LookupPhi                               |      | [Object](xmlobject.html) | Value of phi

If zarc elements are used, the minimal sample rate should be set by using the [option](xmloption.html) <**SampleRate**>:
\htmlinclude examples/samplerate_color.xml

Definition of a zarc element:
\htmlinclude examples/zarc_color.xml

Diffusion - Cotanh
----------
A warburg element with reflective boundary is added by setting the "class" attribute to **WarburgCotanh**.

XML Tag                 | Unit | Type                     | Default value | Explanation                                         
------------------------|------|--------------------------|---------------|-----------------------------------------------------
Attribute WithCapacity  |      | bool                     | true          | If set to false, no additional capacity is added to the ECM and the capacity can be modeled with a voltage source
Attribute RCCounter     |      | int > 0                  | 5             | Number of RC elements to be added
Sigma                   |      | [Object](xmlobject.html) |               | Value of sigma
Clim                    |      | [Object](xmlobject.html) |               | Value of the limiting capacity

If cotanh elements are used, the minimal sample rate should be set by using the [option](xmloption.html) <**SampleRate**>:
\htmlinclude examples/samplerate_color.xml

Definition of a Warburg-Cotanh:
\htmlinclude examples/warburg_coth_color.xml

Diffusion - Tanh
----------
A warburg element with transmissive boundary is added by setting the "class" attribute to **WarburgTanh**.

XML Tag                 | Unit | Type                     | Default value | Explanation                                         
------------------------|------|--------------------------|---------------|-----------------------------------------------------
Attribute RCCounter     |      | int > 0                  | 5             | Number of RC elements to be added
OhmicResistance         | Ohm  | [Object](xmlobject.html) |               | Value of sigma
Tau                     | s    | [Object](xmlobject.html) |               | Value of tau                     

If tanh elements are used, the minimal sample rate should be set by using the [option](xmloption.html) <**SampleRate**>:
\htmlinclude examples/samplerate_color.xml

Definition of a Warburg-Tanh:
\htmlinclude examples/warburg_tanh_color.xml

Battery
----------
This element represents a battery cell and is added by setting the "class" attribute to **CellElement**.

XML Tag                 | Unit        | Type                                                  | Default value                 | Explanation                                         
------------------------|-----        |-------------------------------------------------------|---------------                |-----------------------------------------------------
ThermalState            |             | [State object](xmlstate.html) of class "ThermalState" |                               | [Thermal state](xmlstate.html) of the cell, should be cached and used as parameter for lookup tables
Soc                     |             | [State object](xmlstate.html) of class "Soc"          |                               | [Soc](xmlstate.html) of the cell, should be cached and used as parameter for lookup tables
Children                |             | List of twoports                                      |                               | List of child elements that are connected in series 
ReversibleHeat          | W / (K * A) | [Object](xmlobject.html)                       | no reversible heat generation | Reversible heat generated by the cell

Definition of an example battery in full cell configuration:
\htmlinclude examples/cellelement_color.xml

Definition of an example battery in half cell configuration (see [halfcell simulation](xmlhalfcell.md)):
\htmlinclude examples/cellelement_halfcell_color.xml

Anode particle
--------------
This element represents an anode particle in a [halfcell simulation](xmlhalfcell.md) and is added by setting the "class" attribute to **AnodeElement**.

XML Tag                 | Type                                         | Default value                     | Explanation                                         
------------------------|----------------------------------------------|-----------------------------------|-----------------------------------------------------------------
Children                | List of twoports                             |                                   | List of child elements that are connected in series 
Soc                     | [State object](xmlstate.html) of class "Soc" | Soc of the parent cell, see below | [Soc](xmlstate.html) of the anode element, should be cached and used as parameter for lookup tables

If no soc is given, the capacity of the anode element will be equal to the capacity of the parent cell divided by the number of anode elements and the initial soc will be the same as that of the parent cell.
The children of this element must not be additional serial or parallel connections, only electrical elements are allowed. The values of voltage sources will be inverted, so that the cell OCV is the difference between the cathode and anode OCVs.

Cathode particle
----------------
This element represents a cathode particle in a [halfcell simulation](xmlhalfcell.md) and is added by setting the "class" attribute to **CathodeElement**.

XML Tag                 | Type                                         | Default value                     | Explanation                                         
------------------------|----------------------------------------------|-----------------------------------|-----------------------------------------------------------------
Children                | List of twoports                             |                                   | List of child elements that are connected in series 
Soc                     | [State object](xmlstate.html) of class "Soc" | Soc of the parent cell, see below | [Soc](xmlstate.html) of the anode element, should be cached and used as parameter for lookup tables

If no soc is given, the capacity of the cathode element will be equal to the capacity of the parent cell divided by the number of cathode elements and the initial soc will be the same as that of the parent cell.
The children of this element must not be additional serial or parallel connections, only electrical elements are allowed.