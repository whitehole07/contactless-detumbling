function dydt = stateEquation(t, y, D, C, q, tau, tspan, yD)
    % Compute D and C at present timestep
    n = size(q, 1);
    Dv = double(subs(D, [q; diff(q)], y));
    Cv = double(subs(C, [q; diff(q)], y));

    % tau
%     yD = [0.5 0 0 0 0 0]';
    dyD = [0 0 0 0 0 0]';
    yv = y(1:6);
    dyv = y(7:12);

    tmax = [10 10 10 10 10 10];
    KP = tmax / max(abs(yD - yv));
    KD = sqrt(2) * KP;

    u_bar = KD'.*(dyD - dyv) + KP'.*(yD - yv);
    tau = Dv*u_bar + Cv*dyv;

    % disp(tau);

    % y = [q, dq]
    dq = y(n+1:end);
    ddq = -Dv\(Cv*y(n+1:end)) + Dv\tau;

    % Return
    dydt = [dq; ddq];
    
    % Debug
    currentTime = datestr(now, 'dd-mmm-yyyy HH:MM:SS');
    fprintf("%s -> %.2f%% (%fs)\n", currentTime, (100*(t - tspan(1))/(tspan(end) - tspan(1))), t)
end