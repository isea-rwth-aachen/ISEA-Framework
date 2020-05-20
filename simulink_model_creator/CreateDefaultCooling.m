function[] = CreateDefaultCooling(destination, data, hasConvection, hasRadiation)

% data has 7 columns
% column 1: indices
% column 2: characteristic length
% column 3: area
% column 4: initial temperature
% column 5: location
% column 6: conductivity
% column 7: distance to grid vertex

if hasConvection || hasRadiation

baseDestination = destination;
destination = [destination '/DefaultCooling'];
[position, number] = GetPosition(destination);
destination = SetElemNoToDestination(destination, number);

add_block('ThermalModel/DefaultCooling', destination, 'Position', position);

if hasRadiation
    sigma = 0.0000000567;
    epsilon = 1.0;

    radiationString = [num2str(sigma * epsilon) '*( u.^4 - airTemperature^4  );'];
end
if hasConvection
    pr_Air = 0.73135;
    lambda_Air = 0.0262;
    ny_Air = 0.0000175;
    g = 9.81;
    
    characteristicLength = data(:, 2);
    preFactor = data(:, 5); %=locations: 0=TOP, 1=SIDE, 2=BOTTOM
    for i = 1:size(preFactor, 1)
        if preFactor(i, 1) == 0
            preFactor(i, 1) = 0.71;
        elseif preFactor(i, 1) == 1
            preFactor(i, 1) = 0.548;
         elseif preFactor(i, 1) == 2
            preFactor(i, 1) = 0;
        else
            throw 'Error in CreateDefaultCooling: location enum wrong';
        end
    end

    factor = preFactor * power(pr_Air * g / (ny_Air * ny_Air), 0.25) * lambda_Air;
    factorString = '[ ';
    for i = 1:size(factor, 1)
        factorString = [factorString num2str(factor(i)) ' * power(airTemperature * ' num2str(characteristicLength(i)) ', -0.25); '];
    end
    factorString = [factorString ']'];

    convectionString = [factorString ' .* power( abs(u - airTemperature), 1.25);'];
end

functionString = [sprintf('function y = DefaultCooling(u, airTemperature)\n') '%#codegen' newline];
if hasRadiation
    functionString = [functionString 'radiation = ' radiationString newline];
end
if hasConvection
    functionString = [functionString 'convection = ' convectionString newline];
end
if hasRadiation && hasConvection
    functionString = [functionString 'y = radiation + convection;' newline];
elseif hasRadiation
    functionString = [functionString 'y = radiation;' newline];
elseif hasConvection
    functionString = [functionString 'y = convection;' newline];
end
functionString = [functionString newline ...
                                 sprintf('for i = 1:size(u, 1)\n') ...
                                 sprintf('   if u(i, 1) <= airTemperature\n') ...
                                 sprintf('       y(i, 1) = 0;\n') ...
                                 sprintf('   end\n') ...
                                 sprintf('end\n\n') ...
                                 sprintf('end\n')];

fileID = fopen('DefaultCooling.m','w');
fprintf(fileID, '%s', functionString);
fclose(fileID);

set_param([destination '/Areas'], 'Gain', mat2str(data(:, 3)));

SetCoolingSelector(destination, data);
SetCoolingAdder(data, baseDestination);

end
end
