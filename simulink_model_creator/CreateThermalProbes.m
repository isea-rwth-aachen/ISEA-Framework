function [] = CreateThermalProbes(system, finiteVolumes)

global numberOfFiniteVolumes;
if size(finiteVolumes, 1) == 0
    %delete_line(system, 'FVTemperatures/1', 'FVToProbes/1');
    delete_line([system '/FVToProbes'], 'FVTemperatures/1', 'VolumeSelector/1');
    delete_line([system '/FVToProbes'], 'VolumeSelector/1', 'ThermalProbeGoto/1');
    delete_block([system '/FVToProbes/VolumeSelector']);
else
    set_param([system '/FVToProbes/VolumeSelector'], 'InputPortWidth', num2str(numberOfFiniteVolumes));
    selectorString = cell(1);
    selectorString{1} = mat2str(finiteVolumes);
    set_param([system '/FVToProbes/VolumeSelector'], 'IndexParamArray', selectorString);
end

end