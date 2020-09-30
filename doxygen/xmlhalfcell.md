Half cell simulation     {#xmlhalfcell}
====================

The half cell simulation enables a description of a battery cell that is aware of the electrodes. The ECM is altered so that the open circuit voltage of the full cell is a superposition of anode and cathode potential. The cell's impedance is also diviced into anode and cathode.

<br/>

XML structure
==========

\htmlinclude examples/halfcell_example_color.xml

<**Anode**> This element contains all electrical elements describing the anode

<**AnodeElement**> Each element with the class "AnodeElement" describes a single anode particle with its own SOC to describe the degree of lithiation. The capacity of the cell is equally distributed among all anode particles. In the example above, the anode is described by two anode particles that are connected in parallel.

<**Cathode**> This element contains all electrical elements describing the cathode

<**CathodeElement**> Each element with the class "CathodeElement" describes a single cathode particle with its own SOC to describe the degree of lithiation. The capacity of the cell is equally distributed among all cathode particles. In the example above, the cathode is described by a single particle.

An in-depth example for a half cell simulation can be found in scenarions/thermal_electrical/tesla_halfcell_color.xml
