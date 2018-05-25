#!/usr/bin/env python

import rospy
from std_msgs.msg import Float32
from geometry_msgs.msg import Twist
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import random
from matplotlib import style



style.use('fivethirtyeight')

fig = plt.figure()
ax1 = fig.add_subplot(1,1,1)
xs = []
ys = []
x_next = 0
next_y = None

def animate(i):
	global x_next
	global xs 
	global ys 
	xs.append(x_next)

	if next_y != None:
		ys.append(next_y)
	else:
		ys.append(0)
	x_next += 1
	ax1.clear()
	ax1.plot(xs, ys)

def plot_y_callback(msg):
	global next_y
	if msg.linear.x>= 0:
		next_y = msg.linear.x

rospy.init_node('matplotlib_gui')
rospy.Subscriber("/InfoArduino", Twist, plot_y_callback)
ani = animation.FuncAnimation(fig, animate, interval=1000)
plt.show()