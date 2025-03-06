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
        input[type="text"], input[type="password"] {
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
    </style>
</head>
<body>
    <h1>WiFi Setup</h1>
    <form action="/" method="post">
        <label for="ssid">WiFi Name:</label>
        <input type="text" id="ssid" name="ssid"><br>
        
        <label for="password">Password:</label>
        <input type="password" id="password" name="password"><br>
        
        <label for="dns">DNS:</label>
        <input type="text" id="dns" name="DNS"><br>
        
        <button type="submit">Save</button>
    </form>
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
    <p>Settings saved! Restarting device in 5 seconds.</p>
</body>
</html>
)=====";