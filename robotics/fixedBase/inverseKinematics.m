function q = inverseKinematics(dh, com, T_desired, threshold, max_search, max_iter)
    % Define maximum number of iterations
    max_iterations = max_iter;
    
    % Iterate until convergence or maximum iterations reached
    for search = 1:max_search
        a = 0; % Lower boundary
        b = 2*pi; % Upper boundary
        n = 6; % Number of random values
        % Generate uniform random values and ensure they are within [a, b]
        q = a + (b-a) * rand(1, n).';

        disp("new search")

        for iter = 1:max_iterations
            % Compute end-effector transformation matrix based on current joint angles
            T_current = forwardKinematics(q, dh);
            
            % Compute error in end-effector pose
            error = angle_axis(T_current, T_desired);
            
            % Check if error is below threshold
            if norm(error) < threshold
                disp('Converged')
                Tf = forwardKinematics(q, dh)
                return;
            end
            
            % Compute Jacobian matrix
            J_current = computeJacobian(q, dh, com);
            
            % Compute pseudo-inverse of Jacobian matrix
            J_pseudo_inv = pinv(J_current);
            
            % Compute joint angle increments
            delta_q = J_pseudo_inv * error;
            
            % Update joint angles
            q = q + delta_q;
        end
    end
end

function T0i = forwardKinematics(q, dh)
    % Implement forward kinematics to compute end-effector transformation matrix
    % based on joint angles q
    % Example implementation:
    % T = computeEndEffectorTransformation(q);
    n = size(q, 1);

    % Init transf mat
    T0i = eye(4);

    for i = 1:n
        % Get ith transf
        Ti = transformation(i, q(i), dh);

        % Get total transf
        T0i = T0i * Ti;
    end
end

function Ti = transformation(i, qi, dh)
    Ti = [ % theta, a, d, alpha
        cos(qi), -sin(qi)*cos(dh(i, 4)), sin(qi)*sin(dh(i, 4)), dh(i, 2)*cos(qi)
        sin(qi), cos(qi)*cos(dh(i, 4)), -cos(qi)*sin(dh(i, 4)), dh(i, 2)*sin(qi)
        0, sin(dh(i, 4)), cos(dh(i, 4)), dh(i, 3)
        0, 0, 0, 1
    ];

    Ti = double(Ti);
end

function J = computeJacobian(q, dh, com)
    % Compute Jacobian matrix
    % Example implementation:
    % J = computeJacobianMatrix(q);
    n = size(q, 1);
    
    % Jacobians link i
    JPi = zeros(3, n);
    JOi = zeros(3, n);

    % Position of CoM with respect to the base frame
    T0i = eye(4);
    for j = 1:n; T0i = T0i*transformation(j, q(j), dh); end % Transformation matrices
    pli = T0i(1:3, 4) + T0i(1:3, 1:3) * com(n, :).';
    
    % Init transformation matrix
    T0jp = eye(4);
    for j = 1:n   
        % Update Jacobians
        JPi(:, j) = cross(T0jp(1:3, 3), pli - T0jp(1:3, 4));
        JOi(:, j) = T0jp(1:3, 3);

        % Update transformation matrix
        T0jp = T0jp*transformation(j, q(j), dh);
    end

    % Assemble
    J = [
        JPi
        JOi
    ];
end

function e = angle_axis(T, Td)
    % angle_axis calculates the error vector between T and Td in angle-axis form.
    %
    % Inputs:
    %   T  - The current pose (4x4 transformation matrix)
    %   Td - The desired pose (4x4 transformation matrix)
    %
    % Output:
    %   e - The error vector between T and Td (6x1 vector)

    e = zeros(6, 1);

    % The position error
    e(1:3) = Td(1:3, 4) - T(1:3, 4);
    
    % Orientation error
    R = Td(1:3, 1:3) * T(1:3, 1:3)';
    li = [R(3, 2) - R(2, 3), R(1, 3) - R(3, 1), R(2, 1) - R(1, 2)];

    if norm(li) < 1e-6
        % If li is a zero vector (or very close to it)
        % diagonal matrix case
        if trace(R) > 0
            % (1,1,1) case
            a = zeros(3, 1);
        else
            a = (pi / 2) * (diag(R) + 1);
        end
    else
        % non-diagonal matrix case
        ln = norm(li);
        a = atan2(ln, trace(R) - 1) * (li / ln);
    end

    e(4:6) = a;
end

