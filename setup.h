const char SETUP_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>WiFi Setup</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f4;
            text-align: center;
            padding: 20px;
        }
        h1 {
            color: #333;
        }
        form {
            background-color: #fff;
            padding: 20px;
            border-radius: 8px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
            display: inline-block;
        }
        select, input[type="password"], input[type="text"] {
            width: 250px;
            padding: 8px;
            margin: 10px 0;
            border-radius: 4px;
            border: 1px solid #ccc;
        }
        button {
            padding: 10px 20px;
            border-radius: 5px;
            background-color: #4CAF50;
            color: white;
            border: none;
            cursor: pointer;
        }
        button:hover {
            background-color: #45a049;
        }
        .message {
            font-size: 14px;
            color: #777;
        }
    </style>
    <script>
        let selectedSSID = "";

        // Function to load networks and keep the selection intact
        function loadNetworks() {
            fetch('/scan')
            .then(response => response.json())
            .then(data => {
                let ssidSelect = document.getElementById("ssid");
                let currentSelection = ssidSelect.value;
                
                // Clear the existing options
                ssidSelect.innerHTML = "";

                // Add the new SSID options
                data.forEach(ssid => {
                    let option = document.createElement("option");
                    option.value = ssid;
                    option.textContent = ssid;

                    // Keep the previous selection if it exists in the list
                    if (ssid === currentSelection) {
                        option.selected = true;
                    }
                    ssidSelect.appendChild(option);
                });
            })
            .catch(error => console.error("Error loading networks:", error));
        }

        // Reload the list every 10 seconds
        setInterval(() => {
            loadNetworks();
        }, 10000);

        // Load networks initially when the page loads
        window.onload = loadNetworks;
    </script>
</head>
<body>
    <h1>WiFi Setup</h1>
    <form action="/" method="post">
        <label for="ssid">WiFi:</label>
        <select id="ssid" name="ssid"></select><br>
        
        <label for="password">Parola:</label>
        <input type="password" id="password" name="password"><br>
        
        <label for="dns">Nume:</label>
        <input type="text" id="dns" name="DNS"><br>
        
        <button type="submit">Salveaza</button>
    </form>
    <p class="message">Lista de retele WiFi se actualizeaza odata la 10 secunde.</p>
    <p class="message">Daca nu vezi WiFi-ul tau mai asteapta putin.</p>
    <br></br>
    <p class="message">Numele dispozitivului va fi numele cu care il vei accesa</p>
    <p class="message">Adica de ex: Numele "smartlink" va deveni "http://smartlink.local"</p>
</body>
</html>
)=====";

const char SETUP_success[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>WiFi Setup</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f4;
            text-align: center;
            padding: 20px;
        }
        h1 {
            color: #333;
        }
        p {
            font-size: 18px;
            color: #555;
        }
    </style>
    <script>
        setTimeout(function() {
            window.location.href = "/";
        }, 5000);
    </script>
</head>
<body>
    <h1>WiFi Setup</h1>
    <p>Setarile au fost salvate! Restart in 5 secunde...</p>
</body>
</html>
)=====";