Getting Started   {#gettingStarted}
===============

Getting the framework
---------------------

The framework executbales can either be downloaded directly from Gitlab (see the download links in the readme) or it can be built locally as described in the section [Compiling](compiling.html). There are 4 different executables for running electrical, thermal, thermal-electrical or aging simulations. All executables can be called with the option -v to display version information.

Running a simulation
--------------------

To run a simulation, two files are necessary: an XML file that describes the model and a current or power profile. The XML format is described [here](xmlsprache.html). There are some examples in the folder scenarios. The format for current or power profiles is described [here](xmlexecutable.html). An example current profile that contains a few short pulses can be found in scenarios/profiles/pulse.csv.

Example
-------

To run a simple test simulation:

```bash
mkdir simulation
cd simulation
../build/electricalSimulation ../scenarios/electrical/sanyo_RC_el.xml ../scenarios/profiles/pulse.csv
```

The simulation outputs a lot of data. After a few seconds, the message "Run was succesful" should apper. All generated data is written to the matlab file sanyo_0.mat. This file contains the vector Programmdauer with time values and additional vectors for current, voltage, power and SOC. Current, voltage and power are available as a matrix with values for each simulated cell and as a vector with values for the entire system.