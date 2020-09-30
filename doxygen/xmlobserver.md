Observer      {#xmlobserver}
=======================

General
=========
The simulation output can be controlled in the XML file using filters, except when the simulink model is used.
The filters are executed serially from the first filter to the last one.

![Filter chain for the output of the electrical model](filterChain.png)
<br/>

Filters are added in an <**Observer**> element inside the <**Configuration**> element. The <**Observer**> element can contain the elements <**Electrical**>, <**Thermal**> and <**Aging**>. Filters for the three parts of the model are added in the appropriate element.

<br/>
CSV filter
==========

The CSV filter generates a CSV file that uses the following format:

<pre>
\#Time; Elementnr; Voltage; Current; Power; SOC
\#s; Number ; Voltage / V; Current / A; Power / W; SOC / %
</pre>

\arg Time:  current simulation time in seconds
\arg Elementnr: number of the electrical element
\arg Voltage: Voltage in Volt
\arg Current: Current in Ampere
\arg Power: Thermal dissipation power in Watt
\arg SOC: State of charge in %

XML Tag     | Type   | Default value | Explanation                                                                                 
------------|--------|---------------|------------------------
printHeader | bool   | true          | determines if a header is included in the CSV file
Filename    | string |               | filename of the CSV output 

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