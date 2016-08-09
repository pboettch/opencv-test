export GST_PLUGIN_PATH=.

	gst-launch-1.0 -vvv \
		v4l2src io-mode=4 ! \
		'video/x-raw,format=(string)YUY2,width=640,height=480' ! \
		videoconvert ! \
		myfilter ! \
		videoconvert ! \
		autovideosink
