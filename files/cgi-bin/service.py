#!/usr/bin/env python3

import cgi

# print("Content-Type: text/html\n")

# HTML header
print("""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Python CGI Calculator</title>
    <style>
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            background: linear-gradient(135deg, #667eea, #764ba2, #667eea, #764ba2);
            background-size: 400% 400%;
            animation: gradientBackground 15s ease infinite;
            margin: 0;
        }
        @keyframes gradientBackground {
            0% { background-position: 0% 50%; }
            50% { background-position: 100% 50%; }
            100% { background-position: 0% 50%; }
        }
        .calculator {
            background: rgba(255, 255, 255, 0.1);
            backdrop-filter: blur(10px);
            padding: 30px;
            border-radius: 12px;
            box-shadow: 0 10px 30px rgba(0, 0, 0, 0.2);
            text-align: center;
            width: 320px;
        }
        h2 { color: white; margin-bottom: 20px; }
        input, select, button {
            width: 90%;
            padding: 10px;
            margin: 8px 0;
            border-radius: 5px;
            border: 1px solid #ccc;
            background: rgba(255, 255, 255, 0.2);
            color: white;
            font-size: 18px;
            outline: none;
            transition: 0.3s;
        }
        input:focus { border-color: #001aff57; box-shadow: 0 0 8px rgba(0, 26, 255, 0.7); }
        select { cursor: pointer; }
        button {
            background: #001aff57;
            border: none;
            cursor: pointer;
            transition: 0.3s;
        }
        button:hover { background: #0277bd; }
        .result {
            color: #ffffff;
            font-size: 20px;
            margin-top: 15px;
            padding: 10px;
            border-radius: 5px;
            background: rgba(0, 0, 0, 0.3);
        }
    </style>
</head>
<body>

    <div class="calculator">
        <h2>Python CGI Calculator</h2>
        <form method="post" >
            <input type="number" name="num1" placeholder="Enter first number" required step="any">
            <select name="operation">
                <option value="add">+</option>
                <option value="sub">-</option>
                <option value="mul">×</option>
                <option value="div">÷</option>
            </select>
            <input type="number" name="num2" placeholder="Enter second number" required step="any">
            <button type="submit">Calculate</button>
        </form>
""")

# Get form data
form = cgi.FieldStorage()
num1 = form.getvalue("num1")
num2 = form.getvalue("num2")
operation = form.getvalue("operation")

# Perform calculation if input is provided
if num1 and num2 and operation:
    try:
        num1, num2 = float(num1), float(num2)
        if operation == "add":
            result = num1 + num2
        elif operation == "sub":
            result = num1 - num2
        elif operation == "mul":
            result = num1 * num2
        elif operation == "div":
            result = "Error: Division by zero!" if num2 == 0 else num1 / num2
        else:
            result = "Invalid operation!"
    except ValueError:
        result = "Invalid input!"

    # Display result
    print(f'<div class="result">Result: {result}</div>')

# HTML footer
print("""
    </div>
</body>
</html>
""")
