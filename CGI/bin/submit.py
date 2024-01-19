#!/usr/bin/env python3

import cgi
import os

# Access form data
form = cgi.FieldStorage()

# Print received form data
print("Content-type: text/plain\n")
print("Received form data:")
for field in form.keys():
    print(f"{field} = {form[field].value}")

#HTML template
html_template = """
<!DOCTYPE html>
<html>
<head>
	<title>Submit Result</title>
</head>
<body>
	<h1>Form Submission Result</h1>
	<p>First Name: {first_name}</p>
	<p>Login: {login}</p>
	<p>Email:{email}</p>
	<p>Date of Birth: {dob}</p>
	<p>Password: {password}</p>
</body>
</html>
"""

# Get form data
first_name = form.getvalue("first_name", "")
last_name = form.getvalue("last_name", "")
login = form.getvalue("login", "")
email = form.getvalue("email", "")
dob = form.getvalue("dob", "")
phone = form.getvalue("phone", "")
password = form.getvalue("password", "")

# Substitute form data into HTML template

response_html = html_template.format(
 first_name=first_name,
 last_name=last_name,
 login=login,
 email=email,
 dob=dob,
 phone=phone,
 password=password
)

print(response_html)
