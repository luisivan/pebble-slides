var ip = document.getElementById("ip"),
	textarea = document.getElementById("text")

function load() {
	ip.value = localStorage.getItem("ip")
	textarea.value = localStorage.getItem("text")
}

function save() {
	localStorage.setItem("ip", ip.value)
	localStorage.setItem("text", textarea.value)

	var d = JSON.stringify({ip: ip.value, notes: text2arr(textarea.value)})
	var uri = "pebblejs://close#"+d
	document.location = uri
}

function text2arr() {
	var i = 2,
		text = textarea.value,
		arr = []

	text = text.replace(/(\r\n|\n|\r)/gm,"").replace('1. ', '')
	while(true) {
		text = text.split(i.toString()+'. ')
		arr.push(text[0])
		text = text[1]
		i++
		if (typeof text == 'undefined')
			break
	}
	return arr
}

load()