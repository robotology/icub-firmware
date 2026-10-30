function sl_sender(sys, name, pos, localip, localport, toip, toport)
%SL_SENDER an enabled subsystem with a UDP Send: inputs data, len (bytes to send) and the enable. It sends only when enabled
    sub = [sys '/' name];
    add_block('built-in/Subsystem', sub, 'Position', pos);
    add_block('built-in/Inport', [sub '/data'], 'Position', [40 50 70 64]);
    add_block('built-in/Inport', [sub '/len'], 'Position', [40 110 70 124]);
    add_block('built-in/EnablePort', [sub '/go'], 'Position', [190 10 210 30]);
    snd = [sub '/UDP Send'];
    add_block('slrealtimeiplib/UDP Send', snd, 'Position', [160 40 360 140]);
    sl_setp(snd, 'ipAddress', localip, 'localPort', num2str(localport), 'toAddress', toip, 'toPort', num2str(toport), 'sampleTime', '-1');
    ph = get_param(snd, 'PortHandles');
    add_line(sub, 'data/1', 'UDP Send/1', 'autorouting', 'smart');
    if numel(ph.Inport) >= 2
        add_line(sub, 'len/1', 'UDP Send/2', 'autorouting', 'smart');
    else
        warning('build:udpsend', '%s: the UDP Send has no Length input (Enable Simulink messages on?): turn it off', snd);
    end
end
