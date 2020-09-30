XML options     {#xmloption}
===============
Global options can be set within the <**Options**> node. Depending on the simulation type, the following options can be set:

Electrical model
===============
XML Tag                                 | Unit/Type | Default value | Explanation                                                                                 
----------------------------------------|-----------|---------------|------------------------------------------------------------------------
StepTime                                | s         | 1.0           | Maximum time that is simulated electrically before switching to the thermal model, see the [simulation cycle](simulation_cycle.html)
PowerStopCriterion                      | %         | 1.0           | If a [power profile](xmlexecutable.html) is used, this determines the maximum deviation from the power value given in the profile before the simulation step is stopped and the current is readjusted.
SocStopCriterion                        | %         | 5.0           | Maximum value any SOC can change by during an electrical simulation step before switching to the thermal model. This is a percentage of the cell capacity, not of the SOC value at the beginning of the timestep.
ThermalStopCriterionInDegreeC           | °C        | 5.0           | Maximum temperature change in any thermal state during a thermal simulation step before the electrcal simulation is reset.
Cycles                                  | int > 0   | 1             | Gives the number of times the current/power profile is executed by the [thermal-electrical standalone](xmlexecutable.html)
SampleRate                              | Hz        | 1.0e6         | Sample rate for the automatic simplification of fast time constants, see [automatic simplification](xmlvereinfachung.html)

Thermal model
===============
XML Tag                                 | Unit/Type | Default value | Explanation                                                                                 
----------------------------------------|-----------|---------------|------------------------------------------------------------------------
AirTemperature                          | °C        |               | Ambient temperature
- Attribute ReadFromFile                | bool      |               | If this attribute is given, the air temperature is read as a profile from the specified file. The file has to be in CSV format, similarly to the current/power profile.
ConvectionActivated                     | bool      |               | De-/activates free convection as default cooling mechanism
- Attribute anvancedAreaAggregation   | bool      | true          | If set to false, all coplanar areas are combined to one single area and its specific dimensions are calculated. If set to true, the position of the thermal block is analyzed to separate the calculations for coplanar but unconnected areas.
RadiationActivated                      | bool      |               | De-/activates thermal radiation as standard cooling
GeometricalTolerance                    |           |               | Sets the geometrical tolerance of the thermal model
- Attribute length                    | m         | 1.0e-6        | Maximum distance between two objects to be assumed as connected
- Attribute angleInDegrees            | °         | 1.0e-3        | Maximum difference between two angles to be regarded as equal
- Attribute percentOfQuantity         | %         | 0.1           | Maximum difference between two quantities to be regarded as equal

Aging model {#aging}
============
These options for the aging simulation must be added in <**Configuration**><**Options**><**Aging**>.

XML Tag                                 | Unit/Type     | Default value | Explanation                                                                                 
----------------------------------------|---------------|---------------|----------------------------------------------------------------------------------------
CalculationCycles                       | int > 0       | 1             | Number of times the current or power profile is simulated before each aging step
SteadyStateCycles                       | CalculationCycles > int >= 0      | 0             | Number of cycles in CalculationCycles that are simulated without collecting data for aging
AgingSteps                              | int > 0       | 1             | Number of aging steps
AgingStepTime                           | days          |               | Duration of each aging step