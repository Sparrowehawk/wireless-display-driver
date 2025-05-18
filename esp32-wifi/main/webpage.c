const char index_html[] =
    "<!DOCTYPE html>"
    "<html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>"
    "<title>ESP32 Display Control</title>"
    "<style>"
    "body{font-family:sans-serif;padding:2em;}"
    "label{display:block;margin:1em 0 .5em;}"
    "</style>"
    "</head><body>"
    "<h2>Display Settings</h2>"
    "<form action='/setdisplay' method='get'>"

    "<label for='color'>Choose a color:</label>"
    "<select name='color' id='color'>"
    "<option value='BLACK'>Black</option>"
    "<option value='WHITE'>White</option>"
    "<option value='RED'>Red</option>"
    "<option value='GREEN'>Green</option>"
    "<option value='BLUE'>Blue</option>"
    "<option value='CYAN'>Cyan</option>"
    "<option value='YELLOW'>Yellow</option>"
    "<option value='PURPLE'>Purple</option>"
    "<option value='PINK'>Pink</option>"
    "</select>"

    "<label for='mode'>Select mode:</label>"
    "<select name='mode' id='mode' onchange='toggleInputFields()'>"
    "<option value='1'>Fill Screen</option>"
    "<option value='2'>Text</option>"
    "</select>"

    "<div id='textInputs' style='display:none;'>"
    "<label for='x'>X (0–240):</label>"
    "<input type='number' id='x' name='x' min='0' max='240'>"

    "<label for='y'>Y (0–320):</label>"
    "<input type='number' id='y' name='y' min='0' max='320'>"

    "<label for='message'>Message:</label>"
    "<input type='text' id='message' name='message' maxlength='50'>"
    "</div>"

    "<br><input type='submit' value='Submit'>"
    "</form>"

    "<script>"
    "function toggleInputFields() {"
    "  const mode = document.getElementById('mode').value;"
    "  const textInputs = document.getElementById('textInputs');"
    "  textInputs.style.display = (mode === '2') ? 'block' : 'none';"
    "}"
    // Call once on page load to set correct visibility
    "window.onload = toggleInputFields;"
    "</script>"

    "</body></html>";