State objects     {#xmlstate}
===============
There are two different types of state objects: A thermal state (temperature and heat generation) and a state of charge (SoC).
The state objects are used in connection with a battery.
Apart from the description of the current battery condition, these states are also used for [parameter objects](xmlobject.html) with lookup tables.
As a result, the parameters of the electrical object could be calculated as a function of battery temperature and SoC.

Abstract
=========

Each xml element, which represents a state object in the configuration, possesses the attribute "class".
The object type is defined by this attribute.
State objects are normally stored in the cache and are used multiple times via object references for parameter objects with lookup tables.

Thermal state
=========

This state stores the heat generation by electrical power loss and the temperature.

Object type: __"ThermalState"__

XML Tag                                 | Unit | Type                     | Explanation                                                                                 
----------------------------------------|------|--------------------------|--------------------------------------------------------------------------------
InitialTemperature                      | °C   | double                   | Value of the initial temperature

Definition of a thermal state:
\htmlinclude thermalstate_color.xml


State of charge
=========

Object type: __"Soc"__

XML Tag                                 | Unit | Type                     | Explanation                                                                                 
----------------------------------------|------|--------------------------|--------------------------------------------------------------------------------
InitialCapacity                         | Ah   | double                   | Initial capacity. The value will only change during the simulation if the aging model is used.
InitialSoc                              | %    | double                   | Initial state of charge from 0 to 100

SOC values are not limited and can be greater than 100% or smaller than 0%.

Definition of a state of charge:
\htmlinclude socstate_color.xml


Current, Voltage
=========

The current and voltage of an electric component can be used as an input for lookups. Therefore non-linear behavior e.g. for the Butler–Volmer equation can be approximated. 
Object type: **"ElectricalState"**

XML Tag                                 | Type   | Explanation                                                                                 
----------------------------------------|--------|--------------------------------------------------------------------------------
Attribute StateType                     | string | "current", "voltage" or "power". 

\htmlinclude xmlCurrentState_color.xml
