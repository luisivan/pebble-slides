#!/usr/bin/env python2
from pykeyboard import PyKeyboard
import socket
from wsgiref.simple_server import make_server
from ws4py.websocket import WebSocket
from ws4py.server.wsgirefserver import WSGIServer, WebSocketWSGIRequestHandler
from ws4py.server.wsgiutils import WebSocketWSGIApplication

k = PyKeyboard()

class PebbleWebSocket(WebSocket):
	def received_message(self, message):

		if message.data == 'down':
			k.tap_key(k.right_key)
		elif message.data == 'up':
			k.tap_key(k.left_key)


server = make_server('', 0, server_class=WSGIServer,
					 handler_class=WebSocketWSGIRequestHandler,
					 app=WebSocketWSGIApplication(handler_cls=PebbleWebSocket))

ip = socket.gethostbyname(socket.gethostname())
print('Pebble Slides started. Your address is ' + ip + ':' + str(server.server_port))
server.initialize_websockets_manager()
server.serve_forever()