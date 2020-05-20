Electrical Objects     {#xmlelectrical}
===================

\~German

Nachfolgend sind alle elektrischen Objekttypen, aus denen ein Netzwerk aufgebaut werden kann, aufgelistet.
Für jeden Objekttyp wird der Aufbau des dazugehörigen XML-Elementes beschrieben und ein Codebeispiel gegeben.

<br/>

Allgemein
=========

Jedes XML-Element, das in der Konfiguration ein elektrisches Objekt repräsentiert, besitzt das Attribut "class".
Über dieses Attribut wird der Objekttyp zugeordnet.
Außerdem kann jedes XML-Element das Attribut "observable='true'" besitzen.
Durch dieses Attribut werden Strom, Spannung, Verlustleistung und ggf. Soc des Objektes berechnet und bei Verwendung des Datenloggers mit ausgegeben.

<br/>

Grundtypen
==========

<br/>

Widerstand
----------

__Objekttyp: "OhmicResistance"__

<**Object**>: [Parameterobjekt](xmlobject.html) mit Widerstandswert

Definition eines Widerstandes mit konstantem Widerstandswert:
\htmlinclude ohmic_color.xml


Kondensator
----------

__Objekttyp: "Capacity"__

<**Object**>: [Parameterobjekt](xmlobject.html) mit Kapazitätswert

Definition eines Kondensators mit konstantem Kapazitätswert:
\htmlinclude capacity_color.xml


Spannungsquelle
----------

__Objekttyp: "VoltageSource"__

<**Object**>: [Parameterobjekt](xmlobject.html) mit Spannungswert

Definition einer Spannungsquelle mit konstantem Spannungswert:
\htmlinclude voltage_color.xml


Parallelschaltung
----------

__Objekttyp: "ParallelTwoPort"__

<**Children**>: Enthält weitere XML-Elemente. Eines für jeden Zweig der Parallelschaltung.

Definition einer Parallelschaltung aus einem Kondensator und einem Widerstand:
\htmlinclude parallel_color.xml


Serienschaltung
----------

__Objekttyp: "SerialTwoPort"__

<**Children**>: Enthält weitere XML-Elemente, die in Serie verschaltet sind.

Definition einer Serienschaltung aus einem Kondensator und einem Widerstand:
\htmlinclude serial_color.xml

<br/><br/>

Komplexere Bauelemente
======================

<br/>

Zarc-Element
----------

__Objekttyp: "ZarcElement"__

<**LookupTau**>: [Parameterobjekt](xmlobject.html) für Tau. Dieses Parameterobjekt verwendet ty­pi­scher­wei­se die zwei [Zustände](xmlstate.html) "ThermalState" und "Soc" des übergeordneten "CellElements".

<**LookupOhmicResistance**>: [Parameterobjekt](xmlobject.html) für Widerstand. Dieses Parameterobjekt verwendet ty­pi­scher­wei­se die zwei [Zustände](xmlstate.html) "ThermalState" und "Soc" des übergeordneten "CellElements".

<**LookupPhi**>: [Parameterobjekt](xmlobject.html) für Phi. Dieses Parameterobjekt verwendet ty­pi­scher­wei­se die zwei [Zustände](xmlstate.html) "ThermalState" und "Soc" des übergeordneten "CellElements".

Bei der Verwendung von Zarc-Elementen sollte der Benutzer die minimale Samplerate als [Option](xmloption.html) <**SampleRate**> festlegen:
\htmlinclude examples/samplerate_color.xml

Definition eines Zarc-Elementes:
\htmlinclude examples/zarc_color.xml

Diffusion - Cotanh
----------

__Objekttyp: "WarburgCotanh"__
Attribut "WithCapacity":  "True" (default)
Wenn das Attribut **WithCapacity** auf false gesetzt wird, dann wird keine zusätzliche Kapazität zum Ersatzschaltbild hinzugefügt und diese kann über eine Spannungsquelle modelliert werden.

Attribut "RCCounter":  "5" (default)
Über das Attribut **RCCounter** kann die Genauigkeit der Approximation des Diffusionelementes eingestellt werden. Je höher dieser Wert ist, desto genau die Approximation. Die SampleRate limitiert hierbei auch die Anzahl der RC-Glieder.

<**Sigma**>: [Parameterobjekt](xmlobject.html) für Sigma. Dieses Parameterobjekt verwendet ty­pi­scher­wei­se die zwei [Zustände](xmlstate.html) "ThermalState" und "Soc" des übergeordneten "CellElements".

<**Clim**>: [Parameterobjekt](xmlobject.html) für limiting Capacity. Dieses Parameterobjekt verwendet ty­pi­scher­wei­se die zwei [Zustände](xmlstate.html) "ThermalState" und "Soc" des übergeordneten "CellElements".


Bei der Verwendung von Cotanh-Elementen sollte der Benutzer die minimale Samplerate als [Option](xmloption.html) <**SampleRate**> festlegen:
\htmlinclude examples/samplerate_color.xml

Definition eines Warburg-Cotanh:
\htmlinclude examples/warburg_coth_color.xml

Diffusion - Tanh
----------

__Objekttyp: "WarburgTanh"__

Attribut "RCCounter":  "5" (default)
Über das Attribut **RCCounter** kann die Genauigkeit der Approximation des Diffusionelementes eingestellt werden. Je höher dieser Wert ist, desto genauer die Approximation. Die SampleRate limitiert hierbei auch die Anzahl der RC-Glieder.

<**OhmicResistance**>: [Parameterobjekt](xmlobject.html) für den Ohmschen Anteil. Dieses Parameterobjekt verwendet ty­pi­scher­wei­se die zwei [Zustände](xmlstate.html) "ThermalState" und "Soc" des übergeordneten "CellElements".

<**Tau**>: [Parameterobjekt](xmlobject.html) für die Relaxationskonstante. Dieses Parameterobjekt verwendet ty­pi­scher­wei­se die zwei [Zustände](xmlstate.html) "ThermalState" und "Soc" des übergeordneten "CellElements".


Bei der Verwendung von Tanh-Elementen sollte der Benutzer die minimale Samplerate als [Option](xmloption.html) <**SampleRate**> festlegen:
\htmlinclude examples/samplerate_color.xml

Definition eines Warburg-Tanh:
\htmlinclude examples/warburg_tanh_color.xml




Batterie
----------

__Objekttyp: "CellElement"__

<**ThermalState**>: [Zustandsobjekt](xmlstate.html) vom Typ "ThermalState". Dieses Objekt sollte im Cache zwischengespeichert werden, damit es für die Nachschlagetabellen weiterverwendet werden kann.

<**Soc**>: [Zustandsobjekt](xmlstate.html) vom Typ "Soc". Dieses Objekt sollte im Cache zwischengespeichert werden, damit es für die Nachschlagetabellen weiterverwendet werden kann.

<**Children**>: Enthält weitere XML-Elemente, die in Serie verschaltet sind. Eine Batterie kann aus beliebigen elektrischen Bauteilen aufgebaut sein.

<**ReversibleHeat**/> Beschreibt die reversible Wärmegeneration siehe auch [thermischesModell](xmlthermalmodel.html)

Definition einer Beispielbatterie in Vollzellendarstellung:
\htmlinclude examples/cellelement_color.xml

Definition einer Beispielbatterie in Halbzellendarstellung:
\htmlinclue example/cellelement_halfcell_color.xml


Anodenpartikel
--------------

__Objekttyp: "AnodeElement"__

<**Children**>: Enthält weitere XML-Elemente, die in Serie verschaltet sind. Darf nur elektrische Bauelement beinhalten und keine Verschaltungselemente (SerialTwoPort, ParallelTwoPort) 


Kathodenpartikel
----------------

__Objekttyp: "CathodeElement"__

<**Children**>: Enthält weitere XML-Elemente, die in Serie verschaltet sind. Darf nur elektrische Bauelement beinhalten und keine Verschaltungselemente (SerialTwoPort, ParallelTwoPort) 

\~English

This page lists all types of electrical objects that can be used to build a network, including the structure of the XML elements.

General
=========

Each XML-element that describes an electrical object has the attribute "class". This attribute is used to determine the object type. Additionally, the attribute "observale" can be set to true. This causes current, voltage, dissipation power and (if applicable) soc to be calculated and added to the simulation output.

Basic types
==========

Resistance
----------

__Object type: "OhmicResistance"__

<**Object**>: [Parameter object](xmlobject.html) with resistance value

Definition of a resistance with constant value:
\htmlinclude ohmic_color.xml


Capacity
----------

__Object type: "Capacity"__

<**Object**>: [Parameter object](xmlobject.html) with capacity value

Definition of a capacity with constant value:
\htmlinclude capacity_color.xml


Voltage Source
----------

__Object type: "VoltageSource"__

<**Object**>: [Parameter object](xmlobject.html) with voltage value

Definition of a voltage source with constant value:
\htmlinclude voltage_color.xml


Parallel Connection
----------

__Object type: "ParallelTwoPort"__

<**Children**>: Contains child elements that are connected in parallel

Definition of a parallel connection of a capacitor and a resistor:
\htmlinclude parallel_color.xml


Serial Connection
----------

__Object type: "SerialTwoPort"__

<**Children**>: Contains child elements that are connected in series

Definition of a serial connection of a capacitor and a resistor:
\htmlinclude serial_color.xml

<br/><br/>

More complex elements
======================

<br/>

Zarc Element
----------

__Object type: "ZarcElement"__

<**LookupTau**>: [Parameter object](xmlobject.html) for the time constant tau. This parameter object typically uses the two [states](xmlstate.html) "ThermalState" and "Soc" of the containing "CellElements".

<**LookupOhmicResistance**>: [Parameter object](xmlobject.html) for the resistance. This parameter object typically uses the two [states](xmlstate.html) "ThermalState" and "Soc" of the containing "CellElements".

<**LookupPhi**>: [Parameter object](xmlobject.html) for phi. This parameter object typically uses the two [states](xmlstate.html) "ThermalState" and "Soc" of the containing "CellElements".

If zarc elements are used, the minimal sample rate should be set by using the [option](xmloption.html) <**SampleRate**>:
\htmlinclude examples/samplerate_color.xml

Definition of a zarc element:
\htmlinclude examples/zarc_color.xml

Diffusion - Cotanh
----------

__Object type: "WarburgCotanh"__
Attribute "WithCapacity":  "True" (default)
If the attribute **WithCapacity** is set to false, no additional capacity is added to the ECM and the capacity can be modeled with a voltage source.

Attribute "RCCounter":  "5" (default)
The argument **RCCounter** can be used to adjust how accurately the diffusion element is approximated. The higher this value is, the more exact the approximation gets. The number of RC elements is limited by the SampleRate option.

<**Sigma**>: [Parameter object](xmlobject.html) for sigma. This parameter object typically uses the two [states](xmlstate.html) "ThermalState" and "Soc" of the containing "CellElements".

<**Clim**>: [Parameter object](xmlobject.html) for the limiting capacity. This parameter object typically uses the two [states](xmlstate.html) "ThermalState" and "Soc" of the containing "CellElements".


If cotanh elements are used, the minimal sample rate should be set by using the [option](xmloption.html) <**SampleRate**>:
\htmlinclude examples/samplerate_color.xml

Definition of a Warburg-Cotanh:
\htmlinclude examples/warburg_coth_color.xml

Diffusion - Tanh
----------

__Object type: "WarburgTanh"__

Attribute "RCCounter":  "5" (default)
The argument **RCCounter** can be used to adjust how accurately the diffusion element is approximated. The higher this value is, the more exact the approximation gets. The number of RC elements is limited by the SampleRate option.

<**OhmicResistance**>: [Parameter object](xmlobject.html) for the resistance. This parameter object typically uses the two [states](xmlstate.html) "ThermalState" and "Soc" of the containing "CellElements".

<**Tau**>: [Parameter object](xmlobject.html) for the time constant tau. This parameter object typically uses the two [states](xmlstate.html) "ThermalState" and "Soc" of the containing "CellElements".


If cotanh elements are used, the minimal sample rate should be set by using the [option](xmloption.html) <**SampleRate**>:
\htmlinclude examples/samplerate_color.xml

Definition of a Warburg-Tanh:
\htmlinclude examples/warburg_tanh_color.xml




Battery
----------

__Object type: "CellElement"__

<**ThermalState**>: [State object](xmlstate.html) of type "ThermalState". This object should be stored in the cache so that it can be used as a parameter for lookup tables.

<**Soc**>: [State object](xmlstate.html) of type "Soc". This object should be stored in the cache so that it can be used as a parameter for lookup tables.

<**Children**>: Contains elements that are connected in series. A battery can contains arbitrary electrical elements.

<**ReversibleHeat**/> Describes the reversible heat generation, see the [thermal model](xmlthermalmodel.html)

Definition of an example battery in full cell configuration:
\htmlinclude examples/cellelement_color.xml

Definition of an example battery in half cell configuration:
\htmlinclue example/cellelement_halfcell_color.xml


Anode particle
--------------

__Object type: "AnodeElement"__

<**Children**>: Contains elements that are connected in series. Can only contain electrical elements and no serial or parallel connection elements.


Cathode particle
----------------

__Object type: "CathodeElement"__

<**Children**>: Contains elements that are connected in series. Can only contain electrical elements and no serial or parallel connection elements.