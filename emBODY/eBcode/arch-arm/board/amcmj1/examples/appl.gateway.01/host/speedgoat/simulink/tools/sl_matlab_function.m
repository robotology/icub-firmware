function sl_matlab_function(blk, code, types)
%SL_MATLAB_FUNCTION sets the code of a MATLAB Function block and the data type of some of its ports
%   types: {'portname', 'Bus: SGACT_FB'; ...}  (the buses must exist when the model is compiled: sgact_buses)
    rt = sfroot;
    ch = rt.find('-isa', 'Stateflow.EMChart', 'Path', blk);
    ch.Script = code;
    for i = 1:size(types, 1)
        d = ch.find('-isa', 'Stateflow.Data', 'Name', types{i, 1});
        if isempty(d)
            warning('build:port', '%s: no port named %s', blk, types{i, 1});
        else
            d.DataType = types{i, 2};
        end
    end
end
