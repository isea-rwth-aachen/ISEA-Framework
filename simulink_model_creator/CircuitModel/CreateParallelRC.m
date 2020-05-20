function[] = CreateParallelRC(destination, isIU ...
                            , derivativeTimeConstant ...
                            , rObjRowPoints, rObjColPoints, rObjMatrix ...
                            , tauObjRowPoints, tauObjColPoints, tauObjMatrix)

[position, number] = GetPosition(destination);
destination = SetElemNoToDestination(destination, number);

if isIU
    load_system('ElectricalTwoPorts/ParallelRC');
    add_block('ParallelRC/ParallelRC', destination, 'Position', position);
    SetObject(destination, 'ResistanceValue', 'RVal/1', rObjRowPoints, rObjColPoints, rObjMatrix);
    SetObject(destination, 'TauValue', 'DivideTau/2', tauObjRowPoints, tauObjColPoints, tauObjMatrix);
else
    load_system('ElectricalTwoPorts/ParallelRC_UI');
    add_block('ParallelRC_UI/ParallelRC', destination, 'Position', position);
    SetObject(destination, 'ResistanceValue', 'RVal/1', rObjRowPoints, rObjColPoints, rObjMatrix);
    SetObject(destination, 'TauValue', 'DivideTau/2', tauObjRowPoints, tauObjColPoints, tauObjMatrix);
    set_param([destination '/Derivative'], 'CoefficientInTFapproximation', num2str(derivativeTimeConstant));
end

end
