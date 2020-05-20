function CONFIGDOUBLE = convert2double(CONFIGSTR)
CONFIGDOUBLE = zeros(1, size(CONFIGSTR,2) + 1 );
for i = 1:size(CONFIGSTR, 2)
    CONFIGDOUBLE(i) = double(char(CONFIGSTR(i)));
end
CONFIGDOUBLE(end) = 0;