#ifndef PID_H
#define PID_H
#include <uWS/uWS.h>
#include <ctime>

class PID {
public:
  /*
  * Errors
  */
  double p_error;
  double i_error;
  double d_error;

  /*
   * The error averaged for current session run
   */
  double Average_Error;

  /*
   * The lowest error for a session run
   * This variable is used by the twiddle function
   */
  double Best_Error;

  /*
   * The highest speed reached during a session run
   * This variable is used for twiddle
   */
  double top_speed;

  /*
   * A flag to check if twiddling is enabled
   * True to enable twiddling and find the best coeffients
   * False to run the PID controller
   */
  bool is_twiddle_enabled;

  /*
   * Clock start to compute delta t
   */
  int start;

  /*
   * dt = stop - start
   */
  double dt;

  /*
  * Coefficients
  */ 
  double Kp;
  double Ki;
  double Kd;
  double Best_Kp;
  double Best_Ki;
  double Best_Kd;

  /*
   * Number of cycles (or callbacks)
   */
  int Cycles;

  /*
  * Constructor
  */
  PID();

  /*
  * Destructor.
  */
  virtual ~PID();

  /*
  * Initialize PID.
  */
  void Init(double Kp, double Ki, double Kd);

  /*
   * Call this function to reset the cars position to the starting point
   */
  void Restart(uWS::WebSocket<uWS::SERVER> ws);
  /*
   * Optimize the Coefficients
   */
  void Twiddle(double speed, uWS::WebSocket<uWS::SERVER> ws);

  /*
  * Update the PID error variables given cross track error.
  */
  void UpdateError(double cte);

  /*
  * Calculate the total PID error
  */
  double TotalError();
};

#endif /* PID_H */
