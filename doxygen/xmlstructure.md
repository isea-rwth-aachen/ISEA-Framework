XML description language    {#xmlsprache}
========================
The configuration of the simulation scenarios is done via a particular xml language.
For each scenario, a xml file has to be created by the user.
This xml file can be read and evaluated by the program.

Description of the different object groups:
-   [Electrical objects](xmlelectrical.html)
-   [Parameter objects](xmlobject.html)
-   [State objects](xmlstate.html)
-   [Thermal blocks](xmlthermalblock.html)
-   [Coolings](xmlcooling.html)
-   [Thermal materials](xmlthermalmaterial.html)
-   [Options](xmloption.html)
-   [Observer Filters](xmlobserver.html)


Furthermore:
-   The description of the xml parameterization and the scan process of the [thermal model](xmlthermalmodel.html), which complement the xml structure shown at this point.
-   The peculiarities in the [use of the Simulink-model](xmlcinterface.html).

<br/>

Basic structure
===========

__The base structure of a configuration file:__
\htmlinclude xmlstructure_color.xml


__The configuration file can be divided in three sections__

-   **Options**<br/>
    Global settings can be adjusted within the [options](xmloption.html)

-   **CustomDefinitions**<br/>
    Templates can be defined within this element.
    Afterwards these templates can be used within the [electrical network](xmlelectrical.html) and within the [thermal structure](xmlthermalblock.html).

-   **RootElement**<br/>
    The RootElement is the root node of the electrical network.
      The attribute "class" defines the corresponding object.
      In most cases this object is a ParallelTwoport or a SerialTwoport.

Only necessary if the [thermal model](thermalmodel.xml) is used:
-   **ThermalMaterials**<br/>
    Contains all [materials](xmlthermalmaterial.html), which are used for the thermal model.

-   **CachedCoolings**<br/>
    The <**Cooling**>-tags, which define the type and strength of the cooling within the cooling blocks, can be cached here.
  

-   **CoolingBlocks**<br/>
    Contains [coolings](xmlcooling.html), which are used for the thermal model.
    These are generated here, to be passed via object references afterwards.
    Therefore, the attribute cache="true" has to be set.
    Cooling can also be defined outside of the RootElement.



The electrical network is build with different objects (base class TwoPort).
Each object in the configuration file is represented by an xml element.
The xml elements contain, depending on the object, different attributes and child elements.
A xml element describing a "ParallelTwoport" , for example, contains a child element "Children".
This element for its part contains several child elements as branches of the parallel connection.
A listing of all supported electrical object and a description of each xml element can be found [here](xmlelectrical.html).


__ Besides the electrical objects there are several other possible object types:__

-   **State objects (States)**<br/>
   There are two different state objects:
    +   Soc <br/>
        The object Soc stores the state of charge of a battery
    +   ThermalState <br/>
        The object ThermalState stores the heat generation through power loss and the temperature of an electrical component.
<br/><br/>
-   **Parameter object (object)**<br/>
    A parameter object describes parameters, such as the resistance value of an ohmic resistor or the voltage value of a voltage source.<br/>
    This object group contains several types of parameter objects:
    +   Constant parameter values
    +   Parameter values evaluated according to given functions
    +   Lookup tables (depending on Soc or temperature or Soc and temperature)


Template referencing
====================

In order to create a template, the xml element is added to the section "CustomDefinitions".
The name of the element has to be unambiguously.
Afterwards, the template can be referenced at other points (within "CustomDefinitions" and "RootElement") via the attribute "cacheref".

An example:
\htmlinclude basic_example_color.xml

Object referencing {#xml_object_refs}
============

The xml language permits object referencing.
Object referencing is passing the same object multiple times to other object.
This is especially useful for the description of state objects.

The attribute "cache='true'" has to be used when defining an xml element in order to enable references.
Thus, the object is stored in the cache using its element name.
The object can be referenced via the cacheref attribute at any point.
On further caching of an object with the same name, the previous object will be overwritten (only the pointer).

An example:
\htmlinclude xmlobject_ref_color.xml

The state objects "ThermalState" and "Soc" are cached in this example.
The parameter objects "LookupTau", "LookupOhmicResistance", "LookupPhi" are using this state objects.
As opposed to referencing templates, no further objects will be created when referencing objects.


Example configuration file
============================

A simple simulation scenario with a pure electrical simulation with a total of 16 batteries:
\htmlinclude testconfig_color.xml


