#!/usr/bin/env python3
import os
import cgi
import cgitb

# Habilita la depuración del script CGI
cgitb.enable()

def handle_get():
    print("Content-Type: text/html")
    print()
    print("<html><body>")
    print("<h1>GET Request Handled</h1>")
    print("</body></html>")

def handle_post():
    # Crea una instancia de FieldStorage para manejar los datos del formulario
    form = cgi.FieldStorage()

    print("Content-Type: text/html")
    print()
    print("<html><body>")
    print("<h1>POST Request Handled</h1>")

    # Itera sobre los campos del formulario e imprime cada uno
    for key in form.keys():
        print(f"<p>{key}: {form.getvalue(key)}</p>")

    print("</body></html>")

def handle_delete():
    print("Content-Type: text/html")
    print()
    print("<html><body>")
    print("<h1>DELETE Request Handled</h1>")
    print("</body></html>")

def main():
    method = os.environ.get("REQUEST_METHOD")

    if method == "GET":
        handle_get()
    elif method == "POST":
        handle_post()
    elif method == "DELETE":
        handle_delete()
    else:
        print("Status: 405 Method Not Allowed")
        print("Content-Type: text/html")
        print()
        print("<html><body>")
        print("<h1>405 Method Not Allowed</h1>")
        print("</body></html>")

if __name__ == "__main__":
    main()
