<?php
// Получаем и фильтруем данные из формы
$login = filter_var(trim($_POST['login']), FILTER_SANITIZE_STRING);
$pass = filter_var(trim($_POST['pass']), FILTER_SANITIZE_STRING);

// Проверяем корректность введенного логина
if (!filter_var($login, FILTER_VALIDATE_EMAIL)) {
    header('Location: index.html?error=invalid_email');
    exit();
}

// Устанавливаем соединение с базой данных
$mysql = new mysqli('localhost', 'root', 'root', 'register_bd');

// Проверка соединения
if ($mysql->connect_error) {
    header('Location: index.html?error=db_connection');
    exit();
}

// Подготавливаем и выполняем SQL-запрос с использованием подготовленного выражения
$stmt = $mysql->prepare("SELECT * FROM users WHERE login = ?");
$stmt->bind_param("s", $login);
$stmt->execute();

// Получаем результат запроса
$result = $stmt->get_result();
$user = $result->fetch_assoc();

// Проверяем, найден ли пользователь и соответствует ли пароль
if ($user && password_verify($pass, $user['pass'])) {
    // Успешная аутентификация
    session_start();
    $_SESSION['user'] = $user;
    header('Location: dashboard.php');
    exit();
} else {
    header('Location: index.html?error=invalid_login');
    exit();
}

// Закрываем подготовленное выражение и соединение с базой данных
$stmt->close();
$mysql->close();
?>