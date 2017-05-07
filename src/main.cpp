#include <uWS/uWS.h>
#include <iostream>
#include "json.hpp"
#include "PID.h"

// for convenience
using json = nlohmann::json;

// For converting back and forth between radians and degrees.
constexpr double pi() {
	return M_PI;
}
double deg2rad(double x) {
	return x * pi() / 180;
}
double rad2deg(double x) {
	return x * 180 / pi();
}

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
std::string hasData(std::string s) {
	auto found_null = s.find("null");
	auto b1 = s.find_first_of("[");
	auto b2 = s.find_last_of("]");
	if (found_null != std::string::npos) {
		return "";
	} else if (b1 != std::string::npos && b2 != std::string::npos) {
		return s.substr(b1, b2 - b1 + 1);
	}
	return "";
}

int main() {
	uWS::Hub h;
	// PID controller for controlling the steering
	PID pid_steer;
	// another PID controller for controller the throttle
	PID pid_throttle;

	// TODO: Initialize the pid variable.
	// coeffiencts for both controllers were found after optimization using twiddle
	//pid_steer.Init(0.09, 0.002,0.0021);
	pid_steer.Init(1, 0.002,0.0021);
	pid_throttle.Init(0.0, 0.0, 0.010);

	// initializing

	h.onMessage([&pid_steer, &pid_throttle](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode) {

				// "42" at the start of the message means there's a websocket message event.
				// The 4 signifies a websocket message
				// The 2 signifies a websocket event
				if (length && length > 2 && data[0] == '4' && data[1] == '2')
				{
					auto s = hasData(std::string(data));
					if (s != "") {
						auto j = json::parse(s);
						std::string event = j[0].get<std::string>();
						if (event == "telemetry") {
							// j[1] is the data JSON object
							double cte = std::stod(j[1]["cte"].get<std::string>());
							double speed = std::stod(j[1]["speed"].get<std::string>());
							double angle = std::stod(j[1]["steering_angle"].get<std::string>());
							double steer_value;
							/*
							 * TODO: Calcuate steering value here, remember the steering value is
							 * [-1, 1].
							 * NOTE: Feel free to play around with the throttle and speed. Maybe use
							 * another PID controller to control the speed!
							 */

							pid_steer.UpdateError(cte);
							//Twiddle
							pid_steer.Twiddle(speed, ws);
							steer_value = pid_steer.TotalError();

							// use the steering value as a cross track error for throttle controller
							// to mimic a real driver

							pid_throttle.UpdateError(steer_value);

							// the throttle slows down if the steering rate is high
							// and speeds up if the rate is low
							// added a constant value because if the rate is equal to zero then the car stops
							// so the constant term is to compensate for this
							double throttle = 1.0 + pid_throttle.TotalError();

							// cache the top speed if found
							if(speed > pid_throttle.top_speed) pid_throttle.top_speed = speed;

							// set the percision to 2 for cosmetic
							std::cout.precision(2);
							std::cout << "(" << pid_steer.Cycles
									<< ") top_speed=" << pid_throttle.top_speed
									<< " Best_Error=" << pid_steer.Best_Error
									<< " Best_Kp=" << pid_steer.Best_Kp
									<< " Best_Kd=" << pid_steer.Best_Kd
									<< " Best_Ki=" << pid_steer.Best_Ki
									<< "| throttle= " << throttle
									<< " Avg Error= " << pid_steer.TotalError()
									<< " CTE= " << cte
									<< " Kp=" << pid_steer.Kp
									<< " Kd=" << pid_steer.Kd
									<< " Ki=" << pid_steer.Ki
									<< std::endl;


							json msgJson;
							msgJson["steering_angle"] = steer_value;
							msgJson["throttle"] = throttle;
							auto msg = "42[\"steer\"," + msgJson.dump() + "]";
							//std::cout << msg << std::endl;
							ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
						}
					} else {
						// Manual driving
						//pid_steering.Restart(ws);
//						pid.is_twiddle_enabled = true;
//						std::cout << "Twiddle Enabled" << std::5endl;
//						pid.twiddle_position += 1;
//						if(pid.twiddle_position > 3){
//							pid.twiddle_position = 0;
//						}
//						std::cout << "pid.twiddle_position=" << pid.twiddle_position << std::endl;

//						std::random_device rd;
//						std::default_random_engine generator(rd());
//						std::uniform_real_distribution<> rand_p(0.1, 0.5); //0.079, 0.099
//						std::uniform_real_distribution<> rand_d(2, 5);
//
//						pid.Init(rand_p(generator), 0.0, rand_d(generator));
						std::string msg = "42[\"manual\",{}]";
						ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
					}
				}
			});

	// We don't need this since we're not using HTTP but if it's removed the program
	// doesn't compile :-(
	h.onHttpRequest([](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t, size_t) {
				const std::string s = "<h1>Hello world!</h1>";
				if (req.getUrl().valueLength == 1)
				{
					res->end(s.data(), s.length());
				}
				else
				{
					// i guess this should be done more gracefully?
					res->end(nullptr, 0);
				}
			});

	h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
				std::cout << "Connected!!!" << std::endl;
			});

	h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, char *message, size_t length) {
				ws.close();
				std::cout << "Disconnected" << std::endl;
			});

	int port = 4567;
	if (h.listen(port)) {
		std::cout << "Listening to port " << port << std::endl;
	} else {
		std::cerr << "Failed to listen to port" << std::endl;
		return -1;
	}
	h.run();
}
