Observer      {#xmlobserver}
=======================

\~German

Nachfolgend sind alle Observerfilter aufgeführt.

<br/>

Allgemein
=========
Die Ausgabe der elektrischen Simulation läßt sich ausser im Falle von Simulink per XML-Datei beeinflussen, über sogenannte Filter.
Die Filter werden dabei seriell abgearbeitet vom ersten Filter bis zum letzten.

![Filterkette für die Ausgabe des elektrischen Modells](filterChain.png)
<br/>

Dazu muss in der XML-Datei der unter dem Knoten <Configuration> das Element <Observer> erstellt werden.
Innerhalb des <Observer>-Tags können eine beliebige Anzahl an Filtern hinzugefügt.



<br/>
CSV-Filter
==========

Der CSV-Filter generiert eine CSV-Datei mit den Werten im folgende Format:

<pre>
#Time; Elementnr; Voltage; Current; Power; SOC
#s; Number ; Voltage / V; Current / A; Power / W; SOC / %
</pre>

\arg Time:  aktuelle Simulationzeit in Sekunden
\arg Elementnr: Nummer des  elektrisches Element für den die folgenden Werte gelten
\arg Voltage: Spannung in Volt
\arg Current: Strom in Ampere
\arg Power: thermische Leistung in Watt
\arg SOC: Ladezustand (state of charge) in %

\htmlinclude csvfilter_color.xml

<br/>
STDOut-Filter
==========
Der STDOut-Filter gibt die Werte in ähnlicher Art und Weise wie der CSV-Filter aus.
Jedoch werden die Daten nicht in einer Datei gespeichert sondern direkt über die Standarausgabe ausgegeben.

\htmlinclude stdoutfilter_color.xml
<br/>

Dezimierungs-Filter
========
Der Dezimierungsfilter verhindert, dass zu viele Daten ausgegeben werden. Es werden nur Daten mit einem Simulationszeitstempel, die einen zeitlichen  Mindestabstand einhalten durchgelassen (dezimiert).
Dies ist sinnvoll wenn man mit sehr vielen Daten Rechnen muss, ohne dass diese Daten viel mehr Information bringen.


\htmlinclude decimatefilter_color.xml

<br/>
Matlab-Filter
========
Die Daten werden im Matlab Format (.mat) abgespeichert. Die Daten werden dabei gezippt, sind damit sehr viel platzsparender als im CSV-Format und können direkt über Matlab eingelesen werden.
Die maximale Dateigröße in Samplen kann über den Parameter "maxSampleSize" gesteuert werden. 


\htmlinclude matlabFilter_color.xml

<br/>
Beispiel: Verkettung von Filtern
======
Im folgenden wird eine Filterkette aus einer Matlab Ausgabe, einer Dezimierung,  Stdout-Filter sowie einem CSV-Filter gebildet.<br/>
Dabei werden alle Ausgaben des Modells in der Matlab zwischengespeichert. <br/>
Danach wird eine Dezimierung der Daten durchgeführt, die einen zeitlichen Mindestetabstand von 0.1 Sekunden der Datenpunkte garantiert.<br/>
Diese Daten werden dann auf die Standardausgabe ausgeben und in eine CSV-Datei gespeichert<br/>

\htmlinclude filterChain_color.xml

\~English

This page lists all filters can be used for simulation output.

<br/>

General
=========
The simulation output can be controlled in the XML file using filters, except when the simulink model is used.
The filters are executed serially from the first filter to the last one.

![Filter chain for the output of the electrical model](filterChain.png)
<br/>

Filters are added in an <Observer> element inside the <Configuration> element. The <Observer> element can contain the elements <Electrcal>, <Thermal> and <Aging>. Filters for the three parts of the model are added in the appropriate element.

<br/>
CSV filter
==========

The CSV filter generates a CSV file that used the following format:

<pre>
#Time; Elementnr; Voltage; Current; Power; SOC
#s; Number ; Voltage / V; Current / A; Power / W; SOC / %
</pre>

\arg Time:  current simulation time in seconds
\arg Elementnr: number of the electrical element
\arg Voltage: Voltage in Volt
\arg Current: Current in Ampere
\arg Power: Thermal dissipation power in Watt
\arg SOC: State of charge in %

\htmlinclude csvfilter_color.xml

<br/>
STDOut filter
==========
The STDOut filter writes values to the standard outpt.

\htmlinclude stdoutfilter_color.xml
<br/>

Decimation filter
========
The decimation filter can be used to prevent too much output data. Data is only passed on to the next filter in the chain if a certain amount of time has passed since the last data point.

\htmlinclude decimatefilter_color.xml

<br/>
Matlab filter
========
Data is stored in matlab files (.mat files). The files are compressed and use much less disc space than CSV files. The maximum number of samples in one file can be controlled with the attribute "maxSampleSize".

\htmlinclude matlabFilter_color.xml

<br/>
Example: Chain of filters
======
The following example shows a filter chain with matlab output, decimation, STDOut-Filter and CSV output.
All simulation output is stored in the matlab output file. Afterwards the data is decimated so that the minimum time between two samples is 0.1 seconds. This data is then written to the standard output and saved in a CSV file.

\htmlinclude filterChain_color.xml