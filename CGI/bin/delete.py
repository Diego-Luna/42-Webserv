import os
import json
import sys
import urllib.parse

# Obtén la variable de entorno que contiene los datos
data_cgi = os.environ.get('DATA_CGI', '{}')

# Parsea los datos JSON
form_data = json.loads(data_cgi)

# Función para obtener el valor del campo
def get_form_value(field_name):
    return form_data.get(field_name, "")

# Usa la función para obtener el valor de "First_Name"
first_name = get_form_value("first_name")

#HTML template
html_template = """
<!DOCTYPE html>
<html>
<head>
    <title>Submit Result</title>
</head>
<body>
    <h1>Form Delete Result</h1>
    <p>First Name: {first_name}</p>
</body>
</html>
"""

# Get form data from DATA_CGI
# first_name = get_form_value("first_name")
todo = data_cgi = os.getenv("DATA_CGI", "")

# Substitute form data into HTML template
response_html = html_template.format(
 first_name=first_name,
)

# Ensure to switch to HTML content type before printing HTML
print("Content-type: text/html\n")
print(response_html)
