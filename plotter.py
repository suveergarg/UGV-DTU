import matplotlib.pyplot as plt
import numpy

def plot(data):
	plt.subplot(111, projection='polar')	
	plt.cla()
	#ax.set_theta_zero_location('N')
	plt.plot([0, numpy.deg2rad(int(data))], [0, 5])
	plt.show()
	plt.pause(0.00000000000000000000000000000000000000000001)

while True:
	try:
		d = raw_input()
	except EOFError:
		sleep(0.00005)
		continue

	plot(d)