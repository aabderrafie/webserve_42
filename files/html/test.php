<?php
// PHP info page with WebServer 1337 styling
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>PHP Test Page - WebServer 1337</title>
    <style>
        body { 
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            display: flex;
            justify-content: center;
            align-items: center;
            min-height: 100vh;
            margin: 0;
            padding: 20px;
        }
        .container {
            background: rgba(255,255,255,0.2);
            backdrop-filter: blur(10px);
            border-radius: 15px;
            padding: 40px;
            width: 80%;
            max-width: 800px;
            text-align: center;
        }
        h1 {
            margin-bottom: 20px;
            text-shadow: 2px 2px 4px rgba(0,0,0,0.3);
        }
        .php-info {
            text-align: left;
            background: rgba(0,0,0,0.1);
            padding: 20px;
            border-radius: 10px;
            margin-top: 20px;
            max-height: 400px;
            overflow-y: auto;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>WebServer 1337 PHP Test Page</h1>
        
        <div class="php-info">
            <h2>Server Information</h2>
            <p><strong>PHP Version:</strong> <?php echo phpversion(); ?></p>
            <p><strong>Current Timestamp:</strong> <?php echo date('Y-m-d H:i:s'); ?></p>
            
            <h2>Environment Variables</h2>
            <pre><?php 
                print_r($_SERVER);
            ?></pre>
        </div>
    </div>
</body>
</html>