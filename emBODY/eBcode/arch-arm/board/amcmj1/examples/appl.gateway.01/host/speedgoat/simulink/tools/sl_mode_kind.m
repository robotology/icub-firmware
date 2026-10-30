function kind = sl_mode_kind(name)
%SL_MODE_KIND the starting content of the subsystem mode_<name> (see sl_mode_subsystem): the modes of sgact_panel.toml
%   that come w/ a working start, else 'template'
    switch name
        case 'motor_cubemars_user'
            kind = 'mit';                   % the MIT from the parameters
        case 'spark_dual_motor_actuation'
            kind = 'spark';                 % the MIT from the parameters, the motor_macro at 0, all the feedback selected
        otherwise
            kind = 'template';
    end
end
