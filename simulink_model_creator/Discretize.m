function [] = Discretize(system)

if nargin == 0
    system = 'BuildMatrixModel';
end

integrators = find_system(system, 'BlockType', 'Integrator');

for i = 1:length(integrators)
    block = integrators{i};
    blockSystem = block(1 : find(block == '/', 1, 'last') - 1);
    position = get_param(block, 'Position');
    name = get_param(block, 'Name');
    connections = get_param(block, 'PortConnectivity');
    reset = get_param(block, 'ExternalReset');
    initSource = get_param(block, 'InitialConditionSource');
    initValue = get_param(block, 'InitialCondition');
    limitOutput = get_param(block, 'LimitOutput');
    upperLimit = get_param(block, 'UpperSaturationLimit');
    lowerLimit = get_param(block, 'LowerSaturationLimit');
    
    for j = 1:length(connections)
        conn = connections(j);
        if isempty(conn.DstPort)
            for k = 1:length(conn.SrcBlock)
                srcName = get_param(conn.SrcBlock(k), 'Name');
                delete_line(blockSystem, [srcName '/' num2str(conn.SrcPort(k)+1)], [name '/' conn.Type]);
            end
        else
            for k = 1:length(conn.DstBlock)
                dstName = get_param(conn.DstBlock(k), 'Name');
                delete_line(blockSystem, [name '/' conn.Type], [dstName '/' num2str(conn.DstPort(k)+1)]);
            end
        end
    end
    delete_block(block);
    
    add_block('simulink/Discrete/Discrete-Time Integrator', block, 'Position', position);
    set_param(block, 'ExternalReset', reset);
    set_param(block, 'InitialConditionSource', initSource);
    set_param(block, 'InitialCondition', initValue);
    set_param(block, 'LimitOutput', limitOutput);
    set_param(block, 'UpperSaturationLimit', upperLimit);
    set_param(block, 'LowerSaturationLimit', lowerLimit);
    for j = 1:length(connections)
        conn = connections(j);
        if isempty(conn.DstPort)
            for k = 1:length(conn.SrcBlock)
                srcName = get_param(conn.SrcBlock(k), 'Name');
                add_line(blockSystem, [srcName '/' num2str(conn.SrcPort(k)+1)], [name '/' conn.Type], 'autorouting', 'on');
            end
        else
            for k = 1:length(conn.DstBlock)
                dstName = get_param(conn.DstBlock(k), 'Name');
                add_line(blockSystem, [name '/' conn.Type], [dstName '/' num2str(conn.DstPort(k)+1)], 'autorouting', 'on');
            end
        end
    end
end

