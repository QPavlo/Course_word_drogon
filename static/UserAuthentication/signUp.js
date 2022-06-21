// Firstname requirements
//
// Surname consists of alphanumeric characters (a-z|A-Z|)
//     Upper case must be first character
//     Upper case allow after lower case (example: McLaren)
function checkName() {
    return (document.getElementById("fName").value.match(/([A-Z][a-zA-Z]{1,30})/) !== null)
}

// Surname requirements
//
// Surname consists of alphanumeric characters (a-z|A-Z|)
//     Upper case must be first character
//     Upper case allow after lower case (example: McLaren)
function checkSurName() {
    return (document.getElementById("sName").value.match(/([A-Z][a-zA-Z]{1,30})/) !== null)
}


// Username requirements
//
// Username consists of alphanumeric characters (a-z|A-Z|0-9)
//     Username allowed of the dot (.), underscore (_), and hyphen (-).
//     The dot (.), underscore (_), or hyphen (-) must not be the first or last character.
//     The dot (.), underscore (_), or hyphen (-) does not appear consecutively, e.g.
// The number of characters must be between 5 and 20.
function checkEmail() {
    return (document.getElementById("uName").value.match(/^[\w-\.]+@([\w-]+\.)+[\w-]{2,4}$/) !== null)
}


// Password requirements
//
// Username consists of alphanumeric characters (a-z|A-Z|0-9)
//     Username allowed of the dot (.), underscore (_), and hyphen (-).
//     At least one number [0-9]
//     Must lower and upper case
// The number of characters must be between 8 and 32.
function checkPassword() {
    return (document.getElementById("pass").value.match(/^(?=.*\d)(?=.*[a-z])(?=.*[A-Z])(?=.*[a-zA-Z]).{8,32}$/) !== null)
}

function checkInput() {

    const invalidInputValueBorderColor = "Red";
    const validInputValueBorderColor = "2px solid rgba(0, 0, 0, 0.02)";


    const checkFName = checkName();
    const checkSName = checkSurName();
    const checkEm = checkEmail();
    const checkPass = checkPassword();

    if (checkFName === false) {
        document.getElementById('fName').style.borderColor = invalidInputValueBorderColor;
        document.getElementById('fName').value = "";
    } else {
        document.getElementById('fName').style.border = validInputValueBorderColor;
    }

    if (checkSName === false) {
        document.getElementById('sName').style.borderColor = invalidInputValueBorderColor;
        document.getElementById('sName').value = "";
    } else {
        document.getElementById('sName').style.border = validInputValueBorderColor;
    }

    if (checkEm === false) {
        document.getElementById('uName').style.borderColor = invalidInputValueBorderColor;
        document.getElementById('uName').value = "";
    } else {
        document.getElementById('uName').style.border = validInputValueBorderColor;
    }

    if (checkPass === false) {
        document.getElementById('pass').style.borderColor = invalidInputValueBorderColor;
        document.getElementById('pass').value = "";
        document.getElementById('tooWeak').style = "display:block;";
    } else {
        document.getElementById('pass').style.border = validInputValueBorderColor;
        document.getElementById('tooWeak').style = "display: none;";

    }

    if (checkFName === true && checkSName === true && checkEm === true && checkPass === true) {
        return true;
    } else {
        return false;
    }

}