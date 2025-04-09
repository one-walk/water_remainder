<?php
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $force = $_POST['force'] ?? '0';
    $time = date("Y-m-d H:i:s");
    file_put_contents("force_log.txt", "$time - Force: $force kg\n", FILE_APPEND);
    echo "OK";
} else {
    echo "Invalid request";
}
?>
