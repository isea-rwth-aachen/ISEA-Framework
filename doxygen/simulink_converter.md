Simulink Converter {#simulink_converter}
========================================

The framework can be used with Simulink in two different ways: by using the [s-Functions](xmlcinterface.html), or by using the simulink converter. The converter reads a configuration from an XML file and turns it into a Simulink model that uses only native Simulink blocks.

Running the converter
---------------------
The converter and all necessary files can be downloaded from Gitlab. Alternative, the s-Functions used for the converter can be compiled and copied to the folder simulink_model_creator.

The converter is executed with the following matlab commands:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
configstr = fileread('file.xml');
CreateSimulinkModel(configstr, 1)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This will create the simulink model and open it.
The complete signature of CreateSimulinkModel is:

CreateSimulinkModel(configstring, modelType, modelLocation, initialStateStruct)

* configstring: The contents of an XML file describing the system
* modelType: type of the simulink model, 0 = CircuitModel (currently broken), 1 = MatrixModel
* modelLocation: System that the model will be built in
* initialStateStruct: workspace variable that contains the initial state of the model

Additional functions
====================

A few additional functions are available to modify the model after it was created.

LoadObjectValues
----------------

This function can be used to modify the values used in lookup tables after the model has been created. Apart from the values in the lookup tables, the model must be the same.

LoadObjectValues(configstring, modelLocation='BuildMatrixModel')
Loads the object values from the specified xml string to the simulink model previously created with CreateSimulinkModel.
* configstring: The contents of an XML file describing the system
* modelLocation: System that the model was built in

LoadInitialState
----------------

This function is used to update the intial state of the model if it has changed in the XML file. Apart from the initial state, the model must be the same.

Usage: LoadInitialState(configstring, initialStateStruct='MDL.battery.initialState')
Loads the initial state from the specified xml string to the workspace for a simulink model previously created with CreateSimulinkModel.
* configstring: The contents of an XML file describing the system
* initialStateStruct: workspace variable that contains the initial state of the model

AddSocInput
-----------

This function adds an additional input to the model that is used to set the initial SOC instead of reading it from the workspace.

AddSocInput(modelLocation='BuildMatrixModel')
* modelLocation: System that the model was built in

Discretize
----------

Tis function discretizes the created model by replacing all continuous integrators with discrete-time integrators.

Discretize(modelLocation='BuildMatrixModel')
* modelLocation: System that the model was built in