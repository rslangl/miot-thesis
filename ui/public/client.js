// client.js

const ws = new WebSocket("ws://" + window.location.host)

ws.addEventListener("open", function() 
{
    console.log("WebSocket opened")
})

ws.addEventListener("error", function(error) 
{
    console.log("WebSocket errored: ", error)
})

ws.addEventListener("message", function(message) 
{
    console.log("Received message on WebSocket: ", message)
})
