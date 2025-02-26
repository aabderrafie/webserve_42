#!/usr/bin/python3

import cgi
import os
import html
from urllib.parse import parse_qs

# HTML header
# print("Content-Type: text/html\n")

# HTML structure and styling
html_content = """
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Python CGI Response</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f4;
            margin: 20px;
            padding: 20px;
            text-align: center;
        }
        .container {
            max-width: 600px;
            margin: auto;
            background: white;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
        }
        h1 {
            color: #333;
        }
        p {
            font-size: 18px;
            color: #555;
        }
        a {
            color: #007BFF;
            text-decoration: none;
        }
        a:hover {
            text-decoration: underline;
        }
        .success {
            color: green;
            font-weight: bold;
        }
        .error {
            color: red;
            font-weight: bold;
        }
    </style>
</head>
<body>
    <div class="container">
"""

# Process the request method
form = cgi.FieldStorage()

if os.environ['REQUEST_METHOD'] == 'GET':
    # Handle GET request
    name = form.getvalue('name', 'Unknown')
    age = form.getvalue('age', 'Unknown')

    html_content += f"<h1>Received GET request</h1>"
    html_content += f"<p>Name: <strong>{html.escape(name)}</strong></p>"
    html_content += f"<p>Age: <strong>{html.escape(age)}</strong></p>"

elif os.environ['REQUEST_METHOD'] == 'POST':
    # Handle POST request
    name = form.getvalue('name', 'Unknown')
    age = form.getvalue('age', 'Unknown')

    html_content += f"<h1>Received POST request</h1>"
    html_content += f"<p>Name: <strong>{html.escape(name)}</strong></p>"
    html_content += f"<p>Age: <strong>{html.escape(age)}</strong></p>"

html_content += """
    </div>
</body>
</html>
"""

# Output the complete HTML response
print(html_content)
