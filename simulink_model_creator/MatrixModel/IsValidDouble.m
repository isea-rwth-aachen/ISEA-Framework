function [isDouble] = IsValidDouble(string)

isDouble = true;

if string(1) ~= '+' && string(1) ~= '-' && string(1) ~= '.' && ~isstrprop(string(1), 'digit')
    isDouble = false;
    return;
end

for i=2:size(string, 2)
    if string(i) ~= '.' && ~isstrprop(string(i), 'digit')
        isDouble = false;
        return;
    end
end

end

