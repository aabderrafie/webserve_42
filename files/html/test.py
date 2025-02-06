#!/usr/bin/python3

import cgi
import os
import html
from urllib.parse import parse_qs

# HTML header
print("Content-Type: text/html\n")

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

    # Handle file upload
    if "file" in form:
        file_item = form["file"]
        upload_dir = "./files/html/uploads/"
        upload_dir2 = "/uploads/"
        if file_item.filename:
            filename = os.path.basename(file_item.filename)
            upload_path = os.path.join(upload_dir, filename)
            upload_path2 = os.path.join(upload_dir2, filename)
            try:
                with open(upload_path, 'wb') as f:
                    f.write(file_item.file.read())
                html_content += f"<p class='success'>File uploaded successfully: <a href='{upload_path2}'>{filename}</a></p>"
            except Exception as e:
                html_content += f"<p class='error'>File upload failed: {e}</p>"
        else:
            html_content += "<p class='error'>No file uploaded!</p>"

html_content += """
    </div>
</body>
</html>
"""

# Output the complete HTML response
print(html_content)
