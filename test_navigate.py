from time import sleep
import navigate
import math

INTERVAL = 2#seconds

# Helper functions

nav = navigate.Navigator()
def forward_backward(speed_f, speed_b):
	nav.move_forward(speed_f)
	sleep(INTERVAL)
	nav.move_backward(speed_b)
	sleep(INTERVAL)

def turn_while_moving_forward(speed, angle):
	nav.move_forward(speed)
	sleep(INTERVAL)
	nav.turn(angle)


# The tests
def forward_backward_slow():
	forward_backward(navigate.SLOW, navigate.SLOW)

def forward_backward_fast():
	forward_backward(navigate.FAST, navigate.FAST)

def forward_backward_slow_fast():
	forward_backward(navigate.SLOW, navigate.FAST)

def forward_backward_fast_slow():
	forward_backward(navigate.FAST, navigate.SLOW)

def turn_while_moving_fast():
	turn_while_moving_forward(navigate.FAST, 0)

def turn_while_moving_slow():
	turn_while_moving_forward(navigate.SLOW, 0)


nav.move_forward(navigate.SLOW)
#nav.turn(math.pi/6)
#nav.move_backward(navigate.SLOW)
#sleep(2)
#nav.move_forward(0)