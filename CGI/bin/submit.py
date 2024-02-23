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
print("Content-type: text/plain\n")
print("Received form data from DATA_CGI:")
for key, value in form_data.items():
    print(f"{key} = {value[0]}")

#HTML template
html_template = """
<!DOCTYPE html>
<html>
<head>
    <title>Submit Result</title>
</head>
<body>
    <h1>Form Submission Result Diego</h1>
    <p>First Name: {first_name}</p>
    <p>Last Name: {last_name}</p>
    <p>Login: {login}</p>
    <p>Email: {email}</p>
    <p>Date of Birth: {dob}</p>
    <p>Password: {password}</p>
</body>
</html>
"""

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
print("Content-type: text/html\n")
print(response_html)
