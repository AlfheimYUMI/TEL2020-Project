import json

PATH = '/'.join(__file__.split('/')[:-1])+'/'
f = open(PATH+'menu.json', encoding='utf-8')
t = f.read()
menu = json.loads(t)  

print(json.dumps(menu, indent=2, ensure_ascii=0))