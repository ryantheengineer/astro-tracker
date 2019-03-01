close all
clear all

solar_time = 0:0.0005:1.5;
sidereal = solar_time*1.0027379;
theta = zeros(length(solar_time));
total_steps_vec = zeros(length(solar_time));
error = zeros(length(solar_time));
error_tot = zeros(length(solar_time));
true_theta = zeros(length(solar_time));

r = 10;   % inches, arbitrary

tpi = 12.17;   % threads per inch
spr = 200;  % steps per revolution      
% It looks like changing the steps per revolution is the best way to decrease instantaneous error

total_steps = 0;
for i = 1:length(solar_time)
    theta(i) = (sidereal(i)/10800.0)*(pi/4.0);
    psi = (pi - theta(i))/2;
    d = r*sin(theta(i))/sin(psi);
    steps = (d*tpi)*spr;
    
    if total_steps < steps
        total_steps = total_steps + 1;
    end
    
    total_steps_vec(i) = total_steps;
end

% plot(solar_time,total_steps_vec)

% Take total_steps_vec and use it to calculate true_theta
for i = 1:length(total_steps_vec)
    d = total_steps_vec(i)/(tpi*spr);
    % Solve for theta in terms of d and r:
    true_theta(i) = acos((-d^2 + 2*r^2)/(2*r^2));
end

error_tot_temp = 0;
% Calculate the error between true_theta and theta:
for i = 1:length(theta)
    error(i) = 60*60*(true_theta(i) - theta(i));    % error measured in arcseconds
    error_tot_temp = error_tot_temp + error(i);
    error_tot(i) = error_tot_temp;
end

% figure(1)
% plot(solar_time,true_theta);
% hold on
% plot(solar_time,theta);
% hold off
% 
figure(2)
plot(solar_time,error);
title('Instantaneous error');
xlabel('Solar time, sec');
ylabel('Arcseconds');

% figure(3)
% plot(solar_time,error_tot);