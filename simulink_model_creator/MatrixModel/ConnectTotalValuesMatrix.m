function [outputArg1,outputArg2] = ConnectTotalValuesMatrix(system)

matrix = [system '/TotalValues'];
add_line(matrix, 'Component0/3', 'Current/1');
add_line(matrix, 'Component1/3', 'Voltage/1');
add_line(matrix, 'Component2/3', 'Power/1');

end

