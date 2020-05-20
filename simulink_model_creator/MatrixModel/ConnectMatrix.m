function[] = ConnectMatrix(system)

autorouting = Config.Autorouting;
allComponents = GetAllComponents(system);

if(size(allComponents, 1) > 0) 
    add_block('simulink/Signal Routing/Mux', [system '/Mux'], 'Position', [200 60 205 min(32767,120*size(allComponents, 1))]);
    set_param([system '/Mux'], 'Inputs', num2str(size(allComponents, 1)));
    add_line(system,  'Mux/1', 'Components/1', 'autorouting', autorouting);

    for i = 1:size(allComponents, 1)
%        add_line(system, strcat(allComponents(i,:), '/1'), ['Mux/' num2str(i*3-2)], 'autorouting', autorouting);
%        add_line(system, strcat(allComponents(i,:), '/2'), ['Mux/' num2str(i*3-1)], 'autorouting', autorouting);
       add_line(system, strcat(allComponents(i,:), '/3'), ['Mux/' num2str(i)], 'autorouting', autorouting);
    end
end

end
