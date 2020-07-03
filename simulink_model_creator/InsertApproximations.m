function [symbolicString] = InsertApproximations(symbolicString)
% Replace operators that cannot be inserted directly with their
% approximations. Currently this means replacing TANH.

matches = strfind(symbolicString, 'TANH');

% go from end to start so the matches aren't invalidated by replacements
for startIndex = flip(matches)
    % find the closing paranthesis that matches TANH
    depth = 0;
    endIndex = startIndex;
    for i = startIndex:size(symbolicString, 2)
        if symbolicString(i) == '('
            depth = depth + 1;
        elseif symbolicString(i) == ')'
            depth = depth - 1;
            if depth == 0
                endIndex = i;
                break;
            end
        end
    end
    argument = symbolicString(startIndex + 5:endIndex - 1);
    approximation = ['SUB(1,DIV(2,ADD(POW(2.718,MUL(2,' argument ')),1)))'];
    symbolicString = replaceBetween(symbolicString, startIndex, endIndex, approximation);
end

end

