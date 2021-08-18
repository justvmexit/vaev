import requests

req = requests.post('http://localhost:5000/api/v1/zombie/x64', data={'owner': 'c3ead26f-002b-4026-95c4-538c7405a9d9'})
print(req.text)