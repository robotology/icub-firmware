function sub = sl_enabled_subsystem(parent, name, pos)
%SL_ENABLED_SUBSYSTEM a subsystem of USER CONTROL: inputs run (bus SGACT_RUN) and fb (bus SGACT_FB), output sp (bus
%   SGACT_SP), an enable port. It runs only while enabled, its states restart from their initial values at every
%   enable (a START), its output is held while it is disabled (the Merge takes the one of the enabled subsystem)
    sub = [parent '/' name];
    add_block('built-in/Subsystem', sub, 'Position', pos);
    add_block('simulink/Ports & Subsystems/Enable', [sub '/Enable'], 'Position', [300 20 320 40]);
    sl_setp([sub '/Enable'], 'StatesWhenEnabling', 'reset');
    add_block('built-in/Inport', [sub '/run'], 'Position', [40 100 70 114]);
    sl_setp([sub '/run'], 'OutDataTypeStr', 'Bus: SGACT_RUN', 'BusOutputAsStruct', 'on');
    add_block('built-in/Inport', [sub '/fb'], 'Position', [40 300 70 314]);
    sl_setp([sub '/fb'], 'OutDataTypeStr', 'Bus: SGACT_FB', 'BusOutputAsStruct', 'on');
    add_block('built-in/Outport', [sub '/sp'], 'Position', [760 200 790 214]);
    sl_setp([sub '/sp'], 'OutDataTypeStr', 'Bus: SGACT_SP', 'OutputWhenDisabled', 'held', 'InitialOutput', 'sgact_sp_default()');
end
