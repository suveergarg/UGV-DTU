import numpy as np
import rospy
from std_msgs.msg import Int8MultiArray
from time import sleep

import navigate

ANGLE_THRESHOLD = np.deg2rad(5) #degrees
DELTA_THRESHOLD = 50 # for lane delta, in pixels
DELTA_THRESHOLD_DEVIATION = 10

#rospy.init_node('Lane_Navigator', anonymous=True)
nav = navigate.Navigator()
def on_lanes(raw_data):
	print 'On Lane'
	angle = raw_data.data[0]
	delta = raw_data.data[1]

	if delta < DELTA_THRESHOLD - DELTA_THRESHOLD_DEVIATION or delta > DELTA_THRESHOLD + DELTA_THRESHOLD_DEVIATION:
		# Too close, move right OR Too far, nove left
		nav.turn(angle-np.pi)
subscriber = rospy.Subscriber('lanes', Int8MultiArray, on_lanes)
		
while True:
	sleep(1)
