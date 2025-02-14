%% Window function (of Jing Xie)
function [filteredInput] = WindowFunction(filterLength, inputData)

filteredInput = filtfilt(ones(1, filterLength), filterLength, inputData);

end