function[] = MatrixCreateZarc(destination, iD ...
                            , rObjRowPoints, rObjColPoints, rObjMatrix ...
                            , tauObjRowPoints, tauObjColPoints, tauObjMatrix ...
                            , phiObjRowPoints, phiObjColPoints, phiObjMatrix ...
                            , phi2RAussenPoints, phi2RAussenData ...
                            , phi2RInnenPoints, phi2RInnenData ...
                            , phi2TauPoints, phi2TauData )

[position, number] = GetPosition(destination);
destination = SetElemNoToDestination(destination, number);

add_block('ElectricalElements/Zarc', destination, 'Position', position);
SetObject(destination, 'Resistance', 'ResistanceGoto/1', ['ID' num2str(iD) '_ObjR'], rObjRowPoints, rObjColPoints, rObjMatrix);
set_param([destination '/ResistanceGoto'], 'GotoTag', ['ID' num2str(iD) '_ObjR']);
SetObject(destination, 'Tau', 'TauGoto/1', ['ID' num2str(iD) '_ObjTau'], tauObjRowPoints, tauObjColPoints, tauObjMatrix);
set_param([destination '/TauGoto'], 'GotoTag', ['ID' num2str(iD) '_ObjTau']);
SetObject(destination, 'Phi', 'PhiValue/1', ['ID' num2str(iD) '_ObjPhi'], phiObjRowPoints, phiObjColPoints, phiObjMatrix);

set_param([destination '/Phi2RAussen'], 'BP1', mat2str(phi2RAussenPoints));
set_param([destination '/Phi2RAussen'], 'Table', mat2str(phi2RAussenData));
set_param([destination '/Phi2RAussenGoto'], 'GotoTag', ['ID' num2str(iD) '_LookupPhi2RFactorAussen']);
set_param([destination '/Phi2RInnen'], 'BP1', mat2str(phi2RInnenPoints));
set_param([destination '/Phi2RInnen'], 'Table', mat2str(phi2RInnenData));
set_param([destination '/Phi2RInnenGoto'], 'GotoTag', ['ID' num2str(iD) '_LookupPhi2RFactorInnen']);
set_param([destination '/Phi2Tau'], 'BP1', mat2str(phi2TauPoints));
set_param([destination '/Phi2Tau'], 'Table', mat2str(phi2TauData));
set_param([destination '/Phi2TauGoto'], 'GotoTag', ['ID' num2str(iD) '_LookupPhi2TauFactor']);

end
