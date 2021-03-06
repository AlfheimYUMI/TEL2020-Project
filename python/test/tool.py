instances = {}
# PRINT = None

def only(cls):
    '''get the only instance for class'''
    global instances

    def _only(*args, **kw):
        if cls.__name__ not in instances:
            instances[cls.__name__] = cls(*args, **kw)
        return instances[cls.__name__]

    return _only

def get_only(cls, *args, **kw):
    '''if exist get the one'''
    global instances
    if cls.__name__ not in instances:
        instances[cls.__name__] = cls(*args, **kw)
    return instances[cls.__name__]

def get_var(name):
    '''if exist get the one'''
    global instances
    if name not in instances:
        instances[name] = None
    return instances[name]

def display(*arg, **kwargs):
    print(F'{__name__:15}:', end='')
    print(*arg, **kwargs)

def p(EN, *arg, **kwargs):
    if EN:
        print(*arg, **kwargs)

def enable_p():
    global PRINT
    PRINT = 1