function sl_setp(blk, varargin)
%SL_SETP sets the parameters one by one: a name that this release does not know gives a warning, not an error
    for i = 1:2:numel(varargin)
        try
            set_param(blk, varargin{i}, varargin{i + 1});
        catch e
            warning('build:param', '%s: cannot set %s = %s (%s)', blk, varargin{i}, varargin{i + 1}, e.message);
        end
    end
end
