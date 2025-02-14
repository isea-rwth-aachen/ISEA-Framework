Using the simulink model     {#xmlcinterface}
===============

\~German
Weitere Themen:
-   [Zusätzliche Simulink Bibliothek](bibs.html)
-   [\<Options\>-Knoten](xmloption.html)


Simulink Multicell-Interface
=========
Das Multicell-Interface definiert die Schnittstelle von Simulink zum Framework. Innerhalb des MultiCellInterface liegt die S-Function, welche die unterschiedliche C++ Funktionen gekapselt, die das Verhalten des zu simulierenden Batteriesystem bestimmen.
Konfiguriert werden kann das Modell über einen Konfigurationstring der im Matlab Workspace hinterlegt ist. Prinzipiell gibt es drei verschiedene MultiCellInterfaces für die Anwendungsfälle rein elektrische Simulation,
rein thermische Simulation und gekoppelte thermische/elektrische Simulation. Die gekoppelte Variante steht sowohl als kontinuierlicher Matlab-Block als auch als diskreter Block zur Verfügung.

![MultiCellInterface, Eingänge und Ausgänge](MultiCellInterface.png)

S-Function Argumente
---------
Die S-Funktion erwartet 4 Argumente, CONFIG_STR_A, CONFIG_STR_B, CONFIG_STR_C und DISPLAY_VERSION.
Die ersten drei stellen dabei Variablen für die Konfigurationsdatei dar und werden vor begin der Simulation konkatiniert und ausgewertet.
In einer reinen Simulink-Simulation ist es aussreichend in den CONFIG_STR_A den Konfigurationsstring zu hinterlegen und die Anderen als leer zu kennzeichnen.
<pre>
    CONFIG_STR_A = fileread('XYL.xml');
    CONFIG_STR_B = '';
    CONFIG_STR_C = '';
</pre>


<b> ACHTUNG </b>

Für das Echtzeitboard ist es erforderlich, dass die Strings zu Doubles konvertiert werden. Es muss darauf geachtet werden, dass die einzelnen Konfigurationsstrings nicht zu lang sind.

<pre>
    CONFIG_STR_A = convert2double( fileread('XYL.xml') );
    CONFIG_STR_B = '';
    CONFIG_STR_C = '';
</pre>




Inputports
---------

Die Inputports der verschiedenen MultiCellInterfaces sind in der folgenden Tabelle angegeben:


|Input Ports       | Elektrisches \n Modell | Thermisches \n Modell | Thermisch-Elektrisches \n Modell | Beschreibung |
|------------------| :--------------------: | :-------------------: | :------------------------------: | :----------- |
|Reset             | x | x | x | Wenn Reset=0 ist, passiert nichts. Andernfalls wird das Modell zurückgesetzt. \n Das bedeutet, dass die laufende Simulation endet und das Modell komplett neu mit Hilfe der xml-Datei aufgebaut wird. |
|I_Bat_Source      | x | o | x | Der Ladestrom in die Batterie zum jeweiligen Zeitpunkt. |
|T_Ambient         | o | x | x | Die Umgebungstemperatur zum jeweiligen Zeitpunkt. |
|P_Thermal         | o | x | o | Die Verlustleistung der Batteriezellen zum jeweiligen Zeitpunkt. \n Die Dimension dieses Inputports ist die Anzahl der Zellen in der Batterie. |
|I_Delta_Multicell | x | o | x | Der Ausgleichsstrom (Balancing Current) zwischen den Zellen der Batterie zum jeweiligen Zeitpunkt. \n Dies ist der Strom, der von einem BMS zusätzlich in die einzelne Zelle geladen wird. \n Die Dimension dieses Inputports ist die Anzahl der Zellen in der Batterie. |


x = Port vorhanden,
o = Port vorhanden und ohne Funktion,
"leer" = Port nicht vorhanden


Outputports
---------

|Output Ports      | Elektrisches \n Modell | Thermisches \n Modell | Thermisch-Elektrisches \n Modell | Beschreibung |
|------------------| :--------------------: | :-------------------: | :------------------------------: | :----------- |
|SignalBus         | x | x | x | Fasst mehrere Signale in einem Bus zusammen. \n **Details siehe unten.** |
|V_Bat             | x | x | x | Spannung über das gesamte Batteriepack. |
|V_Batt_Cell       | x | x | x | Spannung über die einzelnen Batteriezellen. \n Die Dimension dieses Outports ist die Anzahl der Zellen in der Batterie. |


x = Port vorhanden,
o = Port vorhanden und ohne Funktion,
"leer" = Port nicht vorhanden


|SignalBus         | Elektrisches \n Modell | Thermisches \n Modell | Thermisch-Elektrisches \n Modell | Beschreibung |
|------------------| :--------------------: | :-------------------: | :------------------------------: | :----------- |
|Current vector                  | x |   | x | Strom in die einzelnen Batteriezellen. \n Die Dimension dieses Outports ist die Anzahl der Zellen in der Batterie. |
|Ohmic heat \n generation vector | x |   | x | Verlustleistung produziert in den einzelnen Batteriezellen. \n Die Dimension dieses Outports ist die Anzahl der Zellen in der Batterie. |
|State of charge Vector          | x |   | x | State of Charge der einzelnen Batteriezellen. \n Die Dimension dieses Outports ist die Anzahl der Zellen in der Batterie. |
|Surface SOC vector              | x |   | x | Oberflächen-State-of-Charge der einzelnen Batteriezellen. \n Die Dimension dieses Outports ist die Anzahl der Zellen in der Batterie. |
|MEM_USED_OR_FRAGMENTED          | x | x | x | Wenn diese Funktionalität nicht implementiert ist, wird immer 0 angezeigt. |
|Total Current                   | x |   | x | Strom in das gesamte Batteriepack. |
|Total heat generation           | x |   | x | Gesamtverlustleistung des Batteriepacks. |
|Total amount of Batteries       | x |   | x | Anzahl der Batteriezellen in dem Batteriepack. |
|Number of used states           | x | x | x | Anzahl der verwendeten Zustände für den Gleichungslöser. |
|Emplacement points              |   | x |   | Anzahl der verwendeten Zustände für den Gleichungslöser. |
|Cell temperature vector         |   | x |   | Temperaturen der einzelnen Zellelemente. |
|Temperature vector              |   |   | x | Temperaturen der einzelnen Zellelemente. |

x = Signal vorhanden,
"leer" = Signal nicht vorhanden


\ifnot RELEASE_DOCU
Falls mit Ausgabe der internen Zustände kompiliert, erhält die S-Function innerhalb des MulticellInterface zwei weitere Ausgänge:
- **State vector:** Die Zustände, die dem Gleichungslöser übergeben werden.
- **Dxdt vector:** Die Zustandsableitungen nach der Zeit, die dem Gleichungslöser übergeben werden.
\endif


Konfigurationsstring
=====
Der Konfigurationsstring entspricht der als char-Array eingelesenen XML-Datei. Für die Echtzeitberechnung muss der Konfigurationsstring (XML-Konfigurationsdatei) @b zu einem double array konvertiert werden.
Dies ist der Eigenschaft vom Simulink zuzuschreiben, bei der im realtime Betrieb die mxArray Klasse keine Unterscheidung zwischen mx_CHAR_CLASS und mx_DOUBLE_CLASS mehr zulässt.
Jeglicher Datentyp wird als mx_DOUBLE_CLASS dargestellt, obwohl dieser zur Kompilierzeit eine andere Größe haben könnte. Dieses kann zu Fehlern führen.
Zur Konvertierung kann das Skript convert2double.m genutzt werden.

Besonderheiten bei \<Options\>-Knoten
===============
\ifnot RELEASE_DOCU
Im <**AirTemperature**>-Knoten darf keine Datei eingelesen werden (ReadFromFile="false" oder ReadFromFile-Attribut existiert nicht).
\endif
Der Inputport für die Außentemperatur muss verbunden und gesetzt sein. Der Wert in <**AirTemperature**> wird ignoriert.


\~English
Further topics:
-   [Additional Simulink libraries](bibs.html)
-   [\<Options\>-node](xmloption.html)


Simulink multicell-interface
=========
The multicell interface defines the interface of simulink with the framework. The S-function, which wraps various C++-functions, determining the behaviour of the simulated battery system, is defined within the MultiCellInterface.
The model can be configured via a configuration string, stored in the Matlab workspace. There are three different MultiCellInterfaces for the use in a pure electrical simulation,
pure thermal or coupled electrical/thermal simulation. The coupled version can be configured as a continuously Matlab-block or as a discrete block.

![MultiCellInterface, inputs and outputs](MultiCellInterface.png)

S-function arguments
---------
The s-function expects 4 arguments, CONFIG_STR_A, CONFIG_STR_B, CONFIG_STR_C and DISPLAY_VERSION.
The first three arguments represent variables for the configuration file and are concatenated and evaluated at the beginning.
Within a pure Simulink-simulation, the following usage is perfectly valid (configuration string as first argument, CONFIG_STR_B and CONFIG_STR_C empty).
<pre>
    CONFIG_STR_A = fileread('XYL.xml');
    CONFIG_STR_B = '';
    CONFIG_STR_C = '';
</pre>


<b> ATTENTION </b>

During execution on the real-time board, it is necessary to convert the chars to doubles. It is necessary to ensure that separate strings are short enough.

<pre>
    CONFIG_STR_A = convert2double( fileread('XYL.xml') );
    CONFIG_STR_B = '';
    CONFIG_STR_C = '';
</pre>




Input ports
---------

The input ports of different MultiCellInterfaces are given in the following table:


|Input ports       | Electrical \n model    |   Thermal \n model    | Thermal and electrical \n model  | Description |
|------------------| :--------------------: | :-------------------: | :------------------------------: | :----------- |
|Reset             | x | x | x | If Reset=0, nothing will happen. Otherwise, the model will be reset. \n This means, that the running simulation will be closed and the model will be completely rebuild with the help of the xml file. |
|I_Bat_Source      | x | o | x | The charging current of the battery at the respective time|
|T_Ambient         | o | x | x | The ambient temperature at the respective time. |
|P_Thermal         | o | x | o | The power loss at the respective time. \n The dimension of this input port is the number of cells in the battery pack. |
|I_Delta_Multicell | x | o | x | The balancing current inbetween the cell of the battery at the respective time. \n This is the current, which is applied additionally from a BMS. \n The dimension of this input port is the number of cells in the battery pack. |


x = port available,
o = port available but without use,
"empty" = port not available


Output ports
---------

|Output ports      | Electrical  \n model   | Thermal \n model      | Thermal and electrical \n model  | Description |
|------------------| :--------------------: | :-------------------: | :------------------------------: | :----------- |
|SignalBus         | x | x | x | Aggregates several signals in a single bus. \n **Details see below** |
|V_Bat             | x | x | x | Voltage across the whole battery pack. |
|V_Batt_Cell       | x | x | x | Voltage across single cells. \n The dimension of this port is the number of cells in the battery. |


x = port available,
o = port available but without use,
"empty" = port not available


|SignalBus         | Electrical \n model    | Thermal \n model      | Thermal and electrical \n model  |  Description |
|------------------| :--------------------: | :-------------------: | :------------------------------: | :----------- |
|Current vector                  | x |   | x | Current in the individual cells. \n The dimension of this port is the number of cells in the battery. |
|Ohmic heat \n generation vector | x |   | x | Power loss produced in the individual cells. \n The dimension of this port is the number of cells in the battery. |
|State of charge Vector          | x |   | x | State of charge of the individual cells. \n The dimension of this port is the number of cells in the battery. |
|Surface SOC vector              | x |   | x | Surface-state-of-charge of the individual cells. \n The dimension of this port is the number of cells in the battery. |
|MEM_USED_OR_FRAGMENTED          | x | x | x | If this function is not implemented, "0" will be displayed at all times. |
|Total Current                   | x |   | x | Current fed into the battery. |
|Total heat generation           | x |   | x | Overall power loss of the battery pack. |
|Total amount of Batteries       | x |   | x | Number of cells in the pack. |
|Number of used states           | x | x | x | Number of used states for the equation solver. |
|Emplacement points              |   | x |   | Number of used states for the equation solver. |
|Cell temperature vector         |   | x |   | Temperatures of the individual cell elements. |
|Temperature vector              |   |   | x | Temperatures of the individual cell elements. |

x = signal available,
"leer" = signal not available


\ifnot RELEASE_DOCU
If compiled with output of the internal states, the S-function will contain two additional ports within the MultiCellInterface:
- **State vector:** The states, which are passed to the equation solver.
- **Dxdt vector:** The time derivatives of the states, which are passed to the equation solver.
\endif


Configuration string
=====
The configuration string is equal with the xml file read as a char-array. The configuration string (XML configuration file) @b has to be converted to a double array, in order to do real time calculation.
This is caused by the property of Simulink in real-time mode, which nullifies the possibility to distinguish inbetween mx_CHAR_CLASS and mx_DOUBLE_CLASS within the mxArray class.
The script convert2double.m can be used for conversion.

Characteristics of the \<Options\>-node
===============
\ifnot RELEASE_DOCU
No files can be read within the <**AirTemperature**>-node (ReadFromFile="false" or ReadFromFile-attribute is not existing).
\endif
The input port for the ambient temperature has to be connected and set. The value within <**AirTemperature**> is ignored.
