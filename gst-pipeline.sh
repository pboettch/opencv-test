export GST_PLUGIN_PATH=.

gst-launch-1.0 -vvv \
	v4l2src io-mode=4 ! \
	'video/x-raw, width=640, height=360, format=YUY2' ! \
	myfilter ! \
	xvimagesink sync=false -vvv

#		videoconvert ! \
#		videoconvert ! \
