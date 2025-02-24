<?php
if ($_SERVER['REQUEST_METHOD'] === 'DELETE') {
    // Get file name from the URL
    if (!isset($_GET['file']) || empty($_GET['file']) || !is_string($_GET['file'])) {
        http_response_code(400);
        echo json_encode(["error" => "No file specified"]);
        exit;
    }

    $fileName = basename($_GET['file']); // Sanitize filename
    $filePath = "../uploads/" . $fileName;

    if (!file_exists($filePath)) {
        http_response_code(404);
        echo json_encode(["error" => "File not found"]);
        exit;
    }

    if (unlink($filePath)) {
        http_response_code(200);
        echo json_encode(["message" => "File deleted successfully"]);
    } else {
        http_response_code(500);
        echo json_encode(["error" => "Failed to delete file"]);
    }
} else {
    http_response_code(405); // Method Not Allowed
    echo json_encode(["error" => "Invalid request method"]);
}
?>
