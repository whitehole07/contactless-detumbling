function q = inverseKinematics(dh, com, T_desired, q_initial, threshold, max_iter)
    % Initialize joint angles
    q = q_initial;
    
    % Define maximum number of iterations
    max_iterations = max_iter;
    
    % Iterate until convergence or maximum iterations reached
    for iter = 1:max_iterations
        % Compute end-effector transformation matrix based on current joint angles
        T_current = forwardKinematics(q, dh);
        
        % Compute error in end-effector pose
        error = computeError(T_desired, T_current);
        
        % Check if error is below threshold
        if norm(error) < threshold
            disp('Converged')
            break;
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

function error = computeError(T_desired, T_current)
    % Compute error between desired and current end-effector pose
    % Convert transformation matrices to homogeneous matrices
    % Compute the error as the difference between the desired and current
    % transformation matrices
    % Orientation error
    R_current = T_current(1:3, 1:3);
    R_desired = T_desired(1:3, 1:3);

    R_error = R_desired' * R_current;
    [axis, angle] = custom_rotm2axang(R_error);

    % Position error
    r_error = T_desired(1:3, 4) - T_current(1:3, 4);
    
    % Extract the position and orientation errors from the error_homog matrix
    error = [r_error; angle * axis];
end

function [axis, angle] = custom_rotm2axang(R)
    % Compute the axis and angle from the rotation matrix
    % Handling singularities using the method described in:
    % https://en.wikipedia.org/wiki/Axis%E2%80%93angle_representation#Conversion_from_rotation_matrix

    % Ensure R is a valid rotation matrix
    assert(isequal(size(R), [3 3]), 'Input must be a 3x3 matrix');

    % Calculate the trace of the rotation matrix
    trace_R = trace(R);
    
    if trace_R > 2 - eps
        % Case when trace is greater than 2
        % This corresponds to the rotation angle being close to zero
        angle = 0;
        axis = [0; 0; 1]; % Any axis can be chosen since angle is zero
    elseif trace_R < -1 + eps
        % Case when trace is less than -1
        % This corresponds to the rotation angle being close to pi
        [~, i] = max([R(1, 1), R(2, 2), R(3, 3)]);
        v = sqrt((R(i, i) + 1) / 2) * [R(1, i); R(2, i); R(3, i)] / 2;
        angle = pi;
        axis = v / norm(v);
    else
        % Case when the angle is between 0 and pi
        angle = acos((trace_R - 1) / 2);
        axis = 1 / (2 * sin(angle)) * [R(3, 2) - R(2, 3); R(1, 3) - R(3, 1); R(2, 1) - R(1, 2)];
    end
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
