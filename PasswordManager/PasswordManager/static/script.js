function register() {
    fetch('/register', {
        method: 'POST',
        body: new URLSearchParams({
            username: document.getElementById("reg-username").value,
            password: document.getElementById("reg-password").value
        })
    }).then(res => res.text()).then(alert);
}

function login() {
    const username = document.getElementById("username").value;
    fetch('/login', {
        method: 'POST',
        body: new URLSearchParams({ username, password: document.getElementById("password").value })
    }).then(res => {
        if (res.ok) {
            sessionStorage.setItem("username", username);
            document.getElementById("auth").style.display = "none";
            document.getElementById("manager").style.display = "block";
            getPasswords();
        } else {
            alert("Неверные логин или пароль!");
        }
    });
}

function savePassword() {
    fetch('/save_password', {
        method: 'POST',
        body: new URLSearchParams({
            username: sessionStorage.getItem("username"),
            service: document.getElementById("service").value,
            password: document.getElementById("saved-password").value
        })
    }).then(res => res.text()).then(() => {
        alert("Сохранено!");
        getPasswords();
    });
}

function getPasswords() {
    fetch(`/get_passwords?username=${sessionStorage.getItem("username")}`)
        .then(res => res.text())
        .then(data => {
            document.getElementById("password-list").innerText = data.trim() ? data : "Нет сохраненных паролей";
        });
}

function logout() {
    sessionStorage.removeItem("username");
    document.getElementById("auth").style.display = "block";
    document.getElementById("manager").style.display = "none";
    document.getElementById("password-list").innerText = "";
}

function generatePassword() {
    const length = 16;
    const chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()-_=+";
    let password = "";
    for (let i = 0; i < length; i++) {
        password += chars.charAt(Math.floor(Math.random() * chars.length));
    }
    document.getElementById("generated-password").value = password;
}

function copyPassword() {
    const passwordField = document.getElementById("generated-password");
    passwordField.select();
    document.execCommand("copy");
    alert("Пароль скопирован");
}

function checkPasswordStrength(password) {
    const strengthBar = document.getElementById("password-strength-bar");
    const strengthText = document.getElementById("password-strength-text");

    let score = 0;
    if (password.length >= 8) score++;
    if (password.match(/[A-Z]/)) score++;
    if (password.match(/[a-z]/)) score++;
    if (password.match(/[0-9]/)) score++;
    if (password.match(/[\W_]/)) score++;

    switch (score) {
        case 0:
        case 1:
            strengthText.innerText = "Плохой пароль!";
            strengthBar.style.width = "20%";
            strengthBar.style.background = "red";
            break;
        case 2:
            strengthText.innerText = "Не очень надежный!";
            strengthBar.style.width = "40%";
            strengthBar.style.background = "orange";
            break;
        case 3:
            strengthText.innerText = "Средней надежности!";
            strengthBar.style.width = "60%";
            strengthBar.style.background = "yellow";
            break;
        case 4:
            strengthText.innerText = "Почти надежный!";
            strengthBar.style.width = "80%";
            strengthBar.style.background = "lightgreen";
            break;
        case 5:
            strengthText.innerText = "Отличный пароль";
            strengthBar.style.width = "100%";
            strengthBar.style.background = "green";
            break;
    }
}

// 🔹 Подключаем проверку пароля к полям ввода
document.getElementById("saved-password").addEventListener("input", function() {
    checkPasswordStrength(this.value);
});

document.getElementById("reg-password").addEventListener("input", function() {
    checkPasswordStrength(this.value);
});