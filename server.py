#!/usr/bin/env python

import sys
import socket
from wsgiref.simple_server import make_server
from ws4py.websocket import WebSocket
from ws4py.server.wsgirefserver import WSGIServer, WebSocketWSGIRequestHandler
from ws4py.server.wsgiutils import WebSocketWSGIApplication

# return true if Mac
def isMac():
	return sys.platform == 'darwin'

# PyKeyboard does not work on Mac
if isMac():
	import os
else:
	from pykeyboard import PyKeyboard

class PebbleWebSocket(WebSocket):

	def received_message(self, message):

		if message.data == 'down':
			if isMac():
				cmd = "osascript -e 'tell application \"System Events\" to keystroke (ASCII character 29)'"
				os.system(cmd)
			else:
				PyKeyboard().tap_key(k.right_key)
		
		elif message.data == 'up':
			if isMac():
				cmd = "osascript -e 'tell application \"System Events\" to keystroke (ASCII character 28)'"
				os.system(cmd)
			else:
				PyKeyboard().tap_key(k.left_key)

# either take a port from arguments or serve on random port
port = 0
if (len(sys.argv) == 2) and int(sys.argv[1] >= 1024) :
	port = int(sys.argv[1])
elif len(sys.argv) != 1:
	print("usage: %s [port]" % sys.argv[0])
	sys.exit()

server = make_server('', port, server_class=WSGIServer,
					 handler_class=WebSocketWSGIRequestHandler,
					 app=WebSocketWSGIApplication(handler_cls=PebbleWebSocket))

ip = socket.gethostbyname(socket.gethostname())
print('Pebble Slides started. Your address is ' + ip + ':' + str(server.server_port))
server.initialize_websockets_manager()
server.serve_forever()
