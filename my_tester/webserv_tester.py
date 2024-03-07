import requests
import pytest

DEFAULT_PORT = 8080
# from requests.exceptions import ConnectionError

# def test_status_code():
#     url = "http://localhost:{DEFAULT_PORT}"  # Replace with your actual server address
#     response = requests.get(url)
#     print(f"Request to {url} returned status code: {response.status_code}")
#     assert response.status_code == 200  # Adjust the expected status code as needed

# def test_wrong_port():
#     url = "http://localhost:9090"
#     with pytest.raises(ConnectionError):
#         requests.get(url)

# def test_multiple_ports():
#     ports = [9090, 9091]
#     for port in ports:
#         url = f"http://localhost:{port}"
#         with pytest.raises(ConnectionError):
#             response = requests.get(url)

def test_wrong_url():
    url = "http://localhost:8080/index/notthere"
    response = requests.get(url)
        # Print the status code and status message
    print(f"Status Code: {response.status_code}")
    print(f"Status Message: {response.reason}")
    
    assert response.status_code == 404  # Expecting a 404 Not Found

def test_unknown_extension():
    url = "http://localhost:8080/submit.xyz"  # Replace with your server's URL and an unknown extension

    response = requests.get(url)

    print("Status Code:", response.status_code)
    print("Response Content:", response.text)

    # Check if the status code matches the expected value
    assert response.status_code == 415, "Unexpected status code"

# def test_keep_alive_connection():
#     url = f"http://localhost:{DEFAULT_PORT}/"
#     headers = {
#         "Connection": "keep-alive",
#     }

#     # Make two consecutive requests using the same connection
#     response1 = requests.get(url, headers=headers)
#     response2 = requests.get(url, headers=headers)

#     # Check the status code and content of the responses
#     assert response1.status_code == 200
#     assert response2.status_code == 200
#     assert response1.text == response2.text

# def test_http_methods():
#     base_url = f"http://localhost:{DEFAULT_PORT}/"
    
#     # Test GET method
#     get_url = base_url + "/index.html"
#     get_response = requests.get(get_url)
#     assert get_response.status_code == 200

#     # Test POST method
#     post_url = base_url + "submit.py"
#     post_data = {"key": "value"}
#     post_response = requests.post(post_url, data=post_data)
#     assert post_response.status_code == 200

#     # Test DELETE method
#     delete_url = base_url + "delete.py"
#     delete_response = requests.delete(delete_url)
#     assert delete_response.status_code == 200



if __name__ == "__main__":
    # Run all tests when the script is executed directly
    pytest.main([__file__])
