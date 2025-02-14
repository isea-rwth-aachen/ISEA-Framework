%% Copy XML file
function CopyXMLFile(pathToFinalRelease, xmlFilename, model, fileID)

if(strcmp(model, 'thermal_electrical_discrete'));
    model = 'thermal_electrical';
end

myFilenameString = [pathToFinalRelease, '/scenarios/', model '/', xmlFilename];
copyfile(myFilenameString);
assignin('base', 'CONFIG_STR_A', fileread(xmlFilename) );
assignin('base', 'CONFIG_STR_B', '');
assignin('base', 'CONFIG_STR_C', '');

fprintf(fileID, [GetTimeStamp(), ' Copied xml file "%s". \n'], myFilenameString);

end