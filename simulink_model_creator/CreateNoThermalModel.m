function[] = CreateNoThermalModel(destination, InitialTemperatureOfCellelements)
if ~exist('InitialTemperatureOfCellelements')
    InitialTemperatureOfCellelements = 25;
end
load_system('NoThermalModel');
add_block('NoThermalModel/NoThermalModel', destination, 'Position', [440 620 560 740]);

global CellElementCounter;
if CellElementCounter > 0
set_param([destination '/Demux'], 'Outputs', num2str(CellElementCounter));
end

set_param([destination '/Cell Temperatures'], 'Value', InitialTemperatureOfCellelements);


for i = 1:CellElementCounter
    add_block('built-in/From', [destination '/PowerDissipation' num2str(i)], 'Position', [30 30+60*i 60 60+60*i]);
    add_block('built-in/Goto', [destination '/Temperature' num2str(i)], 'Position', [1030 30+60*i 1060 60+60*i]);
    gotoPower = ['P_CellElement' num2str(i)];
    gotoTemp = ['T_CellElement' num2str(i)];
    set_param([destination '/PowerDissipation' num2str(i)], 'GotoTag', gotoPower);
    set_param([destination '/Temperature' num2str(i)], 'GotoTag', gotoTemp);
    set_param([destination '/Temperature' num2str(i)], 'TagVisibility', 'global');
    add_line(destination, ['Demux/' num2str(i)], ['Temperature' num2str(i) '/1'] , 'autorouting', 'on');
end

end
