function[] = MatrixCreateElectricalElement(destination, iD ...
                                         , objRowPoints, objColPoints, objMatrix)

suffix = 'Base';
if contains(destination, 'OhmicResistance')
    suffix = 'R';
elseif contains(destination, 'Capacity')
    suffix = 'C';
elseif contains(destination, 'VoltageSource')
    suffix = 'V';
end

[position, number] = GetPosition(destination);
destination = SetElemNoToDestination(destination, number);
objectIdString = ['ID' num2str(iD) '_Obj' suffix];

add_block('ElectricalElements/ElectricalElement', destination, 'Position', position);
SetObject(destination, 'Object', 'ObjectGoto/1', objectIdString, objRowPoints, objColPoints, objMatrix);

set_param([destination '/ObjectGoto'], 'GotoTag', objectIdString);


end
