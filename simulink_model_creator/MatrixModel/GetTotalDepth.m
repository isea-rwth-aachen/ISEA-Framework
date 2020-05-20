function[depth] = GetTotalDepth(symbolicString)

currentDepth = 1;
depth = 1;
for i = 1:size(symbolicString, 2)
    if symbolicString(i) == '('
        currentDepth = currentDepth + 1;
    elseif symbolicString(i) == ')'
        currentDepth = currentDepth - 1;
    end
    if currentDepth > depth
        depth = currentDepth;
    end
end

end