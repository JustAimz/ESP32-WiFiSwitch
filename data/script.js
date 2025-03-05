const serverAddress = "ws://10.21.1.163:81/";
function connectWebSocket() {
    let e = new WebSocket("ws://10.21.1.163:81/");
    return e.onopen = e=>{
        console.log("WebSocket connection opened:", e)
    }
    ,
    e.onmessage = e=>{
        console.log("WebSocket message received:", e.data)
    }
    ,
    e.onerror = e=>{
        console.error("WebSocket error:", e)
    }
    ,
    e.onclose = e=>{
        e.wasClean ? console.log(`WebSocket connection closed cleanly, code=${e.code}, reason=${e.reason}`) : console.error("WebSocket connection abruptly closed")
    }
    ,
    e
}
function sendCommand(e) {
    let n = connectWebSocket()
      , o = e;
    n.onopen = e=>{
        console.log("WebSocket connection opened:", e),
        n.send(o)
    }
}
document.addEventListener("DOMContentLoaded", ()=>{
    let e = document.getElementById("runCommandButton");
    e && e.addEventListener("click", sendCommand)
}
);