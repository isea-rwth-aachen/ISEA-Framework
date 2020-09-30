Simulation Cycle {#simulation_cycle}
====

Electrical Simulation
----
![Schematic of the electrical simulation cycle](Kreislauf.png)

The steps of the electrical simulation are pictured above.

### 1. Initialize the simulation
The circuit elements and their parameters are read from a [configuration file](xmlsprache.html). This file also determines the initial state of the system.

### 2. Set requested current
For each simulation, the current through the simulated system is given as a function of time. For the standalone version, this is done with a [current profile](xmlexecutable.html). For each timestep of the electrical model, the profile is evaluated at the current time and the current is set.

### 3. Update of lookup tables
Most elements in the electrical circuit change their values depending on system states like SoC or temperature. For each timestep, the values are updated by evaluating the respective lookup tables.

### 4. Calculation of differential and algebraic equations (DAE)
Since the values of the electrical system change each time the lookup tables are evaluated, the system equations have to be recalculated for every time step. Since there are both algebraic and differential equations, this results in an ODE system.

### 5. Solving the DAE system and calculating the state space vector
The system is turned into an ordinary differential equation system and solved. 

### 6. Calculating current and voltage values
After the state space vector is calculated, the voltage and current values for each element in the electrical system can be easily calculated.

Thermal Simulation
----
If the thermal model is used, [thermal blocks](xmlthermalblock.html) are added to the configuration file. When the simulation is initialized, the thermal model is built as described [here](xmlthermalmodel.html). This results in a conductivity matrix that is used to calculate the flow of thermal energy between any two thermal elements. In contrast to the electrical model, the parameters of the thermal model stay constant for the duration of the simulation. After each electrical step, the ohmic heat generation during the timestep is passed from the electrical to the thermal model. The conductivity matrix as wall as the parameters given by additional cooling blocks are used to determine the temperature distribution inside the system. The temperature values are passed back to the electrical simulation and influence the electrical parameters for the next timestep.

Aging Simulation
----
![Calculation of the cell capacity](balancing_aging.svg)
To simulate the aging of cells, [aging effects](aging_simulation.html) are added to the cells. These effects are calculated independently from each other. The timespans of the aging model are much larger than those of the electrical and thermal models, so the aging is only calculated after a certain time has passed in the thermal-electrical model. During the thermal-electrical simulation, each aging effect is called regularly to collect data needed for the aging step. This data is used to calculate three values for every aging effect:
- The capacity factor \f$ C_{rel} \f$. The initial capacity of the cell is multiplied with this factor to get the capacity after the aging effect has been applied.
- The resistance factor \f$ R_{rel} \f$. The initial internal resistance of the cell is multiplied with this factor to get the resistance after the aging effect has been applied.
- The SOC offset \f$ \Delta Q \f$. The offset is added to the stored charge when the aging effect is applied.

The total aging for a cell with \f$ n \f$ aging effects are calculated in the following way:

\f{eqnarray*}{
C_{rel,total} & = && 1 - \sum_{n} ( 1 - C_{rel,n} ) \\
R_{rel,total} & = && 1 - \sum_{n} ( 1 - R_{rel,n} ) \\
\Delta Q_{total} & = && \sum_{n} \Delta Q_n \\
\f}

Complete Procedure
----
![Schematic of the simulation cycle](simulation_cycle.svg)
