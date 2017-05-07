#Explaining PID Controller


PID controller stands for proportional–integral–derivative.
The controller's formula is as follows:

output =  - Kp * CTE - Ki * Sum(CTE) - Kd * (CTE - Old-CTE) / dt
where Kp is the proportional coefficient
Ki is the integral coefficient
Kd is the derivative coefficient
dt is the difference in time between the current frame and the last one
CTE is the cross track error
Sum(CTE) is the total of all cross track errors over all frames

The first term in the formula reperesents the P controlelr and tries to compensate the the CTE proportionally, meaning how fast the car will turn.

High Kp leads to high oscillation. Low Kp leads to slow wide oscillations.

The second term is the Integral controller and this term compensates for any erros due to environment factors that affects the CTE such as miss alighed wheels or wind forces (like bad aerodynamics design).

The last term is the Derivative term and this term is important because it dampens the oscillation behavior otherwise the car will overshoot and goes into circles.

Kp is the proportional term. It will determine how fast your car will turn. Kd is used to dampen the behavior because it will otherwise overshoot and oscillate. Ki is used to fix a bias (e.g. cross-wind, bias in steering)

