XML options     {#xmloption}
===============
\~German
Im <**Options**>-Knoten können globale Optionen eingestellt werden. Je nach Art der Simulation werden folgende Optionen eingestellt:

Elektrisches Modell
===============
- <**SampleRate**>: Hier wird die [minimale Samplerate](xmlvereinfachung.html) definiert.
- <**PowerStopCriterion**>: Wenn ein Leistungsprofil verwendet wird, wird hiermit das Leistungs-Stop-Kriterium in Prozent festgelegt. Wenn während eines Simulationsschrittes die prozentuale Abweichung der Leistung vom vorgegeben Wert höher ist als durch das Kriterium zugelassen, wird der Simulationsschritt unterbrochen, um den erforderlichen Strom neu zu berechnen. Ist dieser Knoten nicht vorhanden, wird 1.0 Prozent angenommen.

Thermisches Modell
===============

\ifnot RELEASE_DOCU
- <**AirTemperature** **ReadFromFile**= @b "false" >: Gibt die Temperatur der Umgebung an. Wenn ReadFromFile false ist, wird der Wert in °C direkt angegeben. Wenn ReadFromFile true ist, wird die Adresse der Datei angegeben, in der eine mit der Zeit veränderliche Außentemperatur angegeben ist.
\endif
- <**ConvectionActivated**>: De-/aktiviert freie Konvektion als Standardkühlung.
  - Attribut advancedAreaAggregation: Wenn auf false gesetzt, werden jeweils alle koplanaren Außenflächen zu Gesamtflächen zusammengefasst, von denen die charakteristischen Längen berechnet werden.
                                      Wenn auf true gesetzt, wird zusätzlich die Positionierung der thermischen Blöcke, auf denen die Außenflächen liegen, betrachtet, um koplanare Flächen, die nicht miteinander verbunden sind, bei der Berechnung zu trennen.
                                      Wenn nicht explizit gesetzt, wird true als Defaultwetrt gesetzt.
- <**RadiationActivated**>: De-/aktiviert Wärmestrahlung als Standardkühlung.
\ifnot RELEASE_DOCU
- <**SocStopCriterionInPercent**>: Wenn dieser xml-Knoten existiert, wird der hierin enthaltene Wert als SoC-Stopp-Kriterium verwendet: Wenn sich während eines Simulationsschrittes der SoC eines Zellelements um diesen Betrag ändert, wird eine Neuberechnung des Gleichungssystems erzwungen. Wenn dieser xml-Knoten nicht existiert, wird hierfür 5.0 Prozent angenommen.
- <**ThermalStopCriterionInDegreeC**>: Wenn dieser xml-Knoten existiert, wird der hierin enthaltene Wert als Temperatur-Stopp-Kriterium verwendet: Wenn sich während eines Simulationsschrittes die Temperatur eines Zellelements um diesen Betrag ändert, wird eine Neuberechnung des Gleichungssystems erzwungen. Wenn dieser xml-Knoten nicht existiert, wird hierfür 5.0 Grad Celsius angenommen.
\endif
- <**GeometricalTolerance length="0.000001" angleInDegrees="0.001" percentOfQuantity="0.1"**>: Wenn dieser Knoten existiert, werden hier die Toleranzen gesetzt, die beim Aufbau des [thermischen Modells](xmlthermalmodel.xml) verwendet werden. Folgende Attribute werden verwendet:
  - length: Dieser Wert ist die maximale Entfernung zwischen zwei geometrischen Objekten, mit der beide Objekte noch als einander berührend betrachtet werden. Die Einheit ist Meter. Der Default-Wert ist 0.000001.
  - angleInDegrees: Dieser Wert ist die maximale Winkeldifferenz zwischen zwei Richtungen, mit dem beide Richtungen noch als gleich betrachtet werden. Die Einheit ist Grad. Der Default-Wert ist 0.001.
  - percentOfQuantity: Dieser Wert ist die maximale Differenz zwischen zwei (physikalischen) Größen, mit dem beide Größen noch als gleich betrachtet werden. Die Einheit ist Prozent. Der Default-Wert ist 0.1.
- <**ThermalVisualizer**><br/>: Gültig nur fuer Simulink; bei der Executable werden immer 1000 Frames mit gleichem zeitlichen Abstand über die gesamte Simulationszeit abgespeichert.
<**MaxNumberOfFrames**>1000</ **MaxNumberOfFrames**>         Maximale Anzahl der Bilder, die fuer die Visualisierung aufgenommen werden<br/>
<**TimeBetweenFramesInSec**>1</ **TimeBetweenFramesInSec**>  Zeit zwischen der Aufnahme von zwei Bildern fuer die Visualisierung<br/>
</ **ThermalVisualizer**><br/>
TimeBetweenFramesInSec = 0, kein csv-Output<br/>
TimeBetweenFramesInSec = -1, jeden Zeitschritt <br/>
TimeBetweenFramesInSec > x, Output immer nachdem mindestens x Sekunden verstrichen sind<br/>
MaxNumberOfFrames = 0 und TimeBetweenFramesInSec > 0, schreibt nur Anfangs- und Endzustand der Simulation<br/>

\~English
Global options can be set within the <**Options**>-node. According to the simulation type, following options can be set:

Electrical model
===============
- <**SampleRate**>: definition of the [minimal sample rate](xmlvereinfachung.html).
- <**PowerStopCriterion**>: If a power profile is used, this sets the power stop criterion in percent. If the power deviation in percent from the value given in the profile is greater than specified in the criterion, the simulation step is aborted to recalculate the required current. If this node does not exist, a value of 1.0 percent is assumed.

Thermal model
===============

\ifnot RELEASE_DOCU
- <**AirTemperature** **ReadFromFile**= @b "false" >: Specifies the ambient temperature. If ReadFromFile is false, the value in °C will be given directly. Otherwise, the path to a file with varying temperatures at specific times is given.
\endif
- <**ConvectionActivated**>: De-/activates free connections as default cooling mechanism.
  - attribute advancedAreaAggregation: If set to false, all coplanar areas combined to one single area and its specific dimensions are calculated.
                                      If set to true, the position of the thermal block is analyzed to separate the calculations for coplanar but unconnected areas.
                                      If not set explicitly, true will be used as default.
- <**RadiationActivated**>: De-/activates thermal radiation as standard cooling.
\ifnot RELEASE_DOCU
- <**SocStopCriterionInPercent**>: If this XML node exists, the belonging value is used as SoC stop criteria: If the SoC of a cell-element changes more than this amount, a recalculation of the system of equations is forced. If the node does not exist, 5.0 percents are used.
- <**ThermalStopCriterionInDegreeC**>: If this XML node exists, the belonging value is used as temperature stop criteria: If the temperature of a cell-element changes more than this amount, a recalculation of the system of equations is forced. If the node does not exist, 5.0 degrees Celsius are used.
\endif
- <**GeometricalTolerance length="0.000001" angleInDegrees="0.001" percentOfQuantity="0.1"**>: If this node exists, the tolerances are set, which are used in the construction of the [thermal model](xmlthermalmodel.xml). Following attributes are used:
  - length: the maximal distance between to object which are then assumed as connected. The unit is metre. The default value is 0.000001.
  - angleInDegrees: this value is the maximum angle of two directions which are then assumed as the same. The unit is degree. The default value is 0.001.
  - percentOfQuantity: this value is the maximal difference between two(physical) quantities which are then assumed as the same. The unit is percent. The default value is 0.1.
- <**ThermalVisualizer**><br/>: only valid for Simulink; within the executable 1000 frames with equidistant time steps are saved throughout the whole simulation.
<**MaxNumberOfFrames**>1000</ **MaxNumberOfFrames**>        maximum number of frames saved during the simulation<br/>
<**TimeBetweenFramesInSec**>1</ **TimeBetweenFramesInSec**> time between two frames in seconds<br/>
</ **ThermalVisualizer**><br/>
TimeBetweenFramesInSec = 0, no csv-output<br/>
TimeBetweenFramesInSec = -1, every time step <br/>
TimeBetweenFramesInSec > x, output after a minimum of x seconds passed<br/>
MaxNumberOfFrames = 0 and TimeBetweenFramesInSec > 0, only writes start and endpoint<br/>