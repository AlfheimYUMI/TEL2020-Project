import json

PATH = '/'.join(__file__.split('/')[:-1])+'/'
f = open(PATH+'menu.json', encoding='utf-8')
print(f.read())
# menu = json.load(open(PATH+'menu.json'))  

# print(json.dumps(menu, indent=2))