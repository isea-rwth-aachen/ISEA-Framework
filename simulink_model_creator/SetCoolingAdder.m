function[] = SetCoolingAdder(data, baseDestination)

global CoolingNotYetAdded;
if(CoolingNotYetAdded)
    startIndex = 0;
    endIndex = size(data, 1);
    CoolingNotYetAdded = false;
else
    startIndex = get_param([baseDestination '/CoolAdder/Demux'], 'Outputs');
    startIndex = str2double(startIndex);
    endIndex = startIndex + size(data, 1);
end
set_param([baseDestination '/CoolAdder/Demux'], 'Outputs', num2str(endIndex));
set_param([baseDestination '/CoolAdder/Demux'], 'Position', [230 75 235 75+15*endIndex]);

for i = (startIndex + 1):endIndex
    FVIndex = data(i - startIndex, 1) + 1;
    
    adderName = ['Add_FV_' num2str(FVIndex)];
    adderNameFullPath = [baseDestination '/CoolAdder/' adderName];
    adderInputsString = get_param(adderNameFullPath, 'Inputs');
    if strcmp(adderInputsString, '-')
        set_param(adderNameFullPath, 'Inputs', '+');
        numberOfInputs = 1;
    else
        adderInputsString = [adderInputsString '+'];
        set_param(adderNameFullPath, 'Inputs', adderInputsString);
        numberOfInputs = size(adderInputsString, 2);
    end
    
    add_line([baseDestination '/CoolAdder'], ['Demux/' num2str(i)], [adderName '/' num2str(numberOfInputs)], 'autorouting', 'on');
end

end
