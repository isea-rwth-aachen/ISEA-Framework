Aging Simulation       {#aging_simulation}
===================

In the aging simulation, the aging behavior of a battery cell is derived from its thermal-electrical behavior. The state of aging is described by a decrease in capacity and an increase in resistance as well as an SOC offset.

The aging simulation uses discrete aging steps. At first, the thermal-electrical simulation is run. After each step, data that is needed for the aging calculation is collected. Once the thermal-electrical simulation is finished, the aging is calculated. If more than one aging step should be calculated, the results of the aging simulation are applied to the electrical elements and the thermal-electrical simulation is started again. 

Because the time scale of the aging simulation is much larger than that of the electrical simulation, the aging is scaled up to a larger time step. The XML file used for the simulation specifies a number of calculation cycles and an aging step time. The number of calculation cycles determines how often the given current or power profile is simulated before each aging step. The aging simulation assumes that the thermal-electrical behavior is the same for the rest of the aging step time.

Example
=======

A simulation is started with an XML file containing
~~~~~~~~~~~~~~~~~~~~~~
<AgingStepTime>30</AgingStepTime>
<AgingSteps>2</AgingSteps>
<CalculationCycles>2</CalculationCycles>
~~~~~~~~~~~~~~~~~~~~~~

and the current profile
~~~~~~~~~~~~~~~~~~~~~~
# type=current
0, 3
1, -3
2, 0
~~~~~~~~~~~~~~~~~~~~~~

Because the XML file specifies two calculation cycles, the current profile is simulated two times. This means that all current values are automatically repeated after the profile has ended, resulting in

~~~~~~~~~~~~~~~~~~~~~~
# type=current
2, 3
3, -3
4, 0
~~~~~~~~~~~~~~~~~~~~~~

During this thermal-electrical simulation, the aging simulation collects the data required to perform the next aging step. The electrical simulation took 4 seconds in total, but an aging step time of 30 days is specified in the XML file, so the aging simulation assumes that all 129600 cycles that make up 30 days are the same as the 2 cycles that have actually been simulated. After the aging is calculated, the cahnges in capacity, resistance and SOC are applied to the electrical cellelements and the entire simulation is carried out a second time for the second aging step. 

Halfcell Simulation
===================

The aging model can also be used in combination with a [halfcell simulation](xmlhalfcell.html). Each anode or cathode element can have its own aging effects that are calculated seperately for each element. If an aging effect is added to an entire cell in a halfcell simulation, it is automatically added to each electrode element instead.

To calculate the change of the cell capacity in a halfcell simulation, the cell voltage at 0 and 100 % SOC is stored before the simulation starts. After each aging step, it is determined at what charge the cell reached those voltage limits. The difference between the stored charge at maximum and minimum voltage is then used as the new cell capacity.

Calendric Aging
===============

The calendric aging depends on the cell age t and is calculated using the formulas

\f{eqnarray*}{
C_i & = & 1 - ((1 - C_{i-1})^\frac{1}{n} + \Delta t \cdot (\alpha_{C,i})^\frac{1}{n})^n \\
R_i & = & 1 + ((R_{i-1} - 1)^\frac{1}{n} + \Delta t \cdot (\alpha_{R,i})^\frac{1}{n})^n \\
\Delta Q & = & (1 - C_i) \cdot C_0 \cdot SOC
\f}

\f$ C_i \f$ and \f$ R_i \f$ denote the capacity and resistance relative to their initial values \f$ C_0 \f$ and \f$ R_0 \f$. The stress factors \f$ \alpha_C \f$ and \f$ \alpha_R \f$ are functions of the cell voltage and temperature. The exponent \f$ n \f$ is given as a constant between 0 and 1 and can be different for capacity and resistance. Values with the subscript \f$ i \f$ change in every aging step, so that \f$ i-1 \f$ denotes a value from the previous aging step.

Cyclic Aging
============

The cyclic aging depends on the chargethroughput Q and is calculated using the formulas

\f{eqnarray*}{
C_i & = & 1 - ((1 - C_{i-1})^\frac{1}{n} + \Delta Q \cdot (\beta_{C,i})^\frac{1}{n})^n \\
R_i & = & 1 + ((R_{i-1} - 1)^\frac{1}{n} + \Delta Q \cdot (\beta_{R,i})^\frac{1}{n})^n \\
\Delta Q & = & (1 - C_i) \cdot C_0 \cdot SOC
\f}

The stress factors \f$ \beta_C \f$ and \f$ \beta_R \f$ are functions of the average cell voltage and the depth of discharge. 

Anode Overhang
==============
The anode overhang effect describes the change in capacity caused by an exchange of lithium with the anode overhang. This effect only works in a halfcell simulation by changing the balancing which affects the usable cell capacity.

The exchanged charge is calculated is dependence of a time constant \f$ \tau \f$ using the formula

\f[
Q = C (V_{anode} - V_{overhang}) \cdot (1 - e^\frac{t}{\tau})
\f]

The capacity \f$ C \f$ is calculated by linearising the overhang as a capacitor, resulting in

\f[
C = \frac{SOC_{overhang} \cdot C_{Overhang}}{V_{overhang}}
\f]

The time constant \f$ \tau \f$ depens on the temperature \f$ T \f$ and is calculated from two given constants \f$ A \f$ and \f$ E \f$:

\f[
\tau = A \cdot e^\frac{E}{T}
\f]

XML Format
==========

A detailed example for an aging simulation can be found at scenarions/aging/sanyo_18651_single_cell_aging.xml .

Global Options
--------------

These options for the aging simulation must be added in <Configuration><Options><Aging>.

XML Tag                                 | Type          | Unit      | Explanation                                                                                 
----------------------------------------|---------------|-----------|----------------------------------------------------------------------------------------
CalculationCycles                       | int > 0       |           | Number of times the current or power profile is simulated before each aging step
SteadyStateCycles                       | int >= 0      |           | Number of cycles that are simulated without collecting data for aging before the cycles specified in CalculationCycles are simulated
AgingSteps                              | int > 0       |           | Number of aging steps
AgingStepTime                           | double > 0    | day       | Duration of each aging step

Observer
--------

Filters for the aging simulation can be added in <Configuration><Observer><Aging>. At this point, only the MatlabFilter and the StdoutFilter are supported. For more details, see [Observer](xmlobserver.html).

Cell Aging
----------

The aging for each cell can be specified using the element <Aging> inside the cell's XML element.
The aging element contains a list of aging effects affecting the cell.

If the cell is not new at the beginnig of the simulation, the initial cell age and charge throughput can be set by adding an <AgingState> element with the following children:

XML Tag                                 | Type          | Unit      | Explanation                                                                                 
----------------------------------------|---------------|-----------|----------------------------------------------------------------------------------------
CellAge                                 | double        | day       | Cell age at the start of the simulation
ChargeThroughput                        | double        | Ah        | Charge throughput at the start of the simulation

Halfcell Simulation
-------------------

The general XML format of a halfcell simulation is described [here](xmlhalfcell.html). Each element of the classes AnodeElement and CathodeElement con contain an <Aging> element and an <AgingState> element that work in the same way as those of a cell. An example for a halfcell simulation is given in scenarions/aging/sanyo_18651_single_cell_aging.

Calendric Aging
---------------

The calendric aging is used by adding an element with the class attribute "CalendarianAging" to the cell's aging element.

XML Tag                                 | Type          | Unit      | Explanation                                                                                 
----------------------------------------|---------------|-----------|----------------------------------------------------------------------------------------
TimeExponent                            | double        |           | Exponent n in the formulas used to calculate the calendric aging
FormulaCapacity                         | string        |           | Formula used to calculate the value of alpha. The symbols V and T can be used for the cell's voltage and temperature
FormulaResistance                       | string        |           | Formula used to calculate the value of alpha. The symbols V and T can be used for the cell's voltage and temperature
MinAlphaCapacity                        | double        |           | Minimum value for the stress factor. If a lower value is calculated, this value is used instead.
MinAlphaResistance                      | double        |           | Minimum value for the stress factor. If a lower value is calculated, this value is used instead.
InitialCapacityFactor                   | double        | 0 - 1     | Initial value for the capacity factor. Optional.
InitialResistanceFactor                 | double        | 0 - 1     | Initial value for the capacity factor. Optional.

Cyclic Aging
---------------

The cyclic aging is used by adding an element with the class attribute "CyclicalAging" to the cell's aging element.

XML Tag                                 | Type          | Unit      | Explanation                                                                                 
----------------------------------------|---------------|-----------|----------------------------------------------------------------------------------------
ChargeExponentCapacity                  | double        |           | Exponent n in the formula used to calculate the capacity for the cyclic aging
ChargeExponentResistance                | double        |           | Exponent n in the formula used to calculate the resistance for the cyclic aging
FormulaCapacity                         | string        |           | Formula used to calculate the value of beta. The symbols meanV and deltaDOD can be used for the cell's average voltage and depth of discharge
FormulaResistance                       | string        |           | Formula used to calculate the value of beta. The symbols meanV and deltaDOD can be used for the cell's average voltage and depth of discharge
MinBetaCapacity                         | double        |           | Minimum value for the stress factor. If a lower value is calculated, this value is used instead.
MinBetaResistance                       | double        |           | Minimum value for the stress factor. If a lower value is calculated, this value is used instead.
InitialCapacityFactor                   | double        | 0 - 1     | Initial value for the capacity factor. Optional.
InitialResistanceFactor                 | double        | 0 - 1     | Initial value for the capacity factor. Optional.

Anode Overhang
---------------

The anode overhang is used by adding an element with the class attribute "AnodeOverhang" to the anode's aging element.

XML Tag                                 | Type          | Unit      | Explanation                                                                                 
----------------------------------------|---------------|-----------|----------------------------------------------------------------------------------------
Voltage                                 | Object        | V         | Voltage of the anode overhang, should depend on the overhang SOC
ActivationCoefficient                   | double        | K         | Constant E used to calculate the time constant
TauCoefficient                          | double        | day       | Constant A used to calculate the time constant
SocWhereOffsetIsZero                    | double        | %         | The overhang SOC that results in no change to the balancing. If this is not the same as the initial overhang SOC, the anode SOC is immediately changed before the simulation starts.
Soc                                     | SOC state     |           | Soc object that specifies the overhang capacity and the initial overhang SOC