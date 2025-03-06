const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="ro">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>LED Control</title>
  <style>
    /* General body styling */
    body {
      font-family: Arial, sans-serif;
      background-color: #f4f4f9;
      display: flex;
      justify-content: center;
      align-items: center;
      height: 100vh;
      margin: 0;
    }

    /* Card container styling */
    .card {
      max-width: 400px;
      min-height: 300px;
      background: #02b875;
      color: white;
      padding: 30px;
      box-sizing: border-box;
      border-radius: 10px;
      box-shadow: 0px 4px 18px rgba(0, 0, 0, 0.2);
      text-align: center;
    }

    /* Card header styling */
    h2 {
      font-size: 24px;
      margin-bottom: 10px;
    }

    h3 {
      font-size: 20px;
      margin-bottom: 20px;
    }

    /* Button styling */
    button {
      background-color: #02b875;
      color: white;
      border: none;
      padding: 10px 20px;
      font-size: 16px;
      cursor: pointer;
      border-radius: 5px;
      margin: 10px 0;
      transition: background-color 0.3s ease;
    }

    button:hover {
      background-color: #028c5e;
    }

    /* Status indicator styling */
    span {
      font-weight: bold;
      color: #f7c200;
    }

    /* Styling for the buttons */
    .btn-container {
      display: flex;
      justify-content: space-around;
      margin-top: 20px;
    }
  </style>
</head>
<body>

<div class="card">
  <h2>LED Enable/Disable</h2>
  <h3>Status LED: <span id="ADCValue">0</span></h3>
  
  <div class="btn-container">
    <button onclick='sendCommand("led-on")'>LED ON</button>
    <button onclick='sendCommand("led-off")'>LED OFF</button>
  </div>
</div>

<script>
  getData();

  // Get the IP address using fetch
  async function getIPAddress() {
    try {
      const response = await fetch('/get-ip');
      if (!response.ok) throw new Error('Network response was not ok');
      const data = await response.text();
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

  // WebSocket connection
  async function connectWebSocket() {
    const adIP = await getIPAddress();
    if (!adIP) {
      console.error("Adresa IP nu a fost găsită");
      return;
    }
    const serverAddress = "ws://" + adIP + ":81/";

    let e = new WebSocket(serverAddress);

    e.onmessage = e => {};

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