export GST_PLUGIN_PATH=.

# io-mode=0 ! \
gst-launch-1.0 -v --gst-debug=3 \
	v4l2src io-mode=4 ! \
	'video/x-raw, width=1280, height=720, format=YUY2' ! \
	myfilter ! \
	videoconvert ! \
	autovideosink

