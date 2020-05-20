function[] = SetObject(system, objectName, connectTo ...
                     , objRowPoints, objColPoints, objMatrix)

if(size(objColPoints, 2) == 1 && size(objRowPoints, 2) == 1)
    set_param([system '/Const' objectName], 'Value', mat2str(objMatrix(1,1)));
    delete_block([system '/' objectName]);
    add_line(system, ['Const' objectName '/1'], connectTo);
else
    set_param([system '/' objectName], 'BP1', mat2str(objRowPoints));
    set_param([system '/' objectName], 'BP2', mat2str(objColPoints));
    set_param([system '/' objectName], 'Table', mat2str(objMatrix));
    delete_block([system '/Const' objectName]);
    add_line(system, [objectName '/1'], connectTo);
    add_line(system, 'T/1', [objectName '/1']);
    add_line(system, 'SoC/1', [objectName '/2']);
end

end
