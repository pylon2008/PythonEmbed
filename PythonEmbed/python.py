def add(a, b):
    c = a+ b
    d = (c,9)
    raise ValueError, 123
    return d

def test():
    a = 4
    b = 4
    c = a+ b
    d = (c,9)
    raise ValueError, 123
    return d

if __name__ == "__main__":
    print add(2,4)
