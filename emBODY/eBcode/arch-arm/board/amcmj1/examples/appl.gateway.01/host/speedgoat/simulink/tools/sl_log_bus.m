function sl_log_bus(sys, src, name, fields, prefix, xy)
%SL_LOG_BUS some elements of a bus -> named and logged scalar signals (the Data Inspector picks them while streaming)
%   src: 'block/port' of the bus, fields: {'motor_cubemars_pos', ...}, prefix: the signal of the field f is named <prefix>f
%   the lines out of a Bus Selector keep the name of the element and cannot be renamed: each one goes through a
%   Signal Conversion, whose output line gets the name and the logging
    k = numel(fields);
    sel = [sys '/' name];
    add_block('simulink/Signal Routing/Bus Selector', sel, 'Position', [xy(1) xy(2) xy(1) + 5 xy(2) + 25 * k]);
    set_param(sel, 'OutputSignals', strjoin(fields, ','));
    add_line(sys, src, [name '/1'], 'autorouting', 'smart');
    for i = 1:k
        sig = [prefix fields{i}];
        conv = [sig ' conv'];
        y = xy(2) + 25 * (i - 1) + 4;
        add_block('simulink/Signal Attributes/Signal Conversion', [sys '/' conv], 'Position', [xy(1) + 60 y xy(1) + 90 y + 15], ...
                  'ShowName', 'off');
        add_block('simulink/Sinks/Terminator', [sys '/' sig ' sink'], 'Position', [xy(1) + 200 y xy(1) + 215 y + 15], 'ShowName', 'off');
        add_line(sys, sprintf('%s/%d', name, i), [conv '/1'], 'autorouting', 'smart');
        lh = add_line(sys, [conv '/1'], [sig ' sink/1'], 'autorouting', 'smart');
        set_param(lh, 'Name', sig);
        set_param(get_param(lh, 'SrcPortHandle'), 'DataLogging', 'on');
    end
end
