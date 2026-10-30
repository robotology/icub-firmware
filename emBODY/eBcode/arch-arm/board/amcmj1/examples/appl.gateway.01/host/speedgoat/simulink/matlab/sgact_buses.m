function sgact_buses()
%SGACT_BUSES creates in the base workspace the bus objects between the I/O blocks and the Controller.
%   Every bus is made from its default struct, so the bus and the MATLAB code can never disagree:
%     SGACT_FB    sgact_fb_default    amc.gtw -> the Controller            (amc.gtw RX output)
%     SGACT_REQ   sgact_req_default   the requester -> the Controller       (panel RX output)
%     SGACT_CMD   sgact_cmd_default   the Controller -> amc.gtw            (amc.gtw TX input)
%     SGACT_SUP   sgact_sup_default   the Controller -> the requester       (panel TX input)
%     SGACT_RUN   sgact_run_default   SUP-A -> USER CONTROL, SUP-B          (inside the Controller)
%     SGACT_SP    sgact_sp_default    USER CONTROL -> SUP-B                 (inside the Controller)
%   the models call it when they are loaded (PreLoadFcn). To add a field: add it to the default struct, then reload.
    defs = {'SGACT_FB', sgact_fb_default(); 'SGACT_REQ', sgact_req_default(); ...
            'SGACT_CMD', sgact_cmd_default(); 'SGACT_SUP', sgact_sup_default(); ...
            'SGACT_RUN', sgact_run_default(); 'SGACT_SP', sgact_sp_default()};
    for i = 1:size(defs, 1)
        assignin('base', defs{i, 1}, bus_of(defs{i, 2}, defs{i, 1}));
    end
end


function b = bus_of(s, name)
    f = fieldnames(s);
    el = Simulink.BusElement.empty(0, numel(f));
    for k = 1:numel(f)
        v = s.(f{k});
        e = Simulink.BusElement;
        e.Name = f{k};
        if islogical(v)
            e.DataType = 'boolean';
        else
            e.DataType = class(v);
        end
        if isscalar(v)
            e.Dimensions = 1;
        else
            e.Dimensions = size(v);
        end
        el(k) = e;
    end
    b = Simulink.Bus;
    b.Description = sprintf('%s, made by sgact_buses.m from its default struct', name);
    b.Elements = el;
end
