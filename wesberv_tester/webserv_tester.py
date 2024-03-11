
import requests
import pytest

DEFAULT_PORT = 8080

def test_status_code():
    print("\nTESTING CORRECT PORT")
    url = "http://localhost:{}".format(DEFAULT_PORT)
    expected_result = "Expecting status code 200"
    
    response = requests.get(url)
    print(f"\nINPUT: {url}\n{expected_result}")
    assert response.status_code == 200, "Unexpected status code"
    print(f"Obtained Result: Status Code {response.status_code}")

def test_wrong_port():
    print("\nTESTING WRONG PORT")
    url = "http://localhost:9090"
    expected_result = "Expecting a ConnectionError due to the wrong port"
    timeout_seconds = 5
    
    try:
        response = requests.get(url, timeout=timeout_seconds)
        obtained_result = f"Obtained Result: Status Code {response.status_code}"
    except requests.exceptions.ConnectionError as e:
        obtained_result = "Obtained Result: ConnectionError (as expected)"

    print(f"\nINPUT: {url}\n{expected_result}\n{obtained_result}\n")

def test_multiple_correct_ports():
    print("\nTESTING MULTIPLE CORRECT PORTS")
    print("remeber to adjust according to config file")
    ports = [8080, 3000]
    timeout_seconds = 5
    
    for port in ports:
        url = f"http://localhost:{port}"
        expected_result = "Expecting status code 200"
        
        try:
            response = requests.get(url, timeout=timeout_seconds)
            obtained_result = f"Obtained Result: Status Code {response.status_code}"
        except requests.exceptions.ConnectionError as e:
            obtained_result = "Obtained Result: ConnectionError (as expected)"

        print(f"\nINPUT: {url}\n{expected_result}\n{obtained_result}")

def test_multiple_ports():
    print("\nTESTING MULTIPLE WRONG PORTS")
    ports = [9090, 9091]
    timeout_seconds = 5
    
    for port in ports:
        url = f"http://localhost:{port}"
        expected_result = f"Expecting a ConnectionError for port {port}"
        
        try:
            response = requests.get(url, timeout=timeout_seconds)
            obtained_result = f"Obtained Result: Status Code {response.status_code}"
        except requests.exceptions.ConnectionError as e:
            obtained_result = "Obtained Result: ConnectionError (as expected)"

        print(f"\nINPUT: {url}\n{expected_result}\n{obtained_result}")


def test_wrong_url():
    print("\nTESTING WRONG URL")
    url = "http://localhost:{}/index/notthere".format(DEFAULT_PORT)
    expected_result = "Expecting status code 404"
    response = requests.get(url)

    print(f"\nINPUT: {url}\n{expected_result}")
    print(f"Obtained Result: status code {response.status_code}")

    assert response.status_code == 404  # Expecting a 404 Not Found

def test_unknown_extension():
    print("\nTESTING UNKNOWN EXTENSION")
    url = "http://localhost:8080/submit.xyz"
    expected_result = "Expecting status code 415"
    response = requests.get(url)

    print(f"\nINPUT: {url}\n{expected_result}")
    print(f"Obtained Result: status code {response.status_code}")

    assert response.status_code == 415, "Unexpected status code"

def test_keep_alive_connection():
    print("\nTESTING KEEP_ALIVE CONNECTION")
    url = "http://localhost:{}/".format(DEFAULT_PORT)
    expected_result = "Both connections return status code 200"
    headers = {
        "Connection": "keep-alive",
    }

    # Make two consecutive requests using the same connection
    response1 = requests.get(url, headers=headers)
    response2 = requests.get(url, headers=headers)

    print(f"\nINPUT two connections at: {url}\nBoth have the keep-alive property\n{expected_result}")
    print(f"Obtained Result: response1 status code {response1.status_code}")
    print(f"Obtained Result: response2 status code {response2.status_code}")
    # Check the status code and content of the responses
    assert response1.status_code == 200
    assert response2.status_code == 200
    assert response1.text == response2.text

def test_http_methods():
    print("\nTESTING GET POST DELETE METHODS")
    base_url = "http://localhost:{}/".format(DEFAULT_PORT)
    expected_result = "Expecting status code 200"
    print("GET method")
    get_url = base_url + "index.html"
    get_response = requests.get(get_url)
    print(f"\nINPUT: {get_url}\n{expected_result}")
    print(f"Obtained Result: status code {get_response.status_code}")
    assert get_response.status_code == 200

    print("POST method")
    post_url = base_url + "submit.py"
    post_data = {"key": "value"}
    post_response = requests.post(post_url, data=post_data)
    print(f"\nINPUT: {post_url}\n{expected_result}")
    print(f"Obtained Result: status code {post_response.status_code}")
    assert post_response.status_code == 200

    print("DELETE method")
    delete_url = base_url + "delete.py"
    delete_response = requests.delete(delete_url)
    print(f"\nINPUT: {delete_url}\n{expected_result}")
    print(f"Obtained Result: status code {delete_response.status_code}")
    assert delete_response.status_code == 200

def test_chunked_request():
    print("\nSTARTING CHUNKED TEST")
    url = f"http://localhost:{DEFAULT_PORT}/submit.py"

    # Example data for a chunked request
    chunked_data = "5\r\nHello\r\n7\r\n, World\r\n0\r\n\r\n"

    # Send a chunked request
    response = requests.post(url, data=chunked_data, headers={"Transfer-Encoding": "chunked"}, timeout=5)

    # Print test information
    print(f"\nCHUNKED INPUT: \n5\nHello\n7\n, World\n0\n\n")
    print("Expected result: body of the request should be:\nHello, World\n")
    # print(f"Obtained Result: status code {response.status_code}")

    # Output a message to check the web server logs
    print("Please check the web server logs to confirm the body is unchunked.")

if __name__ == "__main__":
    # Run all tests when the script is executed directly
    pytest.main([__file__, '-s'])










