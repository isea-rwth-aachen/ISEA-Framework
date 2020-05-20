function[] = PlaceSymbol(destination, symbolicString, depth, connectTo)

global posVector;
height = posVector(depth);
position = [-60+depth*120 60+height*120 depth*120 120+height*120];
for i= 1:4
    if position(i) > 32767
        position(i) = 32767;
    end
end
posVector(depth) = posVector(depth) + 1;

currentDepth = 0;
splitPoint = size(symbolicString, 2);
for i = 1:size(symbolicString, 2)
    if symbolicString(i) == '('
        currentDepth = currentDepth + 1;
    elseif symbolicString(i) == ')'
        currentDepth = currentDepth - 1;
    elseif currentDepth == 1 && symbolicString(i) == ','
        splitPoint = i;
        break;
    end
end
splitString1 = symbolicString(5:(splitPoint - 1));
splitString2 = symbolicString((splitPoint + 1):(size(symbolicString, 2) - 1));

if(IsOperator(symbolicString, 'NEG'))
    blockName = ['Neg' num2str(depth) '_' num2str(height)];
    add_block('simulink/Math Operations/Gain', [destination '/' blockName], 'Position', position);
    add_line(destination, [blockName '/1'], connectTo, 'autorouting', 'on');
    set_param([destination '/' blockName], 'Gain', num2str(-1));
    PlaceSymbol(destination, splitString1, depth - 1, [blockName '/1']);
elseif(IsOperator(symbolicString, 'ADD'))
    blockName = ['Add' num2str(depth) '_' num2str(height)];
    add_block('simulink/Math Operations/Add', [destination '/' blockName], 'Position', position);
    add_line(destination, [blockName '/1'], connectTo, 'autorouting', 'on');
    PlaceSymbol(destination, splitString1, depth - 1, [blockName '/1']);
    PlaceSymbol(destination, splitString2, depth - 1, [blockName '/2']);
elseif(IsOperator(symbolicString, 'SUB'))
    blockName = ['Sub' num2str(depth) '_' num2str(height)];
    add_block('simulink/Math Operations/Subtract', [destination '/' blockName], 'Position', position);
    add_line(destination, [blockName '/1'], connectTo, 'autorouting', 'on');
    PlaceSymbol(destination, splitString1, depth - 1, [blockName '/1']);
    PlaceSymbol(destination, splitString2, depth - 1, [blockName '/2']);
elseif(IsOperator(symbolicString, 'MUL'))
    blockName = ['Mul' num2str(depth) '_' num2str(height)];
    add_block('simulink/Math Operations/Product', [destination '/' blockName], 'Position', position);
    add_line(destination, [blockName '/1'], connectTo, 'autorouting', 'on');
    PlaceSymbol(destination, splitString1, depth - 1, [blockName '/1']);
    PlaceSymbol(destination, splitString2, depth - 1, [blockName '/2']);
elseif(IsOperator(symbolicString, 'DIV'))
    blockName = ['Div' num2str(depth) '_' num2str(height)];
    add_block('simulink/Math Operations/Divide', [destination '/' blockName], 'Position', position);
    add_line(destination, [blockName '/1'], connectTo, 'autorouting', 'on');
    PlaceSymbol(destination, splitString1, depth - 1, [blockName '/1']);
    PlaceSymbol(destination, splitString2, depth - 1, [blockName '/2']);
elseif(IsOperator(symbolicString, 'SQU'))
    blockName = ['Squ' num2str(depth) '_' num2str(height)];
    add_block('simulink/Math Operations/Math Function', [destination '/' blockName], 'Position', position);
    add_line(destination, [blockName '/1'], connectTo, 'autorouting', 'on');
    set_param([destination '/' blockName], 'Operator', 'square');
    PlaceSymbol(destination, splitString1, depth - 1, [blockName '/1']);
elseif(IsOperator(symbolicString, 'TANH'))
    splitString1= ['SUB(1,DIV(2,ADD(POW(2.718,MUL(2,' splitString1(2:end) ')),1)))'];
    PlaceSymbol(destination, splitString1, depth , connectTo);
elseif(IsOperator(symbolicString, 'POW'))
    blockName = ['Pow' num2str(depth) '_' num2str(height)];
    add_block('simulink/Math Operations/Math Function', [destination '/' blockName], 'Position', position);
    add_line(destination, [blockName '/1'], connectTo, 'autorouting', 'on');
    set_param([destination '/' blockName], 'Operator', 'pow');
    PlaceSymbol(destination, splitString1, depth - 1, [blockName '/1']);
    PlaceSymbol(destination, splitString2, depth - 1, [blockName '/2']);
elseif(IsValidDouble(symbolicString))
    blockName = ['Const' num2str(depth) '_' num2str(height)];
    add_block('simulink/Sources/Constant', [destination '/' blockName], 'Position', position);
    add_line(destination, [blockName '/1'], connectTo, 'autorouting', 'on');
    set_param([destination '/' blockName], 'Value', num2str(symbolicString));
else
    global gotoBus;
    if isempty( gotoBus)
        gotoBus= struct();
        gotoBus.elements= [];
        gotoBus.counter = 1;
    end
    
    blockName = ['From' num2str(depth) '_' num2str(height)];
    gotoBus.elements{gotoBus.counter} = symbolicString;
    gotoBus.counter = gotoBus.counter +1;
    add_block('simulink/Signal Routing/From', [destination '/' blockName], 'Position', position);
    add_line(destination, [blockName '/1'], connectTo, 'autorouting', 'on');
    set_param([destination '/' blockName], 'GotoTag', symbolicString);
end

end

function[retVal] = IsOperator(symbolicString, operatorString)
    if(size(symbolicString, 2) < size(operatorString, 2))
        retVal = false;
        return;
    end
    if strcmp(symbolicString(1:size(operatorString, 2)), operatorString)
        retVal = true;
    else
        retVal = false;
    end
end
