import argparse
import base64
from datetime import datetime
import os
import shutil

import numpy as np
import socketio
import eventlet
import eventlet.wsgi
from flask import Flask
from io import BytesIO


sio = socketio.Server()
app = Flask(__name__)


@sio.on('telemetry')
def telemetry(sid, data):
    if data:
        # The current steering angle of the car
        steering_angle = 0.5
        # The current throttle of the car
        throttle = 0.5
        # The current speed of the car
        speed = data["speed"]
        #cte = data["cte"]
        # The current image from the center camera of the car
        #print("cte={0} steering_angle={1} throttle={2}".format(cte, steering_angle, throttle) )
        send_control(steering_angle, throttle)

    else:
        # NOTE: DON'T EDIT THIS.
        sio.emit('manual', data={}, skip_sid=True)


@sio.on('connect')
def connect(sid, environ):
    print("connect ", sid)
    send_control(0, 0)


def send_control(steering_angle, throttle):
    sio.emit(
        "steer",
        data={
            'steering_angle': steering_angle.__str__(),
            'throttle': throttle.__str__()
        },
        skip_sid=True)


if __name__ == '__main__':
  

    # wrap Flask application with engineio's middleware
    app = socketio.Middleware(sio, app)

    # deploy as an eventlet WSGI server
    eventlet.wsgi.server(eventlet.listen(('', 4567)), app)
