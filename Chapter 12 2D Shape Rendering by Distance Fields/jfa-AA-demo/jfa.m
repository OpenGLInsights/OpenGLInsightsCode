% dist = JFA(img [, type])
%
% Naive software implementation of the "jump flooding algoritm"
% (JFA) as described by Rong Guodong in his 2007 PhD dissertation. 
% This is inherently a massively parallel algorithm, so this
% sequential software implementation is not terribly efficient.
% This code was written for demonstration and evaluation, not
% for benchmarking. Clarity was chosen over speed.
%
% Input 'img' is an NxM image with 0 representing background pixels
% and any other value representing object pixels.
% Input 'type' defaults to 0, and chooses a variant of the algorithm:
% type = 0 : plain JFA algorithm (halving step size with each pass)
% type = 1 : JFA+1 algorithm (one extra final pass with step size 1)
% type = 2 : JFA+2 algorithm (two extra final passes with step sizes 2 and 1)
% type = 3 : 1+JFA algorithm (one extra initial pass with step size 1)
% type = 4 : JFA^2 algorithm (two full iterations of the JFA algorithm)
% type = -1 : No jumps are performed, and the algorithm is a traditional
% slow flood fill with step size 1 for every iteration. The algorithm
% might require max(N,M) steps in the worst case, but stops when no
% changes are made. (type = -1 is for reference and evaluation purposes
% only. The Matlab function BWDIST from the Image Processing Toolbox
% is significantly faster and produces the same result. In fact, this
% entire function is terribly slow when run in software like this.)
% Output 'dist' is an NxMx2 array, where the two channels represent
% the row and column offsets to the nearest object pixel.
%
% Author: Stefan Gustavson, ITN-LiTH (stegu@itn.liu.se) 2010-08-09

function dist = jfa(img, type)
    if(nargin < 2)
        type = 0;
    end
    [h,w] = size(img);
    dist = zeros([h,w,2]);
    distx = dist(:,:,1);
    disty = dist(:,:,2);
    distx(img==0) = Inf;
    disty(img==0) = Inf;
    dist(:,:,1) = distx;
    dist(:,:,2) = disty;

    if (type == -1)
        change = 1;
        while (change)
            [dist, change] = propagate(dist, 1);
        end
        return;
    end
    
    if (type == 3)
        dist = propagate(dist, 1);
    end
    
    logstep = ceil(log2(max(size(img))))-1; % log2 of initial step size
    while (logstep >= 0)
        stepsize = 2 ^ logstep;
        dist = propagate(dist, stepsize);
        logstep = logstep -1;
    end
    
    if(type == 4)
        logstep = ceil(log2(max(size(img))))-1; % log2 of initial step size
        while (logstep >= 0)
            stepsize = 2 ^ logstep;
            dist = propagate(dist, stepsize);
            logstep = logstep -1;
        end
    end
    
    if (type == 1)
        dist = propagate(dist, 1);
    end

    if (type == 2)
        dist = propagate(dist, 2);
        dist = propagate(dist, 1);
    end
end

function [out, change] = propagate(in, step)
    [h,w,dummy] = size(in);
    out = in; % Copy to simulate parallel, order independent updates
    change = 0;
    for i = 1:h
        for j = 1:w
            % Propagate and update distance from 3x3 "neighborhood"
            dx = out(i,j,1);
            dy = out(i,j,2);
            d = dx*dx + dy*dy;
            if (i-step >= 1)
                if (j-step >= 1)
                    dx = in(i-step,j-step,1) - step;
                    dy = in(i-step,j-step,2) - step;
                    dnew = dx*dx + dy*dy;
                    if (dnew < d)
                        out(i,j,1) = dx;
                        out(i,j,2) = dy;
                        d = dnew;
                        change = 1;
                    end
                end
                dx = in(i-step,j,1) - step;
                dy = in(i-step,j,2);
                dnew = dx*dx + dy*dy;
                if (dnew < d)
                    out(i,j,1) = dx;
                    out(i,j,2) = dy;
                    d = dnew;
                    change = 1;
                end
                if (j+step <= w)
                    dx = in(i-step,j+step,1) - step;
                    dy = in(i-step,j+step,2) + step;
                    dnew = dx*dx + dy*dy;
                    if (dnew < d)
                        out(i,j,1) = dx;
                        out(i,j,2) = dy;
                        d = dnew;
                        change = 1;
                    end
                end
            end
            if (j-step >= 1)
                dx = in(i,j-step,1);
                dy = in(i,j-step,2) - step;
                dnew = dx*dx + dy*dy;
                if (dnew < d)
                    out(i,j,1) = dx;
                    out(i,j,2) = dy;
                    d = dnew;
                    change = 1;
                end
            end
            if (j+step <= w)
                dx = in(i,j+step,1);
                dy = in(i,j+step,2) + step;
                dnew = dx*dx + dy*dy;
                if (dnew < d)
                    out(i,j,1) = dx;
                    out(i,j,2) = dy;
                    d = dnew;
                    change = 1;
                end
            end
            if (i+step <= h)
                if (j-step >= 1)
                    dx = in(i+step,j-step,1) + step;
                    dy = in(i+step,j-step,2) - step;
                    dnew = dx*dx + dy*dy;
                    if (dnew < d)
                        out(i,j,1) = dx;
                        out(i,j,2) = dy;
                        d = dnew;
                        change = 1;
                    end
                end
                dx = in(i+step,j,1) + step;
                dy = in(i+step,j,2);
                dnew = dx*dx + dy*dy;
                if (dnew < d)
                    out(i,j,1) = dx;
                    out(i,j,2) = dy;
                    d = dnew;
                    change = 1;
                end
                if (j+step <= w)
                    dx = in(i+step,j+step,1) + step;
                    dy = in(i+step,j+step,2) + step;
                    dnew = dx*dx + dy*dy;
                    if (dnew < d)
                        out(i,j,1) = dx;
                        out(i,j,2) = dy;
                        d = dnew;
                        change = 1;
                    end
                end
            end
        end
    end
end
