%function[] = CreateWarburgTanh(destination, isIU ...
%                             , rCCounter ...
%                             , rObjRowPoints, rObjColPoints, rObjMatrix ...
%                             , tauObjRowPoints, tauObjColPoints, tauObjMatrix)
function[destination] = CreateWarburgTanh(destination, isIU)

[position, number] = GetPosition(destination);
destination = SetElemNoToDestination(destination, number);

if isIU
    load_system('ElectricalTwoPorts/CircuitWithStates');
    add_block('CircuitWithStates/CircuitWithStates', destination, 'Position', position);
else
    load_system('ElectricalTwoPorts/CircuitWithStates_UI');
    add_block('CircuitWithStates_UI/CircuitWithStates', destination, 'Position', position);
end

end
