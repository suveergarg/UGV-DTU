import rospy
from std_msgs.msg import Int16MultiArray, UInt8MultiArray, Float32MultiArray
from time import sleep


import math
import numpy as np

FORWARD = 1
BACKWARD = 0
SLOW = 50
FAST = 200

PUBLISHER_TOPIC = 'MCU_Input'
SUBSCRIBER_TOPIC = 'MCU_Output'

ANGLE_ERROR = math.pi/180

ANGLE_THRESHOLD = np.deg2rad(5) #degrees
DELTA_THRESHOLD = 250 # for lane delta, in pixels
DELTA_THRESHOLD_DEVIATION = 100

def angle_min_difference(target_angle, source_angle):
	a = (target_angle - source_angle)*180/math.pi
	a = int(math.floor(a))
	a = (a + 180) % 360 - 180
	return a*math.pi/180

def normalize_angle(angle):
	normal_angle = angle
	while normal_angle > 2*math.pi:
		normal_angle -= 2*math.pi

	while normal_angle < -2*math.pi:
		normal_angle += 2*math.pi

	return normal_angle

class Navigator:
	
	def __init__(self):
		rospy.init_node('Navigator', anonymous=True)
		self.publisher = rospy.Publisher(PUBLISHER_TOPIC, UInt8MultiArray, queue_size=5)
		self.subscriber = rospy.Subscriber(SUBSCRIBER_TOPIC, Int16MultiArray, self.update_angle)
		self.lane_subscriber = rospy.Subscriber('lanes', Float32MultiArray, self.on_lanes)
		self.angle = 0
		self.cur_speed = 0
		self.turning = False
		self.r = rospy.Rate(10)
		print 'Initializing...'
		sleep(5)          # Time to properly initialize stuff
		print 'Ready to go'
		#rospy.spin()

	def on_lanes(self, raw_data):
		if self.turning:
			print 'Currently turning...'

		angle = raw_data.data[0]
		delta = raw_data.data[1]

		if delta < DELTA_THRESHOLD - DELTA_THRESHOLD_DEVIATION or delta > DELTA_THRESHOLD + DELTA_THRESHOLD_DEVIATION:
			# Too close, move right OR Too far, nove left
			self.lane_turn(angle-np.pi)

	def update_angle(self, new_angle):
		self.angle = int(new_angle.data[1])*math.pi/180
		if self.angle < 0:
			self.angle = 2*math.pi+self.angle
		#print 'New angle=', self.angle

	def motor_cmd(self, left_dir, left_speed, right_dir, right_speed):
		cmd = UInt8MultiArray(data=[left_dir, left_speed, right_dir, right_speed])
		self.publisher.publish(cmd)
		print 'Published %r' % cmd.data
		self.r.sleep()

	def turn_left_cmd(self, speed):
		self.motor_cmd(FORWARD, 0, FORWARD, speed)

	def turn_right_cmd(self, speed):
		self.motor_cmd(FORWARD, speed, FORWARD, 0)

	def turn_left(self, turn_angle):
		target_angle = normalize_angle(self.angle+turn_angle)
		self.motor_cmd(FORWARD, self.cur_speed, FORWARD, self.cur_speed+SLOW)
		difference = abs(angle_min_difference(target_angle, self.angle))
		while difference > ANGLE_ERROR:
			#print self.angle, difference
			difference = abs(angle_min_difference(target_angle, self.angle))
		print difference

	def turn_right(self, turn_angle):
		target_angle = normalize_angle(self.angle+turn_angle)
		self.motor_cmd(FORWARD, self.cur_speed+SLOW, FORWARD, self.cur_speed)
		difference = abs(angle_min_difference(target_angle, self.angle))
		while difference > ANGLE_ERROR:
			#print self.angle, target_angle
			difference = abs(angle_min_difference(target_angle, self.angle))
		print difference

	def move_forward(self, speed):
		self.motor_cmd(FORWARD, speed, FORWARD, speed)
		self.cur_speed = speed

	def turn(self, angle):
		"""Negative angle = left turn
		   and positive angle = right turn"""
		self.turning = True
		if angle > 0:
			self.turn_right(angle)
		else:
			self.turn_left(angle)
		self.turning = False
		#self.move_forward(0)
		self.move_forward(self.cur_speed)

if __name__ == '__main__':
	nav_obj = Navigator()
	nav_obj.move_forward(SLOW)
	while True:
		pass