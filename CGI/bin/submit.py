#!/usr/bin/env python3

import os
import urllib.parse

# Assuming DATA_CGI contains URL-encoded form data
data_cgi = os.getenv("DATA_CGI", "")
# Parse the URL-encoded data into a dictionary
form_data = urllib.parse.parse_qs(data_cgi)

# Function to safely get a value from the form data
def get_form_value(field_name):
    return form_data.get(field_name, [""])[0]

# Print received form data
# print("Content-type: text/plain\n")
# print("Received form data from DATA_CGI:")
# for key, value in form_data.items():
#     print(f"{key} = {value[0]}")

#HTML template
html_template = """<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Submit Result</title>
    <link href="https://cdn.jsdelivr.net/npm/tailwindcss@2.2.19/dist/tailwind.min.css" rel="stylesheet">
	<link rel="stylesheet" href="style.css">

</head>
<body>
    <nav class="navbar">
        <a href="#" class="navbar-logo">42-Webserver</a>
        <div class="navbar-links">
            <a href="index.html" class="navbar-link">Home</a>
            <a href="form.html" class="navbar-link">CGI</a>
            <a href="team.html" class="navbar-link">Team</a>
            <a href="upload.html" class="navbar-link">Upload</a>
            <a href="chunktest.html" class="navbar-link">Chunked Test</a>
        </div>
    </nav>
    <div class="mx-auto text-center">
        <h1 class="mt-5">Form Submission Result</h1>
        <p class="mt-3"><strong>First Name:</strong> {first_name}</p>
        <p><strong>Last Name:</strong> {last_name}</p>
        <p><strong>Login:</strong> {login}</p>
        <p><strong>Email:</strong> {email}</p>
        <p><strong>Date of Birth:</strong> {dob}</p>
        <p><strong>Password:</strong> {password}</p>
    </div>
</body>
</html>"""

# Get form data from DATA_CGI
first_name = get_form_value("first_name")
last_name = get_form_value("last_name")
login = get_form_value("login")
email = get_form_value("email")
dob = get_form_value("dob")
password = get_form_value("password")

# Substitute form data into HTML template
response_html = html_template.format(
 first_name=first_name,
 last_name=last_name,
 login=login,
 email=email,
 dob=dob,
 password=password
)

# Ensure to switch to HTML content type before printing HTML
# print("Content-type: text/html\r\n")
print(response_html)
