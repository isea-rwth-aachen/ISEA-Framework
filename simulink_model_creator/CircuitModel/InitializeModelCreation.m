function [] = InitializeModelCreation(nameOfSimulinkModel)

addpath('../');
global CellElementCounter;
CellElementCounter = 0;
load_system(nameOfSimulinkModel);

end

