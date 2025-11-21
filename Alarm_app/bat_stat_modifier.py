import json

with open("bat_stat.json", "r") as f:
    data=json.load(f)
data=f"{data['status']}"

with open("bat_stat.txt", "w") as f:
    f.write(data)
    

