function on = sgact_std_on(run)
%SGACT_STD_ON true while a standard mode (impl = matlab) runs or stops: the enable of the block "standard modes"
    s = sgact_spec();
    on = false;
    if run.phase > 0
        for i = 1:s.NMODES
            if s.MODE_ID(i) == run.mode && s.MODE_IMPL(i) == 1
                on = true;
            end
        end
    end
end
