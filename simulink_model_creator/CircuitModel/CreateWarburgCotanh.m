%function[] = CreateWarburgCotanh(destination, isIU ...
%                               , withCapacity, RCCounter ...
%                               , sigmaObjRowPoints, sigmaObjColPoints, sigmaObjMatrix ...
%                               , cLimObjRowPoints, cLimObjColPoints, cLimObjMatrix)
function[destination] = CreateWarburgCotanh(destination, isIU)

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
