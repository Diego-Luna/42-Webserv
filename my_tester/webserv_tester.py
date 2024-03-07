import requests
import pytest
from requests.exceptions import ConnectionError

# def test_status_code():
#     url = "http://localhost:8080"  # Replace with your actual server address
#     response = requests.get(url)
#     print(f"Request to {url} returned status code: {response.status_code}")
#     assert response.status_code == 200  # Adjust the expected status code as needed

# def test_wrong_port():
#     url = "http://localhost:9090"
#     with pytest.raises(ConnectionError):
        # requests.get(url)

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

# def test_wrong_http_version():
#     url = "http://localhost:8080"
#     headers = {"Connection": "upgrade", "Upgrade": "HTTP/2.0"}
#     response = requests.get(url, headers=headers)
#     assert response.status_code == 400  # Adjust the expected status code for wrong HTTP version

# def test_post_request():
#     url = "http://localhost:8080"
#     response = requests.post(url)
#     assert response.status_code == 405  # Expecting a 405 Method Not Allowed

if __name__ == "__main__":
    # Run all tests when the script is executed directly
    pytest.main([__file__])
