function[] = CreateGotoBus(system ) %, symbolicString)
    global gotoBus;
    uniqueElements = unique( gotoBus.elements);
    system_components = find_system(system);

    gotoElements = get_param( system_components(find (strcmp( get_param(system_components, 'BlockType'),  'From') == 1)), 'GotoTag');
    indices = [];
    for i = 1: size(uniqueElements,2)
        indices = [indices; find( strcmp( uniqueElements{i} , gotoElements ) == 1) ];
    end    
    1+1
    indices = unique(indices);
    
end
