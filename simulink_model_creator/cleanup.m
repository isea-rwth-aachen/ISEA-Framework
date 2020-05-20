DeleteIfExists('CharacteristicLengthData.txt');
DeleteIfExists('CharacteristicLengthVisualization_Patch_Areas.csv');
DeleteIfExists('CharacteristicLengthVisualization_Patch_Temperatures.csv');
DeleteIfExists('CharacteristicLengthVisualization_Patch_Vertices.csv');
DeleteIfExists('ConductivityMatrix.m');
DeleteIfExists('DefaultCooling.m');
DeleteIfExists('defines.h');

function [] = DeleteIfExists(filename)
    if isfile(filename)
        delete(filename)
    end
end