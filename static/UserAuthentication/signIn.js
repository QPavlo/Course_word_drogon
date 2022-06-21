function checkEmail() {
    return (document.getElementById("uName").value.match(/^[\w-\.]+@([\w-]+\.)+[\w-]{2,4}$/) !== null || document.getElementById("uName").value === 'admin');
}


function checkPassword() {
    return (document.getElementById("pass").value !== "");
}

function checkInput() {
    const email = "uName";
    const pass = "pass";
    const invalidInputValueBorderColor = "Red";
    const validInputValueBorderColor = "2px solid rgba(0, 0, 0, 0.02)";

    const checkUName = checkEmail();
    const checkPass = checkPassword();

    if (checkUName === false) {
        document.getElementById(email).style.borderColor = invalidInputValueBorderColor;
        document.getElementById(email).value = "";
    } else {
        document.getElementById(email).style.border = validInputValueBorderColor;
    }

    if (checkPass === false) {
        document.getElementById(pass).style.borderColor = invalidInputValueBorderColor;
        document.getElementById(pass).value = "";
    } else {
        document.getElementById(pass).style.border = validInputValueBorderColor;
    }

    if (checkUName === true && checkPass === true) {
        return true;
    } else {
        return false;
    }
}