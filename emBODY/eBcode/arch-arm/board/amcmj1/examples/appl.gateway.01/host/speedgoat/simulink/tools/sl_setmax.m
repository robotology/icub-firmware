function sl_setmax(blk)
%SL_SETMAX a UDP Receive takes one packet per step: the name of the parameter changed between releases
    try
        set_param(blk, 'maxPackets', '1');
    catch
        try
            set_param(blk, 'maxPacket', '1');
        catch
        end
    end
end
