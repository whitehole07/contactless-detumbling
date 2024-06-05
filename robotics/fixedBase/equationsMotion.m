% Find UR10 standard robotic arm equations of motion

%% Clean up
clc
close all
clear

%% Define parameters
syms t1(t) t2(t) t3(t) t4(t) t5(t) t6(t)

% Joint variables
q = [t1(t); t2(t); t3(t); t4(t); t5(t); t6(t)];
tau = [0; 0; 0; 0; 0; 0];

%% 
% Denavit-Hartenberg Parameters
dh = [ % theta, a, d, alpha
    t1(t),   0,  0.5, pi/2
    t2(t), 2.5,    0, 0
    t3(t), 2.5,    0, 0
    t4(t),   0,  0.5, pi/2
    t5(t),   0, 0.25, -pi/2
    t6(t),   0, 0.25, 0
    ];

% Yoshida et al., 1993
m = [5 50 50 10 5 5];

com = [
    0, -dh(1, 3)/2, 0
    dh(2, 2)/2, 0, 0
    dh(3, 2)/2, 0, 0
    0, -dh(4, 3)/2, 0 
    0, dh(5, 3)/2, 0
    0, 0, -dh(6, 3)/2
];

% Moment of inertia
Im = zeros(3, 3, 6);
Im(:, :, 1) = diag([0.0063 0.0292 0.0292]);
Im(:, :, 2) = diag([0.0625 26.1000 26.1000]);
Im(:, :, 3) = diag([0.0625 26.1000 26.1000]);
Im(:, :, 4) = diag([0.0125 0.2150 0.2150]);
Im(:, :, 5) = diag([0.0063 0.0292 0.0292]);
Im(:, :, 6) = diag([0.0292 0.0292 0.0063]);

% Degrees of freedom
n = size(dh, 1);

% Test inverse kinematics
T_desired = [
         1         0         0   -3.0000
         0         1         0    0.0000
         0         0         1    0.0000
         0         0         0    1.0000
];
q_desired = inverseKinematics(dh, com, T_desired, 1e-3, 100, 20)

% Load matrices 
load("C.mat")
load("D.mat")

%% Transformation matrix
% Generic transformation matrix between adjacent frames using DH convention
T = @(j) [ % theta, a, d, alpha
    cos(dh(j, 1)), -sin(dh(j, 1))*cos(dh(j, 4)), sin(dh(j, 1))*sin(dh(j, 4)), dh(j, 2)*cos(dh(j, 1))
    sin(dh(j, 1)), cos(dh(j, 1))*cos(dh(j, 4)), -cos(dh(j, 1))*sin(dh(j, 4)), dh(j, 2)*sin(dh(j, 1))
    0, sin(dh(j, 4)), cos(dh(j, 4)), dh(j, 3)
    0, 0, 0, 1
    ];

%% Inertia matrix
% % Init inertia matrix
% D = zeros(n);
% % Loop
% for i = 1:n
%     % Jacobians link i
%     JPi = sym(zeros(3, n));
%     JOi = sym(zeros(3, n));
% 
%     % Position of CoM
%     T0i = eye(4); T0ip = eye(4); 
%     for j = 1:i; T0ip = T0i; T0i = T0i*T(j); end % Transformation matrices
%     pli = T0ip(1:3, 4) + T0i(1:3, 1:3) * com(i, :).';  % THIS IS PROBABLY
%     WRONG
%     
%     % Init transformation matrix
%     T0jp = eye(4);
%     for j = 1:i     
%         % Update Jacobians
%         JPi(:, j) = cross(T0jp(1:3, 3), pli - T0jp(1:3, 4));
%         JOi(:, j) = T0jp(1:3, 3);
% 
%         % Update transformation matrix
%         T0jp = T0jp*T(j);
%     end
% 
%     % Sum contribution
%     D = D + m(i)*(JPi.')*JPi + (JOi.')*T0i(1:3, 1:3)*Im(:, :, i)*(T0i(1:3, 1:3).')*JOi;
% end
% 
% % Save
% D = simplify(D);
% fprintf("Simplified D")
% save("D.mat", "D");
% matToJSON("D.json", D);
% 
% %% Christoffel symbols
% % Init C tensor
% C = sym(zeros(n));
% for j = 1:n
%     for k = 1:n
%         % Init value
%         ckj = 0;
% 
%         % Summation of christoffels
%         for i = 1:n
%             ckj = ckj + 0.5 * (diff(D(k, j), q(i)) + diff(D(k, i), q(j)) - diff(D(i, j), q(k))) * diff(q(i), t);
%             ckj = simplify(ckj);
%             fprintf("Simplified C (%d, %d, %d)\n", i, j, k)
%         end
% 
%         % Assign value
%         C(k, j) = ckj;
%     end
% end
% 
% % Save
% save("C.mat", "C");
% matToJSON("C.json", C);

%% Initial parameters
tspan = [0, 50];
y0 = [0; -0.7; -0.3; 0; 0; 0; -0.02; 0; 0; 0; 0; 0];

% Solve differential equation
options = odeset('RelTol', 1e-2, 'AbsTol', 1e-4); % Set relative tolerance to 1e-6
[timee, y] = ode15s(@(t, y) stateEquation(t, y, D, C, q, tau, tspan, q_desired), tspan, y0, options);

%% Animate
% Plot evolution
plotMovement(dh, y(:, 1:n));

%% Plot
% Plot joint angles
figure;
time = linspace(tspan(1), tspan(end), size(y, 1));
plot(time, timee);
figure
plot(time, rad2deg(y(:, 1:n)), "linewidth", 2)
xlabel("Time [s]")
ylabel("Joint angle [deg]")
legend("$\theta_1$", "$\theta_2$", "$\theta_3$", "$\theta_4$", "$\theta_5$", "$\theta_6$", 'Interpreter', 'latex')
grid on
title("Evolution of Joint Angles")

% Plot joint velocities
figure;
time = linspace(tspan(1), tspan(end), size(y, 1));
plot(time, rad2deg(y(:, n+1:end)), "linewidth", 2)
xlabel("Time [s]")
ylabel("Joint angular velocity [deg/s]")
legend("$\dot{\theta}_1$", "$\dot{\theta}_2$", "$\dot{\theta}_3$", "$\dot{\theta}_4$", "$\dot{\theta}_5$", "$\dot{\theta}_6$", 'Interpreter', 'latex')
grid on
title("Evolution of Joint Angular Velocities")

% %% Test
% val = [0,-0.7,0,0,0,0,0.1,0,0,0,0,0];
% double(subs(D, [t1(t) t2(t) t3(t) t4(t) t5(t) t6(t) diff(t1(t), t) diff(t2(t), t) diff(t3(t), t) diff(t4(t), t) diff(t5(t), t) diff(t6(t), t)], val))
% double(subs(C, [t1(t) t2(t) t3(t) t4(t) t5(t) t6(t) diff(t1(t), t) diff(t2(t), t) diff(t3(t), t) diff(t4(t), t) diff(t5(t), t) diff(t6(t), t)], val))
% 
% %% Load custom
% p = csvread("prop_result.csv", 1, 0);
% t = p(:, 1);
% p = p(:, 2:n+n+1);
% 
% % plotMovement(dh, p(:, 1:n));
% 
% % Plot joint angles
% figure;
% time = linspace(t(1), t(end), size(p, 1));
% plot(time, rad2deg(p(:, 1:n)), "linewidth", 2)
% xlabel("Time [s]")
% ylabel("Joint angle [deg]")
% legend("$\theta_1$", "$\theta_2$", "$\theta_3$", "$\theta_4$", "$\theta_5$", "$\theta_6$", 'Interpreter', 'latex')
% grid on
% title("Evolution of Joint Angles")
% 
% % Plot joint velocities
% figure;
% plot(time, rad2deg(p(:, n+1:end)), "linewidth", 2)
% xlabel("Time [s]")
% ylabel("Joint angular velocity [deg/s]")
% legend("$\dot{\theta}_1$", "$\dot{\theta}_2$", "$\dot{\theta}_3$", "$\dot{\theta}_4$", "$\dot{\theta}_5$", "$\dot{\theta}_6$", 'Interpreter', 'latex')
% grid on
% title("Evolution of Joint Angular Velocities")
