Executables for electrical/thermal/aging simulation     {#xmlexecutable}
===============

\~German
Allgemein
=========

Die elektrisch/thermische Simulation wird mit variablem Stepsolver ausgeführt.
Es wird jeweils abwechselnd das elektrische und das thermische Modell simuliert.
Das elektrische Modell simuliert zuerst einen bestimmten Zeitraum, danach simuliert das thermische Modell genau diesen Zeitraum.
Wenn die Änderung des SoC oder der Temperatur in einem der Simulationsschritte zu groß ist, wird ein vorzeitiger Wechsel zwischen den Modellen erzwungen.

Ausführung
=========

Die Executable mit elektrisch/thermischer Simulation wird von der Konsole aus aufgerufen. Sie erhält zwei Eingabeparameter:
- Die XML-Datei
- Ein Strom- oder Leistungsprofil

Ein Stromprofil besteht aus Angaben zum von außen gesetzten Strom zu einem bestimmten Zeitpunkt.
Das folgende Beispiel
- setzt den Strom bei 0 sec auf 5 A,
- bei 200 sec auf -2 A und
- ab 500 sec bis zum Ende der Simulation auf offene Klemmen (0 A).
~~~~~~~~~~~~~~~~~~~~~~
# type=current
0, 5
200, -2
500, 0

~~~~~~~~~~~~~~~~~~~~~~
Falls nicht anders angegeben, wird der Strom bei 0 sec auf 0 A gesetzt.

Ein Leistungsprofil gibt auf die geiche Weise die Leistung in Watt an. Im Header wird dafür type=power angegeben. Ist kein Header vorhanden, wird ein Stromprofil angenommen.

Beispiel
=========

Als Beispiel für eine XML-Datei sei folgendes Simulationsszenario gegeben, das auch unter doxygen/examples/TestShowBattery.xml zu finden ist:
Diese könnte mit der Befehlszeile ausgeführt werden:

Executable TestShowBattery.xml Stromprofil.txt

<pre>
\htmlinclude TestShowBattery_color.xml
</pre>


\~English

The electrical/thermal simulation is run with a variable step solver.
The electrical and thermal simulations are run alternately.
Firstly, the electrical model is simulated for a certain period. Afterwards, the thermal model is simulated for exactly the same period.
If the change of any SoC or temperature exceeds a limit set in the [XML options](xmloption.html), a premature switch between the two models is forced.

Execution
=========

The executable with the electrical/thermal simulation is called via the terminal. This call contains two input parameters:
- The xml file
- A current or power profile

A current profile contains specifications of externally applied current at specific times.
The following example
- sets the current at 0s to 5 ampere (A),
- at 200 sec to -2 A
- from 500 sec until the end to open terminal(0 A).
~~~~~~~~~~~~~~~~~~~~~~
# type=current
0, 5
200, -2
500, 0

~~~~~~~~~~~~~~~~~~~~~~
If not otherwise specified, the current is set to 0 A at 0 sec.

A power profile specifies the power in watts in the same manner. For this, the header is changed to type=power. If no header is given, a current profile is assumed.

Example
=========

As an example for a xml file, following simulation scenario is given. *This scenario could also been found at doxygen/examples/TestShowBattery.xml*:
This could be run via terminal:

Executable TestShowBattery.xml Stromprofil.txt

<pre>
\htmlinclude TestShowBattery_color.xml
</pre>
