import requests

# Set up the variables
uid = '19BD0B40'
finalTime = '10'
speed = '3'
penalty = '1'

# Construct the URL
url = f"http://192.168.0.199:80/database/AGILITY32.php?a={uid}&b={finalTime}&c={speed}&d={penalty}"

# Send the request
response = requests.get(url)

# Check the response status
if response.status_code == 200:
    print("Data successfully updated.")
else:
    print(f"Failed to update data. Status code: {response.status_code}")