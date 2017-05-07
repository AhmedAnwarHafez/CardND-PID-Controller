#include "PID.h"
#include <iostream>

using namespace std;

/*
 * TODO: Complete the PID class.
 */

PID::PID() {
}

PID::~PID() {
}

void PID::Init(const double Kp_,const double Ki_,const double Kd_) {
	Kp = Kp_;
	Ki = Ki_;
	Kd = Kd_;

	Best_Kd = Kd_;
	Best_Kp = Kp_;
	Best_Ki = Ki_;

	p_error = 0.0;
	d_error = 0.0;
	i_error = 0.0;

	Cycles = 0;
	top_speed = 0;
	Best_Error = 1e8;
	start = clock();

	is_twiddle_enabled = false;
}

void PID::Restart(uWS::WebSocket<uWS::SERVER> ws){
  std::string reset_msg = "42[\"reset\",{}]";
  ws.send(reset_msg.data(), reset_msg.length(), uWS::OpCode::TEXT);
}

void PID::UpdateError(const double cte) {
	// increase the number of cycles
	Cycles += 1;

	int stop = clock();
	dt = (stop - start) / double(CLOCKS_PER_SEC);
	start = stop;

	// the rate of change (first derivative)
	d_error = (cte - p_error) / dt;
	// propotion
	p_error = cte;
	// integral
	i_error += cte;
}

void PID::Twiddle(const double speed ,uWS::WebSocket<uWS::SERVER> ws) {

	if (is_twiddle_enabled == false) {
		return;
	}

	// when the simulator reached to a 750 cycles
	if(Cycles > 750){

		// set a uniform random generator between certain upper and lower bounds for each coeffient
		std::random_device rd;
		std::default_random_engine generator(rd());
		std::uniform_real_distribution<> rand_p(0.08, 0.1);
		std::uniform_real_distribution<> rand_d(0.002, 0.003);
		std::uniform_real_distribution<> rand_i(0.002, 0.003);

		// check if the error decreasing and speed is increasing
		if(TotalError() < Best_Error && speed > top_speed){

			// true means that the PID is doing well and the car is going fast and not wobbeling around

			// cache the error and speed
			Best_Error = TotalError();
			top_speed = speed;
			// update the coeffiecents, these will be displayed by the caller
			Best_Kp = Kp;
			Best_Kd = Kd;
			Best_Ki = Ki;
		}

		// draw new values from a uniform continous distribution
		Kp = rand_p(generator);
		Kd = rand_d(generator);
		Ki = rand_i(generator);

		// reset cycles and total error
		Cycles = 0;
		i_error = 0.0;

		// reset car's position
		Restart(ws);
	}
}

double PID::TotalError() {

	return -Kp * p_error - Ki * i_error - Kd * d_error;
}

