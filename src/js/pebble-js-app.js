var Utils = {
	http: function(ip, button, cb, errcb) {
		var req = new XMLHttpRequest()
	  	req.open('GET', 'http://'+ip+':8686/'+button, true)
	  	req.send()
	}
}

var IP = localStorage.getItem('ip') || '192.168.1.129'

Pebble.addEventListener("appmessage", function(e) {
	var button = Object.keys(e.payload)[0]
	Utils.http(IP, button)
})