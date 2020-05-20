function[] = CreateZarc(destination, isIU ...
                      , rObjRowPoints, rObjColPoints, rObjMatrix ...
                      , tauObjRowPoints, tauObjColPoints, tauObjMatrix ...
                      , phiObjRowPoints, phiObjColPoints, phiObjMatrix)

[position, number] = GetPosition(destination);
destination = SetElemNoToDestination(destination, number);

if(isIU)
    load_system('ElectricalTwoPorts/Zarc');
    add_block('Zarc/Zarc', destination, 'Position', position);
    SetObject(destination, 'ResistanceValue', 'Res/1', rObjRowPoints, rObjColPoints, rObjMatrix);
    SetObject(destination, 'TauValue', 'Tau/1', tauObjRowPoints, tauObjColPoints, tauObjMatrix);
    SetObject(destination, 'PhiValue', 'Phi/1', phiObjRowPoints, phiObjColPoints, phiObjMatrix);
else
    load_system('ElectricalTwoPorts/Zarc_UI');
    add_block('Zarc_UI/Zarc', destination, 'Position', position);
    SetObject(destination, 'ResistanceValue', 'Res/1', rObjRowPoints, rObjColPoints, rObjMatrix);
    SetObject(destination, 'TauValue', 'Tau/1', tauObjRowPoints, tauObjColPoints, tauObjMatrix);
    SetObject(destination, 'PhiValue', 'Phi/1', phiObjRowPoints, phiObjColPoints, phiObjMatrix);
end

end
