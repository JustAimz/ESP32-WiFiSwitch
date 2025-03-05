const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<style>
.card{
    max-width: 400px;
     min-height: 250px;
     background: #02b875;
     padding: 30px;
     box-sizing: border-box;
     color: #FFF;
     margin:20px;
     box-shadow: 0px 2px 18px -4px rgba(0,0,0,0.75);
}
</style>
<body>

<div class="card">
  <h2>LED Enable/Disable</h4><br>
  <h3>Status LED: <span id="ADCValue">0</span></h1><br>
  <button onclick='sendCommand("led-on")'>Led ON</button>
  <button onclick='sendCommand("led-off")'>Led OFF</button>
</div>
<script>
getData();
// Obținem adresa IP folosind fetch
async function getIPAddress() {
  try {
    const response = await fetch('/get-ip');
    if (!response.ok) throw new Error('Network response was not ok');
    const data = await response.text(); // presupunem că serverul returnează IP-ul ca text
    return data;
  } catch (error) {
    console.error('Error fetching IP:', error);
    return null;
  }
}

async function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("ADCValue").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "readADC", true);
  xhttp.send();
}

// Funcție pentru WebSocket, va folosi IP-ul obținut din fetch
async function connectWebSocket() {
  const adIP = await getIPAddress();
  if (!adIP) {
    console.error("Adresa IP nu a fost găsită");
    return;
  }
  const serverAddress = "ws://" + adIP + ":81/";
  
  let e = new WebSocket(serverAddress);
  
  e.onmessage = e => {
  };
  
  e.onerror = e => {
    console.error("WebSocket error:", e);
  };
  
  e.onclose = e => {
    e.wasClean ? console.log(`WebSocket connection closed cleanly, code=${e.code}, reason=${e.reason}`) : console.error("WebSocket connection abruptly closed");
  };
  
  return e;
}

let clicked = 0;

async function sendCommand(e) {
  const n = await connectWebSocket();
  if (!n) return;
  
  const o = e;
  n.onopen = () => {
    n.send(o);
  };
  
  if (clicked < 1) {
    clicked += 1;
  }
}

function wsRefresh() {
  if (clicked == 1) {
    clicked = 0;
    location.reload();
  }
}

setInterval(function() {
  wsRefresh();
}, 500);

document.addEventListener("DOMContentLoaded", () => {
  let e = document.getElementById("runCommandButton");
  e && e.addEventListener("click", sendCommand);
});

function confirmClearEEPROM() {
  if (confirm("Are you sure you want to erase saved wifi data? After erasing the device will restart in 5 seconds. If it is not restarting, please do manually!")) {
    sendCommand("clearEEPROM");
  }
}
</script>
</body>
</html>
)=====";