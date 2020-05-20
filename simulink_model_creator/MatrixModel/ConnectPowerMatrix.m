function[] = ConnectPowerMatrix(system, numberOfCellElements)

autorouting = Config.Autorouting;

add_block('built-in/Mux', [system '/PowerMatrix/Mux'], 'Position', [220 60 225 60+120*numberOfCellElements]);
set_param([system '/PowerMatrix/Mux'], 'Inputs', num2str(numberOfCellElements));
add_line([system '/PowerMatrix'], 'Mux/1', 'Components/1', 'autorouting', autorouting);

for i = 1:numberOfCellElements
    add_line([system '/PowerMatrix'], ['Component' num2str(i-1) '/3'], ['Mux/' num2str(i)], 'autorouting', autorouting);
end

end
