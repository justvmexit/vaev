import requests
from colorama import *


init(convert=True)

test_good = 0
api_host = 'http://localhost:4000/api/v1/'
owner_id = 'c3ead26f-002b-4026-95c4-538c7405a9d9';
target_architecture = 'tests_x64';
zombie_id = ''

new_zombie = requests.post(f'{api_host}zombie/{target_architecture}', data={'owner': owner_id}).json()
if new_zombie['status'] == "success":
    zombie_id = new_zombie['identifier']
    test_good += 1
    print(f'{Fore.WHITE}[{Fore.GREEN}væv-tests{Fore.WHITE}] {Fore.GREEN}successfully created zombie {Fore.WHITE}{zombie_id} ({Fore.GREEN}{test_good}/4{Fore.WHITE})')
else:
    message = new_zombie['message']
    print(f'{Fore.WHITE}[{Fore.GREEN}væv-tests{Fore.WHITE}] {Fore.RED}failed to create zombie {Fore.WHITE}({Fore.RED}{message}{Fore.WHITE}) ')

ping = requests.get(f'{api_host}ping').json()
if ping['status'] == "success":
    test_good += 1
    print(f'{Fore.WHITE}[{Fore.GREEN}væv-tests{Fore.WHITE}] {Fore.GREEN}successfully pinged server {Fore.WHITE}({Fore.GREEN}{test_good}/4{Fore.WHITE})')
else:
    message = ping['message']
    print(f'{Fore.WHITE}[{Fore.GREEN}væv-tests{Fore.WHITE}] {Fore.RED}failed to ping server {Fore.WHITE}({Fore.RED}{message}{Fore.WHITE}) ')

domain = requests.get(f'{api_host}domain').json()
if domain['status'] == "success":
    test_good += 1
    domain_str = domain['domain']
    print(f'{Fore.WHITE}[{Fore.GREEN}væv-tests{Fore.WHITE}] {Fore.GREEN}successfully found domain {Fore.WHITE}{domain_str} ({Fore.GREEN}{test_good}/4{Fore.WHITE})')
else:
    message = domain['message']
    print(f'{Fore.WHITE}[{Fore.GREEN}væv-tests{Fore.WHITE}] {Fore.RED}failed to ping server {Fore.WHITE}({Fore.RED}{message}{Fore.WHITE}) ')

action = requests.get(f'{api_host}action').json()
if action['status'] == "success" or action['message'] == "No actions for you!":
    test_good += 1
    print(f'{Fore.WHITE}[{Fore.GREEN}væv-tests{Fore.WHITE}] {Fore.GREEN}successfully got action {Fore.WHITE}({Fore.GREEN}{test_good}/4{Fore.WHITE})')
else:
    message = action['message']
    print(f'{Fore.WHITE}[{Fore.GREEN}væv-tests{Fore.WHITE}] {Fore.RED}failed to get action {Fore.WHITE}({Fore.RED}{message}{Fore.WHITE}) ')

if test_good == 4:
    print(f'{Fore.WHITE}[{Fore.GREEN}væv-tests{Fore.WHITE}] {Fore.GREEN}all tests succeeded!')
else:
    print(f'{Fore.WHITE}[{Fore.GREEN}væv-tests{Fore.WHITE}] {Fore.RED}{test_good}/4 succeeded')
