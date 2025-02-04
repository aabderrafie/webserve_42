#!/usr/bin/env python3
import cgi
import cgitb
import json
from datetime import datetime

# Enable detailed error reporting
cgitb.enable()

# Print the HTTP header
print("Content-Type: text/html\n")

# HTML template for CGI response
print("""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>CGI Script Result</title>
    <style>
        body { 
            font-family: Arial, sans-serif; 
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
        }
        .container {
            background: rgba(255,255,255,0.2);
            padding: 30px;
            border-radius: 15px;
            text-align: center;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>CGI Script Executed Successfully!</h1>
        <p>Current Server Time: {}
        <p>Server Environment Details:</p>
        <pre>{}</pre>
    </div>
</body>
</html>
""".format(
    datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
    json.dumps(dict(os.environ), indent=2)
))