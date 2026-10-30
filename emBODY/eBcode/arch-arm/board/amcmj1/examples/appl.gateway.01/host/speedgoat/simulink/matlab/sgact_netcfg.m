function n = sgact_netcfg()
%SGACT_NETCFG the network of the Speedgoat model: used only by build_sgact.m to fill in the UDP blocks
%   Host Link (enp1s0) : the PC with the panel        ETH1 (enp2s0) : amc.gtw, the CM4 of the amcmj1 board
%   change here and rebuild the model (build_sgact) if the addresses change

    % Host Link: panel <-> Speedgoat
    n.HOSTLINK_IP = '10.0.2.150';   % the Speedgoat on the Host Link
    n.PANEL_IP = '10.0.2.104';      % the PC that runs sgact_panel.py: the STATUS goes there
    n.PORT_CTL = 6000;              % CTL from the panel (the Speedgoat listens)
    n.PORT_STATUS = 6001;           % STATUS to the panel (the panel listens)
    n.LOCAL_STATUS = 26001;         % local port of the STATUS sender

    % ETH1: Speedgoat <-> amc.gtw
    n.ETH1_IP = '10.0.1.150';       % the Speedgoat on ETH1
    n.AMCGTW_IP = '10.0.1.99';       % amc.gtw on the amcmj1 board
    n.PORT_CMD = 5001;              % command frames to amc.gtw
    n.PORT_RPC = 5002;              % rpc frames to amc.gtw (enable / disable)
    n.PORT_STATE = 5003;            % state frames from amc.gtw (it sends them to the IP of the command frames)
    n.LOCAL_CMD = 25001;            % local port of the command sender
    n.LOCAL_RPC = 25002;            % local port of the rpc sender
end
