import ujson as json

SOCD_mode = 0

jsonData = {"SOCD": SOCD_mode}

try:
    with open('savedata.json', 'w') as f:
        json.dump(jsonData, f)
except:
    print('Error! Could not save')
