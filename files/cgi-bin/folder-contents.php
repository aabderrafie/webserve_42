<?php
$directory = "../uploads"; // Adjust if needed

if (!is_dir($directory)) {
    http_response_code(500);
    echo json_encode(["error" => "Directory not found"]);
    exit;
}

$files = array_diff(scandir($directory), array('.', '..'));

header('Content-Type: application/json');
echo json_encode(array_values($files));
?>
