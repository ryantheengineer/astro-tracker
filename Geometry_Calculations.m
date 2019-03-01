t_sidereal = 86164.0916;    % Length of sidereal day, s
w = (2*pi)/t_sidereal;     % Angular speed to match star rotation

r = 12;     % radius, in
f = 1.25;   % offset, in

% syms t w r f
% 
% d = (sin(w*t)*(r-f*(tan(w*t/2))))/sin((pi-w*t)/2);
% dd_dt = diff(d,t)

t = linspace(0,(60*60*2));
dd_dt = (w*cos(t*w).*(r - f*tan((t*w)/2)))./sin(pi/2 - (t*w)/2) - (f*w*sin(t*w).*(tan((t*w)/2).^2 + 1))/(2*sin(pi/2 - (t*w)/2)) + (w*cos(pi/2 - (t*w)/2).*sin(t*w).*(r - f*tan((t*w)/2)))./(2*sin(pi/2 - (t*w)/2).^2);
cdistance = cumtrapz(dd_dt);
% plot(t,dd_dt)
plot(t,cdistance)

