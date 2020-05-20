function [] = DeleteSeparator(system)

delete_line(system, 'SepVals/1', 'Separators/1');
delete_block([system '/Separators']);
delete_block([system '/SepVals']);

end

