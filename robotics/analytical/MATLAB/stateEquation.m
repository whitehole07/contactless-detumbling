function dydt = stateEquation(t, y, D, C, q, tau, tspan)
    % Compute D and C at present timestep
    n = size(q, 1);
    Dv = double(subs(D, [q; diff(q)], y));
    Cv = double(subs(C, [q; diff(q)], y));

    % y = [q, dq]
    dq = y(n+1:end);
    ddq = -Dv\(Cv*y(n+1:end)) + Dv\tau;

    % Return
    dydt = [dq; ddq];
    
    % Debug
    currentTime = datestr(now, 'dd-mmm-yyyy HH:MM:SS');
    fprintf("%s -> %.2f%% (%fs)\n", currentTime, (100*(t - tspan(1))/(tspan(end) - tspan(1))), t)
end