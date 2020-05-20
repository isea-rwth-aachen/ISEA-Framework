function []=FinalizeModel()
add_line([system '/CellCurrentMatrices'], ['Demux/' num2str(numberOfCellelements + 1)], 'Terminator/1', 'autorouting', 'on');

end