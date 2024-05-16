function dydt = stateEqFF(t, y, q, b, bh, dh, m, com, Im, m_sc, I0, ...
    dxr0i, dqr0i, dxIi, dqIi, dxJTi, dqJTi, dxJRi, dqJRi, tau, tspan)
    % Compute D and C at present timestep
    n = size(q, 1);

    % Get H_star and C_star
    [H, H_dot, c] = getMatrix( ...
        q, y(12+n+1:end), y(7:6+n), b, y(6+n+1:end-n), y(1:6), bh, dh, ...
        m, com, Im, m_sc, I0, ...
        dxr0i, dqr0i, dxIi, dqIi, dxJTi, dqJTi, dxJRi, dqJRi);

%     % tau
%     yD = [5; 0; 0; 0; 0; 0; 0; 0.5; 0.5; 0; 0; 0];
%     dyD = [0; 0; 0; 0; 0; 0; 0; 0; 0; 0; 0; 0];
%     yv = y(1:12);
%     dyv = y(13:end);
% 
%     tmax = [0 0 0 0 0 0 10 10 10 10 10 10];
%     if max(abs(yD - yv))
%         KP = tmax / max(abs(yD - yv));
%     else
%         KP = zeros(1, 6+n);
%     end
%     KD = sqrt(2) * KP;
% 
%     u_bar = KD'.*(dyD - dyv) + KP'.*(yD - yv);
%     tau = H*u_bar + H_dot*dyv + c;

    % y = [b, q, db, dq]
    dx = y(n+6+1:end);
    ddx = -H\(H_dot*dx) - H\c + H\tau;

    % Return
    dydt = [dx; ddx];
    
    % Debug
    currentTime = datestr(now, 'dd-mmm-yyyy HH:MM:SS');
    fprintf("%s -> %.2f%% (%fs)\n", currentTime, (100*(t - tspan(1))/(tspan(end) - tspan(1))), t)
end