function[] = CreateThermalModel(destination ...
                              , finiteVolumesByCellIndexAndFactor ...
                              , reciprocalsOfCapacity ...
                              , conductivityMatrix ...
                              , InitialTemperaturesOfFiniteVolumes)

load_system('ThermalModel');
add_block('ThermalModel/ThermalModel', destination, 'Position', [440 620 560 740]);



global CellElementCounter;
assert(CellElementCounter > 0);

for i = 1:CellElementCounter
    add_block('built-in/From', [destination '/CellToFV/PowerDissipation' num2str(i)], 'Position', [30 30+60*i 60 60+60*i]);
    add_block('built-in/Goto', [destination '/FVToCell/Temperature' num2str(i)], 'Position', [750 30+60*i 780 60+60*i]);
    gotoPower = ['P_CellElement' num2str(i)];
    gotoTemp = ['T_CellElement' num2str(i)];
    set_param([destination '/CellToFV/PowerDissipation' num2str(i)], 'GotoTag', gotoPower);
    set_param([destination '/FVToCell/Temperature' num2str(i)], 'GotoTag', gotoTemp);
    set_param([destination '/FVToCell/Temperature' num2str(i)], 'TagVisibility', 'global');
end



global numberOfFiniteVolumes;
numberOfFiniteVolumes = size(finiteVolumesByCellIndexAndFactor, 1);
add_block('built-in/Mux', [destination '/CellToFV/Mux'], 'Position', [600 75 605 min(32767,75+ 60*numberOfFiniteVolumes)]);
add_block('built-in/Mux', [destination '/Coolings/CoolAdder/Mux'], 'Position', [600 75 605 min(32767,75+60*numberOfFiniteVolumes)]);
add_block('built-in/Demux', [destination '/FVToCell/Demux'], 'Position', [230 75 235 min(32767,75+60*numberOfFiniteVolumes)]);
set_param([destination '/CellToFV/Mux'], 'Inputs', num2str(numberOfFiniteVolumes));
set_param([destination '/Coolings/CoolAdder/Mux'], 'Inputs', num2str(numberOfFiniteVolumes));
set_param([destination '/FVToCell/Demux'], 'Outputs', num2str(numberOfFiniteVolumes));
add_line([destination '/CellToFV'], 'Mux/1', 'P_FV/1', 'autorouting', 'on');
add_line([destination '/Coolings/CoolAdder'], 'Mux/1', 'P_FV/1', 'autorouting', 'on');
add_line([destination '/FVToCell'], 'T_FV/1', 'Demux/1', 'autorouting', 'on');

x_offset = 0;
for i = 1:numberOfFiniteVolumes
    nameAdder = ['Add_FV_' num2str(i)];
    positionAdder = 15+60*i;
    if positionAdder+40 >= 32767
        positionAdder = mod( positionAdder, 32767-40) ;
        x_offset = 60 * floor(( ( 15+60*i) / 32767));
    end
    
    add_block('simulink/Math Operations/Add', [destination '/Coolings/CoolAdder/' nameAdder], 'Position', [( 500 - x_offset) positionAdder (540- x_offset) positionAdder+40 ]);
    add_line([destination '/Coolings/CoolAdder'], [nameAdder '/1'], ['Mux/' num2str(i)], 'autorouting', 'on');
    set_param([destination '/Coolings/CoolAdder/' nameAdder], 'Inputs', '-');
end
x_offset = 0;
for i = 1:numberOfFiniteVolumes
    positionAdder = 30+60*i;
    if positionAdder+30 >= 32767
        positionAdder = mod( positionAdder, 32767-30) ;
        x_offset = 60 *floor((  ( 30+60*i) / 32767));
    end
    positionP = [(350 + x_offset) positionAdder (380+x_offset) (30+positionAdder)];
    localNameP = ['P_FV_' num2str(i)];
    nameP = [destination '/CellToFV/' localNameP];
    
    if finiteVolumesByCellIndexAndFactor(i, 1) == 0
        add_block('built-in/Constant', nameP, 'Position', positionP);
        set_param(nameP, 'Value', '0');
        add_line([destination '/CellToFV'], [localNameP '/1'], ['Mux/' num2str(i)], 'autorouting', 'on');
    else
        add_block('built-in/Gain', nameP, 'Position', positionP);
        set_param(nameP, 'Gain', num2str(finiteVolumesByCellIndexAndFactor(i, 2)));
        add_line([destination '/CellToFV'], ['PowerDissipation' num2str(finiteVolumesByCellIndexAndFactor(i, 1)) '/1'], [localNameP '/1'], 'autorouting', 'on');
        add_line([destination '/CellToFV'], [localNameP '/1'], ['Mux/' num2str(i)], 'autorouting', 'on');
    end
end
x_offset = 0;
numberOfInputsOfCellAdders = zeros(CellElementCounter, 1);
for i = 1:numberOfFiniteVolumes
    positionAdder = 30+60*i;
    if positionAdder+30 >= 32767
        positionAdder = mod( positionAdder, 32767-30) ;
        x_offset = 60 *floor((  ( 30+60*i) / 32767));
    end
    positionT = [(350+x_offset) positionAdder (380+x_offset) (positionAdder+30)];
    localNameT = ['T_FV_' num2str(i)];
    nameT = [destination '/FVToCell/' localNameT];
    
    if finiteVolumesByCellIndexAndFactor(i, 1) ~= 0
        add_block('built-in/Gain', nameT, 'Position', positionT);
        set_param(nameT, 'Gain', num2str(finiteVolumesByCellIndexAndFactor(i, 2)));
        add_line([destination '/FVToCell'], ['Demux/' num2str(i)], [localNameT '/1'], 'autorouting', 'on');
        numberOfInputsOfCellAdders(finiteVolumesByCellIndexAndFactor(i, 1), 1) = numberOfInputsOfCellAdders(finiteVolumesByCellIndexAndFactor(i, 1), 1) + 1;
    end
end

addCounter = 0;
for i = 1:CellElementCounter
    addCounter = addCounter + 2;
    newAddCounter = addCounter + 1 + numberOfInputsOfCellAdders(i, 1);
    positionAdd = [600 15*addCounter 630 15*newAddCounter];
    addCounter = newAddCounter;
    localNameAdd = ['AddCell' num2str(i)];
    nameAdd = [destination '/FVToCell/' localNameAdd];
    paramString = '+';
    for j = 2:numberOfInputsOfCellAdders(i, 1)
        paramString = [paramString '+'];
    end
    
    add_block('simulink/Math Operations/Add', nameAdd, 'Position', positionAdd);
    set_param(nameAdd, 'Inputs', paramString);
    add_line([destination '/FVToCell'], [localNameAdd '/1'], ['Temperature' num2str(i) '/1'], 'autorouting', 'on');
end

numberOfInputsOfCellAdders = zeros(CellElementCounter, 1);
for i = 1:numberOfFiniteVolumes
    cellToConnectTo = finiteVolumesByCellIndexAndFactor(i, 1);
    if cellToConnectTo ~= 0
        numberOfInputsOfCellAdders(cellToConnectTo, 1) = numberOfInputsOfCellAdders(cellToConnectTo, 1) + 1;
        addCount = numberOfInputsOfCellAdders(cellToConnectTo, 1);
        add_line([destination '/FVToCell'], ['T_FV_' num2str(i) '/1'], ['AddCell' num2str(cellToConnectTo) '/' num2str(addCount)], 'autorouting', 'on');
    end
end



set_param([destination '/Reciprocals of Thermal Capacities'], 'Gain', mat2str(reciprocalsOfCapacity));

set_param([destination '/FV Temperatures'], 'InitialCondition', InitialTemperaturesOfFiniteVolumes);

orderedConductivityMatrix = cell(size(finiteVolumesByCellIndexAndFactor, 1), 1);
for i = 1:size(conductivityMatrix ,1)
    orderedIndex = conductivityMatrix(i,1) + 1;
    col = conductivityMatrix(i,2) + 1;
    val = conductivityMatrix(i,3);
    orderedConductivityMatrix{orderedIndex} = [orderedConductivityMatrix{orderedIndex} col val];
end

fileID = fopen('ConductivityMatrix.m','w');
fileString = [sprintf('function[y] = ConductivityMatrix(u)\n') sprintf('\ny = u;\n')];
fprintf(fileID, '%s', fileString);

for i = 1:size(orderedConductivityMatrix, 1)
    fileString = ['y(' num2str(i) ') ='];
    
    if(isempty(orderedConductivityMatrix{i}))
        fileString = [fileString ' 0'];
    else
        for j=1:2:size(orderedConductivityMatrix{i}, 2)
            fileString = [fileString ' + ' num2str(orderedConductivityMatrix{i}(j+1),16) ' * u(' num2str(orderedConductivityMatrix{i}(j)) ')'];
        end
    end
    
    fileString = [fileString ';' sprintf('\n')];
    fprintf(fileID, '%s', fileString);
end

fileString = sprintf('\nend\n');
fprintf(fileID, '%s', fileString);
fclose(fileID);



global CoolingNotYetAdded;
CoolingNotYetAdded = true;


end
